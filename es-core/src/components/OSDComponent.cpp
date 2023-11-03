#include "OSDComponent.h"
#include "ThemeData.h"
#include "PowerSaver.h"
#include "components/ComponentGrid.h"
#include "components/NinePatchComponent.h"
#include "components/TextComponent.h"
#include "LocaleES.h"
#include "Window.h"
#include "AudioManager.h"
#include "Log.h"

#define PADDING_PX			(Renderer::getScreenWidth()*0.006)
#define PADDING_BAR			(Renderer::isSmallScreen() ? Renderer::getScreenWidth()*0.02 : Renderer::getScreenWidth()*0.006)

#define VISIBLE_TIME		4650
#define FADE_TIME			350

#define BASEOPACITY			200
#define CHECKVOLUMEDELAY	40

OSDComponent::OSDComponent(Window* window)
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


	mLabelCurr = new TextComponent(mWindow, "", font, theme->Text.color, ALIGN_LEFT);
	mLabelTotal = new TextComponent(mWindow, "", font, theme->Text.color, ALIGN_RIGHT);
	
	int h = font->sizeText("100%").y();

	mLabelCurr->setPosition(16, 0);
	mLabelCurr->setSize((Renderer::getScreenWidth() / 2), h/2);
	mLabelCurr->setText("--:--");
	addChild(mLabelCurr);

	mLabelTotal->setPosition((Renderer::getScreenWidth() / 2) - 16, 0);
	mLabelTotal->setSize((Renderer::getScreenWidth() / 2), h/2);
	mLabelTotal->setText("--:--");
	addChild(mLabelTotal);


	// FCA TopLeft
	//float posX = Renderer::getScreenWidth() * 0.02f;
	float posY = Renderer::getScreenHeight() * 0.875f;

	setPosition(0, posY, 0);
	setOpacity(BASEOPACITY);
}

OSDComponent::~OSDComponent()
{
	delete mLabelCurr;
	delete mLabelTotal;
	delete mFrame;
}

void OSDComponent::update(int deltaTime)
{
	GuiComponent::update(deltaTime);

	if(!AudioManager::getInstance()->getVideoMoviePlaying())
	{
		setVisible(false);
		return;
	}

	int _show = AudioManager::getInstance()->VideoShowOSD();

	if (mDisplayTime >= 0)
	{
		mDisplayTime += deltaTime;
		if (mDisplayTime > VISIBLE_TIME + FADE_TIME)
		{
			mDisplayTime = -1;

			if (isVisible())
			{
				setVisible(false);
				PowerSaver::resume();
			}
		}
	}

	mCheckTime += deltaTime;
	if (mCheckTime < CHECKVOLUMEDELAY)
		return;

	mCheckTime = 0;

	currTime 	= AudioManager::getInstance()->VideoGetCurrTime();
	totalTime 	= AudioManager::getInstance()->VideoGetTotalTime();
	paused		= AudioManager::getInstance()->VideoGetPaused();
	mLabelCurr->setText(formatMStoTime(currTime) + " / " + formatMStoTime(totalTime));
	mLabelTotal->setText("-" + formatMStoTime(totalTime - currTime));

	if((showme != _show && _show > 0) || paused)
	{
		showme = _show;
		LOG(LogDebug) << "OSD::show ";
		mDisplayTime = 0;

		if (!isVisible())
		{
			setVisible(true);
			PowerSaver::pause();
		}
	}


}
std::string OSDComponent::dbNum(int num)
{
	if(num < 10){
		return "0" + std::to_string(num);
	}
	return std::to_string(num);
}
std::string OSDComponent::formatMStoTime(int ms)
{
	int seconds = ms / 1000;
	ms %= 1000;

	int minutes = seconds / 60;
	seconds %= 60;

	int hours = minutes / 60;
	minutes %= 60;

	return (hours > 0 ? (dbNum(hours) + ":") : "") + dbNum(minutes) + ":" + dbNum(seconds);
}
void OSDComponent::render(const Transform4x4f& parentTrans)
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
	float r = 8;
	
	auto theme = ThemeData::getMenuTheme();
	// progressbar
	//void drawRoundRect(float x, float y, float width, float height, float radius, unsigned int color, const Blend::Factor _srcBlendFactor, const Blend::Factor _dstBlendFactor)
	//void drawRect(const float _x, const float _y, const float _w, const float _h, const unsigned int _color, const Blend::Factor _srcBlendFactor, const Blend::Factor _dstBlendFactor)
	Renderer::drawRect(x, y, w, h, r, (theme->Text.color & 0xFFFFFF00) | (opacity / 2));
	float px = w * (float(currTime) / float(totalTime));
	Renderer::drawRect(x, y, px, h, r, (theme->TextSmall.color & 0xFFFFFF00) | opacity);
	// pause sign
	if(paused)
	{
		float pauseSpace = 2;
		float pauseW = 6;
		float pauseH = 20;
		float pauseCenter = Renderer::getScreenWidth()/2;
		float pauseY = 24;
		Renderer::drawRect(pauseCenter - pauseSpace - pauseW, pauseY, pauseW, pauseH, (theme->TextSmall.color & 0xFFFFFF00) | opacity);
		Renderer::drawRect(pauseCenter + pauseSpace, pauseY, pauseW, pauseH, (theme->TextSmall.color & 0xFFFFFF00) | opacity);
	}
	

}
