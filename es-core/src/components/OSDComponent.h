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
	OSDComponent(Window* window, bool actionLine = true);
	~OSDComponent();

	void render(const Transform4x4f& parentTrans) override;
	void update(int deltaTime) override;

	void setTotalTime(int t){totalTime = t;}
	void setCurrTime(int t){ currTime = t;}


	void reset() { totalTime = -1; currTime = -1; }

private:
	NinePatchComponent* mFrame;
	TextComponent*		mLabel;

	int totalTime;
	int currTime;

	int mVolume;

	int mCheckTime;
	int mDisplayTime;
};