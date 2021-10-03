#pragma once

#include "stdafx.h"
#include "@@headers.h"

cchar8* enum_names[] =
{
    "JOINT_BODY_SPHERICAL",
    "JOINT_BODY_HINGE",
};

class BoneInfo
{
    Str name;
    Flt mass;
    Flt damping;
    Flt sleepEnergy;
    Flt adamping;
    JOINT_ENUM jointType;
    Flt jointTwist;
    Flt jointSwing;
    Flt jointMinAngle;
    Flt jointMaxAngle;
};

class ParamWindow : Window
{
    Memx<Property> props;
    BoneInfo data;

    Int minAngleIdx, maxAngleIdx, swingIdx, twistIdx;

    void create()
    {
        Gui += Window::create(Rect_C(-0.8, 0.6, 1.0, 1.0), "Properties").barVisible(true);
        props.New().create("Name (Info)", MEMBER(BoneInfo, name)).enabled(false);
        props.New().create("Mass (Info)", MEMBER(BoneInfo, mass)).enabled(false);
        props.New().create("Damping", MEMBER(BoneInfo, damping));
        props.New().create("Angular Damping", MEMBER(BoneInfo, adamping));
        props.New().create("Sleep Energy", MEMBER(BoneInfo, sleepEnergy));
        props.New().create("Joint Type", MEMBER(BoneInfo, jointType)).setEnum(enum_names, Elms(enum_names)).enabled(false);
        swingIdx = props.elms();
        props.New().create("Swing (Deg)", MEMBER(BoneInfo, jointSwing));
        twistIdx = props.elms();
        props.New().create("Twist (Deg)", MEMBER(BoneInfo, jointTwist)).range(-180, 180);
        minAngleIdx = props.elms();
        props.New().create("MinAngle (Deg)", MEMBER(BoneInfo, jointMinAngle));
        maxAngleIdx = props.elms();
        props.New().create("MaxAngle (Deg)", MEMBER(BoneInfo, jointMaxAngle));
        AddProperties(props, *this, Vec2(0.04, -0.04), 0.06, 0.6);
        REPAO(props).toGui(&data);
    }

    void updateData()
    {
        if (data.jointType == JOINT_ENUM::JOINT_BODY_HINGE)
        {
            props[minAngleIdx].enabled(true);
            props[maxAngleIdx].enabled(true);
            props[twistIdx].enabled(false);
            props[swingIdx].enabled(false);
        }
        else if (data.jointType == JOINT_ENUM::JOINT_BODY_SPHERICAL)
        {
            props[minAngleIdx].enabled(false);
            props[maxAngleIdx].enabled(false);
            props[twistIdx].enabled(true);
            props[swingIdx].enabled(true);
        }
        REPAO(props).toGui(&data);
    }

    void updateFromGui()
    {
        REPAO(props).fromGui(&data);
    }
};