/******************************************************************************/
#include "stdafx.h"
#include "Player.h"
#include "Helpers.h"
/******************************************************************************/
static Shape ShapeBone(C Vec& from, C Vec& to, Flt width)
{
    Shape shape;
    if (width >= 0.5f)
    {
        shape.type = SHAPE_BALL; Ball& ball = shape.ball;
        ball.pos = Avg(from, to);
        ball.r = width * (to - from).length();
    }
    else
    {
        shape.type = SHAPE_CAPSULE; Capsule& capsule = shape.capsule;
        capsule.pos = Avg(from, to);
        capsule.up = to - from;
        capsule.h = capsule.up.normalize();
        capsule.r = Max(0.01f, width * capsule.h);

        Flt eps = capsule.r * 0.5f;
        capsule.pos -= eps * capsule.up;
        capsule.h += eps * 2;
    }
    return shape;
}
inline static Shape ShapeBone(C SkelBone& bone) { return ShapeBone(bone.pos, bone.to(), bone.width); } // return shape from bone
/******************************************************************************/
void MyRagdoll::zero()
{
    _scale = 0;
    _skel = null;
}

MyRagdoll::MyRagdoll() { zero(); }

MyRagdoll& MyRagdoll::del()
{
    _joints.del();
    _bones.del();
    _resets.del();
    _aggr.del(); // delete aggregate after actors, so they won't be re-inserted into scene when aggregate is deleted
    zero(); return T;
}

Bool MyRagdoll::createTry(C AnimatedSkeleton& anim_skel, Flt scale, Flt density, Bool kinematic)
{
    del();

    if (T._skel = anim_skel.skeleton())
    {
        T._scale = scale;

        Memt<Shape> shapes;
        C Skeleton& skel = *anim_skel.skeleton();
        Int         body = -1;
        FREPA(skel.bones) // order is important, main bones should be added at start, in case the skeleton doesn't have "body" bone, and as main bone (zero) should be set some different
        {
            C SkelBone& skelBone = skel.bones[i];
            if (skelBone.flag & BONE_RAGDOLL)
            {
                Vec posBone = skelBone.pos,
                    posBoneEnd = skelBone.to();
                Flt widthBone = skelBone.width;

                if (skelBone.type == BONE_FOOT)
                {
                    C SkelBone* b = skel.findBone(BONE_TOE, skelBone.type_index);
                    if (b)
                    {
                        posBone = Avg(skelBone.pos, skelBone.to());
                        posBoneEnd = b->to();
                        widthBone = Avg(widthBone, b->width) * 0.5f;
                        Vec down = skelBone.dir * (widthBone * Dist(posBone, posBoneEnd) * 0.5f);
                        posBone -= down; posBoneEnd -= down;
                    }
                    else
                        widthBone *= 0.8f;
                }
                else
                {
                    if (skelBone.type == BONE_HAND)
                    {
                        C SkelBone* b = skel.findBone(BONE_FINGER, (skelBone.type_index >= 0) ? 2 : -3); // find middle finger (2 for right, -3 for left hand)
                        if (b)
                        {
                            posBoneEnd = b->to();
                            widthBone *= 0.6f;
                        }
                    }
                    else
                    {
                        if (skelBone.type == BONE_SPINE && skelBone.type_sub == 0)
                        {
                            body = _bones.elms();
                            _resets.add(i); // add main bone for resetting
                        }
                    }
                }
                Shape& shapeBone = shapes.New();
                shapeBone = ShapeBone(posBone, posBoneEnd, widthBone);
                Bone& ragdollBone = _bones.New();
                Set(ragdollBone.name, skelBone.name);
                ragdollBone.skel_bone = i;
                ragdollBone.rbon_parent = 0xFF;
                if (!ragdollBone.actor.createTry(shapeBone * T._scale, density, &VecZero, kinematic))
                    return false;
            }
            else
            {
                _resets.add(i);
            }
        }

        // force 'body' bone to have index=0
        if (body > 0)
        {
            Swap(_bones[0], _bones[body]);
            Swap(shapes[0], shapes[body]);
        }

        // set parents, damping and solver iterations
        REPA(T)
        {
            // find first parent which has an actor
            Bone& rb = bone(i);
            C SkelBone& sb = skel.bones[rb.skel_bone];
            if (i) // skip the main bone
            {
                Byte skel_bone_parent = sb.parent;
                if (skel_bone_parent != 0xFF) // if has a parent
                {
                    Int rbone = findBoneIndexFromSkelBone(skel_bone_parent); // find ragdoll bone assigned to skeleton parent bone
                    if (rbone >= 0)rb.rbon_parent = rbone;                      // if exists, then set as ragdoll parent
                }
            }

            if (sb.type == BONE_HEAD)rb.actor.adamping(7);
            else                  rb.actor.adamping(4);
            rb.actor.damping(0.5f).sleepEnergy(0.1f);
            rb.actor.group(GROUP_OBJ);
            rb.actor.user(ptr(i));
        }

        if (!kinematic)
        {
            // joints
            REPA(_bones)
                if (i) // skip the main bone
                {
                    Bone& ragdollBone = bone(i);
                    C SkelBone& skelBone = skel.bones[ragdollBone.skel_bone];
                    Byte rbon_parent = ((ragdollBone.rbon_parent == 0xFF) ? 0 : ragdollBone.rbon_parent); // if doesn't have a parent then use the main bone

                  //if(rbon_parent!=0xFF)
                    {
                        Bone& ragdollBoneParent = _bones[rbon_parent];
                        C SkelBone& skelBoneParent = skel.bones[ragdollBoneParent.skel_bone];
                        if (skelBone.type == BONE_HEAD)
                        {
                            if (skelBoneParent.type == BONE_NECK)
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = skelBone.dir;
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_HINGE;
                                ragdollBone.jointData.minAngle = 30;
                                ragdollBone.jointData.maxAngle = 35;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = skelBone.dir;
                                _joints.New().createBodyHinge(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, skelBone.dir, DegToRad(30), DegToRad(35));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                            else
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = skelBone.dir;
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_HINGE;
                                ragdollBone.jointData.minAngle = 50;
                                ragdollBone.jointData.maxAngle = 40;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = skelBone.dir;
                                _joints.New().createBodyHinge(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, skelBone.dir, DegToRad(50), DegToRad(40));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                        }
                        else
                        {
                            if (skelBone.type == BONE_NECK)
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = skelBone.dir;
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_HINGE;
                                ragdollBone.jointData.minAngle = 20;
                                ragdollBone.jointData.maxAngle = 5;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = skelBone.dir;
                                _joints.New().createBodyHinge(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, skelBone.dir, DegToRad(20), DegToRad(5));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                            else if (skelBone.type == BONE_SHOULDER)
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = skelBone.dir;
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_SPHERICAL;
                                ragdollBone.jointData.swing = 5;
                                ragdollBone.jointData.twist = 5;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = skelBone.dir;
                                _joints.New().createBodySpherical(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, skelBone.dir, DegToRad(5), DegToRad(5));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                            else if (skelBone.type == BONE_UPPER_ARM)
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = skelBone.dir;
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_SPHERICAL;
                                ragdollBone.jointData.swing = 80;
                                ragdollBone.jointData.twist = 30;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = skelBone.dir;
                                _joints.New().createBodySpherical(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, skelBone.dir, DegToRad(80), DegToRad(30));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                            else if (skelBone.type == BONE_UPPER_LEG)
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = skelBone.dir;
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_SPHERICAL;
                                ragdollBone.jointData.swing = 155;
                                ragdollBone.jointData.twist = 25;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = skelBone.dir;
                                _joints.New().createBodySpherical(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, skelBone.dir, DegToRad(155), DegToRad(25));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                            else if (skelBone.type == BONE_SPINE)
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = skelBone.dir;
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_SPHERICAL;
                                ragdollBone.jointData.swing = 40;
                                ragdollBone.jointData.twist = 30;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = skelBone.dir;
                                _joints.New().createBodySpherical(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, skelBone.dir, DegToRad(40), DegToRad(30));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                            else if (skelBone.type == BONE_FOOT)
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = Vec(1, 0, 0);
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_HINGE;
                                ragdollBone.jointData.minAngle = -45;
                                ragdollBone.jointData.maxAngle = 45;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = Vec(1, 0, 0);
                                _joints.New().createBodyHinge(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, Vec(1, 0, 0), -DegToRad(45), DegToRad(45));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                            else if (skelBone.type == BONE_HAND && skelBone.type_index < 0)
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = Vec(0, 1, 0);
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_HINGE;
                                ragdollBone.jointData.minAngle = -80;
                                ragdollBone.jointData.maxAngle = 80;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = Vec(0, 1, 0);
                                _joints.New().createBodyHinge(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, Vec(0, 1, 0), -DegToRad(80), DegToRad(80));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                            else if (skelBone.type == BONE_HAND && skelBone.type_index >= 0)
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = Vec(0, -1, 0);
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_HINGE;
                                ragdollBone.jointData.minAngle = -80;
                                ragdollBone.jointData.maxAngle = 80;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = Vec(0, -1, 0);
                                _joints.New().createBodyHinge(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, Vec(0, -1, 0), -DegToRad(80), DegToRad(80));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                            else if (skelBone.type == BONE_LOWER_ARM && skelBone.type_index < 0)
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = Vec(0, 1, 0);
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_HINGE;
                                ragdollBone.jointData.minAngle = 0;
                                ragdollBone.jointData.maxAngle = 140;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = Vec(0, 1, 0);
                                _joints.New().createBodyHinge(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, Vec(0, 1, 0), 0, DegToRad(140));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                            else if (skelBone.type == BONE_LOWER_ARM && skelBone.type_index >= 0)
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = Vec(0, -1, 0);
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_HINGE;
                                ragdollBone.jointData.minAngle = 0;
                                ragdollBone.jointData.maxAngle = 140;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = Vec(0, -1, 0);
                                _joints.New().createBodyHinge(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, Vec(0, -1, 0), 0, DegToRad(140));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                            else if (skelBone.type == BONE_LOWER_LEG)
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = Vec(1, 0, 0);
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_HINGE;
                                ragdollBone.jointData.minAngle = 0;
                                ragdollBone.jointData.maxAngle = 150;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = Vec(1, 0, 0);
                                _joints.New().createBodyHinge(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, Vec(1, 0, 0), DegToRad(0), DegToRad(150));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                            else
                            {
                                ragdollBone.jointData.anchor = skelBone.pos * _scale;
                                ragdollBone.jointData.axis = skelBone.dir;
                                ragdollBone.jointData.type = JOINT_ENUM::JOINT_BODY_SPHERICAL;
                                ragdollBone.jointData.swing = 40;
                                ragdollBone.jointData.twist = 30;
                                ragdollBone.jointData.idx = _joints.elms();
                                ragdollBone.jointData.skelBonePos = skelBone.pos;
                                ragdollBone.jointData.skelBoneDir = skelBone.dir;
                                _joints.New().createBodySpherical(ragdollBone.actor, ragdollBoneParent.actor, skelBone.pos * _scale, skelBone.dir, DegToRad(40), DegToRad(30));
                                _joints.last().breakable(-1.0f, -1.0f);
                            }
                        }
                    }
                }

            // ignore
            REPA(T)
                REPD(j, i)
                if (Cuts(shapes[i], shapes[j]))
                    bone(i).actor.ignore(bone(j).actor);
        }
        //_aggr.create(bones()); REPA(T)_aggr.add(bone(i).actor);
        return true;
    }
    return false;
}
/******************************************************************************/
MyRagdoll& MyRagdoll::create(C AnimatedSkeleton& anim_skel, Flt scale, Flt density, Bool kinematic)
{
    if (!createTry(anim_skel, scale, density, kinematic))Exit("Can't create Ragdoll");
    return T;
}
/******************************************************************************
void updateShapes(AnimatedSkeleton &anim_skel); // update 'shape_t' according to 'shape' and skeleton animation, 'anim_skel' must be set to the same skeleton which ragdoll was created from
void Ragdoll::updateShape(AnimatedSkeleton &anim_skel)
{
   if(T.skel==anim_skel.skeleton())REP(bones)shape_t[i]=shape[i]*anim_skel.bone(bone[i].skel_bone).matrix;
}
/******************************************************************************/
MyRagdoll& MyRagdoll::fromSkel(C AnimatedSkeleton& anim_skel, C Vec& vel, Bool immediate_even_for_kinematic_ragdoll)
{
    if (_skel == anim_skel.skeleton())
    {
        Bool scaled = (_scale != 1),
            kinematic = (immediate_even_for_kinematic_ragdoll ? false : T.kinematic());
        REPA(T)
        {
            Bone& rbon = bone(i);
            Actor& actor = rbon.actor;

            //#if 1
            Matrix matrix = anim_skel.bones[rbon.skel_bone].matrix();
            //#else
            //            Matrix matrix = (i ? anim_skel.bone(rbon.skel_bone)._matrix : anim_skel.matrix);
            //#endif
            if (scaled)matrix.orn() /= _scale;

            if (kinematic)actor.kinematicMoveTo(matrix);
            else         actor.matrix(matrix).vel(vel).angVel(VecZero);
        }
    }
    return T;
}
MyRagdoll& MyRagdoll::toSkel(AnimatedSkeleton& anim_skel)
{
    if (_skel == anim_skel.skeleton() && bones())
    {
        C Skeleton& skel = *_skel;

        // reset the orientation of non-ragdoll bones (and main and root) for default pose (the one from default Skeleton)
        anim_skel.root.clear();
        REPA(_resets)anim_skel.bones[_resets[i]].clear();
#if 0
        {
            Byte      sbone = _resets[i];
            Orient& bone_orn = anim_skel.bone(sbone).orn;
            SkelBone& skel_bone = skel.bone(sbone);
            Byte      sparent = skel_bone.parent;
            if (sparent == 0xFF)bone_orn = GetAnimOrient(skel_bone);
            else             bone_orn = GetAnimOrient(skel_bone, &skel.bone(sparent));
        }
#endif

        // set bone oriantation according to actors
        Matrix   body = bone(0).actor.matrix();
        Matrix3 ibody; body.orn().inverse(ibody, true);
        for (Int i = 1; i < bones(); i++) // skip the main bone (zero) because it's set in the reset
        {
            Bone& rbon = bone(i);
            Byte          sbone = rbon.skel_bone,
                rparent = rbon.rbon_parent;
            AnimSkelBone& asbon = anim_skel.bones[sbone];
            C SkelBone& skel_bone = skel.bones[sbone];

            asbon.clear();
            if (InRange(rparent, T))asbon.orn = GetAnimOrient(skel_bone, rbon.actor.orn(), &skel.bones[skel_bone.parent], &NoTemp(bone(rparent).actor.orn()));
            else                   asbon.orn = GetAnimOrient(skel_bone, rbon.actor.orn() * ibody);
        }

        body.scaleOrn(_scale);
        anim_skel.updateMatrix(body);
    }
    return T;
}
MyRagdoll& MyRagdoll::toSkelBlend(AnimatedSkeleton& anim_skel, Flt blend)
{
    if (_skel == anim_skel.skeleton() && bones())
    {
        Flt       blend1 = 1 - blend;
        C Skeleton& skel = *_skel;

        // reset the orientation of non-ragdoll bones (and main) for default pose (the one from default Skeleton)
        REPA(_resets)
        {
            Byte          sbone = _resets[i];
            AnimSkelBone& asbon = anim_skel.bones[sbone];
            C SkelBone& skel_bone = skel.bones[sbone];
            Byte          sparent = skel_bone.parent;

            asbon.orn *= blend1;
            asbon.rot *= blend1;
            asbon.pos *= blend1;
            if (sparent == 0xFF)asbon.orn += blend * GetAnimOrient(skel_bone);
            else             asbon.orn += blend * GetAnimOrient(skel_bone, &skel.bones[sparent]);
        }

        // set bone oriantation according to actors
        Matrix         ragdoll_body = bone(0).actor.matrix();
        Matrix3 ibody; ragdoll_body.orn().inverse(ibody, true);
        for (Int i = 1; i < bones(); i++) // skip the main bone (zero) because it's set in the reset
        {
            Bone& rbon = bone(i);
            Byte          sbone = rbon.skel_bone,
                rparent = rbon.rbon_parent;
            AnimSkelBone& asbon = anim_skel.bones[sbone];
            C SkelBone& skel_bone = skel.bones[sbone];

            asbon.orn *= blend1;
            asbon.rot *= blend1;
            asbon.pos *= blend1;
            if (InRange(rparent, T))asbon.orn += blend * GetAnimOrient(skel_bone, rbon.actor.orn(), &skel.bones[skel_bone.parent], &NoTemp(bone(rparent).actor.orn()));
            else                   asbon.orn += blend * GetAnimOrient(skel_bone, rbon.actor.orn() * ibody);
        }

        // convert root to matrix, and zero himself so it won't be taken into account
        Matrix temp = anim_skel.matrix(); temp.normalize(); anim_skel.root.clear();

        // blend matrix with ragdoll main bone matrix
        ragdoll_body *= blend;
        temp *= blend1;
        temp += ragdoll_body;
        temp.normalize().scaleOrn(_scale);

        // update skeleton according to obtained matrix
        anim_skel.updateMatrix(temp);
    }
    return T;
}
/******************************************************************************
Flt area  (); // total surface area of shapes
Flt volume(); // total volume       of shapes
Flt Ragdoll::area  (){Flt a=0; REP(_bones)a+=shape[i].area  (); return a*Sqr (scale);}
Flt Ragdoll::volume(){Flt v=0; REP(_bones)v+=shape[i].volume(); return v*Cube(scale);}
/******************************************************************************/
MyRagdoll& MyRagdoll::pos(C Vec& pos) { Vec delta = pos - T.pos(); REPAO(_bones).actor.pos(bone(i).actor.pos() + delta); return T; }
MyRagdoll& MyRagdoll::vel(C Vec& vel) { REPAO(_bones).actor.vel(vel); return T; }
MyRagdoll& MyRagdoll::damping(Flt       damping) { REPAO(_bones).actor.damping(damping); return T; }
MyRagdoll& MyRagdoll::adamping(Flt       damping) { REPAO(_bones).actor.adamping(damping); return T; }
MyRagdoll& MyRagdoll::kinematic(Bool      on) { REPAO(_bones).actor.kinematic(on); return T; }
MyRagdoll& MyRagdoll::gravity(Bool      on) { REPAO(_bones).actor.gravity(on); return T; }
MyRagdoll& MyRagdoll::ray(Bool      on) { REPAO(_bones).actor.ray(on); return T; }
MyRagdoll& MyRagdoll::collision(Bool      on) { REPAO(_bones).actor.collision(on); return T; }
MyRagdoll& MyRagdoll::active(Bool      on) { REPAO(_bones).actor.active(on); return T; }
MyRagdoll& MyRagdoll::sleep(Bool      sleep) { REPAO(_bones).actor.sleep(sleep); return T; }
MyRagdoll& MyRagdoll::sleepEnergy(Flt       energy) { REPAO(_bones).actor.sleepEnergy(energy); return T; }
MyRagdoll& MyRagdoll::ccd(Bool      on) { REPAO(_bones).actor.ccd(on); return T; }
MyRagdoll& MyRagdoll::user(Ptr       user) { REPAO(_bones).actor.user(user); return T; }
MyRagdoll& MyRagdoll::obj(Ptr       obj) { REPAO(_bones).actor.obj(obj); return T; }
MyRagdoll& MyRagdoll::group(Byte      group) { REPAO(_bones).actor.group(group); return T; }
MyRagdoll& MyRagdoll::dominance(Byte      dominance) { REPAO(_bones).actor.dominance(dominance); return T; }
MyRagdoll& MyRagdoll::material(PhysMtrl* material) { REPAO(_bones).actor.material(material); return T; }
Vec       MyRagdoll::pos()C { return bones() ? bone(0).actor.pos() : 0; }
Vec       MyRagdoll::vel()C { return bones() ? bone(0).actor.vel() : 0; }
Flt       MyRagdoll::damping()C { return bones() ? bone(0).actor.damping() : 0; }
Flt       MyRagdoll::adamping()C { return bones() ? bone(0).actor.adamping() : 0; }
Bool      MyRagdoll::kinematic()C { return bones() ? bone(0).actor.kinematic() : false; }
Bool      MyRagdoll::gravity()C { return bones() ? bone(0).actor.gravity() : false; }
Bool      MyRagdoll::ray()C { return bones() ? bone(0).actor.ray() : false; }
Bool      MyRagdoll::collision()C { return bones() ? bone(0).actor.collision() : false; }
Ptr       MyRagdoll::user()C { return bones() ? bone(0).actor.user() : null; }
Ptr       MyRagdoll::obj()C { return bones() ? bone(0).actor.obj() : null; }
Byte      MyRagdoll::group()C { return bones() ? bone(0).actor.group() : 0; }
Byte      MyRagdoll::dominance()C { return bones() ? bone(0).actor.dominance() : 0; }
PhysMtrl* MyRagdoll::material()C { return bones() ? bone(0).actor.material() : null; }
Bool      MyRagdoll::sleep()C { return bones() ? bone(0).actor.sleep() : false; }
Flt       MyRagdoll::sleepEnergy()C { return bones() ? bone(0).actor.sleepEnergy() : 0; }
Bool      MyRagdoll::ccd()C { return bones() ? bone(0).actor.ccd() : false; }
/******************************************************************************/
MyRagdoll& MyRagdoll::ignore(Actor& actor, Bool ignore) { REPAO(_bones).actor.ignore(actor, ignore); return T; }
/******************************************************************************/
Int   MyRagdoll::findBoneI(CChar8* name) { REPA(T)if (Equal(bone(i).name, name))return i; return -1; }
MyRagdoll::Bone* MyRagdoll::findBone(CChar8* name) { Int      i = findBoneI(name); return (i < 0) ? null : &bone(i); }
Int   MyRagdoll::getBoneI(CChar8* name) { Int      i = findBoneI(name); if (i < 0)Exit(S + "Bone \"" + name + "\" not found in Ragdoll."); return i; }
MyRagdoll::Bone& MyRagdoll::getBone(CChar8* name) { return bone(getBoneI(name)); }
/******************************************************************************/
Int MyRagdoll::findBoneIndexFromSkelBone(Byte skel_bone_index)C
{
    if (bones())
    {
        if (_skel && InRange(skel_bone_index, _skel->bones))for (;;)
        {
            if (_skel->bones[skel_bone_index].flag & BONE_RAGDOLL) // if skeleton bone should contain a bone in the ragdoll
            {
                REPA(T)if (bone(i).skel_bone == skel_bone_index)return i;
            }
            skel_bone_index = _skel->bones[skel_bone_index].parent;
            if (skel_bone_index == 0xFF)break;
        }
        return 0;
    }
    return -1;
}
Int MyRagdoll::findBoneIndexFromVtxMatrix(Byte matrix_index)C
{
    return findBoneIndexFromSkelBone(matrix_index - 1);
}
/******************************************************************************/
void MyRagdoll::draw(C Color& col)C
{
    FREP(_bones.elms())
    {
        _bones[i].actor.draw(col);
    }
}

void MyRagdoll::drawJoints(C Color& col, C Int idx)C
{
    SetMatrix(_bones[idx].actor.matrix());
    _bones[idx].jointData.anchor.draw(col);
    D.line(GREEN, _bones[idx].jointData.anchor, _bones[idx].jointData.anchor + _bones[idx].jointData.axis * 0.2);
}

void MyRagdoll::draw(C Color& color, C Color& colorSpecial, C Int idx)C
{
    if (idx == -1)
    {
        draw(color);
    }
    else
    {
        FREP(_bones.elms())
        {
            if(i == idx)
                _bones[i].actor.draw(colorSpecial);
            else
                _bones[i].actor.draw(color);
        }
    }
}

void MyRagdoll::draw(C Color& color, C Color& colorSelect, C Color& colorParent, C Int idx, C Int parentIdx)C
{
    if (parentIdx == -1)
    {
        draw(color, colorSelect, idx);
    }
    else
    {
        FREP(_bones.elms())
        {
            if (i == idx)
                _bones[i].actor.draw(colorSelect);
            else if (i == parentIdx)
                _bones[i].actor.draw(colorParent);
            else
                _bones[i].actor.draw(color);
        }
    }
}
/******************************************************************************/
#pragma pack(push, 1)
struct RagdollDesc
{
    Byte group, dominance, material;
    UInt user, flag;
    Flt  sleep_energy;
};
struct RagdollActorDesc
{
    Vec    vel, ang_vel;
    Matrix matrix;
};
#pragma pack(pop)

Bool MyRagdoll::saveState(File& f, Bool include_matrix_vel)C
{
    f.cmpUIntV(0);

    RagdollDesc desc;

    _Unaligned(desc.material, 0);
    Unaligned(desc.group, group());
    Unaligned(desc.dominance, dominance());
    _Unaligned(desc.user, (UIntPtr)user());
    Unaligned(desc.sleep_energy, sleepEnergy());

    UInt flag = 0;
    if (kinematic())flag |= ACTOR_KINEMATIC;
    if (gravity())flag |= ACTOR_GRAVITY;
    if (ray())flag |= ACTOR_RAY;
    if (collision())flag |= ACTOR_COLLISION;
    if (sleep())flag |= ACTOR_SLEEP;
    if (ccd())flag |= ACTOR_CCD;
    Unaligned(desc.flag, flag);

    f << desc;
    f.putBool(include_matrix_vel); if (include_matrix_vel)
    {
        f.putInt(bones()); FREPA(T)
        {
            C Actor& actor = bone(i).actor;
            RagdollActorDesc ad;
            Unaligned(ad.vel, actor.vel());
            Unaligned(ad.ang_vel, actor.angVel());
            Unaligned(ad.matrix, actor.matrix());
            f << ad;
        }
    }
    return f.ok();
}
Bool MyRagdoll::loadState(File& f) // don't delete on fail, as here we're loading only state
{
    switch (f.decUIntV())
    {
    case 0:
    {
        RagdollDesc desc; if (f.get(desc))
        {
            group(Unaligned(desc.group));
            dominance(Unaligned(desc.dominance));
            user(Ptr(Unaligned(desc.user)));
            sleepEnergy(Unaligned(desc.sleep_energy));

            kinematic(FlagTest(Unaligned(desc.flag), ACTOR_KINEMATIC));
            gravity(FlagTest(Unaligned(desc.flag), ACTOR_GRAVITY));
            ray(FlagTest(Unaligned(desc.flag), ACTOR_RAY));
            collision(FlagTest(Unaligned(desc.flag), ACTOR_COLLISION));
            sleep(FlagTest(Unaligned(desc.flag), ACTOR_SLEEP));
            ccd(FlagTest(Unaligned(desc.flag), ACTOR_CCD));

            if (f.getBool())
            {
                Int bones = f.getInt();
                if (bones != T.bones()) // number of bones doesn't match (we're dealing with a different ragdoll than before)
                {
                    if (bones >= 1) // if there was at least one bone saved
                    {
                        RagdollActorDesc ad; f >> ad; // load first bone
                        f.skip((bones - 1) * SIZE(ad)); // skip the rest
                        FREPA(T)                    // set all bones basing on the first
                        {
                            bone(i).actor.vel(Unaligned(ad.vel)).angVel(Unaligned(ad.ang_vel)).matrix(Unaligned(ad.matrix));
                        }
                    }
                }
                else
                    FREPA(T)
                {
                    RagdollActorDesc ad; f >> ad;
                    bone(i).actor.vel(Unaligned(ad.vel)).angVel(Unaligned(ad.ang_vel)).matrix(Unaligned(ad.matrix));
                }
            }
            if (f.ok())return true;
        }
    }break;
    }
    return false;
}

void MyRagdoll::recreateJoint(const Int ragdollBoneIdx)
{
    if (ragdollBoneIdx == 0)
        return;
    const JointData& jointData = _bones[ragdollBoneIdx].jointData;
    Bone& ragdollBone = _bones[ragdollBoneIdx];
    Bone& ragdollBoneParent = _bones[((ragdollBone.rbon_parent == 0xFF) ? 0 : ragdollBone.rbon_parent)];
    if (jointData.type == JOINT_ENUM::JOINT_BODY_HINGE)
    {
        int minAngleSign = jointData.minAngle >= 0 ? 1 : -1;
        int maxAngleSign = jointData.maxAngle >= 0 ? 1 : -1;
        _joints[jointData.idx].createBodyHinge(ragdollBone.actor, ragdollBoneParent.actor, jointData.skelBonePos * _scale,
            jointData.skelBoneDir, minAngleSign * DegToRad(jointData.minAngle), maxAngleSign * DegToRad(jointData.maxAngle));
    }
    else if (jointData.type == JOINT_ENUM::JOINT_BODY_SPHERICAL)
    {
        int swingSign = jointData.minAngle >= 0 ? 1 : -1;
        int twistSign = jointData.maxAngle >= 0 ? 1 : -1;
        _joints[jointData.idx].createBodySpherical(ragdollBone.actor, ragdollBoneParent.actor, jointData.skelBonePos * _scale,
            jointData.skelBoneDir, swingSign * DegToRad(jointData.swing), twistSign * DegToRad(jointData.twist));
    }
}
