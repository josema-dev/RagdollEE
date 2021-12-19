#include "stdafx.h"
#include "@@headers.h"

#include "Player.h"
#include "Helpers.h"
#include "ParamWindow.h"
#include "DensityWindow.h"

static const EE::Str ragdollParamsFileName = "ragdoll_params.txt";
static const Flt StartDensity = 5000;

Player player;
Actor ground;
Actor box[3];
Bool physicsEnabled = false;
Int lit = -1;
Grab grab;

Button b_editMode, b_simulationMode,
    b_ragdollDrawDisable, b_meshDrawDisable;

Button b_saveParams, b_loadParams,
    b_updateDensity;

Button b_physicsEnabled;

DensityWindow updateDensityWindow{ StartDensity };
ParamWindow parWindow;
Int ActiveBoneIdx = -1;
Int ParentBoneIdx = -1;

void dummyDataNoBoneSelected()
{
    parWindow.data.name = "";
    parWindow.data.mass = -1.0f;
    parWindow.data.adamping = -1.0f;
    parWindow.data.damping = -1.0f;
    parWindow.data.sleepEnergy = -1.0f;
    parWindow.data.jointType = JOINT_ENUM::JOINT_NO;
    parWindow.data.jointMaxAngle = 0.0f;
    parWindow.data.jointMinAngle = 0.0f;
    parWindow.data.jointSwing = 0.0f;
    parWindow.data.jointTwist = 0.0f;
    parWindow.updateData();
}
void updatePlayerRagdollParams()
{
    if (ActiveBoneIdx < 0 || ActiveBoneIdx >= player.ragdoll.bones())
        return;
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
    player.ragdoll.recreateJoint(ActiveBoneIdx);
}

void UpdateSkelDensity(Flt density)
{
    if (ActiveBoneIdx != -1)
        return;

    updatePlayerRagdollParams();
    Mems<RagdollActorData> ragdoll = player.ragdoll.GetRagdollData();
    RagdollData ragdollData(density, ragdoll);
    player.ragdoll.create(player.skel, ragdollData, player.scale, ragdollData.Density());
}

void paramChanged(C EE::Property& prop)
{
    parWindow.JointTypeChanged(const_cast<EE::Property&>(prop));
}

void disableRagdollDraw(Ptr usr)
{
    if (b_ragdollDrawDisable())
        player.ragdoll.ray(false);
    else
        player.ragdoll.ray(true);
}

void saveParams(Ptr usr)
{
    updatePlayerRagdollParams();
    Mems<RagdollActorData> ragdollData = player.ragdoll.GetRagdollData();
    RagdollDataHelpers::SaveRagdollData(ragdollParamsFileName, RagdollData(player.ragdoll.density(), ragdollData));
}

void loadParams(Ptr usr)
{
    if (ActiveBoneIdx != -1)
        return;
    Flt density = StartDensity;
    Mems<RagdollActorData> ragdoll = RagdollDataHelpers::LoadRagdollData(ragdollParamsFileName, density);
    RagdollData ragdollData(density, ragdoll);
    player.ragdoll.create(player.skel, ragdollData, player.scale, ragdollData.Density());
}

void updateDensity(Ptr usr)
{
    updateDensityWindow.UpdateDensity(player.ragdoll.density());
    updateDensityWindow.fadeIn();
}

void DeselectBoneAndUpdateParams()
{
    if (ActiveBoneIdx >= 0)
    {
        updatePlayerRagdollParams();
    }
    ActiveBoneIdx = -1;
    ParentBoneIdx = -1;
    dummyDataNoBoneSelected();
    b_loadParams.enabled(true);
    b_updateDensity.enabled(true);
}

void EditMode(Ptr usr)
{
    if (b_editMode())
    {
        Cam.dist = 1;
        Cam.yaw = PI;
        b_simulationMode.set(false);
        parWindow.fadeIn();
        b_saveParams.show();
        b_loadParams.show();
        b_updateDensity.show();
        player.pos(Vec(0, -1.5, 0));
        player.ragdoll_mode = Game::Chr::RAGDOLL_NONE;
        player.ragdollDisable();
    }
    else
    {
        parWindow.fadeOut();
        b_saveParams.hide();
        b_loadParams.hide();
        b_updateDensity.hide();
        b_simulationMode.set(true);
        DeselectBoneAndUpdateParams();
    }
}

void SetSimulationPos()
{
    player.pos(Vec(0, 3, 0));
}

void SimulationMode(Ptr usr)
{
    if (b_simulationMode())
    {
        Cam.dist = 5;
        Cam.pitch = -0.3;
        Cam.yaw = PI_2;
        b_editMode.set(false);
        b_physicsEnabled.show();
        REPA(box)box[i].create(Box(1, Vec(0, i * 0.5 + 0.5, i * -0.35 - 0.3)), 0);
        SetSimulationPos();
    }
    else
    {
        b_physicsEnabled.set(false);
        b_physicsEnabled.hide();
        b_editMode.set(true);
        REPA(box)box[i].del();
    }
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
    player.ragdoll.create(player.skel, player.scale, StartDensity);
    player.ragdoll.ray(true);

    Cam.at = player.mesh()->ext.pos;

    Gui += b_editMode.create(Rect_C(-1.0, 0.9, 0.60, 0.08), "Edit Mode").func(EditMode);
    b_editMode.mode = BUTTON_TOGGLE;
    b_editMode.set(true);
    Gui += b_simulationMode.create(Rect_C(-0.4, 0.9, 0.60, 0.08), "Simulation Mode").func(SimulationMode);
    b_simulationMode.mode = BUTTON_TOGGLE;

    Gui += b_ragdollDrawDisable.create(Rect_C(0.2, 0.9, 0.60, 0.08), "Disable Ragdoll Draw").func(disableRagdollDraw);
    b_ragdollDrawDisable.mode = BUTTON_TOGGLE;
    Gui += b_meshDrawDisable.create(Rect_C(0.8, 0.9, 0.60, 0.08), "Disable Mesh Draw");
    b_meshDrawDisable.mode = BUTTON_TOGGLE;

    Gui += b_physicsEnabled.create(Rect_C(1.2, 0.8, 0.55, 0.08), "Start Simulation");
    b_physicsEnabled.mode = BUTTON_TOGGLE;

    Gui += b_saveParams.create(Rect_C(1.2, 0.8, 0.55, 0.08), "Save Params").func(saveParams);
    Gui += b_loadParams.create(Rect_C(1.2, 0.7, 0.55, 0.08), "Load Params").func(loadParams);
    Gui += b_updateDensity.create(Rect_C(1.2, 0.6, 0.55, 0.08), "Update Density").func(updateDensity);
    parWindow.create();

    updateDensityWindow.create();
    updateDensityWindow.SetUpdateFunc(UpdateSkelDensity);

    dummyDataNoBoneSelected();
    RagdollDataHelpers::GetDefaultRagdollData();
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

void UpdateEditMode()
{
    if (Ms.bp(0) && Kb.b(KB_LSHIFT))
    {
        if (lit >= 0 && lit < player.ragdoll.bones())
        {
            if (ActiveBoneIdx >= 0)
                updatePlayerRagdollParams();
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
            b_updateDensity.enabled(false);
            ParentBoneIdx = player.ragdoll.bone(ActiveBoneIdx).rbon_parent;
        }
        else
        {
            DeselectBoneAndUpdateParams();
        }
    }

    if (!(lit >= 0 && lit < player.ragdoll.bones() && Ms.b(1)))
    {
        GetWorldObjectUnderCursor();
    }
}
void UpdateSimulationMode()
{
    if (b_physicsEnabled())
    {
        Physics.startSimulation().stopSimulation();
    }
    else
    {
        player.ragdoll.fromSkel(player.skel, player.ctrl.actor.vel());
    }
}

bool Update()
{
    Gui.update();

    if (Kb.bp(KB_ESC))
        return false;

    player.update();

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

    if (b_editMode())
    {
        UpdateEditMode();
    }
    else if (b_simulationMode())
    {
        UpdateSimulationMode();
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
    if(b_simulationMode())
        REPA(box)box[i].draw(WHITE);

    if (!b_ragdollDrawDisable())
    {
        player.ragdoll.draw(RED, YELLOW, PINK, ActiveBoneIdx, ParentBoneIdx);
        if (ActiveBoneIdx >= 0 && ActiveBoneIdx < player.ragdoll.bones())
        {
            player.ragdoll.drawJoints(BLACK, ActiveBoneIdx);
        }
    }

    Gui.draw();
}