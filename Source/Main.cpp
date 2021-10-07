#include "stdafx.h"
#include "@@headers.h"

#include "Player.h"
#include "Helpers.h"
#include "ParamWindow.h"

Player player;
Actor ground;
Bool physicsEnabled = false;
Int lit = -1;
Grab grab;
Button b_physicsEnabled, b_meshDrawDisable,
	b_ragdollDrawDisable, b_saveParams, b_loadParams;
ParamWindow parWindow;
Int ActiveBoneIdx = -1;
Int ParentBoneIdx = -1;
bool resetRagdoll = false;

void paramChanged(C EE::Property& prop)
{
	parWindow.JointTypeChanged(const_cast<EE::Property&>(prop));
}

void saveParams(Ptr user)
{
	XmlData xml;
	for (int i = 0; i < player.ragdoll.bones(); i++)
	{
		MyRagdoll::Bone& ragdollBone = player.ragdoll.bone(i);
		XmlNode& node = xml.getNode(ragdollBone.name);
		node.params.New().set("Idx", i);
		node.params.New().set("Name", ragdollBone.name);
		node.params.New().set("IdxParent", ragdollBone.rbon_parent);
		node.params.New().set("IdxSkelBone", ragdollBone.skel_bone);
		node.params.New().set("JointAnchor", ragdollBone.jointData.anchor);
		node.params.New().set("JointAxis", ragdollBone.jointData.axis);
		node.params.New().set("IdxJoint", ragdollBone.jointData.idx);
		node.params.New().set("JointMaxAngle", ragdollBone.jointData.maxAngle);
		node.params.New().set("JointMinAngle", ragdollBone.jointData.minAngle);
		node.params.New().set("JointSkelBoneDir", ragdollBone.jointData.skelBoneDir);
		node.params.New().set("JointSkelBonePos", ragdollBone.jointData.skelBonePos);
		node.params.New().set("JointSwing", ragdollBone.jointData.swing);
		node.params.New().set("JointTwist", ragdollBone.jointData.twist);
		node.params.New().set("JointType", static_cast<int>(ragdollBone.jointData.type));
		node.params.New().set("ActorADamping", ragdollBone.actor.adamping());
		node.params.New().set("ActorDamping", ragdollBone.actor.damping());
		node.params.New().set("ActorSleepEnergy", ragdollBone.actor.sleepEnergy());
	}
	xml.save("ragdoll_params.txt");
}
void loadParams(Ptr user)
{
	if (ActiveBoneIdx != -1)
		return;
	XmlData xml;
	xml.load("ragdoll_params.txt"); // load from file
	/*Mems<MyRagdoll::Bone> bones;
	bones.setNum(player.ragdoll.bones());*/

	for (int i = 0; i < player.ragdoll.bones(); i++)
	{
		MyRagdoll::Bone& ragdollBone = player.ragdoll.bone(i);
		if (XmlNode* node = xml.findNode(ragdollBone.name))
		{
			if (XmlParam* param = node->findParam("Name"))
			{
				Set(ragdollBone.name, param->asText());
			}
			if (XmlParam* param = node->findParam("IdxParent"))
			{
				ragdollBone.rbon_parent = param->asInt();
			}
			if (XmlParam* param = node->findParam("IdxSkelBone"))
			{
				ragdollBone.skel_bone = param->asInt();
			}
			if (XmlParam* param = node->findParam("JointAnchor"))
			{
				ragdollBone.jointData.anchor = param->asVec();
			}
			if (XmlParam* param = node->findParam("JointAxis"))
			{
				ragdollBone.jointData.axis = param->asVec();
			}
			if (XmlParam* param = node->findParam("IdxJoint"))
			{
				ragdollBone.jointData.idx = param->asInt();
			}
			if (XmlParam* param = node->findParam("JointMaxAngle"))
			{
				ragdollBone.jointData.maxAngle= param->asFlt();
			}
			if (XmlParam* param = node->findParam("JointMinAngle"))
			{
				ragdollBone.jointData.minAngle = param->asFlt();
			}
			if (XmlParam* param = node->findParam("JointSkelBoneDir"))
			{
				ragdollBone.jointData.skelBoneDir = param->asVec();
			}
			if (XmlParam* param = node->findParam("JointSkelBonePos"))
			{
				ragdollBone.jointData.skelBonePos = param->asVec();
			}
			if (XmlParam* param = node->findParam("JointSwing"))
			{
				ragdollBone.jointData.swing = param->asFlt();
			}
			if (XmlParam* param = node->findParam("JointTwist"))
			{
				ragdollBone.jointData.twist = param->asFlt();
			}
			if (XmlParam* param = node->findParam("JointType"))
			{
				ragdollBone.jointData.type = static_cast<JOINT_ENUM>(param->asInt());
			}
			if (XmlParam* param = node->findParam("ActorADamping"))
			{
				ragdollBone.actor.adamping(param->asFlt());
			}
			if (XmlParam* param = node->findParam("ActorDamping"))
			{
				ragdollBone.actor.damping(param->asFlt());
			}
			if (XmlParam* param = node->findParam("JointTwist"))
			{
				ragdollBone.actor.sleepEnergy(param->asFlt());
			}
			if(i > 0)
				player.ragdoll.recreateJoint(i);
		}
	}
}

void simulationStart(Ptr usr)
{
	if (!b_physicsEnabled())
	{
		resetRagdoll = true;
	}
}

void disableRagdollDraw(Ptr usr)
{
	if (b_ragdollDrawDisable())
		player.ragdoll.ray(false);
	else
		player.ragdoll.ray(true);
}
void InitPre()
{
	EE_INIT();
	App.flag = APP_RESIZABLE | APP_MAXIMIZABLE;
	App.x = 0;
	App.y = 0;

	Cam.dist = 1;
	Cam.yaw = PI;
}

bool Init()
{
	Sky.atmospheric();

	Physics.create();
	ground.create(Box(15, 1, 15, Vec(0, -0.5, 0)), 0);
	ground.group(GROUP_BACKGROUND);
	player.create(*ObjectPtr(UID(2919624831, 1261075521, 753053852, 3651670215)));
	player.pos(Vec(0, -1.5, 0));
	player.ctrl.del();
	player.ragdoll.create(player.skel, player.scale, 5000);
	player.ragdoll.ray(true);
	
	Cam.at = player.mesh()->ext.pos;
	
	Gui += b_ragdollDrawDisable.create(Rect_C(-0.9, 0.9, 0.65, 0.08), "Disable Ragdoll Draw").func(disableRagdollDraw);
	b_ragdollDrawDisable.mode = BUTTON_TOGGLE;
	Gui += b_physicsEnabled.create(Rect_C(-0.3, 0.9, 0.45, 0.08), "Start Simulation").func(simulationStart);
	b_physicsEnabled.mode = BUTTON_TOGGLE;
	Gui += b_meshDrawDisable.create(Rect_C(0.3, 0.9, 0.55, 0.08), "Disable Mesh Draw");
	b_meshDrawDisable.mode = BUTTON_TOGGLE;
	Gui += b_saveParams.create(Rect_C(0.9, 0.9, 0.55, 0.08), "Save Params").func(saveParams);
	Gui += b_loadParams.create(Rect_C(0.9, 0.8, 0.55, 0.08), "Load Params").func(loadParams);
	parWindow.create();

	return true;
}

void Shut()
{

}

void GetWorldObjectUnderCursor()
{
	lit = -1;
	if (!Gui.ms() || Gui.ms() == Gui.desktop())
	{
		Vec pos, dir;
		ScreenToPosDir(Ms.pos(), pos, dir);
		PhysHit phys_hit;
		if (Physics.ray(pos, dir * D.viewRange(), &phys_hit, IndexToFlag(GROUP_OBJ)))
			lit = (intptr)phys_hit.user;
	}
}

bool Update()
{
	Gui.update();
	if (b_physicsEnabled())
	{
		Physics.startSimulation().stopSimulation();
	}

	if (Kb.bp(KB_ESC))
		return false;

	if (resetRagdoll)
	{
		if (player.ragdoll_mode == Game::Chr::RAGDOLL_FULL)
		{
			player.ragdollDisable();
			resetRagdoll = false;
		}
	}

	player.update();

	if (!b_physicsEnabled())
	{
		player.ragdoll.fromSkel(player.skel, player.ctrl.actor.vel());
	}

	if (player.ragdoll_mode != Game::Chr::RAGDOLL_FULL)
		player.ragdollEnable();

	if (Kb.b(KB_LCTRL))
	{
		
		Cam.transformByMouse(0.1, 100, CAMH_ZOOM | (Ms.b(1) ? CAMH_MOVE : CAMH_ROT)); // default camera handling actions
	}
	else
	{
		Cam.transformByMouse(0.1, 100, CAMH_ZOOM);
	}

	if (Ms.bp(0) && Kb.b(KB_LSHIFT))
	{
		if (lit >= 0 && lit < player.ragdoll.bones())
		{
			if (ActiveBoneIdx >= 0)
				player.ragdoll.recreateJoint(ActiveBoneIdx);
			parWindow.data.name = player.ragdoll.bone(lit).name;
			parWindow.data.mass = player.ragdoll.bone(lit).actor.mass();
			parWindow.data.adamping = player.ragdoll.bone(lit).actor.adamping();
			parWindow.data.damping = player.ragdoll.bone(lit).actor.damping();
			parWindow.data.sleepEnergy = player.ragdoll.bone(lit).actor.sleepEnergy();
			parWindow.data.jointType = player.ragdoll.bone(lit).jointData.type;
			parWindow.data.jointMaxAngle = player.ragdoll.bone(lit).jointData.maxAngle;
			parWindow.data.jointMinAngle = player.ragdoll.bone(lit).jointData.minAngle;
			parWindow.data.jointSwing = player.ragdoll.bone(lit).jointData.swing;
			parWindow.data.jointTwist = player.ragdoll.bone(lit).jointData.twist;
			parWindow.updateData();
			ActiveBoneIdx = lit;
			b_loadParams.enabled(false);
			ParentBoneIdx = player.ragdoll.bone(ActiveBoneIdx).rbon_parent;
		}
		else
		{
			if(ActiveBoneIdx >= 0)
				player.ragdoll.recreateJoint(ActiveBoneIdx);
			ActiveBoneIdx = -1;
			ParentBoneIdx = -1;
			b_loadParams.enabled(true);
		}
	}

	if (lit >= 0 && lit < player.ragdoll.bones() && Ms.b(1))
	{
		if (!grab.is())
		{
			Vec pos, dir;
			ScreenToPosDir(Ms.pos(), pos, dir);
			PhysHit selector;
			if (Physics.ray(pos, dir * D.viewRange(), &selector)) {
				grab.pos(selector.plane.pos);
			}

			grab.create(player.ragdoll.bone(lit).actor, selector.plane.pos, 1500.0f);
		}
		else
		{
			Vec dir(0);
			dir.x += Ms.d().x*100;
			dir.y += Ms.d().y*100;
			grab.pos(grab.pos() + dir * Time.d() * 2);
			
			/*Vec result = (0, 0, 0);
			result.x = Ms.d().x * Cos(player.angle.x) - Ms.d().y * Sin(player.angle.x);
			result.z = Ms.d().x * Sin(player.angle.x) + Ms.d().y * Cos(player.angle.x);
			grab.pos(grab.pos() + result * Time.d() * 50);*/
		}
	}
	else
	{
		if (grab.is())
			grab.del();
		GetWorldObjectUnderCursor();
	}

	if (ActiveBoneIdx >= 0 && ActiveBoneIdx < player.ragdoll.bones())
	{
		parWindow.updateFromGui();
		//Don't want this to change as it will break ragdoll!
		//parWindow.data.name = player.ragdoll.bone(lit).name;
		//parWindow.data.mass = player.ragdoll.bone(lit).actor.mass(); //Mass is calculated from density. Don't overwrite it!!!
		player.ragdoll.bone(ActiveBoneIdx).actor.adamping(parWindow.data.adamping);
		player.ragdoll.bone(ActiveBoneIdx).actor.damping(parWindow.data.damping);
		player.ragdoll.bone(ActiveBoneIdx).actor.sleepEnergy(parWindow.data.sleepEnergy);
		player.ragdoll.bone(ActiveBoneIdx).jointData.type = parWindow.data.jointType;
		player.ragdoll.bone(ActiveBoneIdx).jointData.minAngle = parWindow.data.jointMinAngle;
		player.ragdoll.bone(ActiveBoneIdx).jointData.maxAngle = parWindow.data.jointMaxAngle;
		player.ragdoll.bone(ActiveBoneIdx).jointData.twist = parWindow.data.jointTwist;
		player.ragdoll.bone(ActiveBoneIdx).jointData.swing = parWindow.data.jointSwing;
	}

	return true;
}

void Render()
{
	switch (Renderer())
	{
	case RM_BEHIND:
		break;
	case RM_PREPARE:
		if(!b_meshDrawDisable())
			player.drawPrepare();
		LightDir(!(Cam.matrix.x - Cam.matrix.y + Cam.matrix.z)).add();
		break;
	}
}

void Draw()
{
	Renderer(Render);
	ground.draw(WHITE);
	
	if (!b_ragdollDrawDisable())
	{
		player.ragdoll.draw(RED, YELLOW, PINK, ActiveBoneIdx, ParentBoneIdx);
		if (ActiveBoneIdx >= 0 && ActiveBoneIdx < player.ragdoll.bones())
		{
			player.ragdoll.drawJoints(BLACK, ActiveBoneIdx);
		}
	}
	/*D.text(Vec2(0, 0.9f), S+"Physics enabled: " + physicsEnabled + " player: " + player.pos() + " lit: " + lit);
	if (lit >= 0 && lit < player.ragdoll.bones())
	{
		D.text(Vec2(0, 0.8f), S + "Selected bone name: " + player.ragdoll.bone(lit).name);
	}*/

	Gui.draw();
}