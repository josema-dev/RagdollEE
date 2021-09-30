/******************************************************************************/
#include "stdafx.h"
//#include "@@headers.h"
#include "Player.h"
/******************************************************************************/
void Player::ragdollValidate()
{
    if (!ragdoll.is()) // if ragdoll hasn't yet been created
    {
        ragdoll.create(skel, scale)  // create from 'AnimatedSkeleton'
            .obj(this)  // set Game.Chr object
            .ignore(ctrl.actor); // ignore collisions with the character controller
    }
}
void Player::ragdollEnable()
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
void Player::ragdollDisable()
{
    if (ragdoll_mode == RAGDOLL_FULL)
    {
        ragdoll.active(false);
        ctrl.actor.active(true);

        ragdoll_mode = RAGDOLL_NONE;
    }
}
Bool Player::ragdollBlend()
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
Vec Player::pos() { return skel.pos(); }
void Player::pos(C Vec& pos)
{
    Vec delta = pos - T.pos(); // delta which moves from old position to new position

    skel.move(delta);                       // move skeleton
    ctrl.actor.pos(ctrl.actor.pos() + delta); // move controller
    ragdoll.pos(ragdoll.pos() + delta); // move ragdoll
}

void Player::memoryAddressChanged()
{
    ctrl.actor.obj(this);
    ragdoll.obj(this);
}

Bool Player::update()
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
void Player::disable()
{
    super::disable();

    ctrl.actor.kinematic(true);
    if (ragdoll_mode)ragdoll.kinematic(true);
}
void Player::enable()
{
    super::enable();

    ctrl.actor.kinematic((ragdoll_mode == RAGDOLL_FULL) ? true : false); // if character is in full ragdoll mode, then controller should be set as kinematic
    if (ragdoll_mode)ragdoll.kinematic(false);
}
/******************************************************************************/
// IO
/******************************************************************************/
Bool Player::save(File& f)
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
Bool Player::load(File& f)
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
/******************************************************************************/
