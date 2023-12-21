#include "PWRComponent.h"
#include "ThemeData.h"
#include "PowerSaver.h"
#include "components/ComponentGrid.h"
#include "components/NinePatchComponent.h"
#include "components/TextComponent.h"
#include "LocaleES.h"
#include "Window.h"
//#include "ApiSystem.h"
#include "Log.h"

#define PADDING_PX			(Renderer::getScreenWidth()*0.006)
#define PADDING_BAR			(Renderer::isSmallScreen() ? Renderer::getScreenWidth()*0.02 : Renderer::getScreenWidth()*0.006)

#define VISIBLE_TIME		4650
#define FADE_TIME			350

#define BASEOPACITY			200
#define CHECKVOLUMEDELAY	40

PWRComponent::PWRComponent(Window* window)
	: GuiComponent(window)
{
	mDisplayTime = -1;
	totalTime = -1;
	currTime = -1;
	mCheckTime = 0;
	showme = -1;

	auto theme = ThemeData::getMenuTheme();

	auto font = theme->TextSmall.font;
	if (Renderer::isSmallScreen())
		font = theme->Text.font;

	Vector2f fullSize(
		2 * PADDING_PX + font->sizeText("100%").x(),
		2 * PADDING_PX + Renderer::getScreenHeight() * 0.20f);

	fullSize.y() = fullSize.x() * 2.5f;

	setSize(Renderer::getScreenWidth(), Renderer::getScreenHeight() / 8);

	mFrame = new NinePatchComponent(window);
	mFrame->setImagePath(theme->Background.path);
	mFrame->setEdgeColor(theme->Background.color);
	mFrame->setCenterColor(theme->Background.centerColor);
	mFrame->setCornerSize(theme->Background.cornerSize);
	mFrame->fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));
	addChild(mFrame);


	mPercent = new TextComponent(mWindow, "", font, theme->Text.color, ALIGN_LEFT);


	int h = font->sizeText("100%").y();

	mPercent->setPosition(16, 0);
	mPercent->setSize((Renderer::getScreenWidth() / 2), h/2);
	mPercent->setText("???%");
	addChild(mPercent);

	// FCA TopLeft
	//float posX = Renderer::getScreenWidth() * 0.02f;
	float posY = Renderer::getScreenHeight() * 0.875f;

	setPosition(0, posY, 0);
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


	bool CurrStatus = getShOutput("cat /sys/class/power_supply/ac/online") == "1";;

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
	if (mCheckTime < CHECKVOLUMEDELAY)
		return;

	mCheckTime = 0;

	if(plugged != CurrStatus && CurrStatus)
	{
		plugged = CurrStatus;
		mDisplayTime = 0;

		if (!isVisible())
		{
			setVisible(true);
			//PowerSaver::pause();
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

	float x = 16;
	float y = 20;
	float w = Renderer::getScreenWidth() - 32;
	float h = 12;

	auto theme = ThemeData::getMenuTheme();



}
