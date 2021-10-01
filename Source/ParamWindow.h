#pragma once

#include "stdafx.h"
#include "@@headers.h"

class ParamWindow : Window // main menu
{
    //Text t_sleepEnergy;// , b_load, b_options, b_exit;
    Memx<Property> props;
    /*static void New(ptr) { MN.fadeIn(); }
    static void Load(ptr) { ML.fadeIn(); }
    static void Options(ptr) { MO.fadeIn(); }
    static void Exit(ptr) { StateExit.set(); }*/

    //void create()
    //{
    //    Gui += super.create(Rect_C(0, -0.1, 0.6, 0.6)).barVisible(false);
    //    props.New().create("flt_member", MEMBER(Data, flt_member)).range(0, 1);
    //    flag &= ~WIN_MOVABLE; // disable moving of this window
    //    level(-1); // set lower level so this window will be always below the other created in this tutorial
    //}
};