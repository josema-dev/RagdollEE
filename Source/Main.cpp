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
Button b_physicsEnabled, b_ragdollEnabled, b_meshDrawDisable;
ParamWindow parWindow;
Int ActiveBoneIdx = -1;

void InitPre()
{
	EE_INIT();
	Ms.clip(null, 1);

	//D.viewRange(50);
	//Cam.dist = 10;
	Cam.yaw = -PI_4;
	Cam.pitch = -PI_3;

	Cam.dist = 1.2f;
	Cam.roll = 0.0f;
	/*Cam.pitch = 0.0f;
	Cam.yaw = -3.0f;*/
	Cam.matrix.setPos(0.0f, 1.0f, 1.3f);
}
void EnableDisableRagdoll(ptr)
{
	if (player.ragdoll_mode == Game::Chr::RAGDOLL_FULL)
	{
		player.ragdollDisable();
		player.ragdoll.del();
		b_ragdollEnabled.text = "Enable Ragdoll";
	}
	else
	{
		//player.ctrl.del();
		player.ragdollEnable();
		player.ragdoll.ray(true);
		b_ragdollEnabled.text = "Disable Ragdoll";
	}
}
bool Init()
{
	//Cam.dist = 1.2f;
	//Cam.roll = 0.0f;
	//Cam.pitch = 0.0f;
	//Cam.yaw = -3.0f;
	//Cam.matrix.setPos(0.0f, -1.0f, 1.3f);

	Sky.atmospheric();

	Physics.create();
	ground.create(Box(15, 1, 15, Vec(0, -0.5, 0)), 0);
	ground.group(GROUP_BACKGROUND);
	player.create(*ObjectPtr(UID(2919624831, 1261075521, 753053852, 3651670215)));
	player.pos(Vec(0, -1.5, 0));
	player.ctrl.del();
	

	Gui += b_ragdollEnabled.create(Rect_C(-0.3, 0.7, 0.45, 0.08), "Enable Ragdoll").func(EnableDisableRagdoll);
	Gui += b_physicsEnabled.create(Rect_C(0.3, 0.7, 0.45, 0.08), "Enable Physics");
	b_physicsEnabled.mode = BUTTON_TOGGLE;
	Gui += b_meshDrawDisable.create(Rect_C(0.9, 0.7, 0.55, 0.08), "Disable Mesh Draw");
	b_meshDrawDisable.mode = BUTTON_TOGGLE;
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
		Vec     pos, dir;
		ScreenToPosDir(Ms.pos(), pos, dir);
		PhysHit phys_hit;
		if (Physics.ray(pos, dir * D.viewRange(), &phys_hit, IndexToFlag(GROUP_OBJ)))
			lit = (intptr)phys_hit.user;
	}
}

bool Update()
{
	Gui.update();

	if (Kb.bp(KB_ESC))
		return false;

	if (b_physicsEnabled())
	{
		Physics.startSimulation().stopSimulation();
	}

	player.update();

	if (Kb.b(KB_LCTRL))
	{
		Cam.transformByMouse(0.1, 100, CAMH_ZOOM | (Ms.b(1) ? CAMH_MOVE : CAMH_ROT)); // default camera handling actions
	}

	if (Ms.b(0))
	{
		if (lit >= 0 && lit < player.ragdoll.bones())
		{
			parWindow.data.name = player.ragdoll.bone(lit).name;
			parWindow.data.mass = player.ragdoll.bone(lit).actor.mass();
			parWindow.data.adamping = player.ragdoll.bone(lit).actor.adamping();
			parWindow.data.damping = player.ragdoll.bone(lit).actor.damping();
			parWindow.data.sleepEnergy = player.ragdoll.bone(lit).actor.sleepEnergy();
			parWindow.updateData();
			ActiveBoneIdx = lit;
		}
		else
		{
			ActiveBoneIdx = -1;
			//lit = -1;
		}
	}

	if (lit >= 0 && lit < player.ragdoll.bones() && Ms.b(1))
	{
		if (!grab.is())
			grab.create(player.ragdoll.bone(lit).actor, Vec(0, 0.2, 0), 15.0f);
		else
		{
			Vec dir(0);
			dir.x += Ms.d().x*100;
			dir.y += Ms.d().y*100;
			grab.pos(grab.pos() + dir * Time.d() * 2);
		}
	}
	else
	{
		if (grab.is())
			grab.del();
		GetWorldObjectUnderCursor();
	}

	
	/*Cam.updateBegin();
	if (Ms.b(1))
	{
		Cam.yaw -= Ms.d().x;
		Cam.pitch += Ms.d().y;
	}
	Cam.dist* ScaleFactor(Ms.wheel() * -0.2);
	Cam.setPosDir(player.ctrl.actor.pos() + Vec(-1, 1, 0));
	Cam.updateEnd().set();*/

	/*Cam.updateBegin().setSpherical(player.ctrl.actor.pos() + Vec(0, 1, 0),
		player.angle.x, player.angle.y, 0,
		Cam.dist * ScaleFactor(Ms.wheel() * -0.2)).updateEnd().set();*/
	
	//Cam.yaw -= Ms.d().x; // update camera yaw   angle according to mouse delta x
	//Cam.pitch += Ms.d().y; // update camera pitch angle according to mouse delta y
	//Clamp(Cam.pitch, -PI_2, 0); // clamp to possible camera pitch angle
	//Cam.dist = Max(1.0, Cam.dist * ScaleFactor(Ms.wheel() * -0.1)); // update camera distance according to mouse wheel
	//Cam.setSpherical(player.pos(), Cam.yaw, Cam.pitch, 0, Cam.dist); // set spherical camera looking at player using camera angles

	if (ActiveBoneIdx >= 0 && ActiveBoneIdx < player.ragdoll.bones())
	{
		parWindow.updateFromGui();
		//Don't want this to change as it will break ragdoll!
		//parWindow.data.name = player.ragdoll.bone(lit).name;
		//parWindow.data.mass = player.ragdoll.bone(lit).actor.mass(); //Mass is calculated from density. Don't overwrite it!!!
		player.ragdoll.bone(ActiveBoneIdx).actor.adamping(parWindow.data.adamping);
		player.ragdoll.bone(ActiveBoneIdx).actor.damping(parWindow.data.damping);
		player.ragdoll.bone(ActiveBoneIdx).actor.sleepEnergy(parWindow.data.sleepEnergy);
	}

	return true;
}

void Render()
{
	switch (Renderer())
	{
	case RM_BEHIND:
		//player.ragdoll.draw(RED);
		break;
	case RM_PREPARE:
		if(!b_meshDrawDisable())
			player.drawPrepare();
		LightDir(!(Cam.matrix.x - Cam.matrix.y + Cam.matrix.z)).add();
		//LightDir(!Vec(1, -1, 1)).add();
		break;
	}
}

void Draw()
{
	Renderer(Render);
	ground.draw(WHITE);
	//player.ragdoll.draw(RED);
	player.ragdoll.draw(RED, YELLOW, ActiveBoneIdx);
	if (ActiveBoneIdx >= 0 && ActiveBoneIdx < player.ragdoll.bones())
	{
		player.ragdoll.drawJoints(PINK, ActiveBoneIdx);
		player.ragdoll.bone(ActiveBoneIdx).actor.draw(YELLOW);
	}
	D.text(Vec2(0, 0.9f), S+"Physics enabled: " + physicsEnabled + " player: " + player.pos() + " lit: " + lit);
	if (lit >= 0 && lit < player.ragdoll.bones())
	{
		D.text(Vec2(0, 0.8f), S + "Selected bone name: " + player.ragdoll.bone(lit).name);
	}

	Gui.draw();
}