#pragma once

#include "stdafx.h"
#include "@@headers.h"

class BoneInfo
{
    Str name;
    Flt mass;
    Flt damping;
    Flt sleepEnergy;
    Flt adamping;
};

class ParamWindow : Window
{
    Memx<Property> props;
    BoneInfo data;

    void create()
    {
        Gui += Window::create(Rect_C(-0.8, 0.6, 0.6, 0.6), "Properties").barVisible(true);
        props.New().create("Name", MEMBER(BoneInfo, name));
        props.New().create("Mass", MEMBER(BoneInfo, mass));
        props.New().create("Damping", MEMBER(BoneInfo, damping));
        props.New().create("Angular Damping", MEMBER(BoneInfo, adamping));
        props.New().create("Sleep Energy", MEMBER(BoneInfo, sleepEnergy));
        AddProperties(props, *this, Vec2(0.04, -0.04), 0.06, 0.3);
        REPAO(props).toGui(&data);
    }

    void updateData()
    {
        REPAO(props).toGui(&data);
    }

    void updateFromGui()
    {
        REPAO(props).fromGui(&data);
    }
};