#include "stdafx.h"
#include "@@headers.h"

#include "Player.h"

Player player;
Actor ground;
Bool physicsEnabled = false;
Actor *lit;
Grab grab;
void InitPre()
{
	EE_INIT();
	//Ms.clip(null, 1);

	//D.viewRange(50);
	//Cam.dist = 10;
	Cam.yaw = -PI_4;
	Cam.pitch = -PI_3;
}

bool Init()
{
	Cam.dist = 1.2f;
	Cam.roll = 0.0f;
	Cam.pitch = 0.0f;
	Cam.yaw = -3.0f;
	Cam.matrix.setPos(0.0f, -1.0f, 1.3f);

	//skybox = UID(2000723734, 1340256668, 421298842, 213478118);
	Sky.atmospheric();

	Physics.create();
	ground.create(Box(15, 1, 15, Vec(0, -2, 0)), 0);
	player.create(*ObjectPtr(UID(2919624831, 1261075521, 753053852, 3651670215)));
	player.pos(Vec(0, -1.5, 0));
	return true;
}

void Shut()
{

}

void GetWorldObjectUnderCursor()
{
	lit = null;
	if (!Gui.ms() || Gui.ms() == Gui.desktop())
	{
		Vec     pos, dir;
		ScreenToPosDir(Ms.pos(), pos, dir);
		PhysHit phys_hit;
		if (Physics.ray(pos, dir * D.viewRange(), &phys_hit, ~IndexToFlag(AG_DEFAULT)))
			lit = (Actor*)phys_hit.obj;
	}
}

bool Update()
{
	if (Kb.bp(KB_ESC))
		return false;

	if (physicsEnabled)
	{
		Physics.startSimulation().stopSimulation();
	}

	player.update();

	if (Kb.b(KB_LCTRL))
	{
		Cam.transformByMouse(0.1, 100, CAMH_ZOOM | (Ms.b(1) ? CAMH_MOVE : CAMH_ROT)); // default camera handling actions
	}

	if (Kb.bp(KB_1))
	{
		if (player.ragdoll_mode == Game::Chr::RAGDOLL_FULL)
			player.ragdollDisable();
		else
			player.ragdollEnable();
	}

	if (Kb.bp(KB_2))
	{
		physicsEnabled = !physicsEnabled;
		if (!physicsEnabled)
		{
			player.ragdollDisable();
		}
	}

	if (lit != null && Ms.b(1))
	{
		if (!grab.is())
			grab.create(*lit, Vec(0, 2, 0), 50);
		if (grab.is())
		{
			Vec dir(0);
			dir.x += Ms.d().x;
			dir.y += Ms.d().y;
			grab.pos(grab.pos() + dir * Time.d() * 2);
		}
	}
	else
	{
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

	return true;
}

void Render()
{
	switch (Renderer())
	{
	case RM_PREPARE:
		player.drawPrepare();
		LightDir(!(Cam.matrix.x - Cam.matrix.y + Cam.matrix.z)).add();
		//LightDir(!Vec(1, -1, 1)).add();
		break;
	}
}

void Draw()
{
	Renderer(Render);
	Physics.draw();
	//player.ragdoll.draw(RED);
	D.text(Vec2(0, 0.9f), S+"Physics enabled: " + physicsEnabled + " player: " + player.pos() + " lit: " + lit);
	//ground.draw(BLACK, true);
	//D.text(Vec2(0, 0.7f), S + "Cam: " + Cam.dist + " " + Cam.roll + " " + Cam.pitch + " " + Cam.yaw + " " + Cam.matrix.pos);
}