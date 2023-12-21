#pragma once

#include <mutex>
#include "GuiComponent.h"

class ComponentGrid;
class NinePatchComponent;
class TextComponent;
class Window;

class PWRComponent : public GuiComponent
{
public:
	PWRComponent(Window* window);
	~PWRComponent();

	void render(const Transform4x4f& parentTrans) override;
	void update(int deltaTime) override;

  void reset() { totalTime = -1; currTime = -1; }


private:
	NinePatchComponent* mFrame;
	TextComponent*		mPercent;

	int percent;
	bool plugged;

	int mCheckTime;
	int mDisplayTime;
};
