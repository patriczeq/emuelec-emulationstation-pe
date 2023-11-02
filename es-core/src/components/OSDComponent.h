#pragma once

#include <mutex>
#include "GuiComponent.h"

class ComponentGrid;
class NinePatchComponent;
class TextComponent;
class Window;

class OSDComponent : public GuiComponent
{
public:
	OSDComponent(Window* window);
	~OSDComponent();

	void render(const Transform4x4f& parentTrans) override;
	void update(int deltaTime) override;

	void setTotalTime(int t){totalTime = t;}
	void setCurrTime(int t){ currTime = t;}


	void reset() { totalTime = -1; currTime = -1; }

private:
	NinePatchComponent* mFrame;
	TextComponent*		mLabelCurr;
	TextComponent*		mLabelTotal;

	int totalTime;
	int currTime;
	bool paused;

	bool showme;

	int mCheckTime;
	int mDisplayTime;
};