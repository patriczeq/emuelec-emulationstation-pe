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

	void reset() { mVolume = -1; }
	void show(){ showIt = true; }

private:
	NinePatchComponent* mFrame;
	TextComponent*		mLabel;

	int totalTime;
	int currTime;

	bool showIt;

	int mVolume;

	int mCheckTime;
	int mDisplayTime;
};