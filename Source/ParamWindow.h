#pragma once

#include "stdafx.h"
#include "@@headers.h"
class BoneInfo
{
    Str name;
    Flt mass;
};

class ParamWindow : Window // main menu
{
    //Text t_sleepEnergy;// , b_load, b_options, b_exit;
    Memx<Property> props;
    BoneInfo data;
    /*static void New(ptr) { MN.fadeIn(); }
    static void Load(ptr) { ML.fadeIn(); }
    static void Options(ptr) { MO.fadeIn(); }
    static void Exit(ptr) { StateExit.set(); }*/

    void create()
    {
        Gui += Window::create(Rect_C(-0.8, 0.6, 0.6, 0.6), "Properties").barVisible(false);
        props.New().create("flt_member", MEMBER(BoneInfo, mass));
        props.New().create("str_member", MEMBER(BoneInfo, name));// .range(0, 1);
        flag &= ~WIN_MOVABLE; // disable moving of this window
        AddProperties(props, *this, Vec2(0.04, -0.04), 0.06, 0.3);
        REPAO(props).toGui(&data);
        //level(-1); // set lower level so this window will be always below the other created in this tutorial
    }

    virtual void update(C GuiPC& gpc)override
    {
        Window::update(gpc);
        REPAO(props).fromGui(&data);
    }

    void updateData()
    {
        REPAO(props).toGui(&data);
    }
};