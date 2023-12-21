#include "PWRComponent.h"
#include "ThemeData.h"
#include "PowerSaver.h"
#include "components/ComponentGrid.h"
#include "components/NinePatchComponent.h"
#include "components/TextComponent.h"
#include "LocaleES.h"
#include "Window.h"
#include "platform.h"
#include "Log.h"

#define PADDING_PX			(Renderer::getScreenWidth()*0.006)
#define PADDING_BAR			(Renderer::isSmallScreen() ? Renderer::getScreenWidth()*0.02 : Renderer::getScreenWidth()*0.006)

#define VISIBLE_TIME		4650
#define FADE_TIME			350

#define BASEOPACITY			200
#define CHECKPWRDELAY	500

PWRComponent::PWRComponent(Window* window)
	: GuiComponent(window)
{
	mDisplayTime = -1;
	percent = 0;
	mCheckTime = 0;
	plugged = false;

	auto theme = ThemeData::getMenuTheme();

	auto font = theme->TextSmall.font;
	if (Renderer::isSmallScreen())
		font = theme->Text.font;

	Vector2f fullSize(
		2 * PADDING_PX + font->sizeText("100%").x(),
		2 * PADDING_PX + Renderer::getScreenHeight() * 0.20f);

	fullSize.y() = fullSize.x() * 2.5f;

	setSize(32, 32);

	mFrame = new NinePatchComponent(window);
	mFrame->setImagePath(theme->Background.path);
	mFrame->setEdgeColor(theme->Background.color);
	mFrame->setCenterColor(theme->Background.centerColor);
	mFrame->setCornerSize(theme->Background.cornerSize);
	mFrame->fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));
	addChild(mFrame);

	// FCA TopLeft
	//float posX = Renderer::getScreenWidth() * 0.02f;
	//float posY = Renderer::getScreenHeight() * 0.875f;

	setPosition(Renderer::getScreenWidth() - 36, 4, 0);
	setOpacity(BASEOPACITY);
}

PWRComponent::~PWRComponent()
{
	delete mPercent;
	delete mFrame;
}

void PWRComponent::update(int deltaTime)
{
	GuiComponent::update(deltaTime);


	if (mDisplayTime >= 0)
	{
		mDisplayTime += deltaTime;
		if (mDisplayTime > VISIBLE_TIME + FADE_TIME)
		{
			mDisplayTime = -1;

			if (isVisible())
			{
				setVisible(false);
				//PowerSaver::resume();
			}
		}
	}

	mCheckTime += deltaTime;
	if (mCheckTime < CHECKPWRDELAY)
		return;

	mCheckTime = 0;

  bool CurrStatus = getShOutput("cat /sys/class/power_supply/ac/online") == "1";
	if(plugged != CurrStatus)
	{
		plugged = CurrStatus;
		mDisplayTime = 0;

		if (!isVisible() && plugged)
		{
			setVisible(true);

		}
	}


}

void PWRComponent::render(const Transform4x4f& parentTrans)
{
	if (!mVisible || mDisplayTime < 0)
		return;

	int opacity = BASEOPACITY - Math::max(0, (mDisplayTime - VISIBLE_TIME) * BASEOPACITY / FADE_TIME);
	setOpacity(opacity);

	GuiComponent::render(parentTrans);

	Transform4x4f trans = parentTrans * getTransform();
	Renderer::setMatrix(trans);


	auto theme = ThemeData::getMenuTheme();
  // battery body
  Renderer::drawRect(4, 4, 24, 16, (theme->Text.color & 0xFFFFFF00) | (opacity / 2));
  // battery spicka
  Renderer::drawRect(28, 8, 4, 8, (theme->Text.color & 0xFFFFFF00) | (opacity / 2));
  //battery inner
	Renderer::drawRect(6, 6, 20, 12, (theme->TextSmall.color & 0x00FF0000) | opacity);

}
