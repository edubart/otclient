#ifndef MOVINGBUTTON_H
#define MOVINGBUTTON_H

#include <client/declarations.h>
#include <framework/ui/uiwidget.h>

class MovingButton : public UIWidget
{
	//Allows the button to be drawn by the UIManager
	void drawSelf(Fw::DrawPane drawPane);

protected:
	
	//Will change position ypos onClick
	bool onMousePress(const Point& mousePos, Fw::MouseButton button);

	//In lieu of an "Update" function, perhaps it exists within the framework but I haven't stumbled
	//across it yet.
	void onStyleApply(const std::string& styleName, const OTMLNodePtr& styleNode);

};
#endif
