class Player : public Game::Chr
{
public:
	static const Flt EPS_ANIM_BLEND(1.0f / 4096);                                     // Animation Blend Epsilon (default value used for ignoring animations)
	void ragdollValidate() override
	{
		if (!ragdoll.is()) // if ragdoll hasn't yet been created
		{
			Flt densityOut; // Density saved in file or 1000 if not saved.
			Mems<RagdollActorData> rad = RagdollDataHelpers::LoadRagdollData(RAGDOLL_PARAMS_FILE_UID, densityOut); // Parameters for all bones and joints
			RagdollData rd(densityOut, rad); // Object with all data to create ragdoll
			ragdoll.create(skel, rd, scale, densityOut).obj(this).ignore(ctrl.actor); // Create ragdoll from loaded data
		}
	}
	void ragdollEnable() override
	{
		if (ragdoll_mode != RAGDOLL_FULL)
		{
			ragdollValidate(); // make sure the ragdoll is created

			ragdoll.active(true)  // enable ragdoll actors
				.gravity(true)  // gravity should be enabled for full ragdoll mode
				.fromSkel(skel, ctrl.actor.vel()); // set ragdoll initial pose
			ctrl.actor.active(false); // disable character controller completely, do this after setting ragdoll in case deactivating clears velocity which is used above

			ragdoll_mode = RAGDOLL_FULL;
		}
	}
	void ragdollDisable() override
	{
		if (ragdoll_mode == RAGDOLL_FULL)
		{
			ragdoll.active(false);
			ctrl.actor.active(true);

			ragdoll_mode = RAGDOLL_NONE;
		}
	}
	Bool ragdollBlend() override
	{
		if (ragdoll_mode != RAGDOLL_FULL)
		{
			ragdollValidate(); // make sure the ragdoll is created

			ragdoll.active(true) // enable ragdoll collisions
				.gravity(false) // disable gravity for hit-simulation effects because they look better this way
				.fromSkel(skel, ctrl.actor.vel()); // set ragdoll initial pose

			ragdoll_time = 0;
			ragdoll_mode = RAGDOLL_PART;

			return true; // ragdoll set successfully
		}
		return false; // can't set ragdoll mode
	}
	Vec pos() override { return skel.pos(); }
	void pos(C Vec& pos) override
	{
		Vec delta = pos - T.pos(); // delta which moves from old position to new position

		skel.move(delta);                       // move skeleton
		ctrl.actor.pos(ctrl.actor.pos() + delta); // move controller
		ragdoll.pos(ragdoll.pos() + delta); // move ragdoll
	}

	void memoryAddressChanged() override
	{
		ctrl.actor.obj(this);
		ragdoll.obj(this);
	}

	Bool update() override
	{
		skel.updateBegin(); // begin skeleton update

		if (ragdoll_mode == RAGDOLL_FULL) // the character has ragdoll fully enabled, which most likely means that the character is dead, so update only skeleton basing on the ragdoll pose
		{
			ragdoll.toSkel(skel);
		}
		else
		{
			updateAction(); // update automatic actions to set the input
			updateAnimation(); // update all animation parameters
			updateController(); // update character controller
		}

		skel.updateEnd(); // end skeleton update

		return true;
	}

	/******************************************************************************/
	// ENABLE / DISABLE
	/******************************************************************************/
	void disable() override
	{
		super::disable();

		ctrl.actor.kinematic(true);
		if (ragdoll_mode)ragdoll.kinematic(true);
	}
	void enable() override
	{
		super::enable();

		ctrl.actor.kinematic((ragdoll_mode == RAGDOLL_FULL) ? true : false); // if character is in full ragdoll mode, then controller should be set as kinematic
		if (ragdoll_mode)ragdoll.kinematic(false);
	}
	/******************************************************************************/
	// IO
	/******************************************************************************/
	Bool save(File& f) override
	{
		if (super::save(f))
		{
			f.cmpUIntV(0); // version

			f << move_walking << scale << speed << turn_speed << flying_speed << angle;

			if (ctrl.save(f))
				if (skel.save(f))
				{
					f.putAsset(base.id());
					f.putUInt(mesh ? mesh->variationID(mesh_variation) : 0);
					f.putByte(ragdoll_mode);
					if (ragdoll_mode)
					{
						f << ragdoll_time;
						if (!ragdoll.saveState(f))return false;
					}

					f << dodging; if (dodging)f << dodge_step;
					f << move_dir;
					f << anim;

					f.putByte(action);
					if (action == EE::Game::ACTION_MOVE_TO)f << path_target;
					return f.ok();
				}
		}
		return false;
	}
	/******************************************************************************/
	Bool load(File& f) override
	{
		if (super::load(f))switch (f.decUIntV()) // version
		{
		case 0:
		{
			f >> move_walking >> scale >> speed >> turn_speed >> flying_speed >> angle;

			if (ctrl.load(f))
				if (skel.load(f))
				{
					ctrl.actor.obj(this);

					base = f.getAssetID();
					setUnsavedParams();
					UInt mesh_variation_id = f.getUInt(); mesh_variation = (mesh ? mesh->variationFind(mesh_variation_id) : 0);
					sac.set(skel, base);
					if (ragdoll_mode = RAGDOLL_MODE(f.getByte()))
					{
						f >> ragdoll_time;
						ragdollValidate();
						if (!ragdoll.loadState(f))return false; ragdoll.obj(this);
					}

					f >> dodging; if (dodging)f >> dodge_step;
					f >> move_dir;
					f >> anim;

					action = EE::Game::ACTION_TYPE(f.getByte());
					if (action == EE::Game::ACTION_MOVE_TO) { Vec target; f >> target; actionMoveTo(target); } // again set the action to reset the paths

					if (f.ok())return true;
				}
		}break;
		}
		return false;
	}

	void updateAnimation()
	{
		updateAnimationAnim();

		// first animate using only basic animations (standing or crouching) to detect 'foot_offset' (it will be used to align the whole body matrix while crouching so that the foot will always be in constant position - "attached" to certain position on the ground)
		Bool use_foot_offset = false;
		Vec      foot_offset;
		if (anim.fly<1 - EPS_ANIM_BLEND         // only when not flying
			&& sac.toe_l >= 0                      // left foot was detected
			&& anim.stand_crouch>EPS_ANIM_BLEND) // crouching
		{
			use_foot_offset = true;

			Flt time = AnimTime(T);
			Vec pos_foot = skel.skeleton()->bones[sac.toe_l].pos;
			skel.clear();
			skel.animate(sac.stand, time, 1 - anim.stand_crouch).updateMatrixParents(scale, sac.toe_l); foot_offset = pos_foot * skel.bones[sac.toe_l].matrix();
			skel.animate(sac.crouch, time, anim.stand_crouch).updateMatrixParents(scale, sac.toe_l); foot_offset = foot_offset - pos_foot * skel.bones[sac.toe_l].matrix(); foot_offset.y = 0;
		}

		// set animations
		animate();

		// manually adjust body, neck and head bones (according to look angles, movement, ...)
		Int body_sign = ((sac.body >= 0) ? Sign(skel.skeleton()->bones[sac.body].perp.z) : 1);
		Flt body_pitch = angle.y * body_sign;
		body_pitch *= 1 - anim.fly_full_body * anim.fly; // when flying with full body, disable body pitch bending, and rotate main matrix instead
		Flt body_pitch_part = body_pitch / (4 + anim.fly), // body_pitch rotation is split into 4 bones rotations (body,body_u,neck,head), when flying also the main matrix is used (making a total of 5 splits)
			head_neck_pitch = body_pitch_part + anim.fly * anim.fly_z * 0.5f * body_sign; // add flying adjustment split into 2 bones
		{
			Flt body_roll = (-PI_4 / 4) * anim.strafe_yaw, // body roll is split into 4 bones (body, body_u, neck, head)
				head_neck_roll = body_roll;//-angle_head/2

			Vec     v;
			Matrix3 m;

			// body
			v = skel.root.orn.cross(); if (v.allZero())v.x = 1; CHS(v.z); // body x-axis
			if (sac.body >= 0) { m.setRotate(v, body_pitch_part * -body_sign).rotateY(body_roll); skel.bones[sac.body].orn *= m; }

			// body_u
			if (sac.head >= 0)
			{
				v = skel.bones[sac.head].orn.cross(); // lucky guess for upper bones rotation
				if (sac.body_u >= 0) { m.setRotate(v, body_pitch_part).rotateZ(body_roll); skel.bones[sac.body_u].orn *= m; }

				// neck + head
				m.setRotate(v, head_neck_pitch).rotateZ(head_neck_roll);
				if (sac.neck >= 0)skel.bones[sac.neck].orn *= m;
				skel.bones[sac.head].orn *= m;
				skel.bones[sac.head].rot.axis.z -= anim.fly_strafe * anim.fly;
			}
		}

		// main matrix
		Matrix m;
		{
			Flt body_pitch_rest = body_pitch - 4 * body_pitch_part; // part of the 'body_pitch' which was left to set in the main matrix

			// orientation
			m.orn().setRotateY(anim.fly * anim.fly_turn + anim.strafe_yaw * PI_4)
				.rotateZ(anim.lean * (1 - anim.straight_strafe) + anim.fly * (anim.fly_x - anim.fly_strafe))
				.rotateX(anim.lean * (anim.straight_strafe) * (anim.left_right * 2 - 1) + anim.fly * (anim.fly_z - anim.fly_full_body * angle.y) + body_pitch_rest * -body_sign)
				.rotateY(-angle.x);

			// position
			m.pos = ctrl.actor.pos(); // set initial position from the controller actor
			m.pos += ctrl.shapeOffset() - ctrl.shapeOffset() * m.orn(); // this operation forces the custom 'm.orn' rotations to be done relative to the center of character controller

			if (use_foot_offset)
			{
				foot_offset *= m.orn();
				m.pos += foot_offset;
			}

			// adjust legs flying fake movement
			if (anim.fly > EPS_ANIM_BLEND)
			{
				Vec vec = ctrl.actor.vel(); vec.y = -6;
				Flt angle = AbsAngleBetween(vec, Vec(0, -1, 0));
				angle *= 0.2f;
				angle = Log2(angle + 1);
				angle *= (1 - anim.fly_full_body) * anim.fly;
				if (angle > EPS)
				{
					MIN(angle, 0.4f);
					Vec axis = CrossN(vec, Vec(0, -1, 0)) / m.orn() * angle;

					if (sac.leg_lu >= 0)skel.bones[sac.leg_lu].rot.axis += axis;
					if (sac.leg_ru >= 0)skel.bones[sac.leg_ru].rot.axis += axis;
					if (axis.x > EPS)
					{
						if (sac.leg_ld >= 0)skel.bones[sac.leg_ld].rot.axis.x += axis.x;
						if (sac.leg_rd >= 0)skel.bones[sac.leg_rd].rot.axis.x += axis.x;
					}
				}
			}
		}

		// scale
		m.scaleOrn(scale);

		// set the skeleton according to the matrix
		animateUpdateMatrix(m);

		// align feet
		AlignFeet(T, animateAlignFeet());

		// blend the ragdoll with the skeleton
		if (ragdoll_mode == RAGDOLL_PART)
		{
			Flt length = 0.34f;
			ragdoll_time += Time.d();
			if (ragdoll_time >= length)
			{
				ragdoll_mode = RAGDOLL_NONE;
				ragdoll.active(false);
			}
			else
			{
				Flt step = ragdoll_time / length,
					blend = 1 - step * step; if (step < 0.1f)blend *= step / 0.1f;
				ragdoll.toSkelBlend(skel, blend * 0.45f);
			}
		}
	}
	inline void AlignFeet(Chr& chr, Flt step) // align character feet by 'step'=0..1 factor
	{
		if (step > EPS_ANIM_BLEND && chr.sac.toe_l >= 0 && chr.sac.toe_r >= 0)
		{
			Flt y = chr.ctrl.center().y - chr.ctrl.height() * 0.5f,                                                // get controller bottom Y position
				yl = (chr.skel.skeleton()->bones[chr.sac.toe_l].pos * chr.skel.bones[chr.sac.toe_l].matrix()).y, // get left  foot bottom Y position
				yr = (chr.skel.skeleton()->bones[chr.sac.toe_r].pos * chr.skel.bones[chr.sac.toe_r].matrix()).y; // get right foot bottom Y position
			chr.skel.offset(Vec(0, (y - Min(yl, yr) + 0.03f) * step, 0));                                            // offset the animation skeleton according to feet placement
		}
	}

	inline Flt AnimTime(Chr& chr) // get global animation time for the character according to current time and character's 'unique' value
	{
		return Time.time() + chr.anim.unique * 10;
	}
	MyRagdoll ragdoll;
};
