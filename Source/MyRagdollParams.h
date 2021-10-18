#pragma once

#include "stdafx.h"
#include "@@headers.h"


class Player;

enum class JOINT_ENUM
{
    JOINT_NO = -1,
    JOINT_BODY_SPHERICAL = 0,
    JOINT_BODY_HINGE = 1,
};

struct JointData
{
    Vec anchor = Vec(0);
    Vec axis = Vec(0);
    JOINT_ENUM type = JOINT_ENUM::JOINT_NO;
    Flt swing = 0.0f;
    Flt twist = 0.0f;
    Flt minAngle = 0.0f;
    Flt maxAngle = 0.0f;
    Int idx = -1;
};

struct RagdollActorData
{
    Char8 name[32]; // name
    JointData jointData;
    Byte ragdollBoneParentIdx;
    Byte skelBoneIdx;
    Flt angularDamping;
    Flt damping;
    Flt sleepEnergy;
};

class RagdollData
{
public:
    bool SaveRagdollData(Ptr user, const Player& player) const;
    bool LoadRagdollData(Ptr user, Player& player);
    const RagdollActorData* RagdollBone(EE::Str name) const;
    static Mems<RagdollActorData> GetDefaultRagdollData();
    //RagdollActorData* RagdollBone(EE::Str name);

protected:
private:
    Mems<RagdollActorData > _ragdollBones;
    Flt _density;
};