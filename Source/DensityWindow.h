#pragma once

#include "stdafx.h"
#include "@@headers.h"

class DensityWindow : ModalWindow
{
public:
	DensityWindow(Flt density)
	{
		_density = density;
	}

	void create()
	{
		Gui += Window::create(Rect_C(0.8, 1.0, 1.0, 0.5), "Skel Properties").barVisible(true).pos(Vec2(0.2, 0.4)).visible(false);
		T += _text.create(Rect_C(0.3, -0.1, 0.4, 0.1), "Density:");
		T += _textLine.create(Rect_C(0.7, -0.1, 0.4, 0.1), Str(_density));
		T += b_updateValue.create(Rect_C(0.3, -0.3, 0.35, 0.1), "Update").func(Update, T);
		T += b_cancel.create(Rect_C(0.7, -0.3, 0.35, 0.1), "Cancel").func(Cancel, T);
	}

	void SetUpdateFunc(void(*func)(Flt))
	{
		updateSkelDensityFunc = func;
	}

	void UpdateDensity(Flt density)
	{
		_density = density;
		_textLine.set(Str(_density));
	}

	static void Cancel(DensityWindow& densityWindow)
	{
		densityWindow.fadeOut();
	}
	
	static void Update(DensityWindow& densityWindow)
	{
		Flt tmpDensity = TextFlt(densityWindow._textLine());
		if (tmpDensity != 0 && tmpDensity != densityWindow._density)
		{
			densityWindow._density = tmpDensity;
			if(densityWindow.updateSkelDensityFunc != nullptr)
				densityWindow.updateSkelDensityFunc(tmpDensity);
		}
		densityWindow.fadeOut();
	}

protected:
private:
	Flt _density;
	TextLine _textLine;
	Text _text;
	Button b_updateValue, b_cancel;
	void (*updateSkelDensityFunc)(Flt) { nullptr };
};