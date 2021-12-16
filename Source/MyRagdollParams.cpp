#include "stdafx.h"

#include "MyRagdollParams.h"
#include "MyRagdoll.h"

#include "Player.h"

RagdollData::RagdollData() : _density{ 1.0f } {}
RagdollData::RagdollData(Flt density) : _density { density } {}
RagdollData::RagdollData(Flt density, const Mems<RagdollActorData>& ragdollBones) : _density{ density }, _ragdollBones{ ragdollBones }{}

const RagdollActorData* RagdollData::RagdollBone(EE::Str name) const
{
	for(int i=0; i<_ragdollBones.elms(); i++)
	{
		if (_ragdollBones[i].name == name)
			return &_ragdollBones[i];
	}
	return nullptr;
}

RagdollActorData* RagdollData::RagdollBone(EE::Str name)
{
	for (int i = 0; i < _ragdollBones.elms(); i++)
	{
		if (_ragdollBones[i].name == name)
			return &_ragdollBones[i];
	}
	return nullptr;
}

Mems<RagdollActorData> RagdollDataHelpers::GetDefaultRagdollData()
{
	Mems<RagdollActorData> ragdollActors;
	//{name, {anchor, axis, idx, maxAngle, minAngle, swing, twist, joint_type} parentIdx, skelBoneIdx, AngularDamping, Damping, SleepEnergy}
	ragdollActors.add(RagdollActorData{ "Body", JointData{Vec(0), Vec(0), JOINT_ENUM::JOINT_NO, 0, 0, 0, 0, -1}, 255, 2, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "LegLU", JointData{Vec(-0.060035, 0.516415, 0.011794), Vec(-0.012091, -0.993139, -0.11631), JOINT_ENUM::JOINT_BODY_SPHERICAL, 0, 0, 155, 25, 15}, 255, 1, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "LegRU", JointData{Vec(0.055378, 0.516947, 0.007655), Vec(-0.006944, -0.990633, -0.136374), JOINT_ENUM::JOINT_BODY_SPHERICAL, 0, 0, 155, 25, 14}, 255, 0, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "LegRD", JointData{Vec(0.053674, 0.272571, -0.025984), Vec(1, 0, 0), JOINT_ENUM::JOINT_BODY_HINGE, 150, 0, 0, 0, 13}, 2, 3, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "FootR", JointData{Vec(0.068674, 0.058324, -0.054122), Vec(1, 0, 0), JOINT_ENUM::JOINT_BODY_HINGE, 45, -45, 0, 0, 12}, 3, 4, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "LegLD", JointData{Vec(0.053674, 0.272571, -0.025984), Vec(1, 0, 0), JOINT_ENUM::JOINT_BODY_HINGE, 150, 0, 0, 0, 11}, 1, 6, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "FootL", JointData{Vec(0.068674, 0.058324, -0.054122), Vec(1, 0, 0), JOINT_ENUM::JOINT_BODY_HINGE, 45, -45, 0, 0, 10}, 5, 7, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "BodyU", JointData{Vec(-0.000191, 0.671217, 0.032715), Vec(-0.001589, 0.975674, -0.21922), JOINT_ENUM::JOINT_BODY_SPHERICAL, 0, 0, 5, 5, 9}, 0, 9, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "ShoulderL", JointData{Vec(-0.000295, 0.807894, -0.01576), Vec(-0.997831, -0.039715, 0.052495), JOINT_ENUM::JOINT_BODY_SPHERICAL, 0, 0, 40, 30, 8}, 7, 11, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "ShoulderR", JointData{Vec(-0.000317, 0.807881, -0.01576), Vec(0.999386, -0.034774, 0.004399), JOINT_ENUM::JOINT_BODY_SPHERICAL, 0, 0, 40, 30, 7}, 7, 12, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "Head", JointData{Vec(-0.000125, 0.882767, 0.005782), Vec(0.003559, 0.955904, 0.293658), JOINT_ENUM::JOINT_BODY_HINGE, 40, 50, 0, 0, 6}, 7, 13, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "ArmLU", JointData{Vec(-0.129196, 0.803023, -0.009376), Vec(-0.245548, -0.960674, -0.129658), JOINT_ENUM::JOINT_BODY_SPHERICAL, 0, 0, 80, 30, 5}, 8, 14, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "ArmLD", JointData{Vec(-0.160283, 0.681529, -0.025777), Vec(0, 1, 0), JOINT_ENUM::JOINT_BODY_HINGE, 140, 0, 0, 0, 4}, 11, 15, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "HandL", JointData{Vec(-0.221322, 0.531328, 0.011764), Vec(0, 1, 0), JOINT_ENUM::JOINT_BODY_HINGE, 80, -80, 0, 0, 3}, 12, 16, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "ArmRU", JointData{Vec(0.122862, 0.803602, -0.015076), Vec(0.312226, -0.946672, -0.079538), JOINT_ENUM::JOINT_BODY_SPHERICAL, 0, 0, 80, 30, 2}, 9, 27, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "ArmRD", JointData{Vec(0.163643, 0.680062, -0.025458), Vec(0, -1, 0), JOINT_ENUM::JOINT_BODY_HINGE, 140, 0, 0, 0, 1}, 14, 28, 4.0f, 0.5f, 0.1f });
	ragdollActors.add(RagdollActorData{ "HandR", JointData{Vec(-0.221322, 0.531328, 0.011764), Vec(0, -1, 0), JOINT_ENUM::JOINT_BODY_HINGE, 80, -80, 0, 0, 0}, 15, 29, 4.0f, 0.5f, 0.1f });

	return ragdollActors;
}

Mems<RagdollActorData> RagdollDataHelpers::LoadRagdollData(const EE::Str& fileName)
{
	Flt densityOut = 1000.0f;
	return LoadRagdollData(fileName, densityOut);
}

Mems<RagdollActorData> RagdollDataHelpers::LoadRagdollData(const EE::Str& fileName, Flt& densityOut)
{
	densityOut = 1000.0f;
	Mems<RagdollActorData> ragdollData;
	RagdollActorData ragdollActorData;
	XmlData xml;
	xml.load(fileName); // load from file
	for (int i = 0; i < xml.nodes.elms(); i++)
	{
		if (XmlParam* param = xml.nodes[i].findParam("Name"))
		{
			Set(ragdollActorData.name, param->asText());
		}
		if (XmlParam* param = xml.nodes[i].findParam("IdxParent"))
		{
			ragdollActorData.ragdollBoneParentIdx = param->asInt();
		}
		if (XmlParam* param = xml.nodes[i].findParam("IdxSkelBone"))
		{
			ragdollActorData.skelBoneIdx = param->asInt();
		}
		if (XmlParam* param = xml.nodes[i].findParam("JointAnchor"))
		{
			ragdollActorData.jointData.anchor = param->asVec();
		}
		if (XmlParam* param = xml.nodes[i].findParam("JointAxis"))
		{
			ragdollActorData.jointData.axis = param->asVec();
		}
		if (XmlParam* param = xml.nodes[i].findParam("IdxJoint"))
		{
			ragdollActorData.jointData.idx = param->asInt();
		}
		if (XmlParam* param = xml.nodes[i].findParam("JointMaxAngle"))
		{
			ragdollActorData.jointData.maxAngle = param->asFlt();
		}
		if (XmlParam* param = xml.nodes[i].findParam("JointMinAngle"))
		{
			ragdollActorData.jointData.minAngle = param->asFlt();
		}
		if (XmlParam* param = xml.nodes[i].findParam("JointSwing"))
		{
			ragdollActorData.jointData.swing = param->asFlt();
		}
		if (XmlParam* param = xml.nodes[i].findParam("JointTwist"))
		{
			ragdollActorData.jointData.twist = param->asFlt();
		}
		if (XmlParam* param = xml.nodes[i].findParam("JointType"))
		{
			ragdollActorData.jointData.type = static_cast<JOINT_ENUM>(param->asInt());
		}
		if (XmlParam* param = xml.nodes[i].findParam("ActorADamping"))
		{
			ragdollActorData.angularDamping = param->asFlt();
		}
		if (XmlParam* param = xml.nodes[i].findParam("ActorDamping"))
		{
			ragdollActorData.damping = param->asFlt();
		}
		if (XmlParam* param = xml.nodes[i].findParam("ActorSleepEnergy"))
		{
			ragdollActorData.sleepEnergy = param->asFlt();
		}
		if (XmlParam* param = xml.nodes[i].findParam("Density"))
		{
			densityOut = param->asFlt();
		}
		ragdollData.add(ragdollActorData);
	}

	return ragdollData;
}

bool RagdollDataHelpers::SaveRagdollData(const EE::Str& fileName, const RagdollData& ragdollData)
{
	XmlData xml;
	XmlNode& node = xml.getNode("SkelParams");
	node.params.New().set("Density", ragdollData._density);
	for (int i = 0; i < ragdollData._ragdollBones.elms(); i++)
	{
		const RagdollActorData& ragdollBone = ragdollData._ragdollBones[i];
		XmlNode& node = xml.getNode(ragdollBone.name);
		node.params.New().set("Idx", i);
		node.params.New().set("Name", ragdollBone.name);
		node.params.New().set("IdxParent", ragdollBone.ragdollBoneParentIdx);
		node.params.New().set("IdxSkelBone", ragdollBone.skelBoneIdx);
		node.params.New().set("JointAnchor", ragdollBone.jointData.anchor);
		node.params.New().set("JointAxis", ragdollBone.jointData.axis);
		node.params.New().set("IdxJoint", ragdollBone.jointData.idx);
		node.params.New().set("JointMaxAngle", ragdollBone.jointData.maxAngle);
		node.params.New().set("JointMinAngle", ragdollBone.jointData.minAngle);
		node.params.New().set("JointSwing", ragdollBone.jointData.swing);
		node.params.New().set("JointTwist", ragdollBone.jointData.twist);
		node.params.New().set("JointType", static_cast<int>(ragdollBone.jointData.type));
		node.params.New().set("ActorADamping", ragdollBone.angularDamping);
		node.params.New().set("ActorDamping", ragdollBone.damping);
		node.params.New().set("ActorSleepEnergy", ragdollBone.sleepEnergy);
	}
	return xml.save(fileName);
}