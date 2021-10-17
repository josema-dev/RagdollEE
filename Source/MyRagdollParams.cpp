#include "stdafx.h"

#include "MyRagdollParams.h"
#include "MyRagdoll.h"

#include "Player.h"

bool RagdollData::SaveRagdollData(Ptr user, const Player& player) const
{
	XmlData xml;
	for (int i = 0; i < player.ragdoll.bones(); i++)
	{
		const MyRagdoll::Bone& ragdollBone = player.ragdoll.bone(i);
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
		node.params.New().set("JointSwing", ragdollBone.jointData.swing);
		node.params.New().set("JointTwist", ragdollBone.jointData.twist);
		node.params.New().set("JointType", static_cast<int>(ragdollBone.jointData.type));
		node.params.New().set("ActorADamping", ragdollBone.actor.adamping());
		node.params.New().set("ActorDamping", ragdollBone.actor.damping());
		node.params.New().set("ActorSleepEnergy", ragdollBone.actor.sleepEnergy());
	}
	return xml.save("ragdoll_params.txt");
}

bool RagdollData::LoadRagdollData(Ptr user, Player& player)
{
	XmlData xml;
	xml.load("ragdoll_params.txt"); // load from file
	RagdollActorData ragdollActorData;

	for (int i = 0; i < player.ragdoll.bones(); i++)
	{
		MyRagdoll::Bone& ragdollBone = player.ragdoll.bone(i);
		if (XmlNode* node = xml.findNode(ragdollBone.name))
		{
			if (XmlParam* param = node->findParam("Name"))
			{
				Set(ragdollBone.name, param->asText());
				Set(ragdollActorData.name, param->asText());
			}
			if (XmlParam* param = node->findParam("IdxParent"))
			{
				ragdollBone.rbon_parent = param->asInt();
				ragdollActorData.ragdollBoneParentIdx = param->asInt();
			}
			if (XmlParam* param = node->findParam("IdxSkelBone"))
			{
				ragdollBone.skel_bone = param->asInt();
				ragdollActorData.skelBoneIdx = param->asInt();
			}
			if (XmlParam* param = node->findParam("JointAnchor"))
			{
				ragdollBone.jointData.anchor = param->asVec();
				ragdollActorData.jointData.anchor = param->asVec();
			}
			if (XmlParam* param = node->findParam("JointAxis"))
			{
				ragdollBone.jointData.axis = param->asVec();
				ragdollActorData.jointData.axis = param->asVec();
			}
			if (XmlParam* param = node->findParam("IdxJoint"))
			{
				ragdollBone.jointData.idx = param->asInt();
				ragdollActorData.jointData.idx = param->asInt();
			}
			if (XmlParam* param = node->findParam("JointMaxAngle"))
			{
				ragdollBone.jointData.maxAngle = param->asFlt();
			}
			if (XmlParam* param = node->findParam("JointMinAngle"))
			{
				ragdollBone.jointData.minAngle = param->asFlt();
				ragdollActorData.jointData.minAngle = param->asFlt();
			}
			if (XmlParam* param = node->findParam("JointSwing"))
			{
				ragdollBone.jointData.swing = param->asFlt();
				ragdollActorData.jointData.swing = param->asFlt();
			}
			if (XmlParam* param = node->findParam("JointTwist"))
			{
				ragdollBone.jointData.twist = param->asFlt();
				ragdollActorData.jointData.twist = param->asFlt();
			}
			if (XmlParam* param = node->findParam("JointType"))
			{
				ragdollBone.jointData.type = static_cast<JOINT_ENUM>(param->asInt());
				ragdollActorData.jointData.type = static_cast<JOINT_ENUM>(param->asInt());
			}
			if (XmlParam* param = node->findParam("ActorADamping"))
			{
				ragdollBone.actor.adamping(param->asFlt());
				ragdollActorData.angularDamping = param->asFlt();
			}
			if (XmlParam* param = node->findParam("ActorDamping"))
			{
				ragdollBone.actor.damping(param->asFlt());
				ragdollActorData.damping = param->asFlt();
			}
			if (XmlParam* param = node->findParam("ActorSleepEnergy"))
			{
				ragdollBone.actor.sleepEnergy(param->asFlt());
				ragdollActorData.sleepEnergy = param->asFlt();
			}
			if (i > 0)
				player.ragdoll.recreateJoint(i);
			_ragdollBones.add(ragdollActorData);
		}
	}

	return true;
}