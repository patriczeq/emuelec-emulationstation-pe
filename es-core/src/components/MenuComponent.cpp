#include "components/MenuComponent.h"
#include "components/ButtonComponent.h"
#include "components/MultiLineMenuEntry.h"
#include "TextToSpeech.h"

#define BUTTON_GRID_VERT_PADDING  (Renderer::getScreenHeight()*0.0296296)
#define BUTTON_GRID_HORIZ_PADDING (Renderer::getScreenWidth()*0.0052083333)

#define TITLE_HEIGHT (mTitle->getFont()->getLetterHeight() + (mSubtitle ? TITLE_WITHSUB_VERT_PADDING : TITLE_VERT_PADDING) + (mSubtitle ? mSubtitle->getSize().y() + SUBTITLE_VERT_PADDING : 0))

MenuComponent::MenuComponent(Window* window,
	const std::string& title, const std::shared_ptr<Font>& titleFont,
	const std::string& subTitle)
	: GuiComponent(window),
	mBackground(window), mGrid(window, Vector2i(1, 3))
{
	mMaxHeight = 0;

	auto theme = ThemeData::getMenuTheme();

	addChild(&mBackground);
	addChild(&mGrid);

	mGrid.setZIndex(10);

	mBackground.setImagePath(theme->Background.path);
	mBackground.setEdgeColor(theme->Background.color);
	mBackground.setCenterColor(theme->Background.centerColor);
	mBackground.setCornerSize(theme->Background.cornerSize);
	mBackground.setZIndex(2);

	// set up title
	mTitle = std::make_shared<TextComponent>(mWindow);
	mTitle->setHorizontalAlignment(ALIGN_CENTER);
	mTitle->setColor(theme->Title.color); // 0x555555FF

	if (theme->Title.selectorColor != 0x555555FF)
	{
		mTitle->setBackgroundColor(theme->Title.selectorColor);
		mTitle->setRenderBackground(true);
	}

	mHeaderGrid = std::make_shared<ComponentGrid>(mWindow, Vector2i(2, 2));
	mHeaderGrid->setColWidthPerc(0, 1);
	mHeaderGrid->setColWidthPerc(1, 0);

	mHeaderGrid->setEntry(mTitle, Vector2i(0, 0), false, true);

	setTitle(title, theme->Title.font); //  titleFont
	setSubTitle(subTitle);

	mGrid.setEntry(mHeaderGrid, Vector2i(0, 0), false, true);
	//mGrid.setEntry(mTitle, Vector2i(0, 0), false);

	mGrid.setUnhandledInputCallback([this](InputConfig* config, Input input) -> bool {
		if (config->isMappedLike("down", input)) {
			mGrid.setCursorTo(mList);
			mList->setCursorIndex(0);
			return true;
		}
		if (config->isMappedLike("up", input)) {
			mList->setCursorIndex(mList->size() - 1);
			if (mButtons.size()) {
				mGrid.moveCursor(Vector2i(0, 1));
			}
			else {
				mGrid.setCursorTo(mList);
			}
			return true;
		}

		if(callback_X != nullptr && config->isMappedLike("x", input))
			{
				callback_X();
				return true;
			}

		if(callback_Y != nullptr && config->isMappedLike("y", input))
			{
				callback_Y();
				return true;
			}

		return false;
	});

	// set up list which will never change (externally, anyway)
	mList = std::make_shared<ComponentList>(mWindow);
	mGrid.setEntry(mList, Vector2i(0, 1), true);

	updateGrid();
	updateSize();

	mGrid.resetCursor();
}


void MenuComponent::addMenuIcon(Window* window, ComponentListRow& row, const std::string& iconName)
{
	if (iconName.empty())
		return;

	auto theme = ThemeData::getMenuTheme();
	/*std::string iconPath;
	std::vector<std::string> faSplit = Utils::String::split(iconName, '-');
	if(faSplit.size() > 1 && faSplit.at(0) == "fa")
		{
			iconPath = ":/fa-svg/" + Utils::String::replace(iconName, "fa-", "") + ".svg";
		}
	else
		{
			iconPath = theme->getMenuIcon(iconName);
		}*/

	std::string iconPath = theme->getMenuIcon(iconName);
	if (!iconPath.empty())
	{
		// icon
		auto icon = std::make_shared<ImageComponent>(window, true);
		icon->setImage(iconPath);
		icon->setColorShift(theme->Text.color);
		icon->setResize(0, theme->Text.font->getLetterHeight() * 1.25f);
		row.addElement(icon, false);

		// spacer between icon and text
		auto spacer = std::make_shared<GuiComponent>(window);
		spacer->setSize(10, 0);
		row.addElement(spacer, false);

		return;
	}

	std::string label;
	if (iconName == "audio")
		label = _U("\uf028");
	else if (iconName == "keyboard")
		label = _U("\uf11c");
	else if (iconName == "joystick")
		label = _U("\uf11b");
	else if (iconName == "mouse")
		label = _U("\uf124");
	else if (iconName == "unknown")
		label = _U("\uf1de");

		else if(iconName == "fa-0"){
	  label = _U("\u0030"); }

	else if(iconName == "fa-1"){
	  label = _U("\u0031"); }

	else if(iconName == "fa-2"){
	  label = _U("\u0032"); }

	else if(iconName == "fa-3"){
	  label = _U("\u0033"); }

	else if(iconName == "fa-4"){
	  label = _U("\u0034"); }

	else if(iconName == "fa-5"){
	  label = _U("\u0035"); }

	else if(iconName == "fa-6"){
	  label = _U("\u0036"); }

	else if(iconName == "fa-7"){
	  label = _U("\u0037"); }

	else if(iconName == "fa-8"){
	  label = _U("\u0038"); }

	else if(iconName == "fa-9"){
	  label = _U("\u0039"); }

	else if(iconName == "fa-fill-drip"){
	  label = _U("\uf576"); }

	else if(iconName == "fa-arrows-to-circle"){
	  label = _U("\ue4bd"); }

	else if(iconName == "fa-circle-chevron-right"){
	  label = _U("\uf138"); }

	else if(iconName == "fa-chevron-circle-right"){
	  label = _U("\uf138"); }

	else if(iconName == "fa-wagon-covered"){
	  label = _U("\uf8ee"); }

	else if(iconName == "fa-line-height"){
	  label = _U("\uf871"); }

	else if(iconName == "fa-bagel"){
	  label = _U("\ue3d7"); }

	else if(iconName == "fa-transporter-7"){
	  label = _U("\ue2a8"); }

	else if(iconName == "fa-at"){
	  label = _U("\u0040"); }

	else if(iconName == "fa-rectangles-mixed"){
	  label = _U("\ue323"); }

	else if(iconName == "fa-phone-arrow-up-right"){
	  label = _U("\ue224"); }

	else if(iconName == "fa-phone-arrow-up"){
	  label = _U("\ue224"); }

	else if(iconName == "fa-phone-outgoing"){
	  label = _U("\ue224"); }

	else if(iconName == "fa-trash-can"){
	  label = _U("\uf2ed"); }

	else if(iconName == "fa-trash-alt"){
	  label = _U("\uf2ed"); }

	else if(iconName == "fa-circle-l"){
	  label = _U("\ue114"); }

	else if(iconName == "fa-head-side-goggles"){
	  label = _U("\uf6ea"); }

	else if(iconName == "fa-head-vr"){
	  label = _U("\uf6ea"); }

	else if(iconName == "fa-text-height"){
	  label = _U("\uf034"); }

	else if(iconName == "fa-user-xmark"){
	  label = _U("\uf235"); }

	else if(iconName == "fa-user-times"){
	  label = _U("\uf235"); }

	else if(iconName == "fa-face-hand-yawn"){
	  label = _U("\ue379"); }

	else if(iconName == "fa-gauge-simple-min"){
	  label = _U("\uf62d"); }

	else if(iconName == "fa-tachometer-slowest"){
	  label = _U("\uf62d"); }

	else if(iconName == "fa-stethoscope"){
	  label = _U("\uf0f1"); }

	else if(iconName == "fa-coffin"){
	  label = _U("\uf6c6"); }

	else if(iconName == "fa-message"){
	  label = _U("\uf27a"); }

	else if(iconName == "fa-comment-alt"){
	  label = _U("\uf27a"); }

	else if(iconName == "fa-salad"){
	  label = _U("\uf81e"); }

	else if(iconName == "fa-bowl-salad"){
	  label = _U("\uf81e"); }

	else if(iconName == "fa-info"){
	  label = _U("\uf129"); }

	else if(iconName == "fa-robot-astromech"){
	  label = _U("\ue2d2"); }

	else if(iconName == "fa-ring-diamond"){
	  label = _U("\ue5ab"); }

	else if(iconName == "fa-fondue-pot"){
	  label = _U("\ue40d"); }

	else if(iconName == "fa-theta"){
	  label = _U("\uf69e"); }

	else if(iconName == "fa-face-hand-peeking"){
	  label = _U("\ue481"); }

	else if(iconName == "fa-square-user"){
	  label = _U("\ue283"); }

	else if(iconName == "fa-down-left-and-up-right-to-center"){
	  label = _U("\uf422"); }

	else if(iconName == "fa-compress-alt"){
	  label = _U("\uf422"); }

	else if(iconName == "fa-explosion"){
	  label = _U("\ue4e9"); }

	else if(iconName == "fa-file-lines"){
	  label = _U("\uf15c"); }

	else if(iconName == "fa-file-alt"){
	  label = _U("\uf15c"); }

	else if(iconName == "fa-file-text"){
	  label = _U("\uf15c"); }

	else if(iconName == "fa-wave-square"){
	  label = _U("\uf83e"); }

	else if(iconName == "fa-ring"){
	  label = _U("\uf70b"); }

	else if(iconName == "fa-building-un"){
	  label = _U("\ue4d9"); }

	else if(iconName == "fa-dice-three"){
	  label = _U("\uf527"); }

	else if(iconName == "fa-tire-pressure-warning"){
	  label = _U("\uf633"); }

	else if(iconName == "fa-wifi-fair"){
	  label = _U("\uf6ab"); }

	else if(iconName == "fa-wifi-2"){
	  label = _U("\uf6ab"); }

	else if(iconName == "fa-calendar-days"){
	  label = _U("\uf073"); }

	else if(iconName == "fa-calendar-alt"){
	  label = _U("\uf073"); }

	else if(iconName == "fa-mp3-player"){
	  label = _U("\uf8ce"); }

	else if(iconName == "fa-anchor-circle-check"){
	  label = _U("\ue4aa"); }

	else if(iconName == "fa-tally-4"){
	  label = _U("\ue297"); }

	else if(iconName == "fa-rectangle-history"){
	  label = _U("\ue4a2"); }

	else if(iconName == "fa-building-circle-arrow-right"){
	  label = _U("\ue4d1"); }

	else if(iconName == "fa-volleyball"){
	  label = _U("\uf45f"); }

	else if(iconName == "fa-volleyball-ball"){
	  label = _U("\uf45f"); }

	else if(iconName == "fa-sun-haze"){
	  label = _U("\uf765"); }

	else if(iconName == "fa-text-size"){
	  label = _U("\uf894"); }

	else if(iconName == "fa-ufo"){
	  label = _U("\ue047"); }

	else if(iconName == "fa-fork"){
	  label = _U("\uf2e3"); }

	else if(iconName == "fa-utensil-fork"){
	  label = _U("\uf2e3"); }

	else if(iconName == "fa-arrows-up-to-line"){
	  label = _U("\ue4c2"); }

	else if(iconName == "fa-mobile-signal"){
	  label = _U("\ue1ef"); }

	else if(iconName == "fa-barcode-scan"){
	  label = _U("\uf465"); }

	else if(iconName == "fa-sort-down"){
	  label = _U("\uf0dd"); }

	else if(iconName == "fa-sort-desc"){
	  label = _U("\uf0dd"); }

	else if(iconName == "fa-folder-arrow-down"){
	  label = _U("\ue053"); }

	else if(iconName == "fa-folder-download"){
	  label = _U("\ue053"); }

	else if(iconName == "fa-circle-minus"){
	  label = _U("\uf056"); }

	else if(iconName == "fa-minus-circle"){
	  label = _U("\uf056"); }

	else if(iconName == "fa-face-icicles"){
	  label = _U("\ue37c"); }

	else if(iconName == "fa-shovel"){
	  label = _U("\uf713"); }

	else if(iconName == "fa-door-open"){
	  label = _U("\uf52b"); }

	else if(iconName == "fa-films"){
	  label = _U("\ue17a"); }

	else if(iconName == "fa-right-from-bracket"){
	  label = _U("\uf2f5"); }

	else if(iconName == "fa-sign-out-alt"){
	  label = _U("\uf2f5"); }

	else if(iconName == "fa-face-glasses"){
	  label = _U("\ue377"); }

	else if(iconName == "fa-nfc"){
	  label = _U("\ue1f7"); }

	else if(iconName == "fa-atom"){
	  label = _U("\uf5d2"); }

	else if(iconName == "fa-soap"){
	  label = _U("\ue06e"); }

	else if(iconName == "fa-icons"){
	  label = _U("\uf86d"); }

	else if(iconName == "fa-heart-music-camera-bolt"){
	  label = _U("\uf86d"); }

	else if(iconName == "fa-microphone-lines-slash"){
	  label = _U("\uf539"); }

	else if(iconName == "fa-microphone-alt-slash"){
	  label = _U("\uf539"); }

	else if(iconName == "fa-closed-captioning-slash"){
	  label = _U("\ue135"); }

	else if(iconName == "fa-calculator-simple"){
	  label = _U("\uf64c"); }

	else if(iconName == "fa-calculator-alt"){
	  label = _U("\uf64c"); }

	else if(iconName == "fa-bridge-circle-check"){
	  label = _U("\ue4c9"); }

	else if(iconName == "fa-sliders-up"){
	  label = _U("\uf3f1"); }

	else if(iconName == "fa-sliders-v"){
	  label = _U("\uf3f1"); }

	else if(iconName == "fa-location-minus"){
	  label = _U("\uf609"); }

	else if(iconName == "fa-map-marker-minus"){
	  label = _U("\uf609"); }

	else if(iconName == "fa-pump-medical"){
	  label = _U("\ue06a"); }

	else if(iconName == "fa-fingerprint"){
	  label = _U("\uf577"); }

	else if(iconName == "fa-ski-boot"){
	  label = _U("\ue3cc"); }

	else if(iconName == "fa-standard-definition"){
	  label = _U("\ue28a"); }

	else if(iconName == "fa-rectangle-sd"){
	  label = _U("\ue28a"); }

	else if(iconName == "fa-h1"){
	  label = _U("\uf313"); }

	else if(iconName == "fa-hand-point-right"){
	  label = _U("\uf0a4"); }

	else if(iconName == "fa-magnifying-glass-location"){
	  label = _U("\uf689"); }

	else if(iconName == "fa-search-location"){
	  label = _U("\uf689"); }

	else if(iconName == "fa-message-bot"){
	  label = _U("\ue3b8"); }

	else if(iconName == "fa-forward-step"){
	  label = _U("\uf051"); }

	else if(iconName == "fa-step-forward"){
	  label = _U("\uf051"); }

	else if(iconName == "fa-face-smile-beam"){
	  label = _U("\uf5b8"); }

	else if(iconName == "fa-smile-beam"){
	  label = _U("\uf5b8"); }

	else if(iconName == "fa-light-ceiling"){
	  label = _U("\ue016"); }

	else if(iconName == "fa-message-exclamation"){
	  label = _U("\uf4a5"); }

	else if(iconName == "fa-comment-alt-exclamation"){
	  label = _U("\uf4a5"); }

	else if(iconName == "fa-bowl-scoop"){
	  label = _U("\ue3de"); }

	else if(iconName == "fa-bowl-shaved-ice"){
	  label = _U("\ue3de"); }

	else if(iconName == "fa-square-x"){
	  label = _U("\ue286"); }

	else if(iconName == "fa-building-memo"){
	  label = _U("\ue61e"); }

	else if(iconName == "fa-utility-pole-double"){
	  label = _U("\ue2c4"); }

	else if(iconName == "fa-flag-checkered"){
	  label = _U("\uf11e"); }

	else if(iconName == "fa-chevrons-up"){
	  label = _U("\uf325"); }

	else if(iconName == "fa-chevron-double-up"){
	  label = _U("\uf325"); }

	else if(iconName == "fa-football"){
	  label = _U("\uf44e"); }

	else if(iconName == "fa-football-ball"){
	  label = _U("\uf44e"); }

	else if(iconName == "fa-user-vneck"){
	  label = _U("\ue461"); }

	else if(iconName == "fa-school-circle-exclamation"){
	  label = _U("\ue56c"); }

	else if(iconName == "fa-crop"){
	  label = _U("\uf125"); }

	else if(iconName == "fa-angles-down"){
	  label = _U("\uf103"); }

	else if(iconName == "fa-angle-double-down"){
	  label = _U("\uf103"); }

	else if(iconName == "fa-users-rectangle"){
	  label = _U("\ue594"); }

	else if(iconName == "fa-people-roof"){
	  label = _U("\ue537"); }

	else if(iconName == "fa-square-arrow-right"){
	  label = _U("\uf33b"); }

	else if(iconName == "fa-arrow-square-right"){
	  label = _U("\uf33b"); }

	else if(iconName == "fa-location-plus"){
	  label = _U("\uf60a"); }

	else if(iconName == "fa-map-marker-plus"){
	  label = _U("\uf60a"); }

	else if(iconName == "fa-lightbulb-exclamation-on"){
	  label = _U("\ue1ca"); }

	else if(iconName == "fa-people-line"){
	  label = _U("\ue534"); }

	else if(iconName == "fa-beer-mug-empty"){
	  label = _U("\uf0fc"); }

	else if(iconName == "fa-beer"){
	  label = _U("\uf0fc"); }

	else if(iconName == "fa-crate-empty"){
	  label = _U("\ue151"); }

	else if(iconName == "fa-diagram-predecessor"){
	  label = _U("\ue477"); }

	else if(iconName == "fa-transporter"){
	  label = _U("\ue042"); }

	else if(iconName == "fa-calendar-circle-user"){
	  label = _U("\ue471"); }

	else if(iconName == "fa-arrow-up-long"){
	  label = _U("\uf176"); }

	else if(iconName == "fa-long-arrow-up"){
	  label = _U("\uf176"); }

	else if(iconName == "fa-person-carry-box"){
	  label = _U("\uf4cf"); }

	else if(iconName == "fa-person-carry"){
	  label = _U("\uf4cf"); }

	else if(iconName == "fa-fire-flame-simple"){
	  label = _U("\uf46a"); }

	else if(iconName == "fa-burn"){
	  label = _U("\uf46a"); }

	else if(iconName == "fa-person"){
	  label = _U("\uf183"); }

	else if(iconName == "fa-male"){
	  label = _U("\uf183"); }

	else if(iconName == "fa-laptop"){
	  label = _U("\uf109"); }

	else if(iconName == "fa-file-csv"){
	  label = _U("\uf6dd"); }

	else if(iconName == "fa-menorah"){
	  label = _U("\uf676"); }

	else if(iconName == "fa-union"){
	  label = _U("\uf6a2"); }

	else if(iconName == "fa-chevrons-left"){
	  label = _U("\uf323"); }

	else if(iconName == "fa-chevron-double-left"){
	  label = _U("\uf323"); }

	else if(iconName == "fa-circle-heart"){
	  label = _U("\uf4c7"); }

	else if(iconName == "fa-heart-circle"){
	  label = _U("\uf4c7"); }

	else if(iconName == "fa-truck-plane"){
	  label = _U("\ue58f"); }

	else if(iconName == "fa-record-vinyl"){
	  label = _U("\uf8d9"); }

	else if(iconName == "fa-bring-forward"){
	  label = _U("\uf856"); }

	else if(iconName == "fa-square-p"){
	  label = _U("\ue279"); }

	else if(iconName == "fa-face-grin-stars"){
	  label = _U("\uf587"); }

	else if(iconName == "fa-grin-stars"){
	  label = _U("\uf587"); }

	else if(iconName == "fa-sigma"){
	  label = _U("\uf68b"); }

	else if(iconName == "fa-camera-movie"){
	  label = _U("\uf8a9"); }

	else if(iconName == "fa-bong"){
	  label = _U("\uf55c"); }

	else if(iconName == "fa-clarinet"){
	  label = _U("\uf8ad"); }

	else if(iconName == "fa-truck-flatbed"){
	  label = _U("\ue2b6"); }

	else if(iconName == "fa-spaghetti-monster-flying"){
	  label = _U("\uf67b"); }

	else if(iconName == "fa-pastafarianism"){
	  label = _U("\uf67b"); }

	else if(iconName == "fa-arrow-down-up-across-line"){
	  label = _U("\ue4af"); }

	else if(iconName == "fa-arrows-rotate-reverse"){
	  label = _U("\ue630"); }

	else if(iconName == "fa-leaf-heart"){
	  label = _U("\uf4cb"); }

	else if(iconName == "fa-house-building"){
	  label = _U("\ue1b1"); }

	else if(iconName == "fa-cheese-swiss"){
	  label = _U("\uf7f0"); }

	else if(iconName == "fa-spoon"){
	  label = _U("\uf2e5"); }

	else if(iconName == "fa-utensil-spoon"){
	  label = _U("\uf2e5"); }

	else if(iconName == "fa-jar-wheat"){
	  label = _U("\ue517"); }

	else if(iconName == "fa-envelopes-bulk"){
	  label = _U("\uf674"); }

	else if(iconName == "fa-mail-bulk"){
	  label = _U("\uf674"); }

	else if(iconName == "fa-file-circle-exclamation"){
	  label = _U("\ue4eb"); }

	else if(iconName == "fa-bow-arrow"){
	  label = _U("\uf6b9"); }

	else if(iconName == "fa-cart-xmark"){
	  label = _U("\ue0dd"); }

	else if(iconName == "fa-hexagon-xmark"){
	  label = _U("\uf2ee"); }

	else if(iconName == "fa-times-hexagon"){
	  label = _U("\uf2ee"); }

	else if(iconName == "fa-xmark-hexagon"){
	  label = _U("\uf2ee"); }

	else if(iconName == "fa-circle-h"){
	  label = _U("\uf47e"); }

	else if(iconName == "fa-hospital-symbol"){
	  label = _U("\uf47e"); }

	else if(iconName == "fa-merge"){
	  label = _U("\ue526"); }

	else if(iconName == "fa-pager"){
	  label = _U("\uf815"); }

	else if(iconName == "fa-cart-minus"){
	  label = _U("\ue0db"); }

	else if(iconName == "fa-address-book"){
	  label = _U("\uf2b9"); }

	else if(iconName == "fa-contact-book"){
	  label = _U("\uf2b9"); }

	else if(iconName == "fa-pan-frying"){
	  label = _U("\ue42c"); }

	else if(iconName == "fa-grid"){
	  label = _U("\ue195"); }

	else if(iconName == "fa-grid-3"){
	  label = _U("\ue195"); }

	else if(iconName == "fa-football-helmet"){
	  label = _U("\uf44f"); }

	else if(iconName == "fa-hand-love"){
	  label = _U("\ue1a5"); }

	else if(iconName == "fa-trees"){
	  label = _U("\uf724"); }

	else if(iconName == "fa-strikethrough"){
	  label = _U("\uf0cc"); }

	else if(iconName == "fa-page"){
	  label = _U("\ue428"); }

	else if(iconName == "fa-k"){
	  label = _U("\u004b"); }

	else if(iconName == "fa-diagram-previous"){
	  label = _U("\ue478"); }

	else if(iconName == "fa-gauge-min"){
	  label = _U("\uf628"); }

	else if(iconName == "fa-tachometer-alt-slowest"){
	  label = _U("\uf628"); }

	else if(iconName == "fa-folder-grid"){
	  label = _U("\ue188"); }

	else if(iconName == "fa-eggplant"){
	  label = _U("\ue16c"); }

	else if(iconName == "fa-excavator"){
	  label = _U("\ue656"); }

	else if(iconName == "fa-ram"){
	  label = _U("\uf70a"); }

	else if(iconName == "fa-landmark-flag"){
	  label = _U("\ue51c"); }

	else if(iconName == "fa-lips"){
	  label = _U("\uf600"); }

	else if(iconName == "fa-pencil"){
	  label = _U("\uf303"); }

	else if(iconName == "fa-pencil-alt"){
	  label = _U("\uf303"); }

	else if(iconName == "fa-backward"){
	  label = _U("\uf04a"); }

	else if(iconName == "fa-caret-right"){
	  label = _U("\uf0da"); }

	else if(iconName == "fa-comments"){
	  label = _U("\uf086"); }

	else if(iconName == "fa-paste"){
	  label = _U("\uf0ea"); }

	else if(iconName == "fa-file-clipboard"){
	  label = _U("\uf0ea"); }

	else if(iconName == "fa-desktop-arrow-down"){
	  label = _U("\ue155"); }

	else if(iconName == "fa-code-pull-request"){
	  label = _U("\ue13c"); }

	else if(iconName == "fa-pumpkin"){
	  label = _U("\uf707"); }

	else if(iconName == "fa-clipboard-list"){
	  label = _U("\uf46d"); }

	else if(iconName == "fa-pen-field"){
	  label = _U("\ue211"); }

	else if(iconName == "fa-blueberries"){
	  label = _U("\ue2e8"); }

	else if(iconName == "fa-truck-ramp-box"){
	  label = _U("\uf4de"); }

	else if(iconName == "fa-truck-loading"){
	  label = _U("\uf4de"); }

	else if(iconName == "fa-note"){
	  label = _U("\ue1ff"); }

	else if(iconName == "fa-arrow-down-to-square"){
	  label = _U("\ue096"); }

	else if(iconName == "fa-user-check"){
	  label = _U("\uf4fc"); }

	else if(iconName == "fa-cloud-xmark"){
	  label = _U("\ue35f"); }

	else if(iconName == "fa-vial-virus"){
	  label = _U("\ue597"); }

	else if(iconName == "fa-book-blank"){
	  label = _U("\uf5d9"); }

	else if(iconName == "fa-book-alt"){
	  label = _U("\uf5d9"); }

	else if(iconName == "fa-golf-flag-hole"){
	  label = _U("\ue3ac"); }

	else if(iconName == "fa-message-arrow-down"){
	  label = _U("\ue1db"); }

	else if(iconName == "fa-comment-alt-arrow-down"){
	  label = _U("\ue1db"); }

	else if(iconName == "fa-face-unamused"){
	  label = _U("\ue39f"); }

	else if(iconName == "fa-sheet-plastic"){
	  label = _U("\ue571"); }

	else if(iconName == "fa-circle-9"){
	  label = _U("\ue0f6"); }

	else if(iconName == "fa-blog"){
	  label = _U("\uf781"); }

	else if(iconName == "fa-user-ninja"){
	  label = _U("\uf504"); }

	else if(iconName == "fa-pencil-slash"){
	  label = _U("\ue215"); }

	else if(iconName == "fa-bowling-pins"){
	  label = _U("\uf437"); }

	else if(iconName == "fa-person-arrow-up-from-line"){
	  label = _U("\ue539"); }

	else if(iconName == "fa-down-right"){
	  label = _U("\ue16b"); }

	else if(iconName == "fa-scroll-torah"){
	  label = _U("\uf6a0"); }

	else if(iconName == "fa-torah"){
	  label = _U("\uf6a0"); }

	else if(iconName == "fa-webhook"){
	  label = _U("\ue5d5"); }

	else if(iconName == "fa-blinds-open"){
	  label = _U("\uf8fc"); }

	else if(iconName == "fa-fence"){
	  label = _U("\ue303"); }

	else if(iconName == "fa-up"){
	  label = _U("\uf357"); }

	else if(iconName == "fa-arrow-alt-up"){
	  label = _U("\uf357"); }

	else if(iconName == "fa-broom-ball"){
	  label = _U("\uf458"); }

	else if(iconName == "fa-quidditch"){
	  label = _U("\uf458"); }

	else if(iconName == "fa-quidditch-broom-ball"){
	  label = _U("\uf458"); }

	else if(iconName == "fa-drumstick"){
	  label = _U("\uf6d6"); }

	else if(iconName == "fa-square-v"){
	  label = _U("\ue284"); }

	else if(iconName == "fa-face-awesome"){
	  label = _U("\ue409"); }

	else if(iconName == "fa-gave-dandy"){
	  label = _U("\ue409"); }

	else if(iconName == "fa-dial-off"){
	  label = _U("\ue162"); }

	else if(iconName == "fa-toggle-off"){
	  label = _U("\uf204"); }

	else if(iconName == "fa-face-smile-horns"){
	  label = _U("\ue391"); }

	else if(iconName == "fa-box-archive"){
	  label = _U("\uf187"); }

	else if(iconName == "fa-archive"){
	  label = _U("\uf187"); }

	else if(iconName == "fa-grapes"){
	  label = _U("\ue306"); }

	else if(iconName == "fa-person-drowning"){
	  label = _U("\ue545"); }

	else if(iconName == "fa-dial-max"){
	  label = _U("\ue15e"); }

	else if(iconName == "fa-circle-m"){
	  label = _U("\ue115"); }

	else if(iconName == "fa-calendar-image"){
	  label = _U("\ue0d4"); }

	else if(iconName == "fa-circle-caret-down"){
	  label = _U("\uf32d"); }

	else if(iconName == "fa-caret-circle-down"){
	  label = _U("\uf32d"); }

	else if(iconName == "fa-arrow-down-9-1"){
	  label = _U("\uf886"); }

	else if(iconName == "fa-sort-numeric-desc"){
	  label = _U("\uf886"); }

	else if(iconName == "fa-sort-numeric-down-alt"){
	  label = _U("\uf886"); }

	else if(iconName == "fa-face-grin-tongue-squint"){
	  label = _U("\uf58a"); }

	else if(iconName == "fa-grin-tongue-squint"){
	  label = _U("\uf58a"); }

	else if(iconName == "fa-shish-kebab"){
	  label = _U("\uf821"); }

	else if(iconName == "fa-spray-can"){
	  label = _U("\uf5bd"); }

	else if(iconName == "fa-alarm-snooze"){
	  label = _U("\uf845"); }

	else if(iconName == "fa-scarecrow"){
	  label = _U("\uf70d"); }

	else if(iconName == "fa-truck-monster"){
	  label = _U("\uf63b"); }

	else if(iconName == "fa-gift-card"){
	  label = _U("\uf663"); }

	else if(iconName == "fa-w"){
	  label = _U("\u0057"); }

	else if(iconName == "fa-code-pull-request-draft"){
	  label = _U("\ue3fa"); }

	else if(iconName == "fa-square-b"){
	  label = _U("\ue264"); }

	else if(iconName == "fa-elephant"){
	  label = _U("\uf6da"); }

	else if(iconName == "fa-earth-africa"){
	  label = _U("\uf57c"); }

	else if(iconName == "fa-globe-africa"){
	  label = _U("\uf57c"); }

	else if(iconName == "fa-rainbow"){
	  label = _U("\uf75b"); }

	else if(iconName == "fa-circle-notch"){
	  label = _U("\uf1ce"); }

	else if(iconName == "fa-tablet-screen-button"){
	  label = _U("\uf3fa"); }

	else if(iconName == "fa-tablet-alt"){
	  label = _U("\uf3fa"); }

	else if(iconName == "fa-paw"){
	  label = _U("\uf1b0"); }

	else if(iconName == "fa-message-question"){
	  label = _U("\ue1e3"); }

	else if(iconName == "fa-cloud"){
	  label = _U("\uf0c2"); }

	else if(iconName == "fa-trowel-bricks"){
	  label = _U("\ue58a"); }

	else if(iconName == "fa-square-3"){
	  label = _U("\ue258"); }

	else if(iconName == "fa-face-flushed"){
	  label = _U("\uf579"); }

	else if(iconName == "fa-flushed"){
	  label = _U("\uf579"); }

	else if(iconName == "fa-hospital-user"){
	  label = _U("\uf80d"); }

	else if(iconName == "fa-microwave"){
	  label = _U("\ue01b"); }

	else if(iconName == "fa-chf-sign"){
	  label = _U("\ue602"); }

	else if(iconName == "fa-tent-arrow-left-right"){
	  label = _U("\ue57f"); }

	else if(iconName == "fa-cart-circle-arrow-up"){
	  label = _U("\ue3f0"); }

	else if(iconName == "fa-trash-clock"){
	  label = _U("\ue2b0"); }

	else if(iconName == "fa-gavel"){
	  label = _U("\uf0e3"); }

	else if(iconName == "fa-legal"){
	  label = _U("\uf0e3"); }

	else if(iconName == "fa-sprinkler-ceiling"){
	  label = _U("\ue44c"); }

	else if(iconName == "fa-browsers"){
	  label = _U("\ue0cb"); }

	else if(iconName == "fa-trillium"){
	  label = _U("\ue588"); }

	else if(iconName == "fa-music-slash"){
	  label = _U("\uf8d1"); }

	else if(iconName == "fa-truck-ramp"){
	  label = _U("\uf4e0"); }

	else if(iconName == "fa-binoculars"){
	  label = _U("\uf1e5"); }

	else if(iconName == "fa-microphone-slash"){
	  label = _U("\uf131"); }

	else if(iconName == "fa-box-tissue"){
	  label = _U("\ue05b"); }

	else if(iconName == "fa-circle-c"){
	  label = _U("\ue101"); }

	else if(iconName == "fa-star-christmas"){
	  label = _U("\uf7d4"); }

	else if(iconName == "fa-chart-bullet"){
	  label = _U("\ue0e1"); }

	else if(iconName == "fa-motorcycle"){
	  label = _U("\uf21c"); }

	else if(iconName == "fa-tree-christmas"){
	  label = _U("\uf7db"); }

	else if(iconName == "fa-tire-flat"){
	  label = _U("\uf632"); }

	else if(iconName == "fa-sunglasses"){
	  label = _U("\uf892"); }

	else if(iconName == "fa-badge"){
	  label = _U("\uf335"); }

	else if(iconName == "fa-message-pen"){
	  label = _U("\uf4a4"); }

	else if(iconName == "fa-comment-alt-edit"){
	  label = _U("\uf4a4"); }

	else if(iconName == "fa-message-edit"){
	  label = _U("\uf4a4"); }

	else if(iconName == "fa-bell-concierge"){
	  label = _U("\uf562"); }

	else if(iconName == "fa-concierge-bell"){
	  label = _U("\uf562"); }

	else if(iconName == "fa-pen-ruler"){
	  label = _U("\uf5ae"); }

	else if(iconName == "fa-pencil-ruler"){
	  label = _U("\uf5ae"); }

	else if(iconName == "fa-file-mp3"){
	  label = _U("\ue648"); }

	else if(iconName == "fa-arrow-progress"){
	  label = _U("\ue5df"); }

	else if(iconName == "fa-chess-rook-piece"){
	  label = _U("\uf448"); }

	else if(iconName == "fa-chess-rook-alt"){
	  label = _U("\uf448"); }

	else if(iconName == "fa-square-root"){
	  label = _U("\uf697"); }

	else if(iconName == "fa-album-collection-circle-plus"){
	  label = _U("\ue48e"); }

	else if(iconName == "fa-people-arrows"){
	  label = _U("\ue068"); }

	else if(iconName == "fa-people-arrows-left-right"){
	  label = _U("\ue068"); }

	else if(iconName == "fa-sign-post"){
	  label = _U("\ue624"); }

	else if(iconName == "fa-face-angry-horns"){
	  label = _U("\ue368"); }

	else if(iconName == "fa-mars-and-venus-burst"){
	  label = _U("\ue523"); }

	else if(iconName == "fa-tombstone"){
	  label = _U("\uf720"); }

	else if(iconName == "fa-square-caret-right"){
	  label = _U("\uf152"); }

	else if(iconName == "fa-caret-square-right"){
	  label = _U("\uf152"); }

	else if(iconName == "fa-scissors"){
	  label = _U("\uf0c4"); }

	else if(iconName == "fa-cut"){
	  label = _U("\uf0c4"); }

	else if(iconName == "fa-list-music"){
	  label = _U("\uf8c9"); }

	else if(iconName == "fa-sun-plant-wilt"){
	  label = _U("\ue57a"); }

	else if(iconName == "fa-toilets-portable"){
	  label = _U("\ue584"); }

	else if(iconName == "fa-hockey-puck"){
	  label = _U("\uf453"); }

	else if(iconName == "fa-mustache"){
	  label = _U("\ue5bc"); }

	else if(iconName == "fa-hyphen"){
	  label = _U("\u002d"); }

	else if(iconName == "fa-table"){
	  label = _U("\uf0ce"); }

	else if(iconName == "fa-user-chef"){
	  label = _U("\ue3d2"); }

	else if(iconName == "fa-message-image"){
	  label = _U("\ue1e0"); }

	else if(iconName == "fa-comment-alt-image"){
	  label = _U("\ue1e0"); }

	else if(iconName == "fa-users-medical"){
	  label = _U("\uf830"); }

	else if(iconName == "fa-sensor-triangle-exclamation"){
	  label = _U("\ue029"); }

	else if(iconName == "fa-sensor-alert"){
	  label = _U("\ue029"); }

	else if(iconName == "fa-magnifying-glass-arrow-right"){
	  label = _U("\ue521"); }

	else if(iconName == "fa-tachograph-digital"){
	  label = _U("\uf566"); }

	else if(iconName == "fa-digital-tachograph"){
	  label = _U("\uf566"); }

	else if(iconName == "fa-face-mask"){
	  label = _U("\ue37f"); }

	else if(iconName == "fa-pickleball"){
	  label = _U("\ue435"); }

	else if(iconName == "fa-star-sharp-half"){
	  label = _U("\ue28c"); }

	else if(iconName == "fa-users-slash"){
	  label = _U("\ue073"); }

	else if(iconName == "fa-clover"){
	  label = _U("\ue139"); }

	else if(iconName == "fa-meat"){
	  label = _U("\uf814"); }

	else if(iconName == "fa-reply"){
	  label = _U("\uf3e5"); }

	else if(iconName == "fa-mail-reply"){
	  label = _U("\uf3e5"); }

	else if(iconName == "fa-star-and-crescent"){
	  label = _U("\uf699"); }

	else if(iconName == "fa-empty-set"){
	  label = _U("\uf656"); }

	else if(iconName == "fa-house-fire"){
	  label = _U("\ue50c"); }

	else if(iconName == "fa-square-minus"){
	  label = _U("\uf146"); }

	else if(iconName == "fa-minus-square"){
	  label = _U("\uf146"); }

	else if(iconName == "fa-helicopter"){
	  label = _U("\uf533"); }

	else if(iconName == "fa-bird"){
	  label = _U("\ue469"); }

	else if(iconName == "fa-compass"){
	  label = _U("\uf14e"); }

	else if(iconName == "fa-square-caret-down"){
	  label = _U("\uf150"); }

	else if(iconName == "fa-caret-square-down"){
	  label = _U("\uf150"); }

	else if(iconName == "fa-heart-half-stroke"){
	  label = _U("\ue1ac"); }

	else if(iconName == "fa-heart-half-alt"){
	  label = _U("\ue1ac"); }

	else if(iconName == "fa-file-circle-question"){
	  label = _U("\ue4ef"); }

	else if(iconName == "fa-truck-utensils"){
	  label = _U("\ue628"); }

	else if(iconName == "fa-laptop-code"){
	  label = _U("\uf5fc"); }

	else if(iconName == "fa-joystick"){
	  label = _U("\uf8c5"); }

	else if(iconName == "fa-grill-fire"){
	  label = _U("\ue5a4"); }

	else if(iconName == "fa-rectangle-vertical-history"){
	  label = _U("\ue237"); }

	else if(iconName == "fa-swatchbook"){
	  label = _U("\uf5c3"); }

	else if(iconName == "fa-prescription-bottle"){
	  label = _U("\uf485"); }

	else if(iconName == "fa-bars"){
	  label = _U("\uf0c9"); }

	else if(iconName == "fa-navicon"){
	  label = _U("\uf0c9"); }

	else if(iconName == "fa-keyboard-left"){
	  label = _U("\ue1c3"); }

	else if(iconName == "fa-people-group"){
	  label = _U("\ue533"); }

	else if(iconName == "fa-hourglass-end"){
	  label = _U("\uf253"); }

	else if(iconName == "fa-hourglass-3"){
	  label = _U("\uf253"); }

	else if(iconName == "fa-heart-crack"){
	  label = _U("\uf7a9"); }

	else if(iconName == "fa-heart-broken"){
	  label = _U("\uf7a9"); }

	else if(iconName == "fa-face-beam-hand-over-mouth"){
	  label = _U("\ue47c"); }

	else if(iconName == "fa-droplet-percent"){
	  label = _U("\uf750"); }

	else if(iconName == "fa-humidity"){
	  label = _U("\uf750"); }

	else if(iconName == "fa-square-up-right"){
	  label = _U("\uf360"); }

	else if(iconName == "fa-external-link-square-alt"){
	  label = _U("\uf360"); }

	else if(iconName == "fa-face-kiss-beam"){
	  label = _U("\uf597"); }

	else if(iconName == "fa-kiss-beam"){
	  label = _U("\uf597"); }

	else if(iconName == "fa-corn"){
	  label = _U("\uf6c7"); }

	else if(iconName == "fa-roller-coaster"){
	  label = _U("\ue324"); }

	else if(iconName == "fa-photo-film-music"){
	  label = _U("\ue228"); }

	else if(iconName == "fa-radar"){
	  label = _U("\ue024"); }

	else if(iconName == "fa-sickle"){
	  label = _U("\uf822"); }

	else if(iconName == "fa-film"){
	  label = _U("\uf008"); }

	else if(iconName == "fa-coconut"){
	  label = _U("\ue2f6"); }

	else if(iconName == "fa-ruler-horizontal"){
	  label = _U("\uf547"); }

	else if(iconName == "fa-shield-cross"){
	  label = _U("\uf712"); }

	else if(iconName == "fa-cassette-tape"){
	  label = _U("\uf8ab"); }

	else if(iconName == "fa-square-terminal"){
	  label = _U("\ue32a"); }

	else if(iconName == "fa-people-robbery"){
	  label = _U("\ue536"); }

	else if(iconName == "fa-lightbulb"){
	  label = _U("\uf0eb"); }

	else if(iconName == "fa-caret-left"){
	  label = _U("\uf0d9"); }

	else if(iconName == "fa-comment-middle"){
	  label = _U("\ue149"); }

	else if(iconName == "fa-trash-can-list"){
	  label = _U("\ue2ab"); }

	else if(iconName == "fa-block"){
	  label = _U("\ue46a"); }

	else if(iconName == "fa-circle-exclamation"){
	  label = _U("\uf06a"); }

	else if(iconName == "fa-exclamation-circle"){
	  label = _U("\uf06a"); }

	else if(iconName == "fa-school-circle-xmark"){
	  label = _U("\ue56d"); }

	else if(iconName == "fa-arrow-right-from-bracket"){
	  label = _U("\uf08b"); }

	else if(iconName == "fa-sign-out"){
	  label = _U("\uf08b"); }

	else if(iconName == "fa-face-frown-slight"){
	  label = _U("\ue376"); }

	else if(iconName == "fa-circle-chevron-down"){
	  label = _U("\uf13a"); }

	else if(iconName == "fa-chevron-circle-down"){
	  label = _U("\uf13a"); }

	else if(iconName == "fa-sidebar-flip"){
	  label = _U("\ue24f"); }

	else if(iconName == "fa-unlock-keyhole"){
	  label = _U("\uf13e"); }

	else if(iconName == "fa-unlock-alt"){
	  label = _U("\uf13e"); }

	else if(iconName == "fa-temperature-list"){
	  label = _U("\ue299"); }

	else if(iconName == "fa-cloud-showers-heavy"){
	  label = _U("\uf740"); }

	else if(iconName == "fa-headphones-simple"){
	  label = _U("\uf58f"); }

	else if(iconName == "fa-headphones-alt"){
	  label = _U("\uf58f"); }

	else if(iconName == "fa-sitemap"){
	  label = _U("\uf0e8"); }

	else if(iconName == "fa-pipe-section"){
	  label = _U("\ue438"); }

	else if(iconName == "fa-space-station-moon-construction"){
	  label = _U("\ue034"); }

	else if(iconName == "fa-space-station-moon-alt"){
	  label = _U("\ue034"); }

	else if(iconName == "fa-circle-dollar-to-slot"){
	  label = _U("\uf4b9"); }

	else if(iconName == "fa-donate"){
	  label = _U("\uf4b9"); }

	else if(iconName == "fa-memory"){
	  label = _U("\uf538"); }

	else if(iconName == "fa-face-sleeping"){
	  label = _U("\ue38d"); }

	else if(iconName == "fa-road-spikes"){
	  label = _U("\ue568"); }

	else if(iconName == "fa-fire-burner"){
	  label = _U("\ue4f1"); }

	else if(iconName == "fa-squirrel"){
	  label = _U("\uf71a"); }

	else if(iconName == "fa-arrow-up-to-line"){
	  label = _U("\uf341"); }

	else if(iconName == "fa-arrow-to-top"){
	  label = _U("\uf341"); }

	else if(iconName == "fa-flag"){
	  label = _U("\uf024"); }

	else if(iconName == "fa-face-cowboy-hat"){
	  label = _U("\ue36e"); }

	else if(iconName == "fa-hanukiah"){
	  label = _U("\uf6e6"); }

	else if(iconName == "fa-chart-scatter-3d"){
	  label = _U("\ue0e8"); }

	else if(iconName == "fa-display-chart-up"){
	  label = _U("\ue5e3"); }

	else if(iconName == "fa-square-code"){
	  label = _U("\ue267"); }

	else if(iconName == "fa-feather"){
	  label = _U("\uf52d"); }

	else if(iconName == "fa-volume-low"){
	  label = _U("\uf027"); }

	else if(iconName == "fa-volume-down"){
	  label = _U("\uf027"); }

	else if(iconName == "fa-xmark-to-slot"){
	  label = _U("\uf771"); }

	else if(iconName == "fa-times-to-slot"){
	  label = _U("\uf771"); }

	else if(iconName == "fa-vote-nay"){
	  label = _U("\uf771"); }

	else if(iconName == "fa-box-taped"){
	  label = _U("\uf49a"); }

	else if(iconName == "fa-box-alt"){
	  label = _U("\uf49a"); }

	else if(iconName == "fa-comment-slash"){
	  label = _U("\uf4b3"); }

	else if(iconName == "fa-swords"){
	  label = _U("\uf71d"); }

	else if(iconName == "fa-cloud-sun-rain"){
	  label = _U("\uf743"); }

	else if(iconName == "fa-album"){
	  label = _U("\uf89f"); }

	else if(iconName == "fa-circle-n"){
	  label = _U("\ue118"); }

	else if(iconName == "fa-compress"){
	  label = _U("\uf066"); }

	else if(iconName == "fa-wheat-awn"){
	  label = _U("\ue2cd"); }

	else if(iconName == "fa-wheat-alt"){
	  label = _U("\ue2cd"); }

	else if(iconName == "fa-ankh"){
	  label = _U("\uf644"); }

	else if(iconName == "fa-hands-holding-child"){
	  label = _U("\ue4fa"); }

	else if(iconName == "fa-asterisk"){
	  label = _U("\u002a"); }

	else if(iconName == "fa-key-skeleton-left-right"){
	  label = _U("\ue3b4"); }

	else if(iconName == "fa-comment-lines"){
	  label = _U("\uf4b0"); }

	else if(iconName == "fa-luchador-mask"){
	  label = _U("\uf455"); }

	else if(iconName == "fa-luchador"){
	  label = _U("\uf455"); }

	else if(iconName == "fa-mask-luchador"){
	  label = _U("\uf455"); }

	else if(iconName == "fa-square-check"){
	  label = _U("\uf14a"); }

	else if(iconName == "fa-check-square"){
	  label = _U("\uf14a"); }

	else if(iconName == "fa-shredder"){
	  label = _U("\uf68a"); }

	else if(iconName == "fa-book-open-cover"){
	  label = _U("\ue0c0"); }

	else if(iconName == "fa-book-open-alt"){
	  label = _U("\ue0c0"); }

	else if(iconName == "fa-sandwich"){
	  label = _U("\uf81f"); }

	else if(iconName == "fa-peseta-sign"){
	  label = _U("\ue221"); }

	else if(iconName == "fa-square-parking-slash"){
	  label = _U("\uf617"); }

	else if(iconName == "fa-parking-slash"){
	  label = _U("\uf617"); }

	else if(iconName == "fa-train-tunnel"){
	  label = _U("\ue454"); }

	else if(iconName == "fa-heading"){
	  label = _U("\uf1dc"); }

	else if(iconName == "fa-header"){
	  label = _U("\uf1dc"); }

	else if(iconName == "fa-ghost"){
	  label = _U("\uf6e2"); }

	else if(iconName == "fa-face-anguished"){
	  label = _U("\ue369"); }

	else if(iconName == "fa-hockey-sticks"){
	  label = _U("\uf454"); }

	else if(iconName == "fa-abacus"){
	  label = _U("\uf640"); }

	else if(iconName == "fa-film-simple"){
	  label = _U("\uf3a0"); }

	else if(iconName == "fa-film-alt"){
	  label = _U("\uf3a0"); }

	else if(iconName == "fa-list"){
	  label = _U("\uf03a"); }

	else if(iconName == "fa-list-squares"){
	  label = _U("\uf03a"); }

	else if(iconName == "fa-tree-palm"){
	  label = _U("\uf82b"); }

	else if(iconName == "fa-square-phone-flip"){
	  label = _U("\uf87b"); }

	else if(iconName == "fa-phone-square-alt"){
	  label = _U("\uf87b"); }

	else if(iconName == "fa-cart-plus"){
	  label = _U("\uf217"); }

	else if(iconName == "fa-gamepad"){
	  label = _U("\uf11b"); }

	else if(iconName == "fa-border-center-v"){
	  label = _U("\uf89d"); }

	else if(iconName == "fa-circle-dot"){
	  label = _U("\uf192"); }

	else if(iconName == "fa-dot-circle"){
	  label = _U("\uf192"); }

	else if(iconName == "fa-clipboard-medical"){
	  label = _U("\ue133"); }

	else if(iconName == "fa-face-dizzy"){
	  label = _U("\uf567"); }

	else if(iconName == "fa-dizzy"){
	  label = _U("\uf567"); }

	else if(iconName == "fa-egg"){
	  label = _U("\uf7fb"); }

	else if(iconName == "fa-up-to-line"){
	  label = _U("\uf34d"); }

	else if(iconName == "fa-arrow-alt-to-top"){
	  label = _U("\uf34d"); }

	else if(iconName == "fa-house-medical-circle-xmark"){
	  label = _U("\ue513"); }

	else if(iconName == "fa-watch-fitness"){
	  label = _U("\uf63e"); }

	else if(iconName == "fa-clock-nine-thirty"){
	  label = _U("\ue34d"); }

	else if(iconName == "fa-campground"){
	  label = _U("\uf6bb"); }

	else if(iconName == "fa-folder-plus"){
	  label = _U("\uf65e"); }

	else if(iconName == "fa-jug"){
	  label = _U("\uf8c6"); }

	else if(iconName == "fa-futbol"){
	  label = _U("\uf1e3"); }

	else if(iconName == "fa-futbol-ball"){
	  label = _U("\uf1e3"); }

	else if(iconName == "fa-soccer-ball"){
	  label = _U("\uf1e3"); }

	else if(iconName == "fa-snow-blowing"){
	  label = _U("\uf761"); }

	else if(iconName == "fa-paintbrush"){
	  label = _U("\uf1fc"); }

	else if(iconName == "fa-paint-brush"){
	  label = _U("\uf1fc"); }

	else if(iconName == "fa-lock"){
	  label = _U("\uf023"); }

	else if(iconName == "fa-arrow-down-from-line"){
	  label = _U("\uf345"); }

	else if(iconName == "fa-arrow-from-top"){
	  label = _U("\uf345"); }

	else if(iconName == "fa-gas-pump"){
	  label = _U("\uf52f"); }

	else if(iconName == "fa-signal-bars-slash"){
	  label = _U("\uf694"); }

	else if(iconName == "fa-signal-alt-slash"){
	  label = _U("\uf694"); }

	else if(iconName == "fa-monkey"){
	  label = _U("\uf6fb"); }

	else if(iconName == "fa-rectangle-pro"){
	  label = _U("\ue235"); }

	else if(iconName == "fa-pro"){
	  label = _U("\ue235"); }

	else if(iconName == "fa-house-night"){
	  label = _U("\ue010"); }

	else if(iconName == "fa-hot-tub-person"){
	  label = _U("\uf593"); }

	else if(iconName == "fa-hot-tub"){
	  label = _U("\uf593"); }

	else if(iconName == "fa-globe-pointer"){
	  label = _U("\ue60e"); }

	else if(iconName == "fa-blanket"){
	  label = _U("\uf498"); }

	else if(iconName == "fa-map-location"){
	  label = _U("\uf59f"); }

	else if(iconName == "fa-map-marked"){
	  label = _U("\uf59f"); }

	else if(iconName == "fa-house-flood-water"){
	  label = _U("\ue50e"); }

	else if(iconName == "fa-comments-question-check"){
	  label = _U("\ue14f"); }

	else if(iconName == "fa-tree"){
	  label = _U("\uf1bb"); }

	else if(iconName == "fa-arrows-cross"){
	  label = _U("\ue0a2"); }

	else if(iconName == "fa-backpack"){
	  label = _U("\uf5d4"); }

	else if(iconName == "fa-square-small"){
	  label = _U("\ue27e"); }

	else if(iconName == "fa-folder-arrow-up"){
	  label = _U("\ue054"); }

	else if(iconName == "fa-folder-upload"){
	  label = _U("\ue054"); }

	else if(iconName == "fa-bridge-lock"){
	  label = _U("\ue4cc"); }

	else if(iconName == "fa-crosshairs-simple"){
	  label = _U("\ue59f"); }

	else if(iconName == "fa-sack-dollar"){
	  label = _U("\uf81d"); }

	else if(iconName == "fa-pen-to-square"){
	  label = _U("\uf044"); }

	else if(iconName == "fa-edit"){
	  label = _U("\uf044"); }

	else if(iconName == "fa-square-sliders"){
	  label = _U("\uf3f0"); }

	else if(iconName == "fa-sliders-h-square"){
	  label = _U("\uf3f0"); }

	else if(iconName == "fa-car-side"){
	  label = _U("\uf5e4"); }

	else if(iconName == "fa-message-middle-top"){
	  label = _U("\ue1e2"); }

	else if(iconName == "fa-comment-middle-top-alt"){
	  label = _U("\ue1e2"); }

	else if(iconName == "fa-lightbulb-on"){
	  label = _U("\uf672"); }

	else if(iconName == "fa-knife"){
	  label = _U("\uf2e4"); }

	else if(iconName == "fa-utensil-knife"){
	  label = _U("\uf2e4"); }

	else if(iconName == "fa-share-nodes"){
	  label = _U("\uf1e0"); }

	else if(iconName == "fa-share-alt"){
	  label = _U("\uf1e0"); }

	else if(iconName == "fa-display-chart-up-circle-dollar"){
	  label = _U("\ue5e6"); }

	else if(iconName == "fa-wave-sine"){
	  label = _U("\uf899"); }

	else if(iconName == "fa-heart-circle-minus"){
	  label = _U("\ue4ff"); }

	else if(iconName == "fa-circle-w"){
	  label = _U("\ue12c"); }

	else if(iconName == "fa-circle-calendar"){
	  label = _U("\ue102"); }

	else if(iconName == "fa-calendar-circle"){
	  label = _U("\ue102"); }

	else if(iconName == "fa-hourglass-half"){
	  label = _U("\uf252"); }

	else if(iconName == "fa-hourglass-2"){
	  label = _U("\uf252"); }

	else if(iconName == "fa-microscope"){
	  label = _U("\uf610"); }

	else if(iconName == "fa-sunset"){
	  label = _U("\uf767"); }

	else if(iconName == "fa-sink"){
	  label = _U("\ue06d"); }

	else if(iconName == "fa-calendar-exclamation"){
	  label = _U("\uf334"); }

	else if(iconName == "fa-truck-container-empty"){
	  label = _U("\ue2b5"); }

	else if(iconName == "fa-hand-heart"){
	  label = _U("\uf4bc"); }

	else if(iconName == "fa-bag-shopping"){
	  label = _U("\uf290"); }

	else if(iconName == "fa-shopping-bag"){
	  label = _U("\uf290"); }

	else if(iconName == "fa-arrow-down-z-a"){
	  label = _U("\uf881"); }

	else if(iconName == "fa-sort-alpha-desc"){
	  label = _U("\uf881"); }

	else if(iconName == "fa-sort-alpha-down-alt"){
	  label = _U("\uf881"); }

	else if(iconName == "fa-mitten"){
	  label = _U("\uf7b5"); }

	else if(iconName == "fa-reply-clock"){
	  label = _U("\ue239"); }

	else if(iconName == "fa-reply-time"){
	  label = _U("\ue239"); }

	else if(iconName == "fa-person-rays"){
	  label = _U("\ue54d"); }

	else if(iconName == "fa-right"){
	  label = _U("\uf356"); }

	else if(iconName == "fa-arrow-alt-right"){
	  label = _U("\uf356"); }

	else if(iconName == "fa-circle-f"){
	  label = _U("\ue10e"); }

	else if(iconName == "fa-users"){
	  label = _U("\uf0c0"); }

	else if(iconName == "fa-face-pleading"){
	  label = _U("\ue386"); }

	else if(iconName == "fa-eye-slash"){
	  label = _U("\uf070"); }

	else if(iconName == "fa-flask-vial"){
	  label = _U("\ue4f3"); }

	else if(iconName == "fa-police-box"){
	  label = _U("\ue021"); }

	else if(iconName == "fa-cucumber"){
	  label = _U("\ue401"); }

	else if(iconName == "fa-head-side-brain"){
	  label = _U("\uf808"); }

	else if(iconName == "fa-hand"){
	  label = _U("\uf256"); }

	else if(iconName == "fa-hand-paper"){
	  label = _U("\uf256"); }

	else if(iconName == "fa-person-biking-mountain"){
	  label = _U("\uf84b"); }

	else if(iconName == "fa-biking-mountain"){
	  label = _U("\uf84b"); }

	else if(iconName == "fa-utensils-slash"){
	  label = _U("\ue464"); }

	else if(iconName == "fa-print-magnifying-glass"){
	  label = _U("\uf81a"); }

	else if(iconName == "fa-print-search"){
	  label = _U("\uf81a"); }

	else if(iconName == "fa-turn-right"){
	  label = _U("\ue639"); }

	else if(iconName == "fa-folder-bookmark"){
	  label = _U("\ue186"); }

	else if(iconName == "fa-arrow-turn-left-down"){
	  label = _U("\ue633"); }

	else if(iconName == "fa-om"){
	  label = _U("\uf679"); }

	else if(iconName == "fa-pi"){
	  label = _U("\uf67e"); }

	else if(iconName == "fa-flask-round-potion"){
	  label = _U("\uf6e1"); }

	else if(iconName == "fa-flask-potion"){
	  label = _U("\uf6e1"); }

	else if(iconName == "fa-face-shush"){
	  label = _U("\ue38c"); }

	else if(iconName == "fa-worm"){
	  label = _U("\ue599"); }

	else if(iconName == "fa-house-circle-xmark"){
	  label = _U("\ue50b"); }

	else if(iconName == "fa-plug"){
	  label = _U("\uf1e6"); }

	else if(iconName == "fa-calendar-circle-exclamation"){
	  label = _U("\ue46e"); }

	else if(iconName == "fa-square-i"){
	  label = _U("\ue272"); }

	else if(iconName == "fa-chevron-up"){
	  label = _U("\uf077"); }

	else if(iconName == "fa-face-saluting"){
	  label = _U("\ue484"); }

	else if(iconName == "fa-gauge-simple-low"){
	  label = _U("\uf62c"); }

	else if(iconName == "fa-tachometer-slow"){
	  label = _U("\uf62c"); }

	else if(iconName == "fa-face-persevering"){
	  label = _U("\ue385"); }

	else if(iconName == "fa-circle-camera"){
	  label = _U("\ue103"); }

	else if(iconName == "fa-camera-circle"){
	  label = _U("\ue103"); }

	else if(iconName == "fa-hand-spock"){
	  label = _U("\uf259"); }

	else if(iconName == "fa-spider-web"){
	  label = _U("\uf719"); }

	else if(iconName == "fa-circle-microphone"){
	  label = _U("\ue116"); }

	else if(iconName == "fa-microphone-circle"){
	  label = _U("\ue116"); }

	else if(iconName == "fa-book-arrow-up"){
	  label = _U("\ue0ba"); }

	else if(iconName == "fa-popsicle"){
	  label = _U("\ue43e"); }

	else if(iconName == "fa-command"){
	  label = _U("\ue142"); }

	else if(iconName == "fa-blinds"){
	  label = _U("\uf8fb"); }

	else if(iconName == "fa-stopwatch"){
	  label = _U("\uf2f2"); }

	else if(iconName == "fa-saxophone"){
	  label = _U("\uf8dc"); }

	else if(iconName == "fa-square-2"){
	  label = _U("\ue257"); }

	else if(iconName == "fa-field-hockey-stick-ball"){
	  label = _U("\uf44c"); }

	else if(iconName == "fa-field-hockey"){
	  label = _U("\uf44c"); }

	else if(iconName == "fa-arrow-up-square-triangle"){
	  label = _U("\uf88b"); }

	else if(iconName == "fa-sort-shapes-up-alt"){
	  label = _U("\uf88b"); }

	else if(iconName == "fa-face-scream"){
	  label = _U("\ue38b"); }

	else if(iconName == "fa-square-m"){
	  label = _U("\ue276"); }

	else if(iconName == "fa-camera-web"){
	  label = _U("\uf832"); }

	else if(iconName == "fa-webcam"){
	  label = _U("\uf832"); }

	else if(iconName == "fa-comment-arrow-down"){
	  label = _U("\ue143"); }

	else if(iconName == "fa-lightbulb-cfl"){
	  label = _U("\ue5a6"); }

	else if(iconName == "fa-window-frame-open"){
	  label = _U("\ue050"); }

	else if(iconName == "fa-face-kiss"){
	  label = _U("\uf596"); }

	else if(iconName == "fa-kiss"){
	  label = _U("\uf596"); }

	else if(iconName == "fa-bridge-circle-xmark"){
	  label = _U("\ue4cb"); }

	else if(iconName == "fa-period"){
	  label = _U("\u002e"); }

	else if(iconName == "fa-face-grin-tongue"){
	  label = _U("\uf589"); }

	else if(iconName == "fa-grin-tongue"){
	  label = _U("\uf589"); }

	else if(iconName == "fa-up-to-dotted-line"){
	  label = _U("\ue457"); }

	else if(iconName == "fa-thought-bubble"){
	  label = _U("\ue32e"); }

	else if(iconName == "fa-skeleton-ribs"){
	  label = _U("\ue5cb"); }

	else if(iconName == "fa-raygun"){
	  label = _U("\ue025"); }

	else if(iconName == "fa-flute"){
	  label = _U("\uf8b9"); }

	else if(iconName == "fa-acorn"){
	  label = _U("\uf6ae"); }

	else if(iconName == "fa-video-arrow-up-right"){
	  label = _U("\ue2c9"); }

	else if(iconName == "fa-grate-droplet"){
	  label = _U("\ue194"); }

	else if(iconName == "fa-seal-exclamation"){
	  label = _U("\ue242"); }

	else if(iconName == "fa-chess-bishop"){
	  label = _U("\uf43a"); }

	else if(iconName == "fa-message-sms"){
	  label = _U("\ue1e5"); }

	else if(iconName == "fa-coffee-beans"){
	  label = _U("\ue13f"); }

	else if(iconName == "fa-hat-witch"){
	  label = _U("\uf6e7"); }

	else if(iconName == "fa-face-grin-wink"){
	  label = _U("\uf58c"); }

	else if(iconName == "fa-grin-wink"){
	  label = _U("\uf58c"); }

	else if(iconName == "fa-clock-three-thirty"){
	  label = _U("\ue357"); }

	else if(iconName == "fa-ear-deaf"){
	  label = _U("\uf2a4"); }

	else if(iconName == "fa-deaf"){
	  label = _U("\uf2a4"); }

	else if(iconName == "fa-deafness"){
	  label = _U("\uf2a4"); }

	else if(iconName == "fa-hard-of-hearing"){
	  label = _U("\uf2a4"); }

	else if(iconName == "fa-alarm-clock"){
	  label = _U("\uf34e"); }

	else if(iconName == "fa-eclipse"){
	  label = _U("\uf749"); }

	else if(iconName == "fa-face-relieved"){
	  label = _U("\ue389"); }

	else if(iconName == "fa-road-circle-check"){
	  label = _U("\ue564"); }

	else if(iconName == "fa-dice-five"){
	  label = _U("\uf523"); }

	else if(iconName == "fa-octagon-minus"){
	  label = _U("\uf308"); }

	else if(iconName == "fa-minus-octagon"){
	  label = _U("\uf308"); }

	else if(iconName == "fa-square-rss"){
	  label = _U("\uf143"); }

	else if(iconName == "fa-rss-square"){
	  label = _U("\uf143"); }

	else if(iconName == "fa-face-zany"){
	  label = _U("\ue3a4"); }

	else if(iconName == "fa-tricycle"){
	  label = _U("\ue5c3"); }

	else if(iconName == "fa-land-mine-on"){
	  label = _U("\ue51b"); }

	else if(iconName == "fa-square-arrow-up-left"){
	  label = _U("\ue263"); }

	else if(iconName == "fa-i-cursor"){
	  label = _U("\uf246"); }

	else if(iconName == "fa-chart-mixed-up-circle-dollar"){
	  label = _U("\ue5d9"); }

	else if(iconName == "fa-salt-shaker"){
	  label = _U("\ue446"); }

	else if(iconName == "fa-stamp"){
	  label = _U("\uf5bf"); }

	else if(iconName == "fa-file-plus"){
	  label = _U("\uf319"); }

	else if(iconName == "fa-draw-square"){
	  label = _U("\uf5ef"); }

	else if(iconName == "fa-toilet-paper-under-slash"){
	  label = _U("\ue2a1"); }

	else if(iconName == "fa-toilet-paper-reverse-slash"){
	  label = _U("\ue2a1"); }

	else if(iconName == "fa-stairs"){
	  label = _U("\ue289"); }

	else if(iconName == "fa-drone-front"){
	  label = _U("\uf860"); }

	else if(iconName == "fa-drone-alt"){
	  label = _U("\uf860"); }

	else if(iconName == "fa-glass-empty"){
	  label = _U("\ue191"); }

	else if(iconName == "fa-dial-high"){
	  label = _U("\ue15c"); }

	else if(iconName == "fa-user-helmet-safety"){
	  label = _U("\uf82c"); }

	else if(iconName == "fa-user-construction"){
	  label = _U("\uf82c"); }

	else if(iconName == "fa-user-hard-hat"){
	  label = _U("\uf82c"); }

	else if(iconName == "fa-i"){
	  label = _U("\u0049"); }

	else if(iconName == "fa-hryvnia-sign"){
	  label = _U("\uf6f2"); }

	else if(iconName == "fa-hryvnia"){
	  label = _U("\uf6f2"); }

	else if(iconName == "fa-arrow-down-left-and-arrow-up-right-to-center"){
	  label = _U("\ue092"); }

	else if(iconName == "fa-pills"){
	  label = _U("\uf484"); }

	else if(iconName == "fa-face-grin-wide"){
	  label = _U("\uf581"); }

	else if(iconName == "fa-grin-alt"){
	  label = _U("\uf581"); }

	else if(iconName == "fa-tooth"){
	  label = _U("\uf5c9"); }

	else if(iconName == "fa-basketball-hoop"){
	  label = _U("\uf435"); }

	else if(iconName == "fa-objects-align-bottom"){
	  label = _U("\ue3bb"); }

	else if(iconName == "fa-v"){
	  label = _U("\u0056"); }

	else if(iconName == "fa-sparkles"){
	  label = _U("\uf890"); }

	else if(iconName == "fa-squid"){
	  label = _U("\ue450"); }

	else if(iconName == "fa-leafy-green"){
	  label = _U("\ue41d"); }

	else if(iconName == "fa-circle-arrow-up-right"){
	  label = _U("\ue0fc"); }

	else if(iconName == "fa-calendars"){
	  label = _U("\ue0d7"); }

	else if(iconName == "fa-bangladeshi-taka-sign"){
	  label = _U("\ue2e6"); }

	else if(iconName == "fa-bicycle"){
	  label = _U("\uf206"); }

	else if(iconName == "fa-hammer-war"){
	  label = _U("\uf6e4"); }

	else if(iconName == "fa-circle-d"){
	  label = _U("\ue104"); }

	else if(iconName == "fa-spider-black-widow"){
	  label = _U("\uf718"); }

	else if(iconName == "fa-staff-snake"){
	  label = _U("\ue579"); }

	else if(iconName == "fa-rod-asclepius"){
	  label = _U("\ue579"); }

	else if(iconName == "fa-rod-snake"){
	  label = _U("\ue579"); }

	else if(iconName == "fa-staff-aesculapius"){
	  label = _U("\ue579"); }

	else if(iconName == "fa-pear"){
	  label = _U("\ue20c"); }

	else if(iconName == "fa-head-side-cough-slash"){
	  label = _U("\ue062"); }

	else if(iconName == "fa-file-mov"){
	  label = _U("\ue647"); }

	else if(iconName == "fa-triangle"){
	  label = _U("\uf2ec"); }

	else if(iconName == "fa-apartment"){
	  label = _U("\ue468"); }

	else if(iconName == "fa-truck-medical"){
	  label = _U("\uf0f9"); }

	else if(iconName == "fa-ambulance"){
	  label = _U("\uf0f9"); }

	else if(iconName == "fa-pepper"){
	  label = _U("\ue432"); }

	else if(iconName == "fa-piano"){
	  label = _U("\uf8d4"); }

	else if(iconName == "fa-gun-squirt"){
	  label = _U("\ue19d"); }

	else if(iconName == "fa-wheat-awn-circle-exclamation"){
	  label = _U("\ue598"); }

	else if(iconName == "fa-snowman"){
	  label = _U("\uf7d0"); }

	else if(iconName == "fa-user-alien"){
	  label = _U("\ue04a"); }

	else if(iconName == "fa-shield-check"){
	  label = _U("\uf2f7"); }

	else if(iconName == "fa-mortar-pestle"){
	  label = _U("\uf5a7"); }

	else if(iconName == "fa-road-barrier"){
	  label = _U("\ue562"); }

	else if(iconName == "fa-chart-candlestick"){
	  label = _U("\ue0e2"); }

	else if(iconName == "fa-briefcase-blank"){
	  label = _U("\ue0c8"); }

	else if(iconName == "fa-school"){
	  label = _U("\uf549"); }

	else if(iconName == "fa-igloo"){
	  label = _U("\uf7ae"); }

	else if(iconName == "fa-bracket-round"){
	  label = _U("\u0028"); }

	else if(iconName == "fa-parenthesis"){
	  label = _U("\u0028"); }

	else if(iconName == "fa-joint"){
	  label = _U("\uf595"); }

	else if(iconName == "fa-horse-saddle"){
	  label = _U("\uf8c3"); }

	else if(iconName == "fa-mug-marshmallows"){
	  label = _U("\uf7b7"); }

	else if(iconName == "fa-filters"){
	  label = _U("\ue17e"); }

	else if(iconName == "fa-bell-on"){
	  label = _U("\uf8fa"); }

	else if(iconName == "fa-angle-right"){
	  label = _U("\uf105"); }

	else if(iconName == "fa-dial-med"){
	  label = _U("\ue15f"); }

	else if(iconName == "fa-horse"){
	  label = _U("\uf6f0"); }

	else if(iconName == "fa-q"){
	  label = _U("\u0051"); }

	else if(iconName == "fa-monitor-waveform"){
	  label = _U("\uf611"); }

	else if(iconName == "fa-monitor-heart-rate"){
	  label = _U("\uf611"); }

	else if(iconName == "fa-link-simple"){
	  label = _U("\ue1cd"); }

	else if(iconName == "fa-whistle"){
	  label = _U("\uf460"); }

	else if(iconName == "fa-g"){
	  label = _U("\u0047"); }

	else if(iconName == "fa-wine-glass-crack"){
	  label = _U("\uf4bb"); }

	else if(iconName == "fa-fragile"){
	  label = _U("\uf4bb"); }

	else if(iconName == "fa-slot-machine"){
	  label = _U("\ue3ce"); }

	else if(iconName == "fa-notes-medical"){
	  label = _U("\uf481"); }

	else if(iconName == "fa-car-wash"){
	  label = _U("\uf5e6"); }

	else if(iconName == "fa-escalator"){
	  label = _U("\ue171"); }

	else if(iconName == "fa-comment-image"){
	  label = _U("\ue148"); }

	else if(iconName == "fa-temperature-half"){
	  label = _U("\uf2c9"); }

	else if(iconName == "fa-temperature-2"){
	  label = _U("\uf2c9"); }

	else if(iconName == "fa-thermometer-2"){
	  label = _U("\uf2c9"); }

	else if(iconName == "fa-thermometer-half"){
	  label = _U("\uf2c9"); }

	else if(iconName == "fa-dong-sign"){
	  label = _U("\ue169"); }

	else if(iconName == "fa-donut"){
	  label = _U("\ue406"); }

	else if(iconName == "fa-doughnut"){
	  label = _U("\ue406"); }

	else if(iconName == "fa-capsules"){
	  label = _U("\uf46b"); }

	else if(iconName == "fa-poo-storm"){
	  label = _U("\uf75a"); }

	else if(iconName == "fa-poo-bolt"){
	  label = _U("\uf75a"); }

	else if(iconName == "fa-tally-1"){
	  label = _U("\ue294"); }

	else if(iconName == "fa-file-vector"){
	  label = _U("\ue64c"); }

	else if(iconName == "fa-face-frown-open"){
	  label = _U("\uf57a"); }

	else if(iconName == "fa-frown-open"){
	  label = _U("\uf57a"); }

	else if(iconName == "fa-square-dashed"){
	  label = _U("\ue269"); }

	else if(iconName == "fa-bag-shopping-plus"){
	  label = _U("\ue651"); }

	else if(iconName == "fa-square-j"){
	  label = _U("\ue273"); }

	else if(iconName == "fa-hand-point-up"){
	  label = _U("\uf0a6"); }

	else if(iconName == "fa-money-bill"){
	  label = _U("\uf0d6"); }

	else if(iconName == "fa-arrow-up-big-small"){
	  label = _U("\uf88e"); }

	else if(iconName == "fa-sort-size-up"){
	  label = _U("\uf88e"); }

	else if(iconName == "fa-barcode-read"){
	  label = _U("\uf464"); }

	else if(iconName == "fa-baguette"){
	  label = _U("\ue3d8"); }

	else if(iconName == "fa-bowl-soft-serve"){
	  label = _U("\ue46b"); }

	else if(iconName == "fa-face-holding-back-tears"){
	  label = _U("\ue482"); }

	else if(iconName == "fa-square-up"){
	  label = _U("\uf353"); }

	else if(iconName == "fa-arrow-alt-square-up"){
	  label = _U("\uf353"); }

	else if(iconName == "fa-train-subway-tunnel"){
	  label = _U("\ue2a3"); }

	else if(iconName == "fa-subway-tunnel"){
	  label = _U("\ue2a3"); }

	else if(iconName == "fa-square-exclamation"){
	  label = _U("\uf321"); }

	else if(iconName == "fa-exclamation-square"){
	  label = _U("\uf321"); }

	else if(iconName == "fa-semicolon"){
	  label = _U("\u003b"); }

	else if(iconName == "fa-bookmark"){
	  label = _U("\uf02e"); }

	else if(iconName == "fa-fan-table"){
	  label = _U("\ue004"); }

	else if(iconName == "fa-align-justify"){
	  label = _U("\uf039"); }

	else if(iconName == "fa-battery-low"){
	  label = _U("\ue0b1"); }

	else if(iconName == "fa-battery-1"){
	  label = _U("\ue0b1"); }

	else if(iconName == "fa-credit-card-front"){
	  label = _U("\uf38a"); }

	else if(iconName == "fa-brain-arrow-curved-right"){
	  label = _U("\uf677"); }

	else if(iconName == "fa-mind-share"){
	  label = _U("\uf677"); }

	else if(iconName == "fa-umbrella-beach"){
	  label = _U("\uf5ca"); }

	else if(iconName == "fa-helmet-un"){
	  label = _U("\ue503"); }

	else if(iconName == "fa-location-smile"){
	  label = _U("\uf60d"); }

	else if(iconName == "fa-map-marker-smile"){
	  label = _U("\uf60d"); }

	else if(iconName == "fa-arrow-left-to-line"){
	  label = _U("\uf33e"); }

	else if(iconName == "fa-arrow-to-left"){
	  label = _U("\uf33e"); }

	else if(iconName == "fa-bullseye"){
	  label = _U("\uf140"); }

	else if(iconName == "fa-sushi"){
	  label = _U("\ue48a"); }

	else if(iconName == "fa-nigiri"){
	  label = _U("\ue48a"); }

	else if(iconName == "fa-message-captions"){
	  label = _U("\ue1de"); }

	else if(iconName == "fa-comment-alt-captions"){
	  label = _U("\ue1de"); }

	else if(iconName == "fa-trash-list"){
	  label = _U("\ue2b1"); }

	else if(iconName == "fa-bacon"){
	  label = _U("\uf7e5"); }

	else if(iconName == "fa-option"){
	  label = _U("\ue318"); }

	else if(iconName == "fa-raccoon"){
	  label = _U("\ue613"); }

	else if(iconName == "fa-hand-point-down"){
	  label = _U("\uf0a7"); }

	else if(iconName == "fa-arrow-up-from-bracket"){
	  label = _U("\ue09a"); }

	else if(iconName == "fa-head-side-gear"){
	  label = _U("\ue611"); }

	else if(iconName == "fa-trash-plus"){
	  label = _U("\ue2b2"); }

	else if(iconName == "fa-objects-align-top"){
	  label = _U("\ue3c0"); }

	else if(iconName == "fa-folder"){
	  label = _U("\uf07b"); }

	else if(iconName == "fa-folder-blank"){
	  label = _U("\uf07b"); }

	else if(iconName == "fa-face-anxious-sweat"){
	  label = _U("\ue36a"); }

	else if(iconName == "fa-credit-card-blank"){
	  label = _U("\uf389"); }

	else if(iconName == "fa-file-waveform"){
	  label = _U("\uf478"); }

	else if(iconName == "fa-file-medical-alt"){
	  label = _U("\uf478"); }

	else if(iconName == "fa-microchip-ai"){
	  label = _U("\ue1ec"); }

	else if(iconName == "fa-mug"){
	  label = _U("\uf874"); }

	else if(iconName == "fa-plane-up-slash"){
	  label = _U("\ue22e"); }

	else if(iconName == "fa-radiation"){
	  label = _U("\uf7b9"); }

	else if(iconName == "fa-pen-circle"){
	  label = _U("\ue20e"); }

	else if(iconName == "fa-bag-seedling"){
	  label = _U("\ue5f2"); }

	else if(iconName == "fa-chart-simple"){
	  label = _U("\ue473"); }

	else if(iconName == "fa-crutches"){
	  label = _U("\uf7f8"); }

	else if(iconName == "fa-circle-parking"){
	  label = _U("\uf615"); }

	else if(iconName == "fa-parking-circle"){
	  label = _U("\uf615"); }

	else if(iconName == "fa-mars-stroke"){
	  label = _U("\uf229"); }

	else if(iconName == "fa-leaf-oak"){
	  label = _U("\uf6f7"); }

	else if(iconName == "fa-square-bolt"){
	  label = _U("\ue265"); }

	else if(iconName == "fa-vial"){
	  label = _U("\uf492"); }

	else if(iconName == "fa-gauge"){
	  label = _U("\uf624"); }

	else if(iconName == "fa-dashboard"){
	  label = _U("\uf624"); }

	else if(iconName == "fa-gauge-med"){
	  label = _U("\uf624"); }

	else if(iconName == "fa-tachometer-alt-average"){
	  label = _U("\uf624"); }

	else if(iconName == "fa-wand-magic-sparkles"){
	  label = _U("\ue2ca"); }

	else if(iconName == "fa-magic-wand-sparkles"){
	  label = _U("\ue2ca"); }

	else if(iconName == "fa-lambda"){
	  label = _U("\uf66e"); }

	else if(iconName == "fa-e"){
	  label = _U("\u0045"); }

	else if(iconName == "fa-pizza"){
	  label = _U("\uf817"); }

	else if(iconName == "fa-bowl-chopsticks-noodles"){
	  label = _U("\ue2ea"); }

	else if(iconName == "fa-h3"){
	  label = _U("\uf315"); }

	else if(iconName == "fa-pen-clip"){
	  label = _U("\uf305"); }

	else if(iconName == "fa-pen-alt"){
	  label = _U("\uf305"); }

	else if(iconName == "fa-bridge-circle-exclamation"){
	  label = _U("\ue4ca"); }

	else if(iconName == "fa-badge-percent"){
	  label = _U("\uf646"); }

	else if(iconName == "fa-rotate-reverse"){
	  label = _U("\ue631"); }

	else if(iconName == "fa-user"){
	  label = _U("\uf007"); }

	else if(iconName == "fa-sensor"){
	  label = _U("\ue028"); }

	else if(iconName == "fa-comma"){
	  label = _U("\u002c"); }

	else if(iconName == "fa-school-circle-check"){
	  label = _U("\ue56b"); }

	else if(iconName == "fa-toilet-paper-under"){
	  label = _U("\ue2a0"); }

	else if(iconName == "fa-toilet-paper-reverse"){
	  label = _U("\ue2a0"); }

	else if(iconName == "fa-light-emergency"){
	  label = _U("\ue41f"); }

	else if(iconName == "fa-arrow-down-to-arc"){
	  label = _U("\ue4ae"); }

	else if(iconName == "fa-dumpster"){
	  label = _U("\uf793"); }

	else if(iconName == "fa-van-shuttle"){
	  label = _U("\uf5b6"); }

	else if(iconName == "fa-shuttle-van"){
	  label = _U("\uf5b6"); }

	else if(iconName == "fa-building-user"){
	  label = _U("\ue4da"); }

	else if(iconName == "fa-light-switch"){
	  label = _U("\ue017"); }

	else if(iconName == "fa-square-caret-left"){
	  label = _U("\uf191"); }

	else if(iconName == "fa-caret-square-left"){
	  label = _U("\uf191"); }

	else if(iconName == "fa-highlighter"){
	  label = _U("\uf591"); }

	else if(iconName == "fa-wave-pulse"){
	  label = _U("\uf5f8"); }

	else if(iconName == "fa-heart-rate"){
	  label = _U("\uf5f8"); }

	else if(iconName == "fa-key"){
	  label = _U("\uf084"); }

	else if(iconName == "fa-hat-santa"){
	  label = _U("\uf7a7"); }

	else if(iconName == "fa-tamale"){
	  label = _U("\ue451"); }

	else if(iconName == "fa-box-check"){
	  label = _U("\uf467"); }

	else if(iconName == "fa-bullhorn"){
	  label = _U("\uf0a1"); }

	else if(iconName == "fa-steak"){
	  label = _U("\uf824"); }

	else if(iconName == "fa-location-crosshairs-slash"){
	  label = _U("\uf603"); }

	else if(iconName == "fa-location-slash"){
	  label = _U("\uf603"); }

	else if(iconName == "fa-person-dolly"){
	  label = _U("\uf4d0"); }

	else if(iconName == "fa-globe"){
	  label = _U("\uf0ac"); }

	else if(iconName == "fa-synagogue"){
	  label = _U("\uf69b"); }

	else if(iconName == "fa-file-chart-column"){
	  label = _U("\uf659"); }

	else if(iconName == "fa-file-chart-line"){
	  label = _U("\uf659"); }

	else if(iconName == "fa-person-half-dress"){
	  label = _U("\ue548"); }

	else if(iconName == "fa-folder-image"){
	  label = _U("\ue18a"); }

	else if(iconName == "fa-calendar-pen"){
	  label = _U("\uf333"); }

	else if(iconName == "fa-calendar-edit"){
	  label = _U("\uf333"); }

	else if(iconName == "fa-road-bridge"){
	  label = _U("\ue563"); }

	else if(iconName == "fa-face-smile-tear"){
	  label = _U("\ue393"); }

	else if(iconName == "fa-message-plus"){
	  label = _U("\uf4a8"); }

	else if(iconName == "fa-comment-alt-plus"){
	  label = _U("\uf4a8"); }

	else if(iconName == "fa-location-arrow"){
	  label = _U("\uf124"); }

	else if(iconName == "fa-c"){
	  label = _U("\u0043"); }

	else if(iconName == "fa-tablet-button"){
	  label = _U("\uf10a"); }

	else if(iconName == "fa-person-dress-fairy"){
	  label = _U("\ue607"); }

	else if(iconName == "fa-rectangle-history-circle-user"){
	  label = _U("\ue4a4"); }

	else if(iconName == "fa-building-lock"){
	  label = _U("\ue4d6"); }

	else if(iconName == "fa-chart-line-up"){
	  label = _U("\ue0e5"); }

	else if(iconName == "fa-mailbox"){
	  label = _U("\uf813"); }

	else if(iconName == "fa-sign-posts"){
	  label = _U("\ue625"); }

	else if(iconName == "fa-truck-bolt"){
	  label = _U("\ue3d0"); }

	else if(iconName == "fa-pizza-slice"){
	  label = _U("\uf818"); }

	else if(iconName == "fa-money-bill-wave"){
	  label = _U("\uf53a"); }

	else if(iconName == "fa-chart-area"){
	  label = _U("\uf1fe"); }

	else if(iconName == "fa-area-chart"){
	  label = _U("\uf1fe"); }

	else if(iconName == "fa-house-flag"){
	  label = _U("\ue50d"); }

	else if(iconName == "fa-circle-three-quarters-stroke"){
	  label = _U("\ue5d4"); }

	else if(iconName == "fa-person-circle-minus"){
	  label = _U("\ue540"); }

	else if(iconName == "fa-scalpel"){
	  label = _U("\uf61d"); }

	else if(iconName == "fa-ban"){
	  label = _U("\uf05e"); }

	else if(iconName == "fa-cancel"){
	  label = _U("\uf05e"); }

	else if(iconName == "fa-bell-exclamation"){
	  label = _U("\uf848"); }

	else if(iconName == "fa-circle-bookmark"){
	  label = _U("\ue100"); }

	else if(iconName == "fa-bookmark-circle"){
	  label = _U("\ue100"); }

	else if(iconName == "fa-egg-fried"){
	  label = _U("\uf7fc"); }

	else if(iconName == "fa-face-weary"){
	  label = _U("\ue3a1"); }

	else if(iconName == "fa-uniform-martial-arts"){
	  label = _U("\ue3d1"); }

	else if(iconName == "fa-camera-rotate"){
	  label = _U("\ue0d8"); }

	else if(iconName == "fa-sun-dust"){
	  label = _U("\uf764"); }

	else if(iconName == "fa-comment-text"){
	  label = _U("\ue14d"); }

	else if(iconName == "fa-spray-can-sparkles"){
	  label = _U("\uf5d0"); }

	else if(iconName == "fa-air-freshener"){
	  label = _U("\uf5d0"); }

	else if(iconName == "fa-signal-bars"){
	  label = _U("\uf690"); }

	else if(iconName == "fa-signal-alt"){
	  label = _U("\uf690"); }

	else if(iconName == "fa-signal-alt-4"){
	  label = _U("\uf690"); }

	else if(iconName == "fa-signal-bars-strong"){
	  label = _U("\uf690"); }

	else if(iconName == "fa-diamond-exclamation"){
	  label = _U("\ue405"); }

	else if(iconName == "fa-star"){
	  label = _U("\uf005"); }

	else if(iconName == "fa-dial-min"){
	  label = _U("\ue161"); }

	else if(iconName == "fa-repeat"){
	  label = _U("\uf363"); }

	else if(iconName == "fa-cross"){
	  label = _U("\uf654"); }

	else if(iconName == "fa-page-caret-down"){
	  label = _U("\ue429"); }

	else if(iconName == "fa-file-caret-down"){
	  label = _U("\ue429"); }

	else if(iconName == "fa-box"){
	  label = _U("\uf466"); }

	else if(iconName == "fa-venus-mars"){
	  label = _U("\uf228"); }

	else if(iconName == "fa-clock-seven-thirty"){
	  label = _U("\ue351"); }

	else if(iconName == "fa-arrow-pointer"){
	  label = _U("\uf245"); }

	else if(iconName == "fa-mouse-pointer"){
	  label = _U("\uf245"); }

	else if(iconName == "fa-clock-four-thirty"){
	  label = _U("\ue34b"); }

	else if(iconName == "fa-signal-bars-good"){
	  label = _U("\uf693"); }

	else if(iconName == "fa-signal-alt-3"){
	  label = _U("\uf693"); }

	else if(iconName == "fa-cactus"){
	  label = _U("\uf8a7"); }

	else if(iconName == "fa-lightbulb-gear"){
	  label = _U("\ue5fd"); }

	else if(iconName == "fa-maximize"){
	  label = _U("\uf31e"); }

	else if(iconName == "fa-expand-arrows-alt"){
	  label = _U("\uf31e"); }

	else if(iconName == "fa-charging-station"){
	  label = _U("\uf5e7"); }

	else if(iconName == "fa-shapes"){
	  label = _U("\uf61f"); }

	else if(iconName == "fa-triangle-circle-square"){
	  label = _U("\uf61f"); }

	else if(iconName == "fa-plane-tail"){
	  label = _U("\ue22c"); }

	else if(iconName == "fa-gauge-simple-max"){
	  label = _U("\uf62b"); }

	else if(iconName == "fa-tachometer-fastest"){
	  label = _U("\uf62b"); }

	else if(iconName == "fa-circle-u"){
	  label = _U("\ue127"); }

	else if(iconName == "fa-shield-slash"){
	  label = _U("\ue24b"); }

	else if(iconName == "fa-square-phone-hangup"){
	  label = _U("\ue27a"); }

	else if(iconName == "fa-phone-square-down"){
	  label = _U("\ue27a"); }

	else if(iconName == "fa-arrow-up-left"){
	  label = _U("\ue09d"); }

	else if(iconName == "fa-transporter-1"){
	  label = _U("\ue043"); }

	else if(iconName == "fa-peanuts"){
	  label = _U("\ue431"); }

	else if(iconName == "fa-shuffle"){
	  label = _U("\uf074"); }

	else if(iconName == "fa-random"){
	  label = _U("\uf074"); }

	else if(iconName == "fa-person-running"){
	  label = _U("\uf70c"); }

	else if(iconName == "fa-running"){
	  label = _U("\uf70c"); }

	else if(iconName == "fa-mobile-retro"){
	  label = _U("\ue527"); }

	else if(iconName == "fa-grip-lines-vertical"){
	  label = _U("\uf7a5"); }

	else if(iconName == "fa-bin-bottles-recycle"){
	  label = _U("\ue5f6"); }

	else if(iconName == "fa-arrow-up-from-square"){
	  label = _U("\ue09c"); }

	else if(iconName == "fa-file-dashed-line"){
	  label = _U("\uf877"); }

	else if(iconName == "fa-page-break"){
	  label = _U("\uf877"); }

	else if(iconName == "fa-bracket-curly-right"){
	  label = _U("\u007d"); }

	else if(iconName == "fa-spider"){
	  label = _U("\uf717"); }

	else if(iconName == "fa-clock-three"){
	  label = _U("\ue356"); }

	else if(iconName == "fa-hands-bound"){
	  label = _U("\ue4f9"); }

	else if(iconName == "fa-scalpel-line-dashed"){
	  label = _U("\uf61e"); }

	else if(iconName == "fa-scalpel-path"){
	  label = _U("\uf61e"); }

	else if(iconName == "fa-file-invoice-dollar"){
	  label = _U("\uf571"); }

	else if(iconName == "fa-pipe-smoking"){
	  label = _U("\ue3c4"); }

	else if(iconName == "fa-face-astonished"){
	  label = _U("\ue36b"); }

	else if(iconName == "fa-window"){
	  label = _U("\uf40e"); }

	else if(iconName == "fa-plane-circle-exclamation"){
	  label = _U("\ue556"); }

	else if(iconName == "fa-ear"){
	  label = _U("\uf5f0"); }

	else if(iconName == "fa-file-lock"){
	  label = _U("\ue3a6"); }

	else if(iconName == "fa-diagram-venn"){
	  label = _U("\ue15a"); }

	else if(iconName == "fa-x-ray"){
	  label = _U("\uf497"); }

	else if(iconName == "fa-goal-net"){
	  label = _U("\ue3ab"); }

	else if(iconName == "fa-coffin-cross"){
	  label = _U("\ue051"); }

	else if(iconName == "fa-spell-check"){
	  label = _U("\uf891"); }

	else if(iconName == "fa-location-xmark"){
	  label = _U("\uf60e"); }

	else if(iconName == "fa-map-marker-times"){
	  label = _U("\uf60e"); }

	else if(iconName == "fa-map-marker-xmark"){
	  label = _U("\uf60e"); }

	else if(iconName == "fa-circle-quarter-stroke"){
	  label = _U("\ue5d3"); }

	else if(iconName == "fa-lasso"){
	  label = _U("\uf8c8"); }

	else if(iconName == "fa-slash"){
	  label = _U("\uf715"); }

	else if(iconName == "fa-person-to-portal"){
	  label = _U("\ue022"); }

	else if(iconName == "fa-portal-enter"){
	  label = _U("\ue022"); }

	else if(iconName == "fa-calendar-star"){
	  label = _U("\uf736"); }

	else if(iconName == "fa-computer-mouse"){
	  label = _U("\uf8cc"); }

	else if(iconName == "fa-mouse"){
	  label = _U("\uf8cc"); }

	else if(iconName == "fa-arrow-right-to-bracket"){
	  label = _U("\uf090"); }

	else if(iconName == "fa-sign-in"){
	  label = _U("\uf090"); }

	else if(iconName == "fa-pegasus"){
	  label = _U("\uf703"); }

	else if(iconName == "fa-files-medical"){
	  label = _U("\uf7fd"); }

	else if(iconName == "fa-cannon"){
	  label = _U("\ue642"); }

	else if(iconName == "fa-nfc-lock"){
	  label = _U("\ue1f8"); }

	else if(iconName == "fa-person-ski-lift"){
	  label = _U("\uf7c8"); }

	else if(iconName == "fa-ski-lift"){
	  label = _U("\uf7c8"); }

	else if(iconName == "fa-square-6"){
	  label = _U("\ue25b"); }

	else if(iconName == "fa-shop-slash"){
	  label = _U("\ue070"); }

	else if(iconName == "fa-store-alt-slash"){
	  label = _U("\ue070"); }

	else if(iconName == "fa-wind-turbine"){
	  label = _U("\uf89b"); }

	else if(iconName == "fa-sliders-simple"){
	  label = _U("\ue253"); }

	else if(iconName == "fa-grid-round"){
	  label = _U("\ue5da"); }

	else if(iconName == "fa-badge-sheriff"){
	  label = _U("\uf8a2"); }

	else if(iconName == "fa-server"){
	  label = _U("\uf233"); }

	else if(iconName == "fa-virus-covid-slash"){
	  label = _U("\ue4a9"); }

	else if(iconName == "fa-intersection"){
	  label = _U("\uf668"); }

	else if(iconName == "fa-shop-lock"){
	  label = _U("\ue4a5"); }

	else if(iconName == "fa-family"){
	  label = _U("\ue300"); }

	else if(iconName == "fa-hourglass-start"){
	  label = _U("\uf251"); }

	else if(iconName == "fa-hourglass-1"){
	  label = _U("\uf251"); }

	else if(iconName == "fa-user-hair-buns"){
	  label = _U("\ue3d3"); }

	else if(iconName == "fa-blender-phone"){
	  label = _U("\uf6b6"); }

	else if(iconName == "fa-hourglass-clock"){
	  label = _U("\ue41b"); }

	else if(iconName == "fa-person-seat-reclined"){
	  label = _U("\ue21f"); }

	else if(iconName == "fa-paper-plane-top"){
	  label = _U("\ue20a"); }

	else if(iconName == "fa-paper-plane-alt"){
	  label = _U("\ue20a"); }

	else if(iconName == "fa-send"){
	  label = _U("\ue20a"); }

	else if(iconName == "fa-message-arrow-up"){
	  label = _U("\ue1dc"); }

	else if(iconName == "fa-comment-alt-arrow-up"){
	  label = _U("\ue1dc"); }

	else if(iconName == "fa-lightbulb-exclamation"){
	  label = _U("\uf671"); }

	else if(iconName == "fa-layer-minus"){
	  label = _U("\uf5fe"); }

	else if(iconName == "fa-layer-group-minus"){
	  label = _U("\uf5fe"); }

	else if(iconName == "fa-chart-pie-simple-circle-currency"){
	  label = _U("\ue604"); }

	else if(iconName == "fa-circle-e"){
	  label = _U("\ue109"); }

	else if(iconName == "fa-building-wheat"){
	  label = _U("\ue4db"); }

	else if(iconName == "fa-gauge-max"){
	  label = _U("\uf626"); }

	else if(iconName == "fa-tachometer-alt-fastest"){
	  label = _U("\uf626"); }

	else if(iconName == "fa-person-breastfeeding"){
	  label = _U("\ue53a"); }

	else if(iconName == "fa-apostrophe"){
	  label = _U("\u0027"); }

	else if(iconName == "fa-file-png"){
	  label = _U("\ue666"); }

	else if(iconName == "fa-fire-hydrant"){
	  label = _U("\ue17f"); }

	else if(iconName == "fa-right-to-bracket"){
	  label = _U("\uf2f6"); }

	else if(iconName == "fa-sign-in-alt"){
	  label = _U("\uf2f6"); }

	else if(iconName == "fa-video-plus"){
	  label = _U("\uf4e1"); }

	else if(iconName == "fa-square-right"){
	  label = _U("\uf352"); }

	else if(iconName == "fa-arrow-alt-square-right"){
	  label = _U("\uf352"); }

	else if(iconName == "fa-comment-smile"){
	  label = _U("\uf4b4"); }

	else if(iconName == "fa-venus"){
	  label = _U("\uf221"); }

	else if(iconName == "fa-passport"){
	  label = _U("\uf5ab"); }

	else if(iconName == "fa-inbox-in"){
	  label = _U("\uf310"); }

	else if(iconName == "fa-inbox-arrow-down"){
	  label = _U("\uf310"); }

	else if(iconName == "fa-heart-pulse"){
	  label = _U("\uf21e"); }

	else if(iconName == "fa-heartbeat"){
	  label = _U("\uf21e"); }

	else if(iconName == "fa-circle-8"){
	  label = _U("\ue0f5"); }

	else if(iconName == "fa-clouds-moon"){
	  label = _U("\uf745"); }

	else if(iconName == "fa-clock-ten-thirty"){
	  label = _U("\ue355"); }

	else if(iconName == "fa-people-carry-box"){
	  label = _U("\uf4ce"); }

	else if(iconName == "fa-people-carry"){
	  label = _U("\uf4ce"); }

	else if(iconName == "fa-folder-user"){
	  label = _U("\ue18e"); }

	else if(iconName == "fa-trash-can-xmark"){
	  label = _U("\ue2ae"); }

	else if(iconName == "fa-temperature-high"){
	  label = _U("\uf769"); }

	else if(iconName == "fa-microchip"){
	  label = _U("\uf2db"); }

	else if(iconName == "fa-left-long-to-line"){
	  label = _U("\ue41e"); }

	else if(iconName == "fa-crown"){
	  label = _U("\uf521"); }

	else if(iconName == "fa-weight-hanging"){
	  label = _U("\uf5cd"); }

	else if(iconName == "fa-xmarks-lines"){
	  label = _U("\ue59a"); }

	else if(iconName == "fa-file-prescription"){
	  label = _U("\uf572"); }

	else if(iconName == "fa-calendar-range"){
	  label = _U("\ue0d6"); }

	else if(iconName == "fa-flower-daffodil"){
	  label = _U("\uf800"); }

	else if(iconName == "fa-hand-back-point-up"){
	  label = _U("\ue1a2"); }

	else if(iconName == "fa-weight-scale"){
	  label = _U("\uf496"); }

	else if(iconName == "fa-weight"){
	  label = _U("\uf496"); }

	else if(iconName == "fa-arrow-up-to-arc"){
	  label = _U("\ue617"); }

	else if(iconName == "fa-star-exclamation"){
	  label = _U("\uf2f3"); }

	else if(iconName == "fa-books"){
	  label = _U("\uf5db"); }

	else if(iconName == "fa-user-group"){
	  label = _U("\uf500"); }

	else if(iconName == "fa-user-friends"){
	  label = _U("\uf500"); }

	else if(iconName == "fa-arrow-up-a-z"){
	  label = _U("\uf15e"); }

	else if(iconName == "fa-sort-alpha-up"){
	  label = _U("\uf15e"); }

	else if(iconName == "fa-layer-plus"){
	  label = _U("\uf5ff"); }

	else if(iconName == "fa-layer-group-plus"){
	  label = _U("\uf5ff"); }

	else if(iconName == "fa-play-pause"){
	  label = _U("\ue22f"); }

	else if(iconName == "fa-block-question"){
	  label = _U("\ue3dd"); }

	else if(iconName == "fa-snooze"){
	  label = _U("\uf880"); }

	else if(iconName == "fa-zzz"){
	  label = _U("\uf880"); }

	else if(iconName == "fa-scanner-image"){
	  label = _U("\uf8f3"); }

	else if(iconName == "fa-tv-retro"){
	  label = _U("\uf401"); }

	else if(iconName == "fa-square-t"){
	  label = _U("\ue280"); }

	else if(iconName == "fa-farm"){
	  label = _U("\uf864"); }

	else if(iconName == "fa-barn-silo"){
	  label = _U("\uf864"); }

	else if(iconName == "fa-chess-knight"){
	  label = _U("\uf441"); }

	else if(iconName == "fa-bars-sort"){
	  label = _U("\ue0ae"); }

	else if(iconName == "fa-pallet-boxes"){
	  label = _U("\uf483"); }

	else if(iconName == "fa-palette-boxes"){
	  label = _U("\uf483"); }

	else if(iconName == "fa-pallet-alt"){
	  label = _U("\uf483"); }

	else if(iconName == "fa-face-laugh-squint"){
	  label = _U("\uf59b"); }

	else if(iconName == "fa-laugh-squint"){
	  label = _U("\uf59b"); }

	else if(iconName == "fa-code-simple"){
	  label = _U("\ue13d"); }

	else if(iconName == "fa-bolt-slash"){
	  label = _U("\ue0b8"); }

	else if(iconName == "fa-panel-fire"){
	  label = _U("\ue42f"); }

	else if(iconName == "fa-binary-circle-check"){
	  label = _U("\ue33c"); }

	else if(iconName == "fa-comment-minus"){
	  label = _U("\uf4b1"); }

	else if(iconName == "fa-burrito"){
	  label = _U("\uf7ed"); }

	else if(iconName == "fa-violin"){
	  label = _U("\uf8ed"); }

	else if(iconName == "fa-objects-column"){
	  label = _U("\ue3c1"); }

	else if(iconName == "fa-square-chevron-down"){
	  label = _U("\uf329"); }

	else if(iconName == "fa-chevron-square-down"){
	  label = _U("\uf329"); }

	else if(iconName == "fa-comment-plus"){
	  label = _U("\uf4b2"); }

	else if(iconName == "fa-triangle-instrument"){
	  label = _U("\uf8e2"); }

	else if(iconName == "fa-triangle-music"){
	  label = _U("\uf8e2"); }

	else if(iconName == "fa-wheelchair"){
	  label = _U("\uf193"); }

	else if(iconName == "fa-user-pilot-tie"){
	  label = _U("\ue2c1"); }

	else if(iconName == "fa-piano-keyboard"){
	  label = _U("\uf8d5"); }

	else if(iconName == "fa-bed-empty"){
	  label = _U("\uf8f9"); }

	else if(iconName == "fa-circle-arrow-up"){
	  label = _U("\uf0aa"); }

	else if(iconName == "fa-arrow-circle-up"){
	  label = _U("\uf0aa"); }

	else if(iconName == "fa-toggle-on"){
	  label = _U("\uf205"); }

	else if(iconName == "fa-rectangle-vertical"){
	  label = _U("\uf2fb"); }

	else if(iconName == "fa-rectangle-portrait"){
	  label = _U("\uf2fb"); }

	else if(iconName == "fa-person-walking"){
	  label = _U("\uf554"); }

	else if(iconName == "fa-walking"){
	  label = _U("\uf554"); }

	else if(iconName == "fa-l"){
	  label = _U("\u004c"); }

	else if(iconName == "fa-signal-stream"){
	  label = _U("\uf8dd"); }

	else if(iconName == "fa-down-to-bracket"){
	  label = _U("\ue4e7"); }

	else if(iconName == "fa-circle-z"){
	  label = _U("\ue130"); }

	else if(iconName == "fa-stars"){
	  label = _U("\uf762"); }

	else if(iconName == "fa-fire"){
	  label = _U("\uf06d"); }

	else if(iconName == "fa-bed-pulse"){
	  label = _U("\uf487"); }

	else if(iconName == "fa-procedures"){
	  label = _U("\uf487"); }

	else if(iconName == "fa-house-day"){
	  label = _U("\ue00e"); }

	else if(iconName == "fa-shuttle-space"){
	  label = _U("\uf197"); }

	else if(iconName == "fa-space-shuttle"){
	  label = _U("\uf197"); }

	else if(iconName == "fa-shirt-long-sleeve"){
	  label = _U("\ue3c7"); }

	else if(iconName == "fa-chart-pie-simple"){
	  label = _U("\uf64e"); }

	else if(iconName == "fa-chart-pie-alt"){
	  label = _U("\uf64e"); }

	else if(iconName == "fa-face-laugh"){
	  label = _U("\uf599"); }

	else if(iconName == "fa-laugh"){
	  label = _U("\uf599"); }

	else if(iconName == "fa-folder-open"){
	  label = _U("\uf07c"); }

	else if(iconName == "fa-album-collection-circle-user"){
	  label = _U("\ue48f"); }

	else if(iconName == "fa-candy"){
	  label = _U("\ue3e7"); }

	else if(iconName == "fa-bowl-hot"){
	  label = _U("\uf823"); }

	else if(iconName == "fa-soup"){
	  label = _U("\uf823"); }

	else if(iconName == "fa-flatbread"){
	  label = _U("\ue40b"); }

	else if(iconName == "fa-heart-circle-plus"){
	  label = _U("\ue500"); }

	else if(iconName == "fa-code-fork"){
	  label = _U("\ue13b"); }

	else if(iconName == "fa-city"){
	  label = _U("\uf64f"); }

	else if(iconName == "fa-signal-bars-weak"){
	  label = _U("\uf691"); }

	else if(iconName == "fa-signal-alt-1"){
	  label = _U("\uf691"); }

	else if(iconName == "fa-microphone-lines"){
	  label = _U("\uf3c9"); }

	else if(iconName == "fa-microphone-alt"){
	  label = _U("\uf3c9"); }

	else if(iconName == "fa-clock-twelve"){
	  label = _U("\ue358"); }

	else if(iconName == "fa-pepper-hot"){
	  label = _U("\uf816"); }

	else if(iconName == "fa-citrus-slice"){
	  label = _U("\ue2f5"); }

	else if(iconName == "fa-sheep"){
	  label = _U("\uf711"); }

	else if(iconName == "fa-unlock"){
	  label = _U("\uf09c"); }

	else if(iconName == "fa-colon-sign"){
	  label = _U("\ue140"); }

	else if(iconName == "fa-headset"){
	  label = _U("\uf590"); }

	else if(iconName == "fa-badger-honey"){
	  label = _U("\uf6b4"); }

	else if(iconName == "fa-h4"){
	  label = _U("\uf86a"); }

	else if(iconName == "fa-store-slash"){
	  label = _U("\ue071"); }

	else if(iconName == "fa-road-circle-xmark"){
	  label = _U("\ue566"); }

	else if(iconName == "fa-signal-slash"){
	  label = _U("\uf695"); }

	else if(iconName == "fa-user-minus"){
	  label = _U("\uf503"); }

	else if(iconName == "fa-mars-stroke-up"){
	  label = _U("\uf22a"); }

	else if(iconName == "fa-mars-stroke-v"){
	  label = _U("\uf22a"); }

	else if(iconName == "fa-champagne-glasses"){
	  label = _U("\uf79f"); }

	else if(iconName == "fa-glass-cheers"){
	  label = _U("\uf79f"); }

	else if(iconName == "fa-taco"){
	  label = _U("\uf826"); }

	else if(iconName == "fa-hexagon-plus"){
	  label = _U("\uf300"); }

	else if(iconName == "fa-plus-hexagon"){
	  label = _U("\uf300"); }

	else if(iconName == "fa-clipboard"){
	  label = _U("\uf328"); }

	else if(iconName == "fa-house-circle-exclamation"){
	  label = _U("\ue50a"); }

	else if(iconName == "fa-file-arrow-up"){
	  label = _U("\uf574"); }

	else if(iconName == "fa-file-upload"){
	  label = _U("\uf574"); }

	else if(iconName == "fa-wifi"){
	  label = _U("\uf1eb"); }

	else if(iconName == "fa-wifi-3"){
	  label = _U("\uf1eb"); }

	else if(iconName == "fa-wifi-strong"){
	  label = _U("\uf1eb"); }

	else if(iconName == "fa-messages"){
	  label = _U("\uf4b6"); }

	else if(iconName == "fa-comments-alt"){
	  label = _U("\uf4b6"); }

	else if(iconName == "fa-bath"){
	  label = _U("\uf2cd"); }

	else if(iconName == "fa-bathtub"){
	  label = _U("\uf2cd"); }

	else if(iconName == "fa-umbrella-simple"){
	  label = _U("\ue2bc"); }

	else if(iconName == "fa-umbrella-alt"){
	  label = _U("\ue2bc"); }

	else if(iconName == "fa-rectangle-history-circle-plus"){
	  label = _U("\ue4a3"); }

	else if(iconName == "fa-underline"){
	  label = _U("\uf0cd"); }

	else if(iconName == "fa-prescription-bottle-pill"){
	  label = _U("\ue5c0"); }

	else if(iconName == "fa-user-pen"){
	  label = _U("\uf4ff"); }

	else if(iconName == "fa-user-edit"){
	  label = _U("\uf4ff"); }

	else if(iconName == "fa-binary-slash"){
	  label = _U("\ue33e"); }

	else if(iconName == "fa-square-o"){
	  label = _U("\ue278"); }

	else if(iconName == "fa-signature"){
	  label = _U("\uf5b7"); }

	else if(iconName == "fa-stroopwafel"){
	  label = _U("\uf551"); }

	else if(iconName == "fa-bold"){
	  label = _U("\uf032"); }

	else if(iconName == "fa-anchor-lock"){
	  label = _U("\ue4ad"); }

	else if(iconName == "fa-building-ngo"){
	  label = _U("\ue4d7"); }

	else if(iconName == "fa-transporter-3"){
	  label = _U("\ue045"); }

	else if(iconName == "fa-engine-warning"){
	  label = _U("\uf5f2"); }

	else if(iconName == "fa-engine-exclamation"){
	  label = _U("\uf5f2"); }

	else if(iconName == "fa-circle-down-right"){
	  label = _U("\ue108"); }

	else if(iconName == "fa-square-k"){
	  label = _U("\ue274"); }

	else if(iconName == "fa-manat-sign"){
	  label = _U("\ue1d5"); }

	else if(iconName == "fa-money-check-pen"){
	  label = _U("\uf872"); }

	else if(iconName == "fa-money-check-edit"){
	  label = _U("\uf872"); }

	else if(iconName == "fa-not-equal"){
	  label = _U("\uf53e"); }

	else if(iconName == "fa-border-top-left"){
	  label = _U("\uf853"); }

	else if(iconName == "fa-border-style"){
	  label = _U("\uf853"); }

	else if(iconName == "fa-map-location-dot"){
	  label = _U("\uf5a0"); }

	else if(iconName == "fa-map-marked-alt"){
	  label = _U("\uf5a0"); }

	else if(iconName == "fa-tilde"){
	  label = _U("\u007e"); }

	else if(iconName == "fa-jedi"){
	  label = _U("\uf669"); }

	else if(iconName == "fa-square-poll-vertical"){
	  label = _U("\uf681"); }

	else if(iconName == "fa-poll"){
	  label = _U("\uf681"); }

	else if(iconName == "fa-arrow-down-square-triangle"){
	  label = _U("\uf889"); }

	else if(iconName == "fa-sort-shapes-down-alt"){
	  label = _U("\uf889"); }

	else if(iconName == "fa-mug-hot"){
	  label = _U("\uf7b6"); }

	else if(iconName == "fa-dog-leashed"){
	  label = _U("\uf6d4"); }

	else if(iconName == "fa-car-battery"){
	  label = _U("\uf5df"); }

	else if(iconName == "fa-battery-car"){
	  label = _U("\uf5df"); }

	else if(iconName == "fa-face-downcast-sweat"){
	  label = _U("\ue371"); }

	else if(iconName == "fa-mailbox-flag-up"){
	  label = _U("\ue5bb"); }

	else if(iconName == "fa-memo-circle-info"){
	  label = _U("\ue49a"); }

	else if(iconName == "fa-gift"){
	  label = _U("\uf06b"); }

	else if(iconName == "fa-dice-two"){
	  label = _U("\uf528"); }

	else if(iconName == "fa-volume"){
	  label = _U("\uf6a8"); }

	else if(iconName == "fa-volume-medium"){
	  label = _U("\uf6a8"); }

	else if(iconName == "fa-transporter-5"){
	  label = _U("\ue2a6"); }

	else if(iconName == "fa-gauge-circle-bolt"){
	  label = _U("\ue496"); }

	else if(iconName == "fa-coin-front"){
	  label = _U("\ue3fc"); }

	else if(iconName == "fa-file-slash"){
	  label = _U("\ue3a7"); }

	else if(iconName == "fa-message-arrow-up-right"){
	  label = _U("\ue1dd"); }

	else if(iconName == "fa-treasure-chest"){
	  label = _U("\uf723"); }

	else if(iconName == "fa-chess-queen"){
	  label = _U("\uf445"); }

	else if(iconName == "fa-paintbrush-fine"){
	  label = _U("\uf5a9"); }

	else if(iconName == "fa-paint-brush-alt"){
	  label = _U("\uf5a9"); }

	else if(iconName == "fa-paint-brush-fine"){
	  label = _U("\uf5a9"); }

	else if(iconName == "fa-paintbrush-alt"){
	  label = _U("\uf5a9"); }

	else if(iconName == "fa-glasses"){
	  label = _U("\uf530"); }

	else if(iconName == "fa-hood-cloak"){
	  label = _U("\uf6ef"); }

	else if(iconName == "fa-square-quote"){
	  label = _U("\ue329"); }

	else if(iconName == "fa-up-left"){
	  label = _U("\ue2bd"); }

	else if(iconName == "fa-bring-front"){
	  label = _U("\uf857"); }

	else if(iconName == "fa-chess-board"){
	  label = _U("\uf43c"); }

	else if(iconName == "fa-burger-cheese"){
	  label = _U("\uf7f1"); }

	else if(iconName == "fa-cheeseburger"){
	  label = _U("\uf7f1"); }

	else if(iconName == "fa-building-circle-check"){
	  label = _U("\ue4d2"); }

	else if(iconName == "fa-repeat-1"){
	  label = _U("\uf365"); }

	else if(iconName == "fa-arrow-down-to-line"){
	  label = _U("\uf33d"); }

	else if(iconName == "fa-arrow-to-bottom"){
	  label = _U("\uf33d"); }

	else if(iconName == "fa-grid-5"){
	  label = _U("\ue199"); }

	else if(iconName == "fa-swap-arrows"){
	  label = _U("\ue60a"); }

	else if(iconName == "fa-right-long-to-line"){
	  label = _U("\ue444"); }

	else if(iconName == "fa-person-chalkboard"){
	  label = _U("\ue53d"); }

	else if(iconName == "fa-mars-stroke-right"){
	  label = _U("\uf22b"); }

	else if(iconName == "fa-mars-stroke-h"){
	  label = _U("\uf22b"); }

	else if(iconName == "fa-hand-back-fist"){
	  label = _U("\uf255"); }

	else if(iconName == "fa-hand-rock"){
	  label = _U("\uf255"); }

	else if(iconName == "fa-grid-round-5"){
	  label = _U("\ue5de"); }

	else if(iconName == "fa-tally"){
	  label = _U("\uf69c"); }

	else if(iconName == "fa-tally-5"){
	  label = _U("\uf69c"); }

	else if(iconName == "fa-square-caret-up"){
	  label = _U("\uf151"); }

	else if(iconName == "fa-caret-square-up"){
	  label = _U("\uf151"); }

	else if(iconName == "fa-cloud-showers-water"){
	  label = _U("\ue4e4"); }

	else if(iconName == "fa-chart-bar"){
	  label = _U("\uf080"); }

	else if(iconName == "fa-bar-chart"){
	  label = _U("\uf080"); }

	else if(iconName == "fa-hands-bubbles"){
	  label = _U("\ue05e"); }

	else if(iconName == "fa-hands-wash"){
	  label = _U("\ue05e"); }

	else if(iconName == "fa-less-than-equal"){
	  label = _U("\uf537"); }

	else if(iconName == "fa-train"){
	  label = _U("\uf238"); }

	else if(iconName == "fa-up-from-dotted-line"){
	  label = _U("\ue456"); }

	else if(iconName == "fa-eye-low-vision"){
	  label = _U("\uf2a8"); }

	else if(iconName == "fa-low-vision"){
	  label = _U("\uf2a8"); }

	else if(iconName == "fa-traffic-light-go"){
	  label = _U("\uf638"); }

	else if(iconName == "fa-face-exhaling"){
	  label = _U("\ue480"); }

	else if(iconName == "fa-sensor-fire"){
	  label = _U("\ue02a"); }

	else if(iconName == "fa-user-unlock"){
	  label = _U("\ue058"); }

	else if(iconName == "fa-hexagon-divide"){
	  label = _U("\ue1ad"); }

	else if(iconName == "fa-00"){
	  label = _U("\ue467"); }

	else if(iconName == "fa-crow"){
	  label = _U("\uf520"); }

	else if(iconName == "fa-cassette-betamax"){
	  label = _U("\uf8a4"); }

	else if(iconName == "fa-betamax"){
	  label = _U("\uf8a4"); }

	else if(iconName == "fa-sailboat"){
	  label = _U("\ue445"); }

	else if(iconName == "fa-window-restore"){
	  label = _U("\uf2d2"); }

	else if(iconName == "fa-nfc-magnifying-glass"){
	  label = _U("\ue1f9"); }

	else if(iconName == "fa-file-binary"){
	  label = _U("\ue175"); }

	else if(iconName == "fa-circle-v"){
	  label = _U("\ue12a"); }

	else if(iconName == "fa-square-plus"){
	  label = _U("\uf0fe"); }

	else if(iconName == "fa-plus-square"){
	  label = _U("\uf0fe"); }

	else if(iconName == "fa-bowl-scoops"){
	  label = _U("\ue3df"); }

	else if(iconName == "fa-mistletoe"){
	  label = _U("\uf7b4"); }

	else if(iconName == "fa-custard"){
	  label = _U("\ue403"); }

	else if(iconName == "fa-lacrosse-stick"){
	  label = _U("\ue3b5"); }

	else if(iconName == "fa-hockey-mask"){
	  label = _U("\uf6ee"); }

	else if(iconName == "fa-sunrise"){
	  label = _U("\uf766"); }

	else if(iconName == "fa-subtitles"){
	  label = _U("\ue60f"); }

	else if(iconName == "fa-panel-ews"){
	  label = _U("\ue42e"); }

	else if(iconName == "fa-torii-gate"){
	  label = _U("\uf6a1"); }

	else if(iconName == "fa-cloud-exclamation"){
	  label = _U("\ue491"); }

	else if(iconName == "fa-message-lines"){
	  label = _U("\uf4a6"); }

	else if(iconName == "fa-comment-alt-lines"){
	  label = _U("\uf4a6"); }

	else if(iconName == "fa-frog"){
	  label = _U("\uf52e"); }

	else if(iconName == "fa-bucket"){
	  label = _U("\ue4cf"); }

	else if(iconName == "fa-floppy-disk-pen"){
	  label = _U("\ue182"); }

	else if(iconName == "fa-image"){
	  label = _U("\uf03e"); }

	else if(iconName == "fa-window-frame"){
	  label = _U("\ue04f"); }

	else if(iconName == "fa-microphone"){
	  label = _U("\uf130"); }

	else if(iconName == "fa-cow"){
	  label = _U("\uf6c8"); }

	else if(iconName == "fa-file-zip"){
	  label = _U("\ue5ee"); }

	else if(iconName == "fa-square-ring"){
	  label = _U("\ue44f"); }

	else if(iconName == "fa-down-from-line"){
	  label = _U("\uf349"); }

	else if(iconName == "fa-arrow-alt-from-top"){
	  label = _U("\uf349"); }

	else if(iconName == "fa-caret-up"){
	  label = _U("\uf0d8"); }

	else if(iconName == "fa-shield-xmark"){
	  label = _U("\ue24c"); }

	else if(iconName == "fa-shield-times"){
	  label = _U("\ue24c"); }

	else if(iconName == "fa-screwdriver"){
	  label = _U("\uf54a"); }

	else if(iconName == "fa-circle-sort-down"){
	  label = _U("\ue031"); }

	else if(iconName == "fa-sort-circle-down"){
	  label = _U("\ue031"); }

	else if(iconName == "fa-folder-closed"){
	  label = _U("\ue185"); }

	else if(iconName == "fa-house-tsunami"){
	  label = _U("\ue515"); }

	else if(iconName == "fa-square-nfi"){
	  label = _U("\ue576"); }

	else if(iconName == "fa-forklift"){
	  label = _U("\uf47a"); }

	else if(iconName == "fa-arrow-up-from-ground-water"){
	  label = _U("\ue4b5"); }

	else if(iconName == "fa-bracket-square-right"){
	  label = _U("\u005d"); }

	else if(iconName == "fa-martini-glass"){
	  label = _U("\uf57b"); }

	else if(iconName == "fa-glass-martini-alt"){
	  label = _U("\uf57b"); }

	else if(iconName == "fa-rotate-left"){
	  label = _U("\uf2ea"); }

	else if(iconName == "fa-rotate-back"){
	  label = _U("\uf2ea"); }

	else if(iconName == "fa-rotate-backward"){
	  label = _U("\uf2ea"); }

	else if(iconName == "fa-undo-alt"){
	  label = _U("\uf2ea"); }

	else if(iconName == "fa-table-columns"){
	  label = _U("\uf0db"); }

	else if(iconName == "fa-columns"){
	  label = _U("\uf0db"); }

	else if(iconName == "fa-square-a"){
	  label = _U("\ue25f"); }

	else if(iconName == "fa-tick"){
	  label = _U("\ue32f"); }

	else if(iconName == "fa-lemon"){
	  label = _U("\uf094"); }

	else if(iconName == "fa-head-side-mask"){
	  label = _U("\ue063"); }

	else if(iconName == "fa-handshake"){
	  label = _U("\uf2b5"); }

	else if(iconName == "fa-gem"){
	  label = _U("\uf3a5"); }

	else if(iconName == "fa-dolly"){
	  label = _U("\uf472"); }

	else if(iconName == "fa-dolly-box"){
	  label = _U("\uf472"); }

	else if(iconName == "fa-smoking"){
	  label = _U("\uf48d"); }

	else if(iconName == "fa-minimize"){
	  label = _U("\uf78c"); }

	else if(iconName == "fa-compress-arrows-alt"){
	  label = _U("\uf78c"); }

	else if(iconName == "fa-refrigerator"){
	  label = _U("\ue026"); }

	else if(iconName == "fa-monument"){
	  label = _U("\uf5a6"); }

	else if(iconName == "fa-octagon-xmark"){
	  label = _U("\uf2f0"); }

	else if(iconName == "fa-times-octagon"){
	  label = _U("\uf2f0"); }

	else if(iconName == "fa-xmark-octagon"){
	  label = _U("\uf2f0"); }

	else if(iconName == "fa-align-slash"){
	  label = _U("\uf846"); }

	else if(iconName == "fa-snowplow"){
	  label = _U("\uf7d2"); }

	else if(iconName == "fa-angles-right"){
	  label = _U("\uf101"); }

	else if(iconName == "fa-angle-double-right"){
	  label = _U("\uf101"); }

	else if(iconName == "fa-truck-ramp-couch"){
	  label = _U("\uf4dd"); }

	else if(iconName == "fa-truck-couch"){
	  label = _U("\uf4dd"); }

	else if(iconName == "fa-cannabis"){
	  label = _U("\uf55f"); }

	else if(iconName == "fa-circle-play"){
	  label = _U("\uf144"); }

	else if(iconName == "fa-play-circle"){
	  label = _U("\uf144"); }

	else if(iconName == "fa-arrow-up-right-and-arrow-down-left-from-center"){
	  label = _U("\ue0a0"); }

	else if(iconName == "fa-location-arrow-up"){
	  label = _U("\ue63a"); }

	else if(iconName == "fa-tablets"){
	  label = _U("\uf490"); }

	else if(iconName == "fa-360-degrees"){
	  label = _U("\ue2dc"); }

	else if(iconName == "fa-ethernet"){
	  label = _U("\uf796"); }

	else if(iconName == "fa-euro-sign"){
	  label = _U("\uf153"); }

	else if(iconName == "fa-eur"){
	  label = _U("\uf153"); }

	else if(iconName == "fa-euro"){
	  label = _U("\uf153"); }

	else if(iconName == "fa-chair"){
	  label = _U("\uf6c0"); }

	else if(iconName == "fa-circle-check"){
	  label = _U("\uf058"); }

	else if(iconName == "fa-check-circle"){
	  label = _U("\uf058"); }

	else if(iconName == "fa-square-dashed-circle-plus"){
	  label = _U("\ue5c2"); }

	else if(iconName == "fa-hand-holding-circle-dollar"){
	  label = _U("\ue621"); }

	else if(iconName == "fa-money-simple-from-bracket"){
	  label = _U("\ue313"); }

	else if(iconName == "fa-bat"){
	  label = _U("\uf6b5"); }

	else if(iconName == "fa-circle-stop"){
	  label = _U("\uf28d"); }

	else if(iconName == "fa-stop-circle"){
	  label = _U("\uf28d"); }

	else if(iconName == "fa-head-side-headphones"){
	  label = _U("\uf8c2"); }

	else if(iconName == "fa-phone-rotary"){
	  label = _U("\uf8d3"); }

	else if(iconName == "fa-compass-drafting"){
	  label = _U("\uf568"); }

	else if(iconName == "fa-drafting-compass"){
	  label = _U("\uf568"); }

	else if(iconName == "fa-plate-wheat"){
	  label = _U("\ue55a"); }

	else if(iconName == "fa-calendar-circle-minus"){
	  label = _U("\ue46f"); }

	else if(iconName == "fa-chopsticks"){
	  label = _U("\ue3f7"); }

	else if(iconName == "fa-car-wrench"){
	  label = _U("\uf5e3"); }

	else if(iconName == "fa-car-mechanic"){
	  label = _U("\uf5e3"); }

	else if(iconName == "fa-icicles"){
	  label = _U("\uf7ad"); }

	else if(iconName == "fa-person-shelter"){
	  label = _U("\ue54f"); }

	else if(iconName == "fa-neuter"){
	  label = _U("\uf22c"); }

	else if(iconName == "fa-id-badge"){
	  label = _U("\uf2c1"); }

	else if(iconName == "fa-kazoo"){
	  label = _U("\uf8c7"); }

	else if(iconName == "fa-marker"){
	  label = _U("\uf5a1"); }

	else if(iconName == "fa-bin-bottles"){
	  label = _U("\ue5f5"); }

	else if(iconName == "fa-face-laugh-beam"){
	  label = _U("\uf59a"); }

	else if(iconName == "fa-laugh-beam"){
	  label = _U("\uf59a"); }

	else if(iconName == "fa-square-arrow-down-left"){
	  label = _U("\ue261"); }

	else if(iconName == "fa-battery-bolt"){
	  label = _U("\uf376"); }

	else if(iconName == "fa-tree-large"){
	  label = _U("\uf7dd"); }

	else if(iconName == "fa-helicopter-symbol"){
	  label = _U("\ue502"); }

	else if(iconName == "fa-aperture"){
	  label = _U("\ue2df"); }

	else if(iconName == "fa-universal-access"){
	  label = _U("\uf29a"); }

	else if(iconName == "fa-gear-complex"){
	  label = _U("\ue5e9"); }

	else if(iconName == "fa-file-magnifying-glass"){
	  label = _U("\uf865"); }

	else if(iconName == "fa-file-search"){
	  label = _U("\uf865"); }

	else if(iconName == "fa-up-right"){
	  label = _U("\ue2be"); }

	else if(iconName == "fa-circle-chevron-up"){
	  label = _U("\uf139"); }

	else if(iconName == "fa-chevron-circle-up"){
	  label = _U("\uf139"); }

	else if(iconName == "fa-user-police"){
	  label = _U("\ue333"); }

	else if(iconName == "fa-lari-sign"){
	  label = _U("\ue1c8"); }

	else if(iconName == "fa-volcano"){
	  label = _U("\uf770"); }

	else if(iconName == "fa-teddy-bear"){
	  label = _U("\ue3cf"); }

	else if(iconName == "fa-stocking"){
	  label = _U("\uf7d5"); }

	else if(iconName == "fa-person-walking-dashed-line-arrow-right"){
	  label = _U("\ue553"); }

	else if(iconName == "fa-image-slash"){
	  label = _U("\ue1b7"); }

	else if(iconName == "fa-mask-snorkel"){
	  label = _U("\ue3b7"); }

	else if(iconName == "fa-smoke"){
	  label = _U("\uf760"); }

	else if(iconName == "fa-sterling-sign"){
	  label = _U("\uf154"); }

	else if(iconName == "fa-gbp"){
	  label = _U("\uf154"); }

	else if(iconName == "fa-pound-sign"){
	  label = _U("\uf154"); }

	else if(iconName == "fa-battery-exclamation"){
	  label = _U("\ue0b0"); }

	else if(iconName == "fa-viruses"){
	  label = _U("\ue076"); }

	else if(iconName == "fa-square-person-confined"){
	  label = _U("\ue577"); }

	else if(iconName == "fa-user-tie"){
	  label = _U("\uf508"); }

	else if(iconName == "fa-arrow-down-long"){
	  label = _U("\uf175"); }

	else if(iconName == "fa-long-arrow-down"){
	  label = _U("\uf175"); }

	else if(iconName == "fa-tent-arrow-down-to-line"){
	  label = _U("\ue57e"); }

	else if(iconName == "fa-certificate"){
	  label = _U("\uf0a3"); }

	else if(iconName == "fa-crystal-ball"){
	  label = _U("\ue362"); }

	else if(iconName == "fa-reply-all"){
	  label = _U("\uf122"); }

	else if(iconName == "fa-mail-reply-all"){
	  label = _U("\uf122"); }

	else if(iconName == "fa-suitcase"){
	  label = _U("\uf0f2"); }

	else if(iconName == "fa-person-skating"){
	  label = _U("\uf7c5"); }

	else if(iconName == "fa-skating"){
	  label = _U("\uf7c5"); }

	else if(iconName == "fa-star-shooting"){
	  label = _U("\ue036"); }

	else if(iconName == "fa-binary-lock"){
	  label = _U("\ue33d"); }

	else if(iconName == "fa-filter-circle-dollar"){
	  label = _U("\uf662"); }

	else if(iconName == "fa-funnel-dollar"){
	  label = _U("\uf662"); }

	else if(iconName == "fa-camera-retro"){
	  label = _U("\uf083"); }

	else if(iconName == "fa-circle-arrow-down"){
	  label = _U("\uf0ab"); }

	else if(iconName == "fa-arrow-circle-down"){
	  label = _U("\uf0ab"); }

	else if(iconName == "fa-comment-pen"){
	  label = _U("\uf4ae"); }

	else if(iconName == "fa-comment-edit"){
	  label = _U("\uf4ae"); }

	else if(iconName == "fa-file-import"){
	  label = _U("\uf56f"); }

	else if(iconName == "fa-arrow-right-to-file"){
	  label = _U("\uf56f"); }

	else if(iconName == "fa-banjo"){
	  label = _U("\uf8a3"); }

	else if(iconName == "fa-square-arrow-up-right"){
	  label = _U("\uf14c"); }

	else if(iconName == "fa-external-link-square"){
	  label = _U("\uf14c"); }

	else if(iconName == "fa-light-emergency-on"){
	  label = _U("\ue420"); }

	else if(iconName == "fa-kerning"){
	  label = _U("\uf86f"); }

	else if(iconName == "fa-box-open"){
	  label = _U("\uf49e"); }

	else if(iconName == "fa-square-f"){
	  label = _U("\ue270"); }

	else if(iconName == "fa-scroll"){
	  label = _U("\uf70e"); }

	else if(iconName == "fa-spa"){
	  label = _U("\uf5bb"); }

	else if(iconName == "fa-arrow-left-from-line"){
	  label = _U("\uf344"); }

	else if(iconName == "fa-arrow-from-right"){
	  label = _U("\uf344"); }

	else if(iconName == "fa-strawberry"){
	  label = _U("\ue32b"); }

	else if(iconName == "fa-location-pin-lock"){
	  label = _U("\ue51f"); }

	else if(iconName == "fa-pause"){
	  label = _U("\uf04c"); }

	else if(iconName == "fa-clock-eight-thirty"){
	  label = _U("\ue346"); }

	else if(iconName == "fa-plane-engines"){
	  label = _U("\uf3de"); }

	else if(iconName == "fa-plane-alt"){
	  label = _U("\uf3de"); }

	else if(iconName == "fa-hill-avalanche"){
	  label = _U("\ue507"); }

	else if(iconName == "fa-temperature-empty"){
	  label = _U("\uf2cb"); }

	else if(iconName == "fa-temperature-0"){
	  label = _U("\uf2cb"); }

	else if(iconName == "fa-thermometer-0"){
	  label = _U("\uf2cb"); }

	else if(iconName == "fa-thermometer-empty"){
	  label = _U("\uf2cb"); }

	else if(iconName == "fa-bomb"){
	  label = _U("\uf1e2"); }

	else if(iconName == "fa-gauge-low"){
	  label = _U("\uf627"); }

	else if(iconName == "fa-tachometer-alt-slow"){
	  label = _U("\uf627"); }

	else if(iconName == "fa-registered"){
	  label = _U("\uf25d"); }

	else if(iconName == "fa-trash-can-plus"){
	  label = _U("\ue2ac"); }

	else if(iconName == "fa-address-card"){
	  label = _U("\uf2bb"); }

	else if(iconName == "fa-contact-card"){
	  label = _U("\uf2bb"); }

	else if(iconName == "fa-vcard"){
	  label = _U("\uf2bb"); }

	else if(iconName == "fa-scale-unbalanced-flip"){
	  label = _U("\uf516"); }

	else if(iconName == "fa-balance-scale-right"){
	  label = _U("\uf516"); }

	else if(iconName == "fa-globe-snow"){
	  label = _U("\uf7a3"); }

	else if(iconName == "fa-subscript"){
	  label = _U("\uf12c"); }

	else if(iconName == "fa-diamond-turn-right"){
	  label = _U("\uf5eb"); }

	else if(iconName == "fa-directions"){
	  label = _U("\uf5eb"); }

	else if(iconName == "fa-integral"){
	  label = _U("\uf667"); }

	else if(iconName == "fa-burst"){
	  label = _U("\ue4dc"); }

	else if(iconName == "fa-house-laptop"){
	  label = _U("\ue066"); }

	else if(iconName == "fa-laptop-house"){
	  label = _U("\ue066"); }

	else if(iconName == "fa-face-tired"){
	  label = _U("\uf5c8"); }

	else if(iconName == "fa-tired"){
	  label = _U("\uf5c8"); }

	else if(iconName == "fa-money-bills"){
	  label = _U("\ue1f3"); }

	else if(iconName == "fa-blinds-raised"){
	  label = _U("\uf8fd"); }

	else if(iconName == "fa-smog"){
	  label = _U("\uf75f"); }

	else if(iconName == "fa-ufo-beam"){
	  label = _U("\ue048"); }

	else if(iconName == "fa-circle-caret-up"){
	  label = _U("\uf331"); }

	else if(iconName == "fa-caret-circle-up"){
	  label = _U("\uf331"); }

	else if(iconName == "fa-user-vneck-hair-long"){
	  label = _U("\ue463"); }

	else if(iconName == "fa-square-a-lock"){
	  label = _U("\ue44d"); }

	else if(iconName == "fa-crutch"){
	  label = _U("\uf7f7"); }

	else if(iconName == "fa-gas-pump-slash"){
	  label = _U("\uf5f4"); }

	else if(iconName == "fa-cloud-arrow-up"){
	  label = _U("\uf0ee"); }

	else if(iconName == "fa-cloud-upload"){
	  label = _U("\uf0ee"); }

	else if(iconName == "fa-cloud-upload-alt"){
	  label = _U("\uf0ee"); }

	else if(iconName == "fa-palette"){
	  label = _U("\uf53f"); }

	else if(iconName == "fa-transporter-4"){
	  label = _U("\ue2a5"); }

	else if(iconName == "fa-chart-mixed-up-circle-currency"){
	  label = _U("\ue5d8"); }

	else if(iconName == "fa-objects-align-right"){
	  label = _U("\ue3bf"); }

	else if(iconName == "fa-arrows-turn-right"){
	  label = _U("\ue4c0"); }

	else if(iconName == "fa-vest"){
	  label = _U("\ue085"); }

	else if(iconName == "fa-pig"){
	  label = _U("\uf706"); }

	else if(iconName == "fa-inbox-full"){
	  label = _U("\ue1ba"); }

	else if(iconName == "fa-circle-envelope"){
	  label = _U("\ue10c"); }

	else if(iconName == "fa-envelope-circle"){
	  label = _U("\ue10c"); }

	else if(iconName == "fa-triangle-person-digging"){
	  label = _U("\uf85d"); }

	else if(iconName == "fa-construction"){
	  label = _U("\uf85d"); }

	else if(iconName == "fa-ferry"){
	  label = _U("\ue4ea"); }

	else if(iconName == "fa-bullseye-arrow"){
	  label = _U("\uf648"); }

	else if(iconName == "fa-arrows-down-to-people"){
	  label = _U("\ue4b9"); }

	else if(iconName == "fa-seedling"){
	  label = _U("\uf4d8"); }

	else if(iconName == "fa-sprout"){
	  label = _U("\uf4d8"); }

	else if(iconName == "fa-clock-seven"){
	  label = _U("\ue350"); }

	else if(iconName == "fa-left-right"){
	  label = _U("\uf337"); }

	else if(iconName == "fa-arrows-alt-h"){
	  label = _U("\uf337"); }

	else if(iconName == "fa-boxes-packing"){
	  label = _U("\ue4c7"); }

	else if(iconName == "fa-circle-arrow-left"){
	  label = _U("\uf0a8"); }

	else if(iconName == "fa-arrow-circle-left"){
	  label = _U("\uf0a8"); }

	else if(iconName == "fa-flashlight"){
	  label = _U("\uf8b8"); }

	else if(iconName == "fa-file-jpg"){
	  label = _U("\ue646"); }

	else if(iconName == "fa-group-arrows-rotate"){
	  label = _U("\ue4f6"); }

	else if(iconName == "fa-bowl-food"){
	  label = _U("\ue4c6"); }

	else if(iconName == "fa-square-9"){
	  label = _U("\ue25e"); }

	else if(iconName == "fa-candy-cane"){
	  label = _U("\uf786"); }

	else if(iconName == "fa-arrow-down-wide-short"){
	  label = _U("\uf160"); }

	else if(iconName == "fa-sort-amount-asc"){
	  label = _U("\uf160"); }

	else if(iconName == "fa-sort-amount-down"){
	  label = _U("\uf160"); }

	else if(iconName == "fa-square-dollar"){
	  label = _U("\uf2e9"); }

	else if(iconName == "fa-dollar-square"){
	  label = _U("\uf2e9"); }

	else if(iconName == "fa-usd-square"){
	  label = _U("\uf2e9"); }

	else if(iconName == "fa-phone-arrow-right"){
	  label = _U("\ue5be"); }

	else if(iconName == "fa-hand-holding-seedling"){
	  label = _U("\uf4bf"); }

	else if(iconName == "fa-message-check"){
	  label = _U("\uf4a2"); }

	else if(iconName == "fa-comment-alt-check"){
	  label = _U("\uf4a2"); }

	else if(iconName == "fa-cloud-bolt"){
	  label = _U("\uf76c"); }

	else if(iconName == "fa-thunderstorm"){
	  label = _U("\uf76c"); }

	else if(iconName == "fa-chart-line-up-down"){
	  label = _U("\ue5d7"); }

	else if(iconName == "fa-text-slash"){
	  label = _U("\uf87d"); }

	else if(iconName == "fa-remove-format"){
	  label = _U("\uf87d"); }

	else if(iconName == "fa-watch"){
	  label = _U("\uf2e1"); }

	else if(iconName == "fa-circle-down-left"){
	  label = _U("\ue107"); }

	else if(iconName == "fa-text"){
	  label = _U("\uf893"); }

	else if(iconName == "fa-projector"){
	  label = _U("\uf8d6"); }

	else if(iconName == "fa-face-smile-wink"){
	  label = _U("\uf4da"); }

	else if(iconName == "fa-smile-wink"){
	  label = _U("\uf4da"); }

	else if(iconName == "fa-tombstone-blank"){
	  label = _U("\uf721"); }

	else if(iconName == "fa-tombstone-alt"){
	  label = _U("\uf721"); }

	else if(iconName == "fa-chess-king-piece"){
	  label = _U("\uf440"); }

	else if(iconName == "fa-chess-king-alt"){
	  label = _U("\uf440"); }

	else if(iconName == "fa-circle-6"){
	  label = _U("\ue0f3"); }

	else if(iconName == "fa-waves-sine"){
	  label = _U("\ue65d"); }

	else if(iconName == "fa-left"){
	  label = _U("\uf355"); }

	else if(iconName == "fa-arrow-alt-left"){
	  label = _U("\uf355"); }

	else if(iconName == "fa-file-word"){
	  label = _U("\uf1c2"); }

	else if(iconName == "fa-file-powerpoint"){
	  label = _U("\uf1c4"); }

	else if(iconName == "fa-square-down"){
	  label = _U("\uf350"); }

	else if(iconName == "fa-arrow-alt-square-down"){
	  label = _U("\uf350"); }

	else if(iconName == "fa-objects-align-center-vertical"){
	  label = _U("\ue3bd"); }

	else if(iconName == "fa-arrows-left-right"){
	  label = _U("\uf07e"); }

	else if(iconName == "fa-arrows-h"){
	  label = _U("\uf07e"); }

	else if(iconName == "fa-house-lock"){
	  label = _U("\ue510"); }

	else if(iconName == "fa-cloud-arrow-down"){
	  label = _U("\uf0ed"); }

	else if(iconName == "fa-cloud-download"){
	  label = _U("\uf0ed"); }

	else if(iconName == "fa-cloud-download-alt"){
	  label = _U("\uf0ed"); }

	else if(iconName == "fa-wreath"){
	  label = _U("\uf7e2"); }

	else if(iconName == "fa-children"){
	  label = _U("\ue4e1"); }

	else if(iconName == "fa-meter-droplet"){
	  label = _U("\ue1ea"); }

	else if(iconName == "fa-chalkboard"){
	  label = _U("\uf51b"); }

	else if(iconName == "fa-blackboard"){
	  label = _U("\uf51b"); }

	else if(iconName == "fa-user-large-slash"){
	  label = _U("\uf4fa"); }

	else if(iconName == "fa-user-alt-slash"){
	  label = _U("\uf4fa"); }

	else if(iconName == "fa-signal-strong"){
	  label = _U("\uf68f"); }

	else if(iconName == "fa-signal-4"){
	  label = _U("\uf68f"); }

	else if(iconName == "fa-lollipop"){
	  label = _U("\ue424"); }

	else if(iconName == "fa-lollypop"){
	  label = _U("\ue424"); }

	else if(iconName == "fa-list-tree"){
	  label = _U("\ue1d2"); }

	else if(iconName == "fa-envelope-open"){
	  label = _U("\uf2b6"); }

	else if(iconName == "fa-draw-circle"){
	  label = _U("\uf5ed"); }

	else if(iconName == "fa-cat-space"){
	  label = _U("\ue001"); }

	else if(iconName == "fa-handshake-simple-slash"){
	  label = _U("\ue05f"); }

	else if(iconName == "fa-handshake-alt-slash"){
	  label = _U("\ue05f"); }

	else if(iconName == "fa-rabbit-running"){
	  label = _U("\uf709"); }

	else if(iconName == "fa-rabbit-fast"){
	  label = _U("\uf709"); }

	else if(iconName == "fa-memo-pad"){
	  label = _U("\ue1da"); }

	else if(iconName == "fa-mattress-pillow"){
	  label = _U("\ue525"); }

	else if(iconName == "fa-alarm-plus"){
	  label = _U("\uf844"); }

	else if(iconName == "fa-alicorn"){
	  label = _U("\uf6b0"); }

	else if(iconName == "fa-comment-question"){
	  label = _U("\ue14b"); }

	else if(iconName == "fa-gingerbread-man"){
	  label = _U("\uf79d"); }

	else if(iconName == "fa-guarani-sign"){
	  label = _U("\ue19a"); }

	else if(iconName == "fa-burger-fries"){
	  label = _U("\ue0cd"); }

	else if(iconName == "fa-mug-tea"){
	  label = _U("\uf875"); }

	else if(iconName == "fa-border-top"){
	  label = _U("\uf855"); }

	else if(iconName == "fa-arrows-rotate"){
	  label = _U("\uf021"); }

	else if(iconName == "fa-refresh"){
	  label = _U("\uf021"); }

	else if(iconName == "fa-sync"){
	  label = _U("\uf021"); }

	else if(iconName == "fa-circle-book-open"){
	  label = _U("\ue0ff"); }

	else if(iconName == "fa-book-circle"){
	  label = _U("\ue0ff"); }

	else if(iconName == "fa-arrows-to-dotted-line"){
	  label = _U("\ue0a6"); }

	else if(iconName == "fa-fire-extinguisher"){
	  label = _U("\uf134"); }

	else if(iconName == "fa-magnifying-glass-arrows-rotate"){
	  label = _U("\ue65e"); }

	else if(iconName == "fa-garage-open"){
	  label = _U("\ue00b"); }

	else if(iconName == "fa-shelves-empty"){
	  label = _U("\ue246"); }

	else if(iconName == "fa-cruzeiro-sign"){
	  label = _U("\ue152"); }

	else if(iconName == "fa-watch-apple"){
	  label = _U("\ue2cb"); }

	else if(iconName == "fa-watch-calculator"){
	  label = _U("\uf8f0"); }

	else if(iconName == "fa-list-dropdown"){
	  label = _U("\ue1cf"); }

	else if(iconName == "fa-cabinet-filing"){
	  label = _U("\uf64b"); }

	else if(iconName == "fa-burger-soda"){
	  label = _U("\uf858"); }

	else if(iconName == "fa-square-arrow-up"){
	  label = _U("\uf33c"); }

	else if(iconName == "fa-arrow-square-up"){
	  label = _U("\uf33c"); }

	else if(iconName == "fa-greater-than-equal"){
	  label = _U("\uf532"); }

	else if(iconName == "fa-pallet-box"){
	  label = _U("\ue208"); }

	else if(iconName == "fa-face-confounded"){
	  label = _U("\ue36c"); }

	else if(iconName == "fa-shield-halved"){
	  label = _U("\uf3ed"); }

	else if(iconName == "fa-shield-alt"){
	  label = _U("\uf3ed"); }

	else if(iconName == "fa-truck-plow"){
	  label = _U("\uf7de"); }

	else if(iconName == "fa-book-atlas"){
	  label = _U("\uf558"); }

	else if(iconName == "fa-atlas"){
	  label = _U("\uf558"); }

	else if(iconName == "fa-virus"){
	  label = _U("\ue074"); }

	else if(iconName == "fa-grid-round-2"){
	  label = _U("\ue5db"); }

	else if(iconName == "fa-comment-middle-top"){
	  label = _U("\ue14a"); }

	else if(iconName == "fa-wave"){
	  label = _U("\ue65b"); }

	else if(iconName == "fa-envelope-circle-check"){
	  label = _U("\ue4e8"); }

	else if(iconName == "fa-layer-group"){
	  label = _U("\uf5fd"); }

	else if(iconName == "fa-restroom-simple"){
	  label = _U("\ue23a"); }

	else if(iconName == "fa-arrows-to-dot"){
	  label = _U("\ue4be"); }

	else if(iconName == "fa-border-outer"){
	  label = _U("\uf851"); }

	else if(iconName == "fa-hashtag-lock"){
	  label = _U("\ue415"); }

	else if(iconName == "fa-clock-two-thirty"){
	  label = _U("\ue35b"); }

	else if(iconName == "fa-archway"){
	  label = _U("\uf557"); }

	else if(iconName == "fa-heart-circle-check"){
	  label = _U("\ue4fd"); }

	else if(iconName == "fa-house-chimney-crack"){
	  label = _U("\uf6f1"); }

	else if(iconName == "fa-house-damage"){
	  label = _U("\uf6f1"); }

	else if(iconName == "fa-file-zipper"){
	  label = _U("\uf1c6"); }

	else if(iconName == "fa-file-archive"){
	  label = _U("\uf1c6"); }

	else if(iconName == "fa-ticket-perforated"){
	  label = _U("\ue63e"); }

	else if(iconName == "fa-heart-half"){
	  label = _U("\ue1ab"); }

	else if(iconName == "fa-comment-check"){
	  label = _U("\uf4ac"); }

	else if(iconName == "fa-square"){
	  label = _U("\uf0c8"); }

	else if(iconName == "fa-memo"){
	  label = _U("\ue1d8"); }

	else if(iconName == "fa-martini-glass-empty"){
	  label = _U("\uf000"); }

	else if(iconName == "fa-glass-martini"){
	  label = _U("\uf000"); }

	else if(iconName == "fa-couch"){
	  label = _U("\uf4b8"); }

	else if(iconName == "fa-cedi-sign"){
	  label = _U("\ue0df"); }

	else if(iconName == "fa-italic"){
	  label = _U("\uf033"); }

	else if(iconName == "fa-glass-citrus"){
	  label = _U("\uf869"); }

	else if(iconName == "fa-calendar-lines-pen"){
	  label = _U("\ue472"); }

	else if(iconName == "fa-church"){
	  label = _U("\uf51d"); }

	else if(iconName == "fa-person-snowmobiling"){
	  label = _U("\uf7d1"); }

	else if(iconName == "fa-snowmobile"){
	  label = _U("\uf7d1"); }

	else if(iconName == "fa-face-hushed"){
	  label = _U("\ue37b"); }

	else if(iconName == "fa-comments-dollar"){
	  label = _U("\uf653"); }

	else if(iconName == "fa-tickets-simple"){
	  label = _U("\ue659"); }

	else if(iconName == "fa-pickaxe"){
	  label = _U("\ue5bf"); }

	else if(iconName == "fa-link-simple-slash"){
	  label = _U("\ue1ce"); }

	else if(iconName == "fa-democrat"){
	  label = _U("\uf747"); }

	else if(iconName == "fa-face-confused"){
	  label = _U("\ue36d"); }

	else if(iconName == "fa-pinball"){
	  label = _U("\ue229"); }

	else if(iconName == "fa-z"){
	  label = _U("\u005a"); }

	else if(iconName == "fa-person-skiing"){
	  label = _U("\uf7c9"); }

	else if(iconName == "fa-skiing"){
	  label = _U("\uf7c9"); }

	else if(iconName == "fa-deer"){
	  label = _U("\uf78e"); }

	else if(iconName == "fa-input-pipe"){
	  label = _U("\ue1be"); }

	else if(iconName == "fa-road-lock"){
	  label = _U("\ue567"); }

	else if(iconName == "fa-a"){
	  label = _U("\u0041"); }

	else if(iconName == "fa-bookmark-slash"){
	  label = _U("\ue0c2"); }

	else if(iconName == "fa-temperature-arrow-down"){
	  label = _U("\ue03f"); }

	else if(iconName == "fa-temperature-down"){
	  label = _U("\ue03f"); }

	else if(iconName == "fa-mace"){
	  label = _U("\uf6f8"); }

	else if(iconName == "fa-feather-pointed"){
	  label = _U("\uf56b"); }

	else if(iconName == "fa-feather-alt"){
	  label = _U("\uf56b"); }

	else if(iconName == "fa-sausage"){
	  label = _U("\uf820"); }

	else if(iconName == "fa-trash-can-clock"){
	  label = _U("\ue2aa"); }

	else if(iconName == "fa-p"){
	  label = _U("\u0050"); }

	else if(iconName == "fa-broom-wide"){
	  label = _U("\ue5d1"); }

	else if(iconName == "fa-snowflake"){
	  label = _U("\uf2dc"); }

	else if(iconName == "fa-stomach"){
	  label = _U("\uf623"); }

	else if(iconName == "fa-newspaper"){
	  label = _U("\uf1ea"); }

	else if(iconName == "fa-rectangle-ad"){
	  label = _U("\uf641"); }

	else if(iconName == "fa-ad"){
	  label = _U("\uf641"); }

	else if(iconName == "fa-guitar-electric"){
	  label = _U("\uf8be"); }

	else if(iconName == "fa-arrow-turn-down-right"){
	  label = _U("\ue3d6"); }

	else if(iconName == "fa-moon-cloud"){
	  label = _U("\uf754"); }

	else if(iconName == "fa-bread-slice-butter"){
	  label = _U("\ue3e1"); }

	else if(iconName == "fa-circle-arrow-right"){
	  label = _U("\uf0a9"); }

	else if(iconName == "fa-arrow-circle-right"){
	  label = _U("\uf0a9"); }

	else if(iconName == "fa-user-group-crown"){
	  label = _U("\uf6a5"); }

	else if(iconName == "fa-users-crown"){
	  label = _U("\uf6a5"); }

	else if(iconName == "fa-circle-i"){
	  label = _U("\ue111"); }

	else if(iconName == "fa-toilet-paper-check"){
	  label = _U("\ue5b2"); }

	else if(iconName == "fa-filter-circle-xmark"){
	  label = _U("\ue17b"); }

	else if(iconName == "fa-locust"){
	  label = _U("\ue520"); }

	else if(iconName == "fa-sort"){
	  label = _U("\uf0dc"); }

	else if(iconName == "fa-unsorted"){
	  label = _U("\uf0dc"); }

	else if(iconName == "fa-list-ol"){
	  label = _U("\uf0cb"); }

	else if(iconName == "fa-list-1-2"){
	  label = _U("\uf0cb"); }

	else if(iconName == "fa-list-numeric"){
	  label = _U("\uf0cb"); }

	else if(iconName == "fa-chart-waterfall"){
	  label = _U("\ue0eb"); }

	else if(iconName == "fa-sparkle"){
	  label = _U("\ue5d6"); }

	else if(iconName == "fa-face-party"){
	  label = _U("\ue383"); }

	else if(iconName == "fa-kidneys"){
	  label = _U("\uf5fb"); }

	else if(iconName == "fa-wifi-exclamation"){
	  label = _U("\ue2cf"); }

	else if(iconName == "fa-chart-network"){
	  label = _U("\uf78a"); }

	else if(iconName == "fa-person-dress-burst"){
	  label = _U("\ue544"); }

	else if(iconName == "fa-dice-d4"){
	  label = _U("\uf6d0"); }

	else if(iconName == "fa-money-check-dollar"){
	  label = _U("\uf53d"); }

	else if(iconName == "fa-money-check-alt"){
	  label = _U("\uf53d"); }

	else if(iconName == "fa-vector-square"){
	  label = _U("\uf5cb"); }

	else if(iconName == "fa-bread-slice"){
	  label = _U("\uf7ec"); }

	else if(iconName == "fa-language"){
	  label = _U("\uf1ab"); }

	else if(iconName == "fa-wheat-awn-slash"){
	  label = _U("\ue338"); }

	else if(iconName == "fa-face-kiss-wink-heart"){
	  label = _U("\uf598"); }

	else if(iconName == "fa-kiss-wink-heart"){
	  label = _U("\uf598"); }

	else if(iconName == "fa-dagger"){
	  label = _U("\uf6cb"); }

	else if(iconName == "fa-podium"){
	  label = _U("\uf680"); }

	else if(iconName == "fa-memo-circle-check"){
	  label = _U("\ue1d9"); }

	else if(iconName == "fa-route-highway"){
	  label = _U("\uf61a"); }

	else if(iconName == "fa-down-to-line"){
	  label = _U("\uf34a"); }

	else if(iconName == "fa-arrow-alt-to-bottom"){
	  label = _U("\uf34a"); }

	else if(iconName == "fa-filter"){
	  label = _U("\uf0b0"); }

	else if(iconName == "fa-square-g"){
	  label = _U("\ue271"); }

	else if(iconName == "fa-circle-phone"){
	  label = _U("\ue11b"); }

	else if(iconName == "fa-phone-circle"){
	  label = _U("\ue11b"); }

	else if(iconName == "fa-clipboard-prescription"){
	  label = _U("\uf5e8"); }

	else if(iconName == "fa-user-nurse-hair"){
	  label = _U("\ue45d"); }

	else if(iconName == "fa-question"){
	  label = _U("\u003f"); }

	else if(iconName == "fa-file-signature"){
	  label = _U("\uf573"); }

	else if(iconName == "fa-toggle-large-on"){
	  label = _U("\ue5b1"); }

	else if(iconName == "fa-up-down-left-right"){
	  label = _U("\uf0b2"); }

	else if(iconName == "fa-arrows-alt"){
	  label = _U("\uf0b2"); }

	else if(iconName == "fa-dryer-heat"){
	  label = _U("\uf862"); }

	else if(iconName == "fa-dryer-alt"){
	  label = _U("\uf862"); }

	else if(iconName == "fa-house-chimney-user"){
	  label = _U("\ue065"); }

	else if(iconName == "fa-hand-holding-heart"){
	  label = _U("\uf4be"); }

	else if(iconName == "fa-arrow-up-small-big"){
	  label = _U("\uf88f"); }

	else if(iconName == "fa-sort-size-up-alt"){
	  label = _U("\uf88f"); }

	else if(iconName == "fa-train-track"){
	  label = _U("\ue453"); }

	else if(iconName == "fa-puzzle-piece"){
	  label = _U("\uf12e"); }

	else if(iconName == "fa-money-check"){
	  label = _U("\uf53c"); }

	else if(iconName == "fa-star-half-stroke"){
	  label = _U("\uf5c0"); }

	else if(iconName == "fa-star-half-alt"){
	  label = _U("\uf5c0"); }

	else if(iconName == "fa-file-exclamation"){
	  label = _U("\uf31a"); }

	else if(iconName == "fa-code"){
	  label = _U("\uf121"); }

	else if(iconName == "fa-whiskey-glass"){
	  label = _U("\uf7a0"); }

	else if(iconName == "fa-glass-whiskey"){
	  label = _U("\uf7a0"); }

	else if(iconName == "fa-moon-stars"){
	  label = _U("\uf755"); }

	else if(iconName == "fa-building-circle-exclamation"){
	  label = _U("\ue4d3"); }

	else if(iconName == "fa-clothes-hanger"){
	  label = _U("\ue136"); }

	else if(iconName == "fa-mobile-notch"){
	  label = _U("\ue1ee"); }

	else if(iconName == "fa-mobile-iphone"){
	  label = _U("\ue1ee"); }

	else if(iconName == "fa-magnifying-glass-chart"){
	  label = _U("\ue522"); }

	else if(iconName == "fa-arrow-up-right-from-square"){
	  label = _U("\uf08e"); }

	else if(iconName == "fa-external-link"){
	  label = _U("\uf08e"); }

	else if(iconName == "fa-cubes-stacked"){
	  label = _U("\ue4e6"); }

	else if(iconName == "fa-images-user"){
	  label = _U("\ue1b9"); }

	else if(iconName == "fa-won-sign"){
	  label = _U("\uf159"); }

	else if(iconName == "fa-krw"){
	  label = _U("\uf159"); }

	else if(iconName == "fa-won"){
	  label = _U("\uf159"); }

	else if(iconName == "fa-image-polaroid-user"){
	  label = _U("\ue1b6"); }

	else if(iconName == "fa-virus-covid"){
	  label = _U("\ue4a8"); }

	else if(iconName == "fa-square-ellipsis"){
	  label = _U("\ue26e"); }

	else if(iconName == "fa-pie"){
	  label = _U("\uf705"); }

	else if(iconName == "fa-chess-knight-piece"){
	  label = _U("\uf442"); }

	else if(iconName == "fa-chess-knight-alt"){
	  label = _U("\uf442"); }

	else if(iconName == "fa-austral-sign"){
	  label = _U("\ue0a9"); }

	else if(iconName == "fa-cloud-plus"){
	  label = _U("\ue35e"); }

	else if(iconName == "fa-f"){
	  label = _U("\u0046"); }

	else if(iconName == "fa-leaf"){
	  label = _U("\uf06c"); }

	else if(iconName == "fa-bed-bunk"){
	  label = _U("\uf8f8"); }

	else if(iconName == "fa-road"){
	  label = _U("\uf018"); }

	else if(iconName == "fa-taxi"){
	  label = _U("\uf1ba"); }

	else if(iconName == "fa-cab"){
	  label = _U("\uf1ba"); }

	else if(iconName == "fa-person-circle-plus"){
	  label = _U("\ue541"); }

	else if(iconName == "fa-chart-pie"){
	  label = _U("\uf200"); }

	else if(iconName == "fa-pie-chart"){
	  label = _U("\uf200"); }

	else if(iconName == "fa-bolt-lightning"){
	  label = _U("\ue0b7"); }

	else if(iconName == "fa-clock-eight"){
	  label = _U("\ue345"); }

	else if(iconName == "fa-sack-xmark"){
	  label = _U("\ue56a"); }

	else if(iconName == "fa-file-xls"){
	  label = _U("\ue64d"); }

	else if(iconName == "fa-file-excel"){
	  label = _U("\uf1c3"); }

	else if(iconName == "fa-file-contract"){
	  label = _U("\uf56c"); }

	else if(iconName == "fa-fish-fins"){
	  label = _U("\ue4f2"); }

	else if(iconName == "fa-circle-q"){
	  label = _U("\ue11e"); }

	else if(iconName == "fa-building-flag"){
	  label = _U("\ue4d5"); }

	else if(iconName == "fa-face-grin-beam"){
	  label = _U("\uf582"); }

	else if(iconName == "fa-grin-beam"){
	  label = _U("\uf582"); }

	else if(iconName == "fa-object-ungroup"){
	  label = _U("\uf248"); }

	else if(iconName == "fa-face-disguise"){
	  label = _U("\ue370"); }

	else if(iconName == "fa-circle-arrow-down-right"){
	  label = _U("\ue0fa"); }

	else if(iconName == "fa-alien-8bit"){
	  label = _U("\uf8f6"); }

	else if(iconName == "fa-alien-monster"){
	  label = _U("\uf8f6"); }

	else if(iconName == "fa-hand-point-ribbon"){
	  label = _U("\ue1a6"); }

	else if(iconName == "fa-poop"){
	  label = _U("\uf619"); }

	else if(iconName == "fa-object-exclude"){
	  label = _U("\ue49c"); }

	else if(iconName == "fa-telescope"){
	  label = _U("\ue03e"); }

	else if(iconName == "fa-location-pin"){
	  label = _U("\uf041"); }

	else if(iconName == "fa-map-marker"){
	  label = _U("\uf041"); }

	else if(iconName == "fa-square-list"){
	  label = _U("\ue489"); }

	else if(iconName == "fa-kaaba"){
	  label = _U("\uf66b"); }

	else if(iconName == "fa-toilet-paper"){
	  label = _U("\uf71e"); }

	else if(iconName == "fa-helmet-safety"){
	  label = _U("\uf807"); }

	else if(iconName == "fa-hard-hat"){
	  label = _U("\uf807"); }

	else if(iconName == "fa-hat-hard"){
	  label = _U("\uf807"); }

	else if(iconName == "fa-comment-code"){
	  label = _U("\ue147"); }

	else if(iconName == "fa-sim-cards"){
	  label = _U("\ue251"); }

	else if(iconName == "fa-starship"){
	  label = _U("\ue039"); }

	else if(iconName == "fa-eject"){
	  label = _U("\uf052"); }

	else if(iconName == "fa-circle-right"){
	  label = _U("\uf35a"); }

	else if(iconName == "fa-arrow-alt-circle-right"){
	  label = _U("\uf35a"); }

	else if(iconName == "fa-plane-circle-check"){
	  label = _U("\ue555"); }

	else if(iconName == "fa-seal"){
	  label = _U("\ue241"); }

	else if(iconName == "fa-user-cowboy"){
	  label = _U("\uf8ea"); }

	else if(iconName == "fa-hexagon-vertical-nft"){
	  label = _U("\ue505"); }

	else if(iconName == "fa-face-rolling-eyes"){
	  label = _U("\uf5a5"); }

	else if(iconName == "fa-meh-rolling-eyes"){
	  label = _U("\uf5a5"); }

	else if(iconName == "fa-bread-loaf"){
	  label = _U("\uf7eb"); }

	else if(iconName == "fa-rings-wedding"){
	  label = _U("\uf81b"); }

	else if(iconName == "fa-object-group"){
	  label = _U("\uf247"); }

	else if(iconName == "fa-french-fries"){
	  label = _U("\uf803"); }

	else if(iconName == "fa-chart-line"){
	  label = _U("\uf201"); }

	else if(iconName == "fa-line-chart"){
	  label = _U("\uf201"); }

	else if(iconName == "fa-calendar-arrow-down"){
	  label = _U("\ue0d0"); }

	else if(iconName == "fa-calendar-download"){
	  label = _U("\ue0d0"); }

	else if(iconName == "fa-send-back"){
	  label = _U("\uf87e"); }

	else if(iconName == "fa-mask-ventilator"){
	  label = _U("\ue524"); }

	else if(iconName == "fa-tickets"){
	  label = _U("\ue658"); }

	else if(iconName == "fa-signature-lock"){
	  label = _U("\ue3ca"); }

	else if(iconName == "fa-arrow-right"){
	  label = _U("\uf061"); }

	else if(iconName == "fa-signs-post"){
	  label = _U("\uf277"); }

	else if(iconName == "fa-map-signs"){
	  label = _U("\uf277"); }

	else if(iconName == "fa-octagon-plus"){
	  label = _U("\uf301"); }

	else if(iconName == "fa-plus-octagon"){
	  label = _U("\uf301"); }

	else if(iconName == "fa-cash-register"){
	  label = _U("\uf788"); }

	else if(iconName == "fa-person-circle-question"){
	  label = _U("\ue542"); }

	else if(iconName == "fa-melon-slice"){
	  label = _U("\ue311"); }

	else if(iconName == "fa-space-station-moon"){
	  label = _U("\ue033"); }

	else if(iconName == "fa-message-smile"){
	  label = _U("\uf4aa"); }

	else if(iconName == "fa-comment-alt-smile"){
	  label = _U("\uf4aa"); }

	else if(iconName == "fa-cup-straw"){
	  label = _U("\ue363"); }

	else if(iconName == "fa-left-from-line"){
	  label = _U("\uf348"); }

	else if(iconName == "fa-arrow-alt-from-right"){
	  label = _U("\uf348"); }

	else if(iconName == "fa-h"){
	  label = _U("\u0048"); }

	else if(iconName == "fa-basket-shopping-simple"){
	  label = _U("\ue0af"); }

	else if(iconName == "fa-shopping-basket-alt"){
	  label = _U("\ue0af"); }

	else if(iconName == "fa-hands-holding-heart"){
	  label = _U("\uf4c3"); }

	else if(iconName == "fa-hands-heart"){
	  label = _U("\uf4c3"); }

	else if(iconName == "fa-clock-nine"){
	  label = _U("\ue34c"); }

	else if(iconName == "fa-hammer-brush"){
	  label = _U("\ue620"); }

	else if(iconName == "fa-tarp"){
	  label = _U("\ue57b"); }

	else if(iconName == "fa-face-sleepy"){
	  label = _U("\ue38e"); }

	else if(iconName == "fa-hand-horns"){
	  label = _U("\ue1a9"); }

	else if(iconName == "fa-screwdriver-wrench"){
	  label = _U("\uf7d9"); }

	else if(iconName == "fa-tools"){
	  label = _U("\uf7d9"); }

	else if(iconName == "fa-arrows-to-eye"){
	  label = _U("\ue4bf"); }

	else if(iconName == "fa-circle-three-quarters"){
	  label = _U("\ue125"); }

	else if(iconName == "fa-trophy-star"){
	  label = _U("\uf2eb"); }

	else if(iconName == "fa-trophy-alt"){
	  label = _U("\uf2eb"); }

	else if(iconName == "fa-plug-circle-bolt"){
	  label = _U("\ue55b"); }

	else if(iconName == "fa-face-thermometer"){
	  label = _U("\ue39a"); }

	else if(iconName == "fa-grid-round-4"){
	  label = _U("\ue5dd"); }

	else if(iconName == "fa-sign-posts-wrench"){
	  label = _U("\ue626"); }

	else if(iconName == "fa-shirt-running"){
	  label = _U("\ue3c8"); }

	else if(iconName == "fa-book-circle-arrow-up"){
	  label = _U("\ue0bd"); }

	else if(iconName == "fa-face-nauseated"){
	  label = _U("\ue381"); }

	else if(iconName == "fa-heart"){
	  label = _U("\uf004"); }

	else if(iconName == "fa-file-chart-pie"){
	  label = _U("\uf65a"); }

	else if(iconName == "fa-mars-and-venus"){
	  label = _U("\uf224"); }

	else if(iconName == "fa-house-user"){
	  label = _U("\ue1b0"); }

	else if(iconName == "fa-home-user"){
	  label = _U("\ue1b0"); }

	else if(iconName == "fa-circle-arrow-down-left"){
	  label = _U("\ue0f9"); }

	else if(iconName == "fa-dumpster-fire"){
	  label = _U("\uf794"); }

	else if(iconName == "fa-hexagon-minus"){
	  label = _U("\uf307"); }

	else if(iconName == "fa-minus-hexagon"){
	  label = _U("\uf307"); }

	else if(iconName == "fa-left-to-line"){
	  label = _U("\uf34b"); }

	else if(iconName == "fa-arrow-alt-to-left"){
	  label = _U("\uf34b"); }

	else if(iconName == "fa-house-crack"){
	  label = _U("\ue3b1"); }

	else if(iconName == "fa-paw-simple"){
	  label = _U("\uf701"); }

	else if(iconName == "fa-paw-alt"){
	  label = _U("\uf701"); }

	else if(iconName == "fa-arrow-left-long-to-line"){
	  label = _U("\ue3d4"); }

	else if(iconName == "fa-brackets-round"){
	  label = _U("\ue0c5"); }

	else if(iconName == "fa-parentheses"){
	  label = _U("\ue0c5"); }

	else if(iconName == "fa-martini-glass-citrus"){
	  label = _U("\uf561"); }

	else if(iconName == "fa-cocktail"){
	  label = _U("\uf561"); }

	else if(iconName == "fa-user-shakespeare"){
	  label = _U("\ue2c2"); }

	else if(iconName == "fa-arrow-right-to-arc"){
	  label = _U("\ue4b2"); }

	else if(iconName == "fa-face-surprise"){
	  label = _U("\uf5c2"); }

	else if(iconName == "fa-surprise"){
	  label = _U("\uf5c2"); }

	else if(iconName == "fa-bottle-water"){
	  label = _U("\ue4c5"); }

	else if(iconName == "fa-circle-pause"){
	  label = _U("\uf28b"); }

	else if(iconName == "fa-pause-circle"){
	  label = _U("\uf28b"); }

	else if(iconName == "fa-gauge-circle-plus"){
	  label = _U("\ue498"); }

	else if(iconName == "fa-folders"){
	  label = _U("\uf660"); }

	else if(iconName == "fa-angel"){
	  label = _U("\uf779"); }

	else if(iconName == "fa-value-absolute"){
	  label = _U("\uf6a6"); }

	else if(iconName == "fa-rabbit"){
	  label = _U("\uf708"); }

	else if(iconName == "fa-toilet-paper-slash"){
	  label = _U("\ue072"); }

	else if(iconName == "fa-circle-euro"){
	  label = _U("\ue5ce"); }

	else if(iconName == "fa-apple-whole"){
	  label = _U("\uf5d1"); }

	else if(iconName == "fa-apple-alt"){
	  label = _U("\uf5d1"); }

	else if(iconName == "fa-kitchen-set"){
	  label = _U("\ue51a"); }

	else if(iconName == "fa-diamond-half"){
	  label = _U("\ue5b7"); }

	else if(iconName == "fa-lock-keyhole"){
	  label = _U("\uf30d"); }

	else if(iconName == "fa-lock-alt"){
	  label = _U("\uf30d"); }

	else if(iconName == "fa-r"){
	  label = _U("\u0052"); }

	else if(iconName == "fa-temperature-quarter"){
	  label = _U("\uf2ca"); }

	else if(iconName == "fa-temperature-1"){
	  label = _U("\uf2ca"); }

	else if(iconName == "fa-thermometer-1"){
	  label = _U("\uf2ca"); }

	else if(iconName == "fa-thermometer-quarter"){
	  label = _U("\uf2ca"); }

	else if(iconName == "fa-square-info"){
	  label = _U("\uf30f"); }

	else if(iconName == "fa-info-square"){
	  label = _U("\uf30f"); }

	else if(iconName == "fa-wifi-slash"){
	  label = _U("\uf6ac"); }

	else if(iconName == "fa-toilet-paper-xmark"){
	  label = _U("\ue5b3"); }

	else if(iconName == "fa-hands-holding-dollar"){
	  label = _U("\uf4c5"); }

	else if(iconName == "fa-hands-usd"){
	  label = _U("\uf4c5"); }

	else if(iconName == "fa-cube"){
	  label = _U("\uf1b2"); }

	else if(iconName == "fa-arrow-down-triangle-square"){
	  label = _U("\uf888"); }

	else if(iconName == "fa-sort-shapes-down"){
	  label = _U("\uf888"); }

	else if(iconName == "fa-bitcoin-sign"){
	  label = _U("\ue0b4"); }

	else if(iconName == "fa-shutters"){
	  label = _U("\ue449"); }

	else if(iconName == "fa-shield-dog"){
	  label = _U("\ue573"); }

	else if(iconName == "fa-solar-panel"){
	  label = _U("\uf5ba"); }

	else if(iconName == "fa-lock-open"){
	  label = _U("\uf3c1"); }

	else if(iconName == "fa-table-tree"){
	  label = _U("\ue293"); }

	else if(iconName == "fa-house-chimney-heart"){
	  label = _U("\ue1b2"); }

	else if(iconName == "fa-tally-3"){
	  label = _U("\ue296"); }

	else if(iconName == "fa-elevator"){
	  label = _U("\ue16d"); }

	else if(iconName == "fa-money-bill-transfer"){
	  label = _U("\ue528"); }

	else if(iconName == "fa-money-bill-trend-up"){
	  label = _U("\ue529"); }

	else if(iconName == "fa-house-flood-water-circle-arrow-right"){
	  label = _U("\ue50f"); }

	else if(iconName == "fa-square-poll-horizontal"){
	  label = _U("\uf682"); }

	else if(iconName == "fa-poll-h"){
	  label = _U("\uf682"); }

	else if(iconName == "fa-circle"){
	  label = _U("\uf111"); }

	else if(iconName == "fa-cart-circle-exclamation"){
	  label = _U("\ue3f2"); }

	else if(iconName == "fa-sword"){
	  label = _U("\uf71c"); }

	else if(iconName == "fa-backward-fast"){
	  label = _U("\uf049"); }

	else if(iconName == "fa-fast-backward"){
	  label = _U("\uf049"); }

	else if(iconName == "fa-recycle"){
	  label = _U("\uf1b8"); }

	else if(iconName == "fa-user-astronaut"){
	  label = _U("\uf4fb"); }

	else if(iconName == "fa-interrobang"){
	  label = _U("\ue5ba"); }

	else if(iconName == "fa-plane-slash"){
	  label = _U("\ue069"); }

	else if(iconName == "fa-circle-dashed"){
	  label = _U("\ue105"); }

	else if(iconName == "fa-trademark"){
	  label = _U("\uf25c"); }

	else if(iconName == "fa-basketball"){
	  label = _U("\uf434"); }

	else if(iconName == "fa-basketball-ball"){
	  label = _U("\uf434"); }

	else if(iconName == "fa-fork-knife"){
	  label = _U("\uf2e6"); }

	else if(iconName == "fa-utensils-alt"){
	  label = _U("\uf2e6"); }

	else if(iconName == "fa-satellite-dish"){
	  label = _U("\uf7c0"); }

	else if(iconName == "fa-badge-check"){
	  label = _U("\uf336"); }

	else if(iconName == "fa-circle-up"){
	  label = _U("\uf35b"); }

	else if(iconName == "fa-arrow-alt-circle-up"){
	  label = _U("\uf35b"); }

	else if(iconName == "fa-slider"){
	  label = _U("\ue252"); }

	else if(iconName == "fa-mobile-screen-button"){
	  label = _U("\uf3cd"); }

	else if(iconName == "fa-mobile-alt"){
	  label = _U("\uf3cd"); }

	else if(iconName == "fa-clock-one-thirty"){
	  label = _U("\ue34f"); }

	else if(iconName == "fa-inbox-out"){
	  label = _U("\uf311"); }

	else if(iconName == "fa-inbox-arrow-up"){
	  label = _U("\uf311"); }

	else if(iconName == "fa-cloud-slash"){
	  label = _U("\ue137"); }

	else if(iconName == "fa-volume-high"){
	  label = _U("\uf028"); }

	else if(iconName == "fa-volume-up"){
	  label = _U("\uf028"); }

	else if(iconName == "fa-users-rays"){
	  label = _U("\ue593"); }

	else if(iconName == "fa-wallet"){
	  label = _U("\uf555"); }

	else if(iconName == "fa-octagon-check"){
	  label = _U("\ue426"); }

	else if(iconName == "fa-flatbread-stuffed"){
	  label = _U("\ue40c"); }

	else if(iconName == "fa-clipboard-check"){
	  label = _U("\uf46c"); }

	else if(iconName == "fa-cart-circle-plus"){
	  label = _U("\ue3f3"); }

	else if(iconName == "fa-truck-clock"){
	  label = _U("\uf48c"); }

	else if(iconName == "fa-shipping-timed"){
	  label = _U("\uf48c"); }

	else if(iconName == "fa-pool-8-ball"){
	  label = _U("\ue3c5"); }

	else if(iconName == "fa-file-audio"){
	  label = _U("\uf1c7"); }

	else if(iconName == "fa-turn-down-left"){
	  label = _U("\ue331"); }

	else if(iconName == "fa-lock-hashtag"){
	  label = _U("\ue423"); }

	else if(iconName == "fa-chart-radar"){
	  label = _U("\ue0e7"); }

	else if(iconName == "fa-staff"){
	  label = _U("\uf71b"); }

	else if(iconName == "fa-burger"){
	  label = _U("\uf805"); }

	else if(iconName == "fa-hamburger"){
	  label = _U("\uf805"); }

	else if(iconName == "fa-utility-pole"){
	  label = _U("\ue2c3"); }

	else if(iconName == "fa-transporter-6"){
	  label = _U("\ue2a7"); }

	else if(iconName == "fa-arrow-turn-left"){
	  label = _U("\ue632"); }

	else if(iconName == "fa-wrench"){
	  label = _U("\uf0ad"); }

	else if(iconName == "fa-bugs"){
	  label = _U("\ue4d0"); }

	else if(iconName == "fa-vector-polygon"){
	  label = _U("\ue2c7"); }

	else if(iconName == "fa-diagram-nested"){
	  label = _U("\ue157"); }

	else if(iconName == "fa-rupee-sign"){
	  label = _U("\uf156"); }

	else if(iconName == "fa-rupee"){
	  label = _U("\uf156"); }

	else if(iconName == "fa-file-image"){
	  label = _U("\uf1c5"); }

	else if(iconName == "fa-circle-question"){
	  label = _U("\uf059"); }

	else if(iconName == "fa-question-circle"){
	  label = _U("\uf059"); }

	else if(iconName == "fa-tickets-perforated"){
	  label = _U("\ue63f"); }

	else if(iconName == "fa-image-user"){
	  label = _U("\ue1b8"); }

	else if(iconName == "fa-buoy"){
	  label = _U("\ue5b5"); }

	else if(iconName == "fa-plane-departure"){
	  label = _U("\uf5b0"); }

	else if(iconName == "fa-handshake-slash"){
	  label = _U("\ue060"); }

	else if(iconName == "fa-book-bookmark"){
	  label = _U("\ue0bb"); }

	else if(iconName == "fa-border-center-h"){
	  label = _U("\uf89c"); }

	else if(iconName == "fa-can-food"){
	  label = _U("\ue3e6"); }

	else if(iconName == "fa-typewriter"){
	  label = _U("\uf8e7"); }

	else if(iconName == "fa-arrow-right-from-arc"){
	  label = _U("\ue4b1"); }

	else if(iconName == "fa-circle-k"){
	  label = _U("\ue113"); }

	else if(iconName == "fa-face-hand-over-mouth"){
	  label = _U("\ue378"); }

	else if(iconName == "fa-popcorn"){
	  label = _U("\uf819"); }

	else if(iconName == "fa-house-water"){
	  label = _U("\uf74f"); }

	else if(iconName == "fa-house-flood"){
	  label = _U("\uf74f"); }

	else if(iconName == "fa-object-subtract"){
	  label = _U("\ue49e"); }

	else if(iconName == "fa-code-branch"){
	  label = _U("\uf126"); }

	else if(iconName == "fa-warehouse-full"){
	  label = _U("\uf495"); }

	else if(iconName == "fa-warehouse-alt"){
	  label = _U("\uf495"); }

	else if(iconName == "fa-hat-cowboy"){
	  label = _U("\uf8c0"); }

	else if(iconName == "fa-bridge"){
	  label = _U("\ue4c8"); }

	else if(iconName == "fa-phone-flip"){
	  label = _U("\uf879"); }

	else if(iconName == "fa-phone-alt"){
	  label = _U("\uf879"); }

	else if(iconName == "fa-arrow-down-from-dotted-line"){
	  label = _U("\ue090"); }

	else if(iconName == "fa-file-doc"){
	  label = _U("\ue5ed"); }

	else if(iconName == "fa-square-quarters"){
	  label = _U("\ue44e"); }

	else if(iconName == "fa-truck-front"){
	  label = _U("\ue2b7"); }

	else if(iconName == "fa-cat"){
	  label = _U("\uf6be"); }

	else if(iconName == "fa-trash-xmark"){
	  label = _U("\ue2b4"); }

	else if(iconName == "fa-circle-caret-left"){
	  label = _U("\uf32e"); }

	else if(iconName == "fa-caret-circle-left"){
	  label = _U("\uf32e"); }

	else if(iconName == "fa-files"){
	  label = _U("\ue178"); }

	else if(iconName == "fa-anchor-circle-exclamation"){
	  label = _U("\ue4ab"); }

	else if(iconName == "fa-face-clouds"){
	  label = _U("\ue47d"); }

	else if(iconName == "fa-user-crown"){
	  label = _U("\uf6a4"); }

	else if(iconName == "fa-basket-shopping-plus"){
	  label = _U("\ue653"); }

	else if(iconName == "fa-truck-field"){
	  label = _U("\ue58d"); }

	else if(iconName == "fa-route"){
	  label = _U("\uf4d7"); }

	else if(iconName == "fa-cart-circle-check"){
	  label = _U("\ue3f1"); }

	else if(iconName == "fa-clipboard-question"){
	  label = _U("\ue4e3"); }

	else if(iconName == "fa-panorama"){
	  label = _U("\ue209"); }

	else if(iconName == "fa-comment-medical"){
	  label = _U("\uf7f5"); }

	else if(iconName == "fa-teeth-open"){
	  label = _U("\uf62f"); }

	else if(iconName == "fa-user-tie-hair-long"){
	  label = _U("\ue460"); }

	else if(iconName == "fa-file-circle-minus"){
	  label = _U("\ue4ed"); }

	else if(iconName == "fa-head-side-medical"){
	  label = _U("\uf809"); }

	else if(iconName == "fa-arrow-turn-right"){
	  label = _U("\ue635"); }

	else if(iconName == "fa-tags"){
	  label = _U("\uf02c"); }

	else if(iconName == "fa-wine-glass"){
	  label = _U("\uf4e3"); }

	else if(iconName == "fa-forward-fast"){
	  label = _U("\uf050"); }

	else if(iconName == "fa-fast-forward"){
	  label = _U("\uf050"); }

	else if(iconName == "fa-face-meh-blank"){
	  label = _U("\uf5a4"); }

	else if(iconName == "fa-meh-blank"){
	  label = _U("\uf5a4"); }

	else if(iconName == "fa-user-robot"){
	  label = _U("\ue04b"); }

	else if(iconName == "fa-square-parking"){
	  label = _U("\uf540"); }

	else if(iconName == "fa-parking"){
	  label = _U("\uf540"); }

	else if(iconName == "fa-card-diamond"){
	  label = _U("\ue3ea"); }

	else if(iconName == "fa-face-zipper"){
	  label = _U("\ue3a5"); }

	else if(iconName == "fa-face-raised-eyebrow"){
	  label = _U("\ue388"); }

	else if(iconName == "fa-house-signal"){
	  label = _U("\ue012"); }

	else if(iconName == "fa-square-chevron-up"){
	  label = _U("\uf32c"); }

	else if(iconName == "fa-chevron-square-up"){
	  label = _U("\uf32c"); }

	else if(iconName == "fa-bars-progress"){
	  label = _U("\uf828"); }

	else if(iconName == "fa-tasks-alt"){
	  label = _U("\uf828"); }

	else if(iconName == "fa-faucet-drip"){
	  label = _U("\ue006"); }

	else if(iconName == "fa-arrows-to-line"){
	  label = _U("\ue0a7"); }

	else if(iconName == "fa-dolphin"){
	  label = _U("\ue168"); }

	else if(iconName == "fa-arrow-up-right"){
	  label = _U("\ue09f"); }

	else if(iconName == "fa-circle-r"){
	  label = _U("\ue120"); }

	else if(iconName == "fa-cart-flatbed"){
	  label = _U("\uf474"); }

	else if(iconName == "fa-dolly-flatbed"){
	  label = _U("\uf474"); }

	else if(iconName == "fa-ban-smoking"){
	  label = _U("\uf54d"); }

	else if(iconName == "fa-smoking-ban"){
	  label = _U("\uf54d"); }

	else if(iconName == "fa-circle-sort-up"){
	  label = _U("\ue032"); }

	else if(iconName == "fa-sort-circle-up"){
	  label = _U("\ue032"); }

	else if(iconName == "fa-terminal"){
	  label = _U("\uf120"); }

	else if(iconName == "fa-mobile-button"){
	  label = _U("\uf10b"); }

	else if(iconName == "fa-house-medical-flag"){
	  label = _U("\ue514"); }

	else if(iconName == "fa-basket-shopping"){
	  label = _U("\uf291"); }

	else if(iconName == "fa-shopping-basket"){
	  label = _U("\uf291"); }

	else if(iconName == "fa-tape"){
	  label = _U("\uf4db"); }

	else if(iconName == "fa-chestnut"){
	  label = _U("\ue3f6"); }

	else if(iconName == "fa-bus-simple"){
	  label = _U("\uf55e"); }

	else if(iconName == "fa-bus-alt"){
	  label = _U("\uf55e"); }

	else if(iconName == "fa-eye"){
	  label = _U("\uf06e"); }

	else if(iconName == "fa-face-sad-cry"){
	  label = _U("\uf5b3"); }

	else if(iconName == "fa-sad-cry"){
	  label = _U("\uf5b3"); }

	else if(iconName == "fa-heat"){
	  label = _U("\ue00c"); }

	else if(iconName == "fa-ticket-airline"){
	  label = _U("\ue29a"); }

	else if(iconName == "fa-ticket-perforated-plane"){
	  label = _U("\ue29a"); }

	else if(iconName == "fa-ticket-plane"){
	  label = _U("\ue29a"); }

	else if(iconName == "fa-boot-heeled"){
	  label = _U("\ue33f"); }

	else if(iconName == "fa-arrows-minimize"){
	  label = _U("\ue0a5"); }

	else if(iconName == "fa-compress-arrows"){
	  label = _U("\ue0a5"); }

	else if(iconName == "fa-audio-description"){
	  label = _U("\uf29e"); }

	else if(iconName == "fa-person-military-to-person"){
	  label = _U("\ue54c"); }

	else if(iconName == "fa-file-shield"){
	  label = _U("\ue4f0"); }

	else if(iconName == "fa-hexagon"){
	  label = _U("\uf312"); }

	else if(iconName == "fa-manhole"){
	  label = _U("\ue1d6"); }

	else if(iconName == "fa-user-slash"){
	  label = _U("\uf506"); }

	else if(iconName == "fa-pen"){
	  label = _U("\uf304"); }

	else if(iconName == "fa-tower-observation"){
	  label = _U("\ue586"); }

	else if(iconName == "fa-floppy-disks"){
	  label = _U("\ue183"); }

	else if(iconName == "fa-toilet-paper-blank-under"){
	  label = _U("\ue29f"); }

	else if(iconName == "fa-toilet-paper-reverse-alt"){
	  label = _U("\ue29f"); }

	else if(iconName == "fa-file-code"){
	  label = _U("\uf1c9"); }

	else if(iconName == "fa-signal"){
	  label = _U("\uf012"); }

	else if(iconName == "fa-signal-5"){
	  label = _U("\uf012"); }

	else if(iconName == "fa-signal-perfect"){
	  label = _U("\uf012"); }

	else if(iconName == "fa-pump"){
	  label = _U("\ue442"); }

	else if(iconName == "fa-bus"){
	  label = _U("\uf207"); }

	else if(iconName == "fa-heart-circle-xmark"){
	  label = _U("\ue501"); }

	else if(iconName == "fa-arrow-up-left-from-circle"){
	  label = _U("\ue09e"); }

	else if(iconName == "fa-house-chimney"){
	  label = _U("\ue3af"); }

	else if(iconName == "fa-home-lg"){
	  label = _U("\ue3af"); }

	else if(iconName == "fa-window-maximize"){
	  label = _U("\uf2d0"); }

	else if(iconName == "fa-dryer"){
	  label = _U("\uf861"); }

	else if(iconName == "fa-face-frown"){
	  label = _U("\uf119"); }

	else if(iconName == "fa-frown"){
	  label = _U("\uf119"); }

	else if(iconName == "fa-chess-bishop-piece"){
	  label = _U("\uf43b"); }

	else if(iconName == "fa-chess-bishop-alt"){
	  label = _U("\uf43b"); }

	else if(iconName == "fa-shirt-tank-top"){
	  label = _U("\ue3c9"); }

	else if(iconName == "fa-diploma"){
	  label = _U("\uf5ea"); }

	else if(iconName == "fa-scroll-ribbon"){
	  label = _U("\uf5ea"); }

	else if(iconName == "fa-screencast"){
	  label = _U("\ue23e"); }

	else if(iconName == "fa-walker"){
	  label = _U("\uf831"); }

	else if(iconName == "fa-prescription"){
	  label = _U("\uf5b1"); }

	else if(iconName == "fa-shop"){
	  label = _U("\uf54f"); }

	else if(iconName == "fa-store-alt"){
	  label = _U("\uf54f"); }

	else if(iconName == "fa-floppy-disk"){
	  label = _U("\uf0c7"); }

	else if(iconName == "fa-save"){
	  label = _U("\uf0c7"); }

	else if(iconName == "fa-vihara"){
	  label = _U("\uf6a7"); }

	else if(iconName == "fa-face-kiss-closed-eyes"){
	  label = _U("\ue37d"); }

	else if(iconName == "fa-scale-unbalanced"){
	  label = _U("\uf515"); }

	else if(iconName == "fa-balance-scale-left"){
	  label = _U("\uf515"); }

	else if(iconName == "fa-file-user"){
	  label = _U("\uf65c"); }

	else if(iconName == "fa-user-police-tie"){
	  label = _U("\ue334"); }

	else if(iconName == "fa-face-tongue-money"){
	  label = _U("\ue39d"); }

	else if(iconName == "fa-tennis-ball"){
	  label = _U("\uf45e"); }

	else if(iconName == "fa-square-l"){
	  label = _U("\ue275"); }

	else if(iconName == "fa-sort-up"){
	  label = _U("\uf0de"); }

	else if(iconName == "fa-sort-asc"){
	  label = _U("\uf0de"); }

	else if(iconName == "fa-calendar-arrow-up"){
	  label = _U("\ue0d1"); }

	else if(iconName == "fa-calendar-upload"){
	  label = _U("\ue0d1"); }

	else if(iconName == "fa-comment-dots"){
	  label = _U("\uf4ad"); }

	else if(iconName == "fa-commenting"){
	  label = _U("\uf4ad"); }

	else if(iconName == "fa-plant-wilt"){
	  label = _U("\ue5aa"); }

	else if(iconName == "fa-scarf"){
	  label = _U("\uf7c1"); }

	else if(iconName == "fa-album-circle-plus"){
	  label = _U("\ue48c"); }

	else if(iconName == "fa-user-nurse-hair-long"){
	  label = _U("\ue45e"); }

	else if(iconName == "fa-diamond"){
	  label = _U("\uf219"); }

	else if(iconName == "fa-square-left"){
	  label = _U("\uf351"); }

	else if(iconName == "fa-arrow-alt-square-left"){
	  label = _U("\uf351"); }

	else if(iconName == "fa-face-grin-squint"){
	  label = _U("\uf585"); }

	else if(iconName == "fa-grin-squint"){
	  label = _U("\uf585"); }

	else if(iconName == "fa-circle-ellipsis-vertical"){
	  label = _U("\ue10b"); }

	else if(iconName == "fa-hand-holding-dollar"){
	  label = _U("\uf4c0"); }

	else if(iconName == "fa-hand-holding-usd"){
	  label = _U("\uf4c0"); }

	else if(iconName == "fa-grid-dividers"){
	  label = _U("\ue3ad"); }

	else if(iconName == "fa-bacterium"){
	  label = _U("\ue05a"); }

	else if(iconName == "fa-hand-pointer"){
	  label = _U("\uf25a"); }

	else if(iconName == "fa-drum-steelpan"){
	  label = _U("\uf56a"); }

	else if(iconName == "fa-hand-scissors"){
	  label = _U("\uf257"); }

	else if(iconName == "fa-hands-praying"){
	  label = _U("\uf684"); }

	else if(iconName == "fa-praying-hands"){
	  label = _U("\uf684"); }

	else if(iconName == "fa-face-pensive"){
	  label = _U("\ue384"); }

	else if(iconName == "fa-user-music"){
	  label = _U("\uf8eb"); }

	else if(iconName == "fa-arrow-rotate-right"){
	  label = _U("\uf01e"); }

	else if(iconName == "fa-arrow-right-rotate"){
	  label = _U("\uf01e"); }

	else if(iconName == "fa-arrow-rotate-forward"){
	  label = _U("\uf01e"); }

	else if(iconName == "fa-redo"){
	  label = _U("\uf01e"); }

	else if(iconName == "fa-messages-dollar"){
	  label = _U("\uf652"); }

	else if(iconName == "fa-comments-alt-dollar"){
	  label = _U("\uf652"); }

	else if(iconName == "fa-sensor-on"){
	  label = _U("\ue02b"); }

	else if(iconName == "fa-balloon"){
	  label = _U("\ue2e3"); }

	else if(iconName == "fa-biohazard"){
	  label = _U("\uf780"); }

	else if(iconName == "fa-chess-queen-piece"){
	  label = _U("\uf446"); }

	else if(iconName == "fa-chess-queen-alt"){
	  label = _U("\uf446"); }

	else if(iconName == "fa-location-crosshairs"){
	  label = _U("\uf601"); }

	else if(iconName == "fa-location"){
	  label = _U("\uf601"); }

	else if(iconName == "fa-mars-double"){
	  label = _U("\uf227"); }

	else if(iconName == "fa-house-person-leave"){
	  label = _U("\ue00f"); }

	else if(iconName == "fa-house-leave"){
	  label = _U("\ue00f"); }

	else if(iconName == "fa-house-person-depart"){
	  label = _U("\ue00f"); }

	else if(iconName == "fa-ruler-triangle"){
	  label = _U("\uf61c"); }

	else if(iconName == "fa-card-club"){
	  label = _U("\ue3e9"); }

	else if(iconName == "fa-child-dress"){
	  label = _U("\ue59c"); }

	else if(iconName == "fa-users-between-lines"){
	  label = _U("\ue591"); }

	else if(iconName == "fa-lungs-virus"){
	  label = _U("\ue067"); }

	else if(iconName == "fa-spinner-third"){
	  label = _U("\uf3f4"); }

	else if(iconName == "fa-face-grin-tears"){
	  label = _U("\uf588"); }

	else if(iconName == "fa-grin-tears"){
	  label = _U("\uf588"); }

	else if(iconName == "fa-phone"){
	  label = _U("\uf095"); }

	else if(iconName == "fa-computer-mouse-scrollwheel"){
	  label = _U("\uf8cd"); }

	else if(iconName == "fa-mouse-alt"){
	  label = _U("\uf8cd"); }

	else if(iconName == "fa-calendar-xmark"){
	  label = _U("\uf273"); }

	else if(iconName == "fa-calendar-times"){
	  label = _U("\uf273"); }

	else if(iconName == "fa-child-reaching"){
	  label = _U("\ue59d"); }

	else if(iconName == "fa-table-layout"){
	  label = _U("\ue290"); }

	else if(iconName == "fa-narwhal"){
	  label = _U("\uf6fe"); }

	else if(iconName == "fa-ramp-loading"){
	  label = _U("\uf4d4"); }

	else if(iconName == "fa-calendar-circle-plus"){
	  label = _U("\ue470"); }

	else if(iconName == "fa-toothbrush"){
	  label = _U("\uf635"); }

	else if(iconName == "fa-border-inner"){
	  label = _U("\uf84e"); }

	else if(iconName == "fa-paw-claws"){
	  label = _U("\uf702"); }

	else if(iconName == "fa-kiwi-fruit"){
	  label = _U("\ue30c"); }

	else if(iconName == "fa-traffic-light-slow"){
	  label = _U("\uf639"); }

	else if(iconName == "fa-rectangle-code"){
	  label = _U("\ue322"); }

	else if(iconName == "fa-head-side-virus"){
	  label = _U("\ue064"); }

	else if(iconName == "fa-keyboard-brightness"){
	  label = _U("\ue1c0"); }

	else if(iconName == "fa-books-medical"){
	  label = _U("\uf7e8"); }

	else if(iconName == "fa-lightbulb-slash"){
	  label = _U("\uf673"); }

	else if(iconName == "fa-house-blank"){
	  label = _U("\ue487"); }

	else if(iconName == "fa-home-blank"){
	  label = _U("\ue487"); }

	else if(iconName == "fa-square-5"){
	  label = _U("\ue25a"); }

	else if(iconName == "fa-square-heart"){
	  label = _U("\uf4c8"); }

	else if(iconName == "fa-heart-square"){
	  label = _U("\uf4c8"); }

	else if(iconName == "fa-puzzle"){
	  label = _U("\ue443"); }

	else if(iconName == "fa-user-gear"){
	  label = _U("\uf4fe"); }

	else if(iconName == "fa-user-cog"){
	  label = _U("\uf4fe"); }

	else if(iconName == "fa-pipe-circle-check"){
	  label = _U("\ue436"); }

	else if(iconName == "fa-arrow-up-1-9"){
	  label = _U("\uf163"); }

	else if(iconName == "fa-sort-numeric-up"){
	  label = _U("\uf163"); }

	else if(iconName == "fa-octagon-exclamation"){
	  label = _U("\ue204"); }

	else if(iconName == "fa-dial-low"){
	  label = _U("\ue15d"); }

	else if(iconName == "fa-door-closed"){
	  label = _U("\uf52a"); }

	else if(iconName == "fa-laptop-mobile"){
	  label = _U("\uf87a"); }

	else if(iconName == "fa-phone-laptop"){
	  label = _U("\uf87a"); }

	else if(iconName == "fa-conveyor-belt-boxes"){
	  label = _U("\uf46f"); }

	else if(iconName == "fa-conveyor-belt-alt"){
	  label = _U("\uf46f"); }

	else if(iconName == "fa-shield-virus"){
	  label = _U("\ue06c"); }

	else if(iconName == "fa-starfighter-twin-ion-engine-advanced"){
	  label = _U("\ue28e"); }

	else if(iconName == "fa-starfighter-alt-advanced"){
	  label = _U("\ue28e"); }

	else if(iconName == "fa-dice-six"){
	  label = _U("\uf526"); }

	else if(iconName == "fa-starfighter-twin-ion-engine"){
	  label = _U("\ue038"); }

	else if(iconName == "fa-starfighter-alt"){
	  label = _U("\ue038"); }

	else if(iconName == "fa-rocket-launch"){
	  label = _U("\ue027"); }

	else if(iconName == "fa-mosquito-net"){
	  label = _U("\ue52c"); }

	else if(iconName == "fa-vent-damper"){
	  label = _U("\ue465"); }

	else if(iconName == "fa-bridge-water"){
	  label = _U("\ue4ce"); }

	else if(iconName == "fa-ban-bug"){
	  label = _U("\uf7f9"); }

	else if(iconName == "fa-debug"){
	  label = _U("\uf7f9"); }

	else if(iconName == "fa-person-booth"){
	  label = _U("\uf756"); }

	else if(iconName == "fa-text-width"){
	  label = _U("\uf035"); }

	else if(iconName == "fa-garage-car"){
	  label = _U("\ue00a"); }

	else if(iconName == "fa-square-kanban"){
	  label = _U("\ue488"); }

	else if(iconName == "fa-hat-wizard"){
	  label = _U("\uf6e8"); }

	else if(iconName == "fa-chart-kanban"){
	  label = _U("\ue64f"); }

	else if(iconName == "fa-pen-fancy"){
	  label = _U("\uf5ac"); }

	else if(iconName == "fa-coffee-pot"){
	  label = _U("\ue002"); }

	else if(iconName == "fa-mouse-field"){
	  label = _U("\ue5a8"); }

	else if(iconName == "fa-person-digging"){
	  label = _U("\uf85e"); }

	else if(iconName == "fa-digging"){
	  label = _U("\uf85e"); }

	else if(iconName == "fa-shower-down"){
	  label = _U("\ue24d"); }

	else if(iconName == "fa-shower-alt"){
	  label = _U("\ue24d"); }

	else if(iconName == "fa-box-circle-check"){
	  label = _U("\ue0c4"); }

	else if(iconName == "fa-brightness"){
	  label = _U("\ue0c9"); }

	else if(iconName == "fa-car-side-bolt"){
	  label = _U("\ue344"); }

	else if(iconName == "fa-file-xml"){
	  label = _U("\ue654"); }

	else if(iconName == "fa-ornament"){
	  label = _U("\uf7b8"); }

	else if(iconName == "fa-phone-arrow-down-left"){
	  label = _U("\ue223"); }

	else if(iconName == "fa-phone-arrow-down"){
	  label = _U("\ue223"); }

	else if(iconName == "fa-phone-incoming"){
	  label = _U("\ue223"); }

	else if(iconName == "fa-cloud-word"){
	  label = _U("\ue138"); }

	else if(iconName == "fa-hand-fingers-crossed"){
	  label = _U("\ue1a3"); }

	else if(iconName == "fa-trash"){
	  label = _U("\uf1f8"); }

	else if(iconName == "fa-gauge-simple"){
	  label = _U("\uf629"); }

	else if(iconName == "fa-gauge-simple-med"){
	  label = _U("\uf629"); }

	else if(iconName == "fa-tachometer-average"){
	  label = _U("\uf629"); }

	else if(iconName == "fa-arrow-down-small-big"){
	  label = _U("\uf88d"); }

	else if(iconName == "fa-sort-size-down-alt"){
	  label = _U("\uf88d"); }

	else if(iconName == "fa-book-medical"){
	  label = _U("\uf7e6"); }

	else if(iconName == "fa-face-melting"){
	  label = _U("\ue483"); }

	else if(iconName == "fa-poo"){
	  label = _U("\uf2fe"); }

	else if(iconName == "fa-pen-clip-slash"){
	  label = _U("\ue20f"); }

	else if(iconName == "fa-pen-alt-slash"){
	  label = _U("\ue20f"); }

	else if(iconName == "fa-quote-right"){
	  label = _U("\uf10e"); }

	else if(iconName == "fa-quote-right-alt"){
	  label = _U("\uf10e"); }

	else if(iconName == "fa-scroll-old"){
	  label = _U("\uf70f"); }

	else if(iconName == "fa-guitars"){
	  label = _U("\uf8bf"); }

	else if(iconName == "fa-phone-xmark"){
	  label = _U("\ue227"); }

	else if(iconName == "fa-hose"){
	  label = _U("\ue419"); }

	else if(iconName == "fa-clock-six"){
	  label = _U("\ue352"); }

	else if(iconName == "fa-shirt"){
	  label = _U("\uf553"); }

	else if(iconName == "fa-t-shirt"){
	  label = _U("\uf553"); }

	else if(iconName == "fa-tshirt"){
	  label = _U("\uf553"); }

	else if(iconName == "fa-billboard"){
	  label = _U("\ue5cd"); }

	else if(iconName == "fa-square-r"){
	  label = _U("\ue27c"); }

	else if(iconName == "fa-cubes"){
	  label = _U("\uf1b3"); }

	else if(iconName == "fa-envelope-open-dollar"){
	  label = _U("\uf657"); }

	else if(iconName == "fa-divide"){
	  label = _U("\uf529"); }

	else if(iconName == "fa-sun-cloud"){
	  label = _U("\uf763"); }

	else if(iconName == "fa-lamp-floor"){
	  label = _U("\ue015"); }

	else if(iconName == "fa-square-7"){
	  label = _U("\ue25c"); }

	else if(iconName == "fa-tenge-sign"){
	  label = _U("\uf7d7"); }

	else if(iconName == "fa-tenge"){
	  label = _U("\uf7d7"); }

	else if(iconName == "fa-headphones"){
	  label = _U("\uf025"); }

	else if(iconName == "fa-hands-holding"){
	  label = _U("\uf4c2"); }

	else if(iconName == "fa-campfire"){
	  label = _U("\uf6ba"); }

	else if(iconName == "fa-circle-ampersand"){
	  label = _U("\ue0f8"); }

	else if(iconName == "fa-snowflakes"){
	  label = _U("\uf7cf"); }

	else if(iconName == "fa-hands-clapping"){
	  label = _U("\ue1a8"); }

	else if(iconName == "fa-republican"){
	  label = _U("\uf75e"); }

	else if(iconName == "fa-leaf-maple"){
	  label = _U("\uf6f6"); }

	else if(iconName == "fa-arrow-left"){
	  label = _U("\uf060"); }

	else if(iconName == "fa-person-circle-xmark"){
	  label = _U("\ue543"); }

	else if(iconName == "fa-ruler"){
	  label = _U("\uf545"); }

	else if(iconName == "fa-cup-straw-swoosh"){
	  label = _U("\ue364"); }

	else if(iconName == "fa-temperature-sun"){
	  label = _U("\uf76a"); }

	else if(iconName == "fa-temperature-hot"){
	  label = _U("\uf76a"); }

	else if(iconName == "fa-align-left"){
	  label = _U("\uf036"); }

	else if(iconName == "fa-dice-d6"){
	  label = _U("\uf6d1"); }

	else if(iconName == "fa-restroom"){
	  label = _U("\uf7bd"); }

	else if(iconName == "fa-high-definition"){
	  label = _U("\ue1ae"); }

	else if(iconName == "fa-rectangle-hd"){
	  label = _U("\ue1ae"); }

	else if(iconName == "fa-j"){
	  label = _U("\u004a"); }

	else if(iconName == "fa-galaxy"){
	  label = _U("\ue008"); }

	else if(iconName == "fa-users-viewfinder"){
	  label = _U("\ue595"); }

	else if(iconName == "fa-file-video"){
	  label = _U("\uf1c8"); }

	else if(iconName == "fa-cherries"){
	  label = _U("\ue0ec"); }

	else if(iconName == "fa-up-right-from-square"){
	  label = _U("\uf35d"); }

	else if(iconName == "fa-external-link-alt"){
	  label = _U("\uf35d"); }

	else if(iconName == "fa-circle-sort"){
	  label = _U("\ue030"); }

	else if(iconName == "fa-sort-circle"){
	  label = _U("\ue030"); }

	else if(iconName == "fa-table-cells"){
	  label = _U("\uf00a"); }

	else if(iconName == "fa-th"){
	  label = _U("\uf00a"); }

	else if(iconName == "fa-bag-shopping-minus"){
	  label = _U("\ue650"); }

	else if(iconName == "fa-file-pdf"){
	  label = _U("\uf1c1"); }

	else if(iconName == "fa-siren"){
	  label = _U("\ue02d"); }

	else if(iconName == "fa-arrow-up-to-dotted-line"){
	  label = _U("\ue0a1"); }

	else if(iconName == "fa-image-landscape"){
	  label = _U("\ue1b5"); }

	else if(iconName == "fa-landscape"){
	  label = _U("\ue1b5"); }

	else if(iconName == "fa-tank-water"){
	  label = _U("\ue452"); }

	else if(iconName == "fa-curling-stone"){
	  label = _U("\uf44a"); }

	else if(iconName == "fa-curling"){
	  label = _U("\uf44a"); }

	else if(iconName == "fa-gamepad-modern"){
	  label = _U("\ue5a2"); }

	else if(iconName == "fa-gamepad-alt"){
	  label = _U("\ue5a2"); }

	else if(iconName == "fa-messages-question"){
	  label = _U("\ue1e7"); }

	else if(iconName == "fa-book-bible"){
	  label = _U("\uf647"); }

	else if(iconName == "fa-bible"){
	  label = _U("\uf647"); }

	else if(iconName == "fa-o"){
	  label = _U("\u004f"); }

	else if(iconName == "fa-suitcase-medical"){
	  label = _U("\uf0fa"); }

	else if(iconName == "fa-medkit"){
	  label = _U("\uf0fa"); }

	else if(iconName == "fa-briefcase-arrow-right"){
	  label = _U("\ue2f2"); }

	else if(iconName == "fa-expand-wide"){
	  label = _U("\uf320"); }

	else if(iconName == "fa-clock-eleven-thirty"){
	  label = _U("\ue348"); }

	else if(iconName == "fa-rv"){
	  label = _U("\uf7be"); }

	else if(iconName == "fa-user-secret"){
	  label = _U("\uf21b"); }

	else if(iconName == "fa-otter"){
	  label = _U("\uf700"); }

	else if(iconName == "fa-dreidel"){
	  label = _U("\uf792"); }

	else if(iconName == "fa-person-dress"){
	  label = _U("\uf182"); }

	else if(iconName == "fa-female"){
	  label = _U("\uf182"); }

	else if(iconName == "fa-comment-dollar"){
	  label = _U("\uf651"); }

	else if(iconName == "fa-business-time"){
	  label = _U("\uf64a"); }

	else if(iconName == "fa-briefcase-clock"){
	  label = _U("\uf64a"); }

	else if(iconName == "fa-flower-tulip"){
	  label = _U("\uf801"); }

	else if(iconName == "fa-people-pants-simple"){
	  label = _U("\ue21a"); }

	else if(iconName == "fa-cloud-drizzle"){
	  label = _U("\uf738"); }

	else if(iconName == "fa-table-cells-large"){
	  label = _U("\uf009"); }

	else if(iconName == "fa-th-large"){
	  label = _U("\uf009"); }

	else if(iconName == "fa-book-tanakh"){
	  label = _U("\uf827"); }

	else if(iconName == "fa-tanakh"){
	  label = _U("\uf827"); }

	else if(iconName == "fa-solar-system"){
	  label = _U("\ue02f"); }

	else if(iconName == "fa-seal-question"){
	  label = _U("\ue243"); }

	else if(iconName == "fa-phone-volume"){
	  label = _U("\uf2a0"); }

	else if(iconName == "fa-volume-control-phone"){
	  label = _U("\uf2a0"); }

	else if(iconName == "fa-disc-drive"){
	  label = _U("\uf8b5"); }

	else if(iconName == "fa-hat-cowboy-side"){
	  label = _U("\uf8c1"); }

	else if(iconName == "fa-table-rows"){
	  label = _U("\ue292"); }

	else if(iconName == "fa-rows"){
	  label = _U("\ue292"); }

	else if(iconName == "fa-location-exclamation"){
	  label = _U("\uf608"); }

	else if(iconName == "fa-map-marker-exclamation"){
	  label = _U("\uf608"); }

	else if(iconName == "fa-face-fearful"){
	  label = _U("\ue375"); }

	else if(iconName == "fa-clipboard-user"){
	  label = _U("\uf7f3"); }

	else if(iconName == "fa-bus-school"){
	  label = _U("\uf5dd"); }

	else if(iconName == "fa-film-slash"){
	  label = _U("\ue179"); }

	else if(iconName == "fa-square-arrow-down-right"){
	  label = _U("\ue262"); }

	else if(iconName == "fa-book-sparkles"){
	  label = _U("\uf6b8"); }

	else if(iconName == "fa-book-spells"){
	  label = _U("\uf6b8"); }

	else if(iconName == "fa-washing-machine"){
	  label = _U("\uf898"); }

	else if(iconName == "fa-washer"){
	  label = _U("\uf898"); }

	else if(iconName == "fa-child"){
	  label = _U("\uf1ae"); }

	else if(iconName == "fa-lira-sign"){
	  label = _U("\uf195"); }

	else if(iconName == "fa-user-visor"){
	  label = _U("\ue04c"); }

	else if(iconName == "fa-file-plus-minus"){
	  label = _U("\ue177"); }

	else if(iconName == "fa-chess-clock-flip"){
	  label = _U("\uf43e"); }

	else if(iconName == "fa-chess-clock-alt"){
	  label = _U("\uf43e"); }

	else if(iconName == "fa-satellite"){
	  label = _U("\uf7bf"); }

	else if(iconName == "fa-truck-fire"){
	  label = _U("\ue65a"); }

	else if(iconName == "fa-plane-lock"){
	  label = _U("\ue558"); }

	else if(iconName == "fa-steering-wheel"){
	  label = _U("\uf622"); }

	else if(iconName == "fa-tag"){
	  label = _U("\uf02b"); }

	else if(iconName == "fa-stretcher"){
	  label = _U("\uf825"); }

	else if(iconName == "fa-book-section"){
	  label = _U("\ue0c1"); }

	else if(iconName == "fa-book-law"){
	  label = _U("\ue0c1"); }

	else if(iconName == "fa-inboxes"){
	  label = _U("\ue1bb"); }

	else if(iconName == "fa-coffee-bean"){
	  label = _U("\ue13e"); }

	else if(iconName == "fa-circle-yen"){
	  label = _U("\ue5d0"); }

	else if(iconName == "fa-brackets-curly"){
	  label = _U("\uf7ea"); }

	else if(iconName == "fa-ellipsis-stroke-vertical"){
	  label = _U("\uf39c"); }

	else if(iconName == "fa-ellipsis-v-alt"){
	  label = _U("\uf39c"); }

	else if(iconName == "fa-comment"){
	  label = _U("\uf075"); }

	else if(iconName == "fa-square-1"){
	  label = _U("\ue256"); }

	else if(iconName == "fa-cake-candles"){
	  label = _U("\uf1fd"); }

	else if(iconName == "fa-birthday-cake"){
	  label = _U("\uf1fd"); }

	else if(iconName == "fa-cake"){
	  label = _U("\uf1fd"); }

	else if(iconName == "fa-head-side"){
	  label = _U("\uf6e9"); }

	else if(iconName == "fa-truck-ladder"){
	  label = _U("\ue657"); }

	else if(iconName == "fa-envelope"){
	  label = _U("\uf0e0"); }

	else if(iconName == "fa-dolly-empty"){
	  label = _U("\uf473"); }

	else if(iconName == "fa-face-tissue"){
	  label = _U("\ue39c"); }

	else if(iconName == "fa-angles-up"){
	  label = _U("\uf102"); }

	else if(iconName == "fa-angle-double-up"){
	  label = _U("\uf102"); }

	else if(iconName == "fa-bin-recycle"){
	  label = _U("\ue5f7"); }

	else if(iconName == "fa-paperclip"){
	  label = _U("\uf0c6"); }

	else if(iconName == "fa-chart-line-down"){
	  label = _U("\uf64d"); }

	else if(iconName == "fa-arrow-right-to-city"){
	  label = _U("\ue4b3"); }

	else if(iconName == "fa-lock-a"){
	  label = _U("\ue422"); }

	else if(iconName == "fa-ribbon"){
	  label = _U("\uf4d6"); }

	else if(iconName == "fa-lungs"){
	  label = _U("\uf604"); }

	else if(iconName == "fa-person-pinball"){
	  label = _U("\ue21d"); }

	else if(iconName == "fa-arrow-up-9-1"){
	  label = _U("\uf887"); }

	else if(iconName == "fa-sort-numeric-up-alt"){
	  label = _U("\uf887"); }

	else if(iconName == "fa-apple-core"){
	  label = _U("\ue08f"); }

	else if(iconName == "fa-circle-y"){
	  label = _U("\ue12f"); }

	else if(iconName == "fa-h6"){
	  label = _U("\ue413"); }

	else if(iconName == "fa-litecoin-sign"){
	  label = _U("\ue1d3"); }

	else if(iconName == "fa-circle-small"){
	  label = _U("\ue122"); }

	else if(iconName == "fa-border-none"){
	  label = _U("\uf850"); }

	else if(iconName == "fa-arrow-turn-down-left"){
	  label = _U("\ue2e1"); }

	else if(iconName == "fa-circle-nodes"){
	  label = _U("\ue4e2"); }

	else if(iconName == "fa-parachute-box"){
	  label = _U("\uf4cd"); }

	else if(iconName == "fa-reflect-horizontal"){
	  label = _U("\ue664"); }

	else if(iconName == "fa-message-medical"){
	  label = _U("\uf7f4"); }

	else if(iconName == "fa-comment-alt-medical"){
	  label = _U("\uf7f4"); }

	else if(iconName == "fa-rugby-ball"){
	  label = _U("\ue3c6"); }

	else if(iconName == "fa-comment-music"){
	  label = _U("\uf8b0"); }

	else if(iconName == "fa-indent"){
	  label = _U("\uf03c"); }

	else if(iconName == "fa-tree-deciduous"){
	  label = _U("\uf400"); }

	else if(iconName == "fa-tree-alt"){
	  label = _U("\uf400"); }

	else if(iconName == "fa-puzzle-piece-simple"){
	  label = _U("\ue231"); }

	else if(iconName == "fa-puzzle-piece-alt"){
	  label = _U("\ue231"); }

	else if(iconName == "fa-truck-field-un"){
	  label = _U("\ue58e"); }

	else if(iconName == "fa-nfc-trash"){
	  label = _U("\ue1fd"); }

	else if(iconName == "fa-hourglass"){
	  label = _U("\uf254"); }

	else if(iconName == "fa-hourglass-empty"){
	  label = _U("\uf254"); }

	else if(iconName == "fa-mountain"){
	  label = _U("\uf6fc"); }

	else if(iconName == "fa-file-xmark"){
	  label = _U("\uf317"); }

	else if(iconName == "fa-file-times"){
	  label = _U("\uf317"); }

	else if(iconName == "fa-house-heart"){
	  label = _U("\uf4c9"); }

	else if(iconName == "fa-home-heart"){
	  label = _U("\uf4c9"); }

	else if(iconName == "fa-house-chimney-blank"){
	  label = _U("\ue3b0"); }

	else if(iconName == "fa-meter-bolt"){
	  label = _U("\ue1e9"); }

	else if(iconName == "fa-user-doctor"){
	  label = _U("\uf0f0"); }

	else if(iconName == "fa-user-md"){
	  label = _U("\uf0f0"); }

	else if(iconName == "fa-slash-back"){
	  label = _U("\u005c"); }

	else if(iconName == "fa-circle-info"){
	  label = _U("\uf05a"); }

	else if(iconName == "fa-info-circle"){
	  label = _U("\uf05a"); }

	else if(iconName == "fa-fishing-rod"){
	  label = _U("\ue3a8"); }

	else if(iconName == "fa-hammer-crash"){
	  label = _U("\ue414"); }

	else if(iconName == "fa-message-heart"){
	  label = _U("\ue5c9"); }

	else if(iconName == "fa-cloud-meatball"){
	  label = _U("\uf73b"); }

	else if(iconName == "fa-camera-polaroid"){
	  label = _U("\uf8aa"); }

	else if(iconName == "fa-camera"){
	  label = _U("\uf030"); }

	else if(iconName == "fa-camera-alt"){
	  label = _U("\uf030"); }

	else if(iconName == "fa-square-virus"){
	  label = _U("\ue578"); }

	else if(iconName == "fa-cart-arrow-up"){
	  label = _U("\ue3ee"); }

	else if(iconName == "fa-meteor"){
	  label = _U("\uf753"); }

	else if(iconName == "fa-car-on"){
	  label = _U("\ue4dd"); }

	else if(iconName == "fa-sleigh"){
	  label = _U("\uf7cc"); }

	else if(iconName == "fa-arrow-down-1-9"){
	  label = _U("\uf162"); }

	else if(iconName == "fa-sort-numeric-asc"){
	  label = _U("\uf162"); }

	else if(iconName == "fa-sort-numeric-down"){
	  label = _U("\uf162"); }

	else if(iconName == "fa-buoy-mooring"){
	  label = _U("\ue5b6"); }

	else if(iconName == "fa-square-4"){
	  label = _U("\ue259"); }

	else if(iconName == "fa-hand-holding-droplet"){
	  label = _U("\uf4c1"); }

	else if(iconName == "fa-hand-holding-water"){
	  label = _U("\uf4c1"); }

	else if(iconName == "fa-file-eps"){
	  label = _U("\ue644"); }

	else if(iconName == "fa-tricycle-adult"){
	  label = _U("\ue5c4"); }

	else if(iconName == "fa-waveform"){
	  label = _U("\uf8f1"); }

	else if(iconName == "fa-water"){
	  label = _U("\uf773"); }

	else if(iconName == "fa-star-sharp-half-stroke"){
	  label = _U("\ue28d"); }

	else if(iconName == "fa-star-sharp-half-alt"){
	  label = _U("\ue28d"); }

	else if(iconName == "fa-nfc-signal"){
	  label = _U("\ue1fb"); }

	else if(iconName == "fa-plane-prop"){
	  label = _U("\ue22b"); }

	else if(iconName == "fa-calendar-check"){
	  label = _U("\uf274"); }

	else if(iconName == "fa-clock-desk"){
	  label = _U("\ue134"); }

	else if(iconName == "fa-calendar-clock"){
	  label = _U("\ue0d2"); }

	else if(iconName == "fa-calendar-time"){
	  label = _U("\ue0d2"); }

	else if(iconName == "fa-braille"){
	  label = _U("\uf2a1"); }

	else if(iconName == "fa-prescription-bottle-medical"){
	  label = _U("\uf486"); }

	else if(iconName == "fa-prescription-bottle-alt"){
	  label = _U("\uf486"); }

	else if(iconName == "fa-plate-utensils"){
	  label = _U("\ue43b"); }

	else if(iconName == "fa-family-pants"){
	  label = _U("\ue302"); }

	else if(iconName == "fa-hose-reel"){
	  label = _U("\ue41a"); }

	else if(iconName == "fa-house-window"){
	  label = _U("\ue3b3"); }

	else if(iconName == "fa-landmark"){
	  label = _U("\uf66f"); }

	else if(iconName == "fa-truck"){
	  label = _U("\uf0d1"); }

	else if(iconName == "fa-music-magnifying-glass"){
	  label = _U("\ue662"); }

	else if(iconName == "fa-crosshairs"){
	  label = _U("\uf05b"); }

	else if(iconName == "fa-cloud-rainbow"){
	  label = _U("\uf73e"); }

	else if(iconName == "fa-person-cane"){
	  label = _U("\ue53c"); }

	else if(iconName == "fa-alien"){
	  label = _U("\uf8f5"); }

	else if(iconName == "fa-tent"){
	  label = _U("\ue57d"); }

	else if(iconName == "fa-laptop-binary"){
	  label = _U("\ue5e7"); }

	else if(iconName == "fa-vest-patches"){
	  label = _U("\ue086"); }

	else if(iconName == "fa-people-dress-simple"){
	  label = _U("\ue218"); }

	else if(iconName == "fa-check-double"){
	  label = _U("\uf560"); }

	else if(iconName == "fa-arrow-down-a-z"){
	  label = _U("\uf15d"); }

	else if(iconName == "fa-sort-alpha-asc"){
	  label = _U("\uf15d"); }

	else if(iconName == "fa-sort-alpha-down"){
	  label = _U("\uf15d"); }

	else if(iconName == "fa-bowling-ball-pin"){
	  label = _U("\ue0c3"); }

	else if(iconName == "fa-bell-school-slash"){
	  label = _U("\uf5d6"); }

	else if(iconName == "fa-plus-large"){
	  label = _U("\ue59e"); }

	else if(iconName == "fa-money-bill-wheat"){
	  label = _U("\ue52a"); }

	else if(iconName == "fa-camera-viewfinder"){
	  label = _U("\ue0da"); }

	else if(iconName == "fa-screenshot"){
	  label = _U("\ue0da"); }

	else if(iconName == "fa-message-music"){
	  label = _U("\uf8af"); }

	else if(iconName == "fa-comment-alt-music"){
	  label = _U("\uf8af"); }

	else if(iconName == "fa-car-building"){
	  label = _U("\uf859"); }

	else if(iconName == "fa-border-bottom-right"){
	  label = _U("\uf854"); }

	else if(iconName == "fa-border-style-alt"){
	  label = _U("\uf854"); }

	else if(iconName == "fa-octagon"){
	  label = _U("\uf306"); }

	else if(iconName == "fa-comment-arrow-up-right"){
	  label = _U("\ue145"); }

	else if(iconName == "fa-octagon-divide"){
	  label = _U("\ue203"); }

	else if(iconName == "fa-cookie"){
	  label = _U("\uf563"); }

	else if(iconName == "fa-arrow-rotate-left"){
	  label = _U("\uf0e2"); }

	else if(iconName == "fa-arrow-left-rotate"){
	  label = _U("\uf0e2"); }

	else if(iconName == "fa-arrow-rotate-back"){
	  label = _U("\uf0e2"); }

	else if(iconName == "fa-arrow-rotate-backward"){
	  label = _U("\uf0e2"); }

	else if(iconName == "fa-undo"){
	  label = _U("\uf0e2"); }

	else if(iconName == "fa-tv-music"){
	  label = _U("\uf8e6"); }

	else if(iconName == "fa-hard-drive"){
	  label = _U("\uf0a0"); }

	else if(iconName == "fa-hdd"){
	  label = _U("\uf0a0"); }

	else if(iconName == "fa-reel"){
	  label = _U("\ue238"); }

	else if(iconName == "fa-face-grin-squint-tears"){
	  label = _U("\uf586"); }

	else if(iconName == "fa-grin-squint-tears"){
	  label = _U("\uf586"); }

	else if(iconName == "fa-dumbbell"){
	  label = _U("\uf44b"); }

	else if(iconName == "fa-rectangle-list"){
	  label = _U("\uf022"); }

	else if(iconName == "fa-list-alt"){
	  label = _U("\uf022"); }

	else if(iconName == "fa-tarp-droplet"){
	  label = _U("\ue57c"); }

	else if(iconName == "fa-alarm-exclamation"){
	  label = _U("\uf843"); }

	else if(iconName == "fa-house-medical-circle-check"){
	  label = _U("\ue511"); }

	else if(iconName == "fa-traffic-cone"){
	  label = _U("\uf636"); }

	else if(iconName == "fa-grate"){
	  label = _U("\ue193"); }

	else if(iconName == "fa-arrow-down-right"){
	  label = _U("\ue093"); }

	else if(iconName == "fa-person-skiing-nordic"){
	  label = _U("\uf7ca"); }

	else if(iconName == "fa-skiing-nordic"){
	  label = _U("\uf7ca"); }

	else if(iconName == "fa-calendar-plus"){
	  label = _U("\uf271"); }

	else if(iconName == "fa-person-from-portal"){
	  label = _U("\ue023"); }

	else if(iconName == "fa-portal-exit"){
	  label = _U("\ue023"); }

	else if(iconName == "fa-plane-arrival"){
	  label = _U("\uf5af"); }

	else if(iconName == "fa-cowbell-circle-plus"){
	  label = _U("\uf8b4"); }

	else if(iconName == "fa-cowbell-more"){
	  label = _U("\uf8b4"); }

	else if(iconName == "fa-circle-left"){
	  label = _U("\uf359"); }

	else if(iconName == "fa-arrow-alt-circle-left"){
	  label = _U("\uf359"); }

	else if(iconName == "fa-distribute-spacing-vertical"){
	  label = _U("\ue366"); }

	else if(iconName == "fa-signal-bars-fair"){
	  label = _U("\uf692"); }

	else if(iconName == "fa-signal-alt-2"){
	  label = _U("\uf692"); }

	else if(iconName == "fa-sportsball"){
	  label = _U("\ue44b"); }

	else if(iconName == "fa-game-console-handheld-crank"){
	  label = _U("\ue5b9"); }

	else if(iconName == "fa-train-subway"){
	  label = _U("\uf239"); }

	else if(iconName == "fa-subway"){
	  label = _U("\uf239"); }

	else if(iconName == "fa-chart-gantt"){
	  label = _U("\ue0e4"); }

	else if(iconName == "fa-face-smile-upside-down"){
	  label = _U("\ue395"); }

	else if(iconName == "fa-ball-pile"){
	  label = _U("\uf77e"); }

	else if(iconName == "fa-badge-dollar"){
	  label = _U("\uf645"); }

	else if(iconName == "fa-money-bills-simple"){
	  label = _U("\ue1f4"); }

	else if(iconName == "fa-money-bills-alt"){
	  label = _U("\ue1f4"); }

	else if(iconName == "fa-list-timeline"){
	  label = _U("\ue1d1"); }

	else if(iconName == "fa-indian-rupee-sign"){
	  label = _U("\ue1bc"); }

	else if(iconName == "fa-indian-rupee"){
	  label = _U("\ue1bc"); }

	else if(iconName == "fa-inr"){
	  label = _U("\ue1bc"); }

	else if(iconName == "fa-crop-simple"){
	  label = _U("\uf565"); }

	else if(iconName == "fa-crop-alt"){
	  label = _U("\uf565"); }

	else if(iconName == "fa-money-bill-1"){
	  label = _U("\uf3d1"); }

	else if(iconName == "fa-money-bill-alt"){
	  label = _U("\uf3d1"); }

	else if(iconName == "fa-left-long"){
	  label = _U("\uf30a"); }

	else if(iconName == "fa-long-arrow-alt-left"){
	  label = _U("\uf30a"); }

	else if(iconName == "fa-keyboard-down"){
	  label = _U("\ue1c2"); }

	else if(iconName == "fa-circle-up-right"){
	  label = _U("\ue129"); }

	else if(iconName == "fa-cloud-bolt-moon"){
	  label = _U("\uf76d"); }

	else if(iconName == "fa-thunderstorm-moon"){
	  label = _U("\uf76d"); }

	else if(iconName == "fa-turn-left-up"){
	  label = _U("\ue638"); }

	else if(iconName == "fa-dna"){
	  label = _U("\uf471"); }

	else if(iconName == "fa-virus-slash"){
	  label = _U("\ue075"); }

	else if(iconName == "fa-bracket-round-right"){
	  label = _U("\u0029"); }

	else if(iconName == "fa-circle-sterling"){
	  label = _U("\ue5cf"); }

	else if(iconName == "fa-circle-5"){
	  label = _U("\ue0f2"); }

	else if(iconName == "fa-minus"){
	  label = _U("\uf068"); }

	else if(iconName == "fa-subtract"){
	  label = _U("\uf068"); }

	else if(iconName == "fa-fire-flame"){
	  label = _U("\uf6df"); }

	else if(iconName == "fa-flame"){
	  label = _U("\uf6df"); }

	else if(iconName == "fa-right-to-line"){
	  label = _U("\uf34c"); }

	else if(iconName == "fa-arrow-alt-to-right"){
	  label = _U("\uf34c"); }

	else if(iconName == "fa-gif"){
	  label = _U("\ue190"); }

	else if(iconName == "fa-chess"){
	  label = _U("\uf439"); }

	else if(iconName == "fa-trash-slash"){
	  label = _U("\ue2b3"); }

	else if(iconName == "fa-arrow-left-long"){
	  label = _U("\uf177"); }

	else if(iconName == "fa-long-arrow-left"){
	  label = _U("\uf177"); }

	else if(iconName == "fa-plug-circle-check"){
	  label = _U("\ue55c"); }

	else if(iconName == "fa-font-case"){
	  label = _U("\uf866"); }

	else if(iconName == "fa-street-view"){
	  label = _U("\uf21d"); }

	else if(iconName == "fa-arrow-down-left"){
	  label = _U("\ue091"); }

	else if(iconName == "fa-franc-sign"){
	  label = _U("\ue18f"); }

	else if(iconName == "fa-flask-round-poison"){
	  label = _U("\uf6e0"); }

	else if(iconName == "fa-flask-poison"){
	  label = _U("\uf6e0"); }

	else if(iconName == "fa-volume-off"){
	  label = _U("\uf026"); }

	else if(iconName == "fa-book-circle-arrow-right"){
	  label = _U("\ue0bc"); }

	else if(iconName == "fa-chart-user"){
	  label = _U("\uf6a3"); }

	else if(iconName == "fa-user-chart"){
	  label = _U("\uf6a3"); }

	else if(iconName == "fa-hands-asl-interpreting"){
	  label = _U("\uf2a3"); }

	else if(iconName == "fa-american-sign-language-interpreting"){
	  label = _U("\uf2a3"); }

	else if(iconName == "fa-asl-interpreting"){
	  label = _U("\uf2a3"); }

	else if(iconName == "fa-hands-american-sign-language-interpreting"){
	  label = _U("\uf2a3"); }

	else if(iconName == "fa-presentation-screen"){
	  label = _U("\uf685"); }

	else if(iconName == "fa-presentation"){
	  label = _U("\uf685"); }

	else if(iconName == "fa-circle-bolt"){
	  label = _U("\ue0fe"); }

	else if(iconName == "fa-face-smile-halo"){
	  label = _U("\ue38f"); }

	else if(iconName == "fa-cart-circle-arrow-down"){
	  label = _U("\ue3ef"); }

	else if(iconName == "fa-house-person-return"){
	  label = _U("\ue011"); }

	else if(iconName == "fa-house-person-arrive"){
	  label = _U("\ue011"); }

	else if(iconName == "fa-house-return"){
	  label = _U("\ue011"); }

	else if(iconName == "fa-message-xmark"){
	  label = _U("\uf4ab"); }

	else if(iconName == "fa-comment-alt-times"){
	  label = _U("\uf4ab"); }

	else if(iconName == "fa-message-times"){
	  label = _U("\uf4ab"); }

	else if(iconName == "fa-file-certificate"){
	  label = _U("\uf5f3"); }

	else if(iconName == "fa-file-award"){
	  label = _U("\uf5f3"); }

	else if(iconName == "fa-user-doctor-hair-long"){
	  label = _U("\ue459"); }

	else if(iconName == "fa-camera-security"){
	  label = _U("\uf8fe"); }

	else if(iconName == "fa-camera-home"){
	  label = _U("\uf8fe"); }

	else if(iconName == "fa-gear"){
	  label = _U("\uf013"); }

	else if(iconName == "fa-cog"){
	  label = _U("\uf013"); }

	else if(iconName == "fa-droplet-slash"){
	  label = _U("\uf5c7"); }

	else if(iconName == "fa-tint-slash"){
	  label = _U("\uf5c7"); }

	else if(iconName == "fa-book-heart"){
	  label = _U("\uf499"); }

	else if(iconName == "fa-mosque"){
	  label = _U("\uf678"); }

	else if(iconName == "fa-duck"){
	  label = _U("\uf6d8"); }

	else if(iconName == "fa-mosquito"){
	  label = _U("\ue52b"); }

	else if(iconName == "fa-star-of-david"){
	  label = _U("\uf69a"); }

	else if(iconName == "fa-flag-swallowtail"){
	  label = _U("\uf74c"); }

	else if(iconName == "fa-flag-alt"){
	  label = _U("\uf74c"); }

	else if(iconName == "fa-person-military-rifle"){
	  label = _U("\ue54b"); }

	else if(iconName == "fa-car-garage"){
	  label = _U("\uf5e2"); }

	else if(iconName == "fa-cart-shopping"){
	  label = _U("\uf07a"); }

	else if(iconName == "fa-shopping-cart"){
	  label = _U("\uf07a"); }

	else if(iconName == "fa-book-font"){
	  label = _U("\ue0bf"); }

	else if(iconName == "fa-shield-plus"){
	  label = _U("\ue24a"); }

	else if(iconName == "fa-vials"){
	  label = _U("\uf493"); }

	else if(iconName == "fa-eye-dropper-full"){
	  label = _U("\ue172"); }

	else if(iconName == "fa-distribute-spacing-horizontal"){
	  label = _U("\ue365"); }

	else if(iconName == "fa-tablet-rugged"){
	  label = _U("\uf48f"); }

	else if(iconName == "fa-temperature-snow"){
	  label = _U("\uf768"); }

	else if(iconName == "fa-temperature-frigid"){
	  label = _U("\uf768"); }

	else if(iconName == "fa-moped"){
	  label = _U("\ue3b9"); }

	else if(iconName == "fa-face-smile-plus"){
	  label = _U("\uf5b9"); }

	else if(iconName == "fa-smile-plus"){
	  label = _U("\uf5b9"); }

	else if(iconName == "fa-radio-tuner"){
	  label = _U("\uf8d8"); }

	else if(iconName == "fa-radio-alt"){
	  label = _U("\uf8d8"); }

	else if(iconName == "fa-face-swear"){
	  label = _U("\ue399"); }

	else if(iconName == "fa-water-arrow-down"){
	  label = _U("\uf774"); }

	else if(iconName == "fa-water-lower"){
	  label = _U("\uf774"); }

	else if(iconName == "fa-scanner-touchscreen"){
	  label = _U("\uf48a"); }

	else if(iconName == "fa-circle-7"){
	  label = _U("\ue0f4"); }

	else if(iconName == "fa-plug-circle-plus"){
	  label = _U("\ue55f"); }

	else if(iconName == "fa-person-ski-jumping"){
	  label = _U("\uf7c7"); }

	else if(iconName == "fa-ski-jump"){
	  label = _U("\uf7c7"); }

	else if(iconName == "fa-place-of-worship"){
	  label = _U("\uf67f"); }

	else if(iconName == "fa-water-arrow-up"){
	  label = _U("\uf775"); }

	else if(iconName == "fa-water-rise"){
	  label = _U("\uf775"); }

	else if(iconName == "fa-waveform-lines"){
	  label = _U("\uf8f2"); }

	else if(iconName == "fa-waveform-path"){
	  label = _U("\uf8f2"); }

	else if(iconName == "fa-split"){
	  label = _U("\ue254"); }

	else if(iconName == "fa-film-canister"){
	  label = _U("\uf8b7"); }

	else if(iconName == "fa-film-cannister"){
	  label = _U("\uf8b7"); }

	else if(iconName == "fa-folder-xmark"){
	  label = _U("\uf65f"); }

	else if(iconName == "fa-folder-times"){
	  label = _U("\uf65f"); }

	else if(iconName == "fa-toilet-paper-blank"){
	  label = _U("\uf71f"); }

	else if(iconName == "fa-toilet-paper-alt"){
	  label = _U("\uf71f"); }

	else if(iconName == "fa-tablet-screen"){
	  label = _U("\uf3fc"); }

	else if(iconName == "fa-tablet-android-alt"){
	  label = _U("\uf3fc"); }

	else if(iconName == "fa-hexagon-vertical-nft-slanted"){
	  label = _U("\ue506"); }

	else if(iconName == "fa-folder-music"){
	  label = _U("\ue18d"); }

	else if(iconName == "fa-display-medical"){
	  label = _U("\ue166"); }

	else if(iconName == "fa-desktop-medical"){
	  label = _U("\ue166"); }

	else if(iconName == "fa-share-all"){
	  label = _U("\uf367"); }

	else if(iconName == "fa-peapod"){
	  label = _U("\ue31c"); }

	else if(iconName == "fa-chess-clock"){
	  label = _U("\uf43d"); }

	else if(iconName == "fa-axe"){
	  label = _U("\uf6b2"); }

	else if(iconName == "fa-square-d"){
	  label = _U("\ue268"); }

	else if(iconName == "fa-grip-vertical"){
	  label = _U("\uf58e"); }

	else if(iconName == "fa-mobile-signal-out"){
	  label = _U("\ue1f0"); }

	else if(iconName == "fa-arrow-turn-up"){
	  label = _U("\uf148"); }

	else if(iconName == "fa-level-up"){
	  label = _U("\uf148"); }

	else if(iconName == "fa-u"){
	  label = _U("\u0055"); }

	else if(iconName == "fa-arrow-up-from-dotted-line"){
	  label = _U("\ue09b"); }

	else if(iconName == "fa-square-root-variable"){
	  label = _U("\uf698"); }

	else if(iconName == "fa-square-root-alt"){
	  label = _U("\uf698"); }

	else if(iconName == "fa-light-switch-on"){
	  label = _U("\ue019"); }

	else if(iconName == "fa-arrow-down-arrow-up"){
	  label = _U("\uf883"); }

	else if(iconName == "fa-sort-alt"){
	  label = _U("\uf883"); }

	else if(iconName == "fa-raindrops"){
	  label = _U("\uf75c"); }

	else if(iconName == "fa-dash"){
	  label = _U("\ue404"); }

	else if(iconName == "fa-minus-large"){
	  label = _U("\ue404"); }

	else if(iconName == "fa-clock"){
	  label = _U("\uf017"); }

	else if(iconName == "fa-clock-four"){
	  label = _U("\uf017"); }

	else if(iconName == "fa-input-numeric"){
	  label = _U("\ue1bd"); }

	else if(iconName == "fa-truck-tow"){
	  label = _U("\ue2b8"); }

	else if(iconName == "fa-backward-step"){
	  label = _U("\uf048"); }

	else if(iconName == "fa-step-backward"){
	  label = _U("\uf048"); }

	else if(iconName == "fa-pallet"){
	  label = _U("\uf482"); }

	else if(iconName == "fa-car-bolt"){
	  label = _U("\ue341"); }

	else if(iconName == "fa-arrows-maximize"){
	  label = _U("\uf31d"); }

	else if(iconName == "fa-expand-arrows"){
	  label = _U("\uf31d"); }

	else if(iconName == "fa-faucet"){
	  label = _U("\ue005"); }

	else if(iconName == "fa-cloud-sleet"){
	  label = _U("\uf741"); }

	else if(iconName == "fa-lamp-street"){
	  label = _U("\ue1c5"); }

	else if(iconName == "fa-list-radio"){
	  label = _U("\ue1d0"); }

	else if(iconName == "fa-pen-nib-slash"){
	  label = _U("\ue4a1"); }

	else if(iconName == "fa-baseball-bat-ball"){
	  label = _U("\uf432"); }

	else if(iconName == "fa-square-up-left"){
	  label = _U("\ue282"); }

	else if(iconName == "fa-overline"){
	  label = _U("\uf876"); }

	else if(iconName == "fa-s"){
	  label = _U("\u0053"); }

	else if(iconName == "fa-timeline"){
	  label = _U("\ue29c"); }

	else if(iconName == "fa-keyboard"){
	  label = _U("\uf11c"); }

	else if(iconName == "fa-arrows-from-dotted-line"){
	  label = _U("\ue0a3"); }

	else if(iconName == "fa-usb-drive"){
	  label = _U("\uf8e9"); }

	else if(iconName == "fa-ballot"){
	  label = _U("\uf732"); }

	else if(iconName == "fa-caret-down"){
	  label = _U("\uf0d7"); }

	else if(iconName == "fa-location-dot-slash"){
	  label = _U("\uf605"); }

	else if(iconName == "fa-map-marker-alt-slash"){
	  label = _U("\uf605"); }

	else if(iconName == "fa-cards"){
	  label = _U("\ue3ed"); }

	else if(iconName == "fa-house-chimney-medical"){
	  label = _U("\uf7f2"); }

	else if(iconName == "fa-clinic-medical"){
	  label = _U("\uf7f2"); }

	else if(iconName == "fa-boxing-glove"){
	  label = _U("\uf438"); }

	else if(iconName == "fa-glove-boxing"){
	  label = _U("\uf438"); }

	else if(iconName == "fa-temperature-three-quarters"){
	  label = _U("\uf2c8"); }

	else if(iconName == "fa-temperature-3"){
	  label = _U("\uf2c8"); }

	else if(iconName == "fa-thermometer-3"){
	  label = _U("\uf2c8"); }

	else if(iconName == "fa-thermometer-three-quarters"){
	  label = _U("\uf2c8"); }

	else if(iconName == "fa-bell-school"){
	  label = _U("\uf5d5"); }

	else if(iconName == "fa-mobile-screen"){
	  label = _U("\uf3cf"); }

	else if(iconName == "fa-mobile-android-alt"){
	  label = _U("\uf3cf"); }

	else if(iconName == "fa-plane-up"){
	  label = _U("\ue22d"); }

	else if(iconName == "fa-folder-heart"){
	  label = _U("\ue189"); }

	else if(iconName == "fa-circle-location-arrow"){
	  label = _U("\uf602"); }

	else if(iconName == "fa-location-circle"){
	  label = _U("\uf602"); }

	else if(iconName == "fa-face-head-bandage"){
	  label = _U("\ue37a"); }

	else if(iconName == "fa-sushi-roll"){
	  label = _U("\ue48b"); }

	else if(iconName == "fa-maki-roll"){
	  label = _U("\ue48b"); }

	else if(iconName == "fa-makizushi"){
	  label = _U("\ue48b"); }

	else if(iconName == "fa-car-bump"){
	  label = _U("\uf5e0"); }

	else if(iconName == "fa-piggy-bank"){
	  label = _U("\uf4d3"); }

	else if(iconName == "fa-racquet"){
	  label = _U("\uf45a"); }

	else if(iconName == "fa-car-mirrors"){
	  label = _U("\ue343"); }

	else if(iconName == "fa-industry-windows"){
	  label = _U("\uf3b3"); }

	else if(iconName == "fa-industry-alt"){
	  label = _U("\uf3b3"); }

	else if(iconName == "fa-bolt-auto"){
	  label = _U("\ue0b6"); }

	else if(iconName == "fa-battery-half"){
	  label = _U("\uf242"); }

	else if(iconName == "fa-battery-3"){
	  label = _U("\uf242"); }

	else if(iconName == "fa-flux-capacitor"){
	  label = _U("\uf8ba"); }

	else if(iconName == "fa-mountain-city"){
	  label = _U("\ue52e"); }

	else if(iconName == "fa-coins"){
	  label = _U("\uf51e"); }

	else if(iconName == "fa-honey-pot"){
	  label = _U("\ue418"); }

	else if(iconName == "fa-olive"){
	  label = _U("\ue316"); }

	else if(iconName == "fa-khanda"){
	  label = _U("\uf66d"); }

	else if(iconName == "fa-filter-list"){
	  label = _U("\ue17c"); }

	else if(iconName == "fa-outlet"){
	  label = _U("\ue01c"); }

	else if(iconName == "fa-sliders"){
	  label = _U("\uf1de"); }

	else if(iconName == "fa-sliders-h"){
	  label = _U("\uf1de"); }

	else if(iconName == "fa-cauldron"){
	  label = _U("\uf6bf"); }

	else if(iconName == "fa-people"){
	  label = _U("\ue216"); }

	else if(iconName == "fa-folder-tree"){
	  label = _U("\uf802"); }

	else if(iconName == "fa-network-wired"){
	  label = _U("\uf6ff"); }

	else if(iconName == "fa-croissant"){
	  label = _U("\uf7f6"); }

	else if(iconName == "fa-map-pin"){
	  label = _U("\uf276"); }

	else if(iconName == "fa-hamsa"){
	  label = _U("\uf665"); }

	else if(iconName == "fa-cent-sign"){
	  label = _U("\ue3f5"); }

	else if(iconName == "fa-swords-laser"){
	  label = _U("\ue03d"); }

	else if(iconName == "fa-flask"){
	  label = _U("\uf0c3"); }

	else if(iconName == "fa-person-pregnant"){
	  label = _U("\ue31e"); }

	else if(iconName == "fa-square-u"){
	  label = _U("\ue281"); }

	else if(iconName == "fa-wand-sparkles"){
	  label = _U("\uf72b"); }

	else if(iconName == "fa-router"){
	  label = _U("\uf8da"); }

	else if(iconName == "fa-ellipsis-vertical"){
	  label = _U("\uf142"); }

	else if(iconName == "fa-ellipsis-v"){
	  label = _U("\uf142"); }

	else if(iconName == "fa-sword-laser-alt"){
	  label = _U("\ue03c"); }

	else if(iconName == "fa-ticket"){
	  label = _U("\uf145"); }

	else if(iconName == "fa-power-off"){
	  label = _U("\uf011"); }

	else if(iconName == "fa-coin"){
	  label = _U("\uf85c"); }

	else if(iconName == "fa-laptop-slash"){
	  label = _U("\ue1c7"); }

	else if(iconName == "fa-right-long"){
	  label = _U("\uf30b"); }

	else if(iconName == "fa-long-arrow-alt-right"){
	  label = _U("\uf30b"); }

	else if(iconName == "fa-circle-b"){
	  label = _U("\ue0fd"); }

	else if(iconName == "fa-person-dress-simple"){
	  label = _U("\ue21c"); }

	else if(iconName == "fa-pipe-collar"){
	  label = _U("\ue437"); }

	else if(iconName == "fa-lights-holiday"){
	  label = _U("\uf7b2"); }

	else if(iconName == "fa-citrus"){
	  label = _U("\ue2f4"); }

	else if(iconName == "fa-flag-usa"){
	  label = _U("\uf74d"); }

	else if(iconName == "fa-laptop-file"){
	  label = _U("\ue51d"); }

	else if(iconName == "fa-tty"){
	  label = _U("\uf1e4"); }

	else if(iconName == "fa-teletype"){
	  label = _U("\uf1e4"); }

	else if(iconName == "fa-chart-tree-map"){
	  label = _U("\ue0ea"); }

	else if(iconName == "fa-diagram-next"){
	  label = _U("\ue476"); }

	else if(iconName == "fa-person-rifle"){
	  label = _U("\ue54e"); }

	else if(iconName == "fa-clock-five-thirty"){
	  label = _U("\ue34a"); }

	else if(iconName == "fa-pipe-valve"){
	  label = _U("\ue439"); }

	else if(iconName == "fa-arrow-up-from-arc"){
	  label = _U("\ue4b4"); }

	else if(iconName == "fa-face-spiral-eyes"){
	  label = _U("\ue485"); }

	else if(iconName == "fa-compress-wide"){
	  label = _U("\uf326"); }

	else if(iconName == "fa-circle-phone-hangup"){
	  label = _U("\ue11d"); }

	else if(iconName == "fa-phone-circle-down"){
	  label = _U("\ue11d"); }

	else if(iconName == "fa-gear-complex-code"){
	  label = _U("\ue5eb"); }

	else if(iconName == "fa-house-medical-circle-exclamation"){
	  label = _U("\ue512"); }

	else if(iconName == "fa-badminton"){
	  label = _U("\ue33a"); }

	else if(iconName == "fa-closed-captioning"){
	  label = _U("\uf20a"); }

	else if(iconName == "fa-person-hiking"){
	  label = _U("\uf6ec"); }

	else if(iconName == "fa-hiking"){
	  label = _U("\uf6ec"); }

	else if(iconName == "fa-right-from-line"){
	  label = _U("\uf347"); }

	else if(iconName == "fa-arrow-alt-from-left"){
	  label = _U("\uf347"); }

	else if(iconName == "fa-venus-double"){
	  label = _U("\uf226"); }

	else if(iconName == "fa-images"){
	  label = _U("\uf302"); }

	else if(iconName == "fa-calculator"){
	  label = _U("\uf1ec"); }

	else if(iconName == "fa-shuttlecock"){
	  label = _U("\uf45b"); }

	else if(iconName == "fa-user-hair"){
	  label = _U("\ue45a"); }

	else if(iconName == "fa-eye-evil"){
	  label = _U("\uf6db"); }

	else if(iconName == "fa-people-pulling"){
	  label = _U("\ue535"); }

	else if(iconName == "fa-n"){
	  label = _U("\u004e"); }

	else if(iconName == "fa-swap"){
	  label = _U("\ue609"); }

	else if(iconName == "fa-garage"){
	  label = _U("\ue009"); }

	else if(iconName == "fa-cable-car"){
	  label = _U("\uf7da"); }

	else if(iconName == "fa-tram"){
	  label = _U("\uf7da"); }

	else if(iconName == "fa-shovel-snow"){
	  label = _U("\uf7c3"); }

	else if(iconName == "fa-cloud-rain"){
	  label = _U("\uf73d"); }

	else if(iconName == "fa-face-lying"){
	  label = _U("\ue37e"); }

	else if(iconName == "fa-sprinkler"){
	  label = _U("\ue035"); }

	else if(iconName == "fa-building-circle-xmark"){
	  label = _U("\ue4d4"); }

	else if(iconName == "fa-person-sledding"){
	  label = _U("\uf7cb"); }

	else if(iconName == "fa-sledding"){
	  label = _U("\uf7cb"); }

	else if(iconName == "fa-game-console-handheld"){
	  label = _U("\uf8bb"); }

	else if(iconName == "fa-ship"){
	  label = _U("\uf21a"); }

	else if(iconName == "fa-clock-six-thirty"){
	  label = _U("\ue353"); }

	else if(iconName == "fa-battery-slash"){
	  label = _U("\uf377"); }

	else if(iconName == "fa-tugrik-sign"){
	  label = _U("\ue2ba"); }

	else if(iconName == "fa-arrows-down-to-line"){
	  label = _U("\ue4b8"); }

	else if(iconName == "fa-download"){
	  label = _U("\uf019"); }

	else if(iconName == "fa-angles-up-down"){
	  label = _U("\ue60d"); }

	else if(iconName == "fa-shelves"){
	  label = _U("\uf480"); }

	else if(iconName == "fa-inventory"){
	  label = _U("\uf480"); }

	else if(iconName == "fa-cloud-snow"){
	  label = _U("\uf742"); }

	else if(iconName == "fa-face-grin"){
	  label = _U("\uf580"); }

	else if(iconName == "fa-grin"){
	  label = _U("\uf580"); }

	else if(iconName == "fa-delete-left"){
	  label = _U("\uf55a"); }

	else if(iconName == "fa-backspace"){
	  label = _U("\uf55a"); }

	else if(iconName == "fa-oven"){
	  label = _U("\ue01d"); }

	else if(iconName == "fa-cloud-binary"){
	  label = _U("\ue601"); }

	else if(iconName == "fa-eye-dropper"){
	  label = _U("\uf1fb"); }

	else if(iconName == "fa-eye-dropper-empty"){
	  label = _U("\uf1fb"); }

	else if(iconName == "fa-eyedropper"){
	  label = _U("\uf1fb"); }

	else if(iconName == "fa-comment-captions"){
	  label = _U("\ue146"); }

	else if(iconName == "fa-comments-question"){
	  label = _U("\ue14e"); }

	else if(iconName == "fa-scribble"){
	  label = _U("\ue23f"); }

	else if(iconName == "fa-rotate-exclamation"){
	  label = _U("\ue23c"); }

	else if(iconName == "fa-file-circle-check"){
	  label = _U("\ue5a0"); }

	else if(iconName == "fa-glass"){
	  label = _U("\uf804"); }

	else if(iconName == "fa-loader"){
	  label = _U("\ue1d4"); }

	else if(iconName == "fa-forward"){
	  label = _U("\uf04e"); }

	else if(iconName == "fa-user-pilot"){
	  label = _U("\ue2c0"); }

	else if(iconName == "fa-mobile"){
	  label = _U("\uf3ce"); }

	else if(iconName == "fa-mobile-android"){
	  label = _U("\uf3ce"); }

	else if(iconName == "fa-mobile-phone"){
	  label = _U("\uf3ce"); }

	else if(iconName == "fa-code-pull-request-closed"){
	  label = _U("\ue3f9"); }

	else if(iconName == "fa-face-meh"){
	  label = _U("\uf11a"); }

	else if(iconName == "fa-meh"){
	  label = _U("\uf11a"); }

	else if(iconName == "fa-align-center"){
	  label = _U("\uf037"); }

	else if(iconName == "fa-book-skull"){
	  label = _U("\uf6b7"); }

	else if(iconName == "fa-book-dead"){
	  label = _U("\uf6b7"); }

	else if(iconName == "fa-id-card"){
	  label = _U("\uf2c2"); }

	else if(iconName == "fa-drivers-license"){
	  label = _U("\uf2c2"); }

	else if(iconName == "fa-face-dotted"){
	  label = _U("\ue47f"); }

	else if(iconName == "fa-face-worried"){
	  label = _U("\ue3a3"); }

	else if(iconName == "fa-outdent"){
	  label = _U("\uf03b"); }

	else if(iconName == "fa-dedent"){
	  label = _U("\uf03b"); }

	else if(iconName == "fa-court-sport"){
	  label = _U("\ue643"); }

	else if(iconName == "fa-heart-circle-exclamation"){
	  label = _U("\ue4fe"); }

	else if(iconName == "fa-house"){
	  label = _U("\uf015"); }

	else if(iconName == "fa-home"){
	  label = _U("\uf015"); }

	else if(iconName == "fa-home-alt"){
	  label = _U("\uf015"); }

	else if(iconName == "fa-home-lg-alt"){
	  label = _U("\uf015"); }

	else if(iconName == "fa-vector-circle"){
	  label = _U("\ue2c6"); }

	else if(iconName == "fa-car-circle-bolt"){
	  label = _U("\ue342"); }

	else if(iconName == "fa-calendar-week"){
	  label = _U("\uf784"); }

	else if(iconName == "fa-flying-disc"){
	  label = _U("\ue3a9"); }

	else if(iconName == "fa-laptop-medical"){
	  label = _U("\uf812"); }

	else if(iconName == "fa-square-down-right"){
	  label = _U("\ue26c"); }

	else if(iconName == "fa-b"){
	  label = _U("\u0042"); }

	else if(iconName == "fa-seat-airline"){
	  label = _U("\ue244"); }

	else if(iconName == "fa-moon-over-sun"){
	  label = _U("\uf74a"); }

	else if(iconName == "fa-eclipse-alt"){
	  label = _U("\uf74a"); }

	else if(iconName == "fa-pipe"){
	  label = _U("\u007c"); }

	else if(iconName == "fa-file-medical"){
	  label = _U("\uf477"); }

	else if(iconName == "fa-potato"){
	  label = _U("\ue440"); }

	else if(iconName == "fa-dice-one"){
	  label = _U("\uf525"); }

	else if(iconName == "fa-circle-a"){
	  label = _U("\ue0f7"); }

	else if(iconName == "fa-helmet-battle"){
	  label = _U("\uf6eb"); }

	else if(iconName == "fa-butter"){
	  label = _U("\ue3e4"); }

	else if(iconName == "fa-blanket-fire"){
	  label = _U("\ue3da"); }

	else if(iconName == "fa-kiwi-bird"){
	  label = _U("\uf535"); }

	else if(iconName == "fa-castle"){
	  label = _U("\ue0de"); }

	else if(iconName == "fa-golf-club"){
	  label = _U("\uf451"); }

	else if(iconName == "fa-arrow-right-arrow-left"){
	  label = _U("\uf0ec"); }

	else if(iconName == "fa-exchange"){
	  label = _U("\uf0ec"); }

	else if(iconName == "fa-rotate-right"){
	  label = _U("\uf2f9"); }

	else if(iconName == "fa-redo-alt"){
	  label = _U("\uf2f9"); }

	else if(iconName == "fa-rotate-forward"){
	  label = _U("\uf2f9"); }

	else if(iconName == "fa-utensils"){
	  label = _U("\uf2e7"); }

	else if(iconName == "fa-cutlery"){
	  label = _U("\uf2e7"); }

	else if(iconName == "fa-arrow-up-wide-short"){
	  label = _U("\uf161"); }

	else if(iconName == "fa-sort-amount-up"){
	  label = _U("\uf161"); }

	else if(iconName == "fa-chart-pie-simple-circle-dollar"){
	  label = _U("\ue605"); }

	else if(iconName == "fa-balloons"){
	  label = _U("\ue2e4"); }

	else if(iconName == "fa-mill-sign"){
	  label = _U("\ue1ed"); }

	else if(iconName == "fa-bowl-rice"){
	  label = _U("\ue2eb"); }

	else if(iconName == "fa-timeline-arrow"){
	  label = _U("\ue29d"); }

	else if(iconName == "fa-skull"){
	  label = _U("\uf54c"); }

	else if(iconName == "fa-game-board-simple"){
	  label = _U("\uf868"); }

	else if(iconName == "fa-game-board-alt"){
	  label = _U("\uf868"); }

	else if(iconName == "fa-circle-video"){
	  label = _U("\ue12b"); }

	else if(iconName == "fa-video-circle"){
	  label = _U("\ue12b"); }

	else if(iconName == "fa-chart-scatter-bubble"){
	  label = _U("\ue0e9"); }

	else if(iconName == "fa-house-turret"){
	  label = _U("\ue1b4"); }

	else if(iconName == "fa-banana"){
	  label = _U("\ue2e5"); }

	else if(iconName == "fa-hand-holding-skull"){
	  label = _U("\ue1a4"); }

	else if(iconName == "fa-people-dress"){
	  label = _U("\ue217"); }

	else if(iconName == "fa-loveseat"){
	  label = _U("\uf4cc"); }

	else if(iconName == "fa-couch-small"){
	  label = _U("\uf4cc"); }

	else if(iconName == "fa-tower-broadcast"){
	  label = _U("\uf519"); }

	else if(iconName == "fa-broadcast-tower"){
	  label = _U("\uf519"); }

	else if(iconName == "fa-truck-pickup"){
	  label = _U("\uf63c"); }

	else if(iconName == "fa-block-quote"){
	  label = _U("\ue0b5"); }

	else if(iconName == "fa-up-long"){
	  label = _U("\uf30c"); }

	else if(iconName == "fa-long-arrow-alt-up"){
	  label = _U("\uf30c"); }

	else if(iconName == "fa-stop"){
	  label = _U("\uf04d"); }

	else if(iconName == "fa-code-merge"){
	  label = _U("\uf387"); }

	else if(iconName == "fa-money-check-dollar-pen"){
	  label = _U("\uf873"); }

	else if(iconName == "fa-money-check-edit-alt"){
	  label = _U("\uf873"); }

	else if(iconName == "fa-up-from-line"){
	  label = _U("\uf346"); }

	else if(iconName == "fa-arrow-alt-from-bottom"){
	  label = _U("\uf346"); }

	else if(iconName == "fa-upload"){
	  label = _U("\uf093"); }

	else if(iconName == "fa-hurricane"){
	  label = _U("\uf751"); }

	else if(iconName == "fa-grid-round-2-plus"){
	  label = _U("\ue5dc"); }

	else if(iconName == "fa-people-pants"){
	  label = _U("\ue219"); }

	else if(iconName == "fa-mound"){
	  label = _U("\ue52d"); }

	else if(iconName == "fa-windsock"){
	  label = _U("\uf777"); }

	else if(iconName == "fa-circle-half"){
	  label = _U("\ue110"); }

	else if(iconName == "fa-brake-warning"){
	  label = _U("\ue0c7"); }

	else if(iconName == "fa-toilet-portable"){
	  label = _U("\ue583"); }

	else if(iconName == "fa-compact-disc"){
	  label = _U("\uf51f"); }

	else if(iconName == "fa-file-arrow-down"){
	  label = _U("\uf56d"); }

	else if(iconName == "fa-file-download"){
	  label = _U("\uf56d"); }

	else if(iconName == "fa-saxophone-fire"){
	  label = _U("\uf8db"); }

	else if(iconName == "fa-sax-hot"){
	  label = _U("\uf8db"); }

	else if(iconName == "fa-camera-web-slash"){
	  label = _U("\uf833"); }

	else if(iconName == "fa-webcam-slash"){
	  label = _U("\uf833"); }

	else if(iconName == "fa-folder-medical"){
	  label = _U("\ue18c"); }

	else if(iconName == "fa-folder-gear"){
	  label = _U("\ue187"); }

	else if(iconName == "fa-folder-cog"){
	  label = _U("\ue187"); }

	else if(iconName == "fa-hand-wave"){
	  label = _U("\ue1a7"); }

	else if(iconName == "fa-arrow-up-arrow-down"){
	  label = _U("\ue099"); }

	else if(iconName == "fa-sort-up-down"){
	  label = _U("\ue099"); }

	else if(iconName == "fa-caravan"){
	  label = _U("\uf8ff"); }

	else if(iconName == "fa-shield-cat"){
	  label = _U("\ue572"); }

	else if(iconName == "fa-message-slash"){
	  label = _U("\uf4a9"); }

	else if(iconName == "fa-comment-alt-slash"){
	  label = _U("\uf4a9"); }

	else if(iconName == "fa-bolt"){
	  label = _U("\uf0e7"); }

	else if(iconName == "fa-zap"){
	  label = _U("\uf0e7"); }

	else if(iconName == "fa-trash-can-check"){
	  label = _U("\ue2a9"); }

	else if(iconName == "fa-glass-water"){
	  label = _U("\ue4f4"); }

	else if(iconName == "fa-oil-well"){
	  label = _U("\ue532"); }

	else if(iconName == "fa-person-simple"){
	  label = _U("\ue220"); }

	else if(iconName == "fa-arrow-turn-left-up"){
	  label = _U("\ue634"); }

	else if(iconName == "fa-vault"){
	  label = _U("\ue2c5"); }

	else if(iconName == "fa-mars"){
	  label = _U("\uf222"); }

	else if(iconName == "fa-toilet"){
	  label = _U("\uf7d8"); }

	else if(iconName == "fa-plane-circle-xmark"){
	  label = _U("\ue557"); }

	else if(iconName == "fa-yen-sign"){
	  label = _U("\uf157"); }

	else if(iconName == "fa-cny"){
	  label = _U("\uf157"); }

	else if(iconName == "fa-jpy"){
	  label = _U("\uf157"); }

	else if(iconName == "fa-rmb"){
	  label = _U("\uf157"); }

	else if(iconName == "fa-yen"){
	  label = _U("\uf157"); }

	else if(iconName == "fa-gear-code"){
	  label = _U("\ue5e8"); }

	else if(iconName == "fa-notes"){
	  label = _U("\ue202"); }

	else if(iconName == "fa-ruble-sign"){
	  label = _U("\uf158"); }

	else if(iconName == "fa-rouble"){
	  label = _U("\uf158"); }

	else if(iconName == "fa-rub"){
	  label = _U("\uf158"); }

	else if(iconName == "fa-ruble"){
	  label = _U("\uf158"); }

	else if(iconName == "fa-trash-undo"){
	  label = _U("\uf895"); }

	else if(iconName == "fa-trash-arrow-turn-left"){
	  label = _U("\uf895"); }

	else if(iconName == "fa-champagne-glass"){
	  label = _U("\uf79e"); }

	else if(iconName == "fa-glass-champagne"){
	  label = _U("\uf79e"); }

	else if(iconName == "fa-objects-align-center-horizontal"){
	  label = _U("\ue3bc"); }

	else if(iconName == "fa-sun"){
	  label = _U("\uf185"); }

	else if(iconName == "fa-trash-can-slash"){
	  label = _U("\ue2ad"); }

	else if(iconName == "fa-trash-alt-slash"){
	  label = _U("\ue2ad"); }

	else if(iconName == "fa-screen-users"){
	  label = _U("\uf63d"); }

	else if(iconName == "fa-users-class"){
	  label = _U("\uf63d"); }

	else if(iconName == "fa-guitar"){
	  label = _U("\uf7a6"); }

	else if(iconName == "fa-square-arrow-left"){
	  label = _U("\uf33a"); }

	else if(iconName == "fa-arrow-square-left"){
	  label = _U("\uf33a"); }

	else if(iconName == "fa-square-8"){
	  label = _U("\ue25d"); }

	else if(iconName == "fa-face-smile-hearts"){
	  label = _U("\ue390"); }

	else if(iconName == "fa-brackets-square"){
	  label = _U("\uf7e9"); }

	else if(iconName == "fa-brackets"){
	  label = _U("\uf7e9"); }

	else if(iconName == "fa-laptop-arrow-down"){
	  label = _U("\ue1c6"); }

	else if(iconName == "fa-hockey-stick-puck"){
	  label = _U("\ue3ae"); }

	else if(iconName == "fa-house-tree"){
	  label = _U("\ue1b3"); }

	else if(iconName == "fa-signal-fair"){
	  label = _U("\uf68d"); }

	else if(iconName == "fa-signal-2"){
	  label = _U("\uf68d"); }

	else if(iconName == "fa-face-laugh-wink"){
	  label = _U("\uf59c"); }

	else if(iconName == "fa-laugh-wink"){
	  label = _U("\uf59c"); }

	else if(iconName == "fa-circle-dollar"){
	  label = _U("\uf2e8"); }

	else if(iconName == "fa-dollar-circle"){
	  label = _U("\uf2e8"); }

	else if(iconName == "fa-usd-circle"){
	  label = _U("\uf2e8"); }

	else if(iconName == "fa-horse-head"){
	  label = _U("\uf7ab"); }

	else if(iconName == "fa-arrows-repeat"){
	  label = _U("\uf364"); }

	else if(iconName == "fa-repeat-alt"){
	  label = _U("\uf364"); }

	else if(iconName == "fa-bore-hole"){
	  label = _U("\ue4c3"); }

	else if(iconName == "fa-industry"){
	  label = _U("\uf275"); }

	else if(iconName == "fa-image-polaroid"){
	  label = _U("\uf8c4"); }

	else if(iconName == "fa-wave-triangle"){
	  label = _U("\uf89a"); }

	else if(iconName == "fa-turn-left-down"){
	  label = _U("\ue637"); }

	else if(iconName == "fa-person-running-fast"){
	  label = _U("\ue5ff"); }

	else if(iconName == "fa-circle-down"){
	  label = _U("\uf358"); }

	else if(iconName == "fa-arrow-alt-circle-down"){
	  label = _U("\uf358"); }

	else if(iconName == "fa-grill"){
	  label = _U("\ue5a3"); }

	else if(iconName == "fa-arrows-turn-to-dots"){
	  label = _U("\ue4c1"); }

	else if(iconName == "fa-chart-mixed"){
	  label = _U("\uf643"); }

	else if(iconName == "fa-analytics"){
	  label = _U("\uf643"); }

	else if(iconName == "fa-florin-sign"){
	  label = _U("\ue184"); }

	else if(iconName == "fa-arrow-down-short-wide"){
	  label = _U("\uf884"); }

	else if(iconName == "fa-sort-amount-desc"){
	  label = _U("\uf884"); }

	else if(iconName == "fa-sort-amount-down-alt"){
	  label = _U("\uf884"); }

	else if(iconName == "fa-less-than"){
	  label = _U("\u003c"); }

	else if(iconName == "fa-display-code"){
	  label = _U("\ue165"); }

	else if(iconName == "fa-desktop-code"){
	  label = _U("\ue165"); }

	else if(iconName == "fa-face-drooling"){
	  label = _U("\ue372"); }

	else if(iconName == "fa-oil-temperature"){
	  label = _U("\uf614"); }

	else if(iconName == "fa-oil-temp"){
	  label = _U("\uf614"); }

	else if(iconName == "fa-square-question"){
	  label = _U("\uf2fd"); }

	else if(iconName == "fa-question-square"){
	  label = _U("\uf2fd"); }

	else if(iconName == "fa-air-conditioner"){
	  label = _U("\uf8f4"); }

	else if(iconName == "fa-angle-down"){
	  label = _U("\uf107"); }

	else if(iconName == "fa-mountains"){
	  label = _U("\uf6fd"); }

	else if(iconName == "fa-omega"){
	  label = _U("\uf67a"); }

	else if(iconName == "fa-car-tunnel"){
	  label = _U("\ue4de"); }

	else if(iconName == "fa-person-dolly-empty"){
	  label = _U("\uf4d1"); }

	else if(iconName == "fa-pan-food"){
	  label = _U("\ue42b"); }

	else if(iconName == "fa-head-side-cough"){
	  label = _U("\ue061"); }

	else if(iconName == "fa-grip-lines"){
	  label = _U("\uf7a4"); }

	else if(iconName == "fa-thumbs-down"){
	  label = _U("\uf165"); }

	else if(iconName == "fa-user-lock"){
	  label = _U("\uf502"); }

	else if(iconName == "fa-arrow-right-long"){
	  label = _U("\uf178"); }

	else if(iconName == "fa-long-arrow-right"){
	  label = _U("\uf178"); }

	else if(iconName == "fa-tickets-airline"){
	  label = _U("\ue29b"); }

	else if(iconName == "fa-tickets-perforated-plane"){
	  label = _U("\ue29b"); }

	else if(iconName == "fa-tickets-plane"){
	  label = _U("\ue29b"); }

	else if(iconName == "fa-tent-double-peak"){
	  label = _U("\ue627"); }

	else if(iconName == "fa-anchor-circle-xmark"){
	  label = _U("\ue4ac"); }

	else if(iconName == "fa-ellipsis"){
	  label = _U("\uf141"); }

	else if(iconName == "fa-ellipsis-h"){
	  label = _U("\uf141"); }

	else if(iconName == "fa-nfc-slash"){
	  label = _U("\ue1fc"); }

	else if(iconName == "fa-chess-pawn"){
	  label = _U("\uf443"); }

	else if(iconName == "fa-kit-medical"){
	  label = _U("\uf479"); }

	else if(iconName == "fa-first-aid"){
	  label = _U("\uf479"); }

	else if(iconName == "fa-grid-2-plus"){
	  label = _U("\ue197"); }

	else if(iconName == "fa-bells"){
	  label = _U("\uf77f"); }

	else if(iconName == "fa-person-through-window"){
	  label = _U("\ue5a9"); }

	else if(iconName == "fa-toolbox"){
	  label = _U("\uf552"); }

	else if(iconName == "fa-envelope-dot"){
	  label = _U("\ue16f"); }

	else if(iconName == "fa-envelope-badge"){
	  label = _U("\ue16f"); }

	else if(iconName == "fa-magnifying-glass-waveform"){
	  label = _U("\ue661"); }

	else if(iconName == "fa-hands-holding-circle"){
	  label = _U("\ue4fb"); }

	else if(iconName == "fa-bug"){
	  label = _U("\uf188"); }

	else if(iconName == "fa-bowl-chopsticks"){
	  label = _U("\ue2e9"); }

	else if(iconName == "fa-credit-card"){
	  label = _U("\uf09d"); }

	else if(iconName == "fa-credit-card-alt"){
	  label = _U("\uf09d"); }

	else if(iconName == "fa-circle-s"){
	  label = _U("\ue121"); }

	else if(iconName == "fa-box-ballot"){
	  label = _U("\uf735"); }

	else if(iconName == "fa-car"){
	  label = _U("\uf1b9"); }

	else if(iconName == "fa-automobile"){
	  label = _U("\uf1b9"); }

	else if(iconName == "fa-hand-holding-hand"){
	  label = _U("\ue4f7"); }

	else if(iconName == "fa-user-tie-hair"){
	  label = _U("\ue45f"); }

	else if(iconName == "fa-podium-star"){
	  label = _U("\uf758"); }

	else if(iconName == "fa-user-hair-mullet"){
	  label = _U("\ue45c"); }

	else if(iconName == "fa-business-front"){
	  label = _U("\ue45c"); }

	else if(iconName == "fa-party-back"){
	  label = _U("\ue45c"); }

	else if(iconName == "fa-trian-balbot"){
	  label = _U("\ue45c"); }

	else if(iconName == "fa-microphone-stand"){
	  label = _U("\uf8cb"); }

	else if(iconName == "fa-book-open-reader"){
	  label = _U("\uf5da"); }

	else if(iconName == "fa-book-reader"){
	  label = _U("\uf5da"); }

	else if(iconName == "fa-family-dress"){
	  label = _U("\ue301"); }

	else if(iconName == "fa-circle-x"){
	  label = _U("\ue12e"); }

	else if(iconName == "fa-cabin"){
	  label = _U("\ue46d"); }

	else if(iconName == "fa-mountain-sun"){
	  label = _U("\ue52f"); }

	else if(iconName == "fa-chart-simple-horizontal"){
	  label = _U("\ue474"); }

	else if(iconName == "fa-arrows-left-right-to-line"){
	  label = _U("\ue4ba"); }

	else if(iconName == "fa-hand-back-point-left"){
	  label = _U("\ue19f"); }

	else if(iconName == "fa-message-dots"){
	  label = _U("\uf4a3"); }

	else if(iconName == "fa-comment-alt-dots"){
	  label = _U("\uf4a3"); }

	else if(iconName == "fa-messaging"){
	  label = _U("\uf4a3"); }

	else if(iconName == "fa-file-heart"){
	  label = _U("\ue176"); }

	else if(iconName == "fa-beer-mug"){
	  label = _U("\ue0b3"); }

	else if(iconName == "fa-beer-foam"){
	  label = _U("\ue0b3"); }

	else if(iconName == "fa-dice-d20"){
	  label = _U("\uf6cf"); }

	else if(iconName == "fa-drone"){
	  label = _U("\uf85f"); }

	else if(iconName == "fa-truck-droplet"){
	  label = _U("\ue58c"); }

	else if(iconName == "fa-file-circle-xmark"){
	  label = _U("\ue5a1"); }

	else if(iconName == "fa-temperature-arrow-up"){
	  label = _U("\ue040"); }

	else if(iconName == "fa-temperature-up"){
	  label = _U("\ue040"); }

	else if(iconName == "fa-medal"){
	  label = _U("\uf5a2"); }

	else if(iconName == "fa-person-fairy"){
	  label = _U("\ue608"); }

	else if(iconName == "fa-bed"){
	  label = _U("\uf236"); }

	else if(iconName == "fa-book-copy"){
	  label = _U("\ue0be"); }

	else if(iconName == "fa-square-h"){
	  label = _U("\uf0fd"); }

	else if(iconName == "fa-h-square"){
	  label = _U("\uf0fd"); }

	else if(iconName == "fa-square-c"){
	  label = _U("\ue266"); }

	else if(iconName == "fa-clock-two"){
	  label = _U("\ue35a"); }

	else if(iconName == "fa-square-ellipsis-vertical"){
	  label = _U("\ue26f"); }

	else if(iconName == "fa-calendar-users"){
	  label = _U("\ue5e2"); }

	else if(iconName == "fa-podcast"){
	  label = _U("\uf2ce"); }

	else if(iconName == "fa-bee"){
	  label = _U("\ue0b2"); }

	else if(iconName == "fa-temperature-full"){
	  label = _U("\uf2c7"); }

	else if(iconName == "fa-temperature-4"){
	  label = _U("\uf2c7"); }

	else if(iconName == "fa-thermometer-4"){
	  label = _U("\uf2c7"); }

	else if(iconName == "fa-thermometer-full"){
	  label = _U("\uf2c7"); }

	else if(iconName == "fa-bell"){
	  label = _U("\uf0f3"); }

	else if(iconName == "fa-candy-bar"){
	  label = _U("\ue3e8"); }

	else if(iconName == "fa-chocolate-bar"){
	  label = _U("\ue3e8"); }

	else if(iconName == "fa-xmark-large"){
	  label = _U("\ue59b"); }

	else if(iconName == "fa-pinata"){
	  label = _U("\ue3c3"); }

	else if(iconName == "fa-file-ppt"){
	  label = _U("\ue64a"); }

	else if(iconName == "fa-arrows-from-line"){
	  label = _U("\ue0a4"); }

	else if(iconName == "fa-superscript"){
	  label = _U("\uf12b"); }

	else if(iconName == "fa-bowl-spoon"){
	  label = _U("\ue3e0"); }

	else if(iconName == "fa-hexagon-check"){
	  label = _U("\ue416"); }

	else if(iconName == "fa-plug-circle-xmark"){
	  label = _U("\ue560"); }

	else if(iconName == "fa-star-of-life"){
	  label = _U("\uf621"); }

	else if(iconName == "fa-phone-slash"){
	  label = _U("\uf3dd"); }

	else if(iconName == "fa-traffic-light-stop"){
	  label = _U("\uf63a"); }

	else if(iconName == "fa-paint-roller"){
	  label = _U("\uf5aa"); }

	else if(iconName == "fa-accent-grave"){
	  label = _U("\u0060"); }

	else if(iconName == "fa-handshake-angle"){
	  label = _U("\uf4c4"); }

	else if(iconName == "fa-hands-helping"){
	  label = _U("\uf4c4"); }

	else if(iconName == "fa-circle-0"){
	  label = _U("\ue0ed"); }

	else if(iconName == "fa-dial-med-low"){
	  label = _U("\ue160"); }

	else if(iconName == "fa-location-dot"){
	  label = _U("\uf3c5"); }

	else if(iconName == "fa-map-marker-alt"){
	  label = _U("\uf3c5"); }

	else if(iconName == "fa-crab"){
	  label = _U("\ue3ff"); }

	else if(iconName == "fa-box-open-full"){
	  label = _U("\uf49c"); }

	else if(iconName == "fa-box-full"){
	  label = _U("\uf49c"); }

	else if(iconName == "fa-file"){
	  label = _U("\uf15b"); }

	else if(iconName == "fa-greater-than"){
	  label = _U("\u003e"); }

	else if(iconName == "fa-quotes"){
	  label = _U("\ue234"); }

	else if(iconName == "fa-pretzel"){
	  label = _U("\ue441"); }

	else if(iconName == "fa-t-rex"){
	  label = _U("\ue629"); }

	else if(iconName == "fa-person-swimming"){
	  label = _U("\uf5c4"); }

	else if(iconName == "fa-swimmer"){
	  label = _U("\uf5c4"); }

	else if(iconName == "fa-arrow-down"){
	  label = _U("\uf063"); }

	else if(iconName == "fa-user-robot-xmarks"){
	  label = _U("\ue4a7"); }

	else if(iconName == "fa-message-quote"){
	  label = _U("\ue1e4"); }

	else if(iconName == "fa-comment-alt-quote"){
	  label = _U("\ue1e4"); }

	else if(iconName == "fa-candy-corn"){
	  label = _U("\uf6bd"); }

	else if(iconName == "fa-folder-magnifying-glass"){
	  label = _U("\ue18b"); }

	else if(iconName == "fa-folder-search"){
	  label = _U("\ue18b"); }

	else if(iconName == "fa-notebook"){
	  label = _U("\ue201"); }

	else if(iconName == "fa-droplet"){
	  label = _U("\uf043"); }

	else if(iconName == "fa-tint"){
	  label = _U("\uf043"); }

	else if(iconName == "fa-bullseye-pointer"){
	  label = _U("\uf649"); }

	else if(iconName == "fa-eraser"){
	  label = _U("\uf12d"); }

	else if(iconName == "fa-hexagon-image"){
	  label = _U("\ue504"); }

	else if(iconName == "fa-earth-americas"){
	  label = _U("\uf57d"); }

	else if(iconName == "fa-earth"){
	  label = _U("\uf57d"); }

	else if(iconName == "fa-earth-america"){
	  label = _U("\uf57d"); }

	else if(iconName == "fa-globe-americas"){
	  label = _U("\uf57d"); }

	else if(iconName == "fa-file-svg"){
	  label = _U("\ue64b"); }

	else if(iconName == "fa-crate-apple"){
	  label = _U("\uf6b1"); }

	else if(iconName == "fa-apple-crate"){
	  label = _U("\uf6b1"); }

	else if(iconName == "fa-person-burst"){
	  label = _U("\ue53b"); }

	else if(iconName == "fa-game-board"){
	  label = _U("\uf867"); }

	else if(iconName == "fa-hat-chef"){
	  label = _U("\uf86b"); }

	else if(iconName == "fa-hand-back-point-right"){
	  label = _U("\ue1a1"); }

	else if(iconName == "fa-dove"){
	  label = _U("\uf4ba"); }

	else if(iconName == "fa-snowflake-droplets"){
	  label = _U("\ue5c1"); }

	else if(iconName == "fa-battery-empty"){
	  label = _U("\uf244"); }

	else if(iconName == "fa-battery-0"){
	  label = _U("\uf244"); }

	else if(iconName == "fa-grid-4"){
	  label = _U("\ue198"); }

	else if(iconName == "fa-socks"){
	  label = _U("\uf696"); }

	else if(iconName == "fa-face-sunglasses"){
	  label = _U("\ue398"); }

	else if(iconName == "fa-inbox"){
	  label = _U("\uf01c"); }

	else if(iconName == "fa-square-0"){
	  label = _U("\ue255"); }

	else if(iconName == "fa-section"){
	  label = _U("\ue447"); }

	else if(iconName == "fa-square-this-way-up"){
	  label = _U("\uf49f"); }

	else if(iconName == "fa-box-up"){
	  label = _U("\uf49f"); }

	else if(iconName == "fa-gauge-high"){
	  label = _U("\uf625"); }

	else if(iconName == "fa-tachometer-alt"){
	  label = _U("\uf625"); }

	else if(iconName == "fa-tachometer-alt-fast"){
	  label = _U("\uf625"); }

	else if(iconName == "fa-square-ampersand"){
	  label = _U("\ue260"); }

	else if(iconName == "fa-envelope-open-text"){
	  label = _U("\uf658"); }

	else if(iconName == "fa-lamp-desk"){
	  label = _U("\ue014"); }

	else if(iconName == "fa-hospital"){
	  label = _U("\uf0f8"); }

	else if(iconName == "fa-hospital-alt"){
	  label = _U("\uf0f8"); }

	else if(iconName == "fa-hospital-wide"){
	  label = _U("\uf0f8"); }

	else if(iconName == "fa-poll-people"){
	  label = _U("\uf759"); }

	else if(iconName == "fa-whiskey-glass-ice"){
	  label = _U("\uf7a1"); }

	else if(iconName == "fa-glass-whiskey-rocks"){
	  label = _U("\uf7a1"); }

	else if(iconName == "fa-wine-bottle"){
	  label = _U("\uf72f"); }

	else if(iconName == "fa-chess-rook"){
	  label = _U("\uf447"); }

	else if(iconName == "fa-user-bounty-hunter"){
	  label = _U("\ue2bf"); }

	else if(iconName == "fa-bars-staggered"){
	  label = _U("\uf550"); }

	else if(iconName == "fa-reorder"){
	  label = _U("\uf550"); }

	else if(iconName == "fa-stream"){
	  label = _U("\uf550"); }

	else if(iconName == "fa-diagram-sankey"){
	  label = _U("\ue158"); }

	else if(iconName == "fa-cloud-hail-mixed"){
	  label = _U("\uf73a"); }

	else if(iconName == "fa-circle-up-left"){
	  label = _U("\ue128"); }

	else if(iconName == "fa-dharmachakra"){
	  label = _U("\uf655"); }

	else if(iconName == "fa-objects-align-left"){
	  label = _U("\ue3be"); }

	else if(iconName == "fa-oil-can-drip"){
	  label = _U("\ue205"); }

	else if(iconName == "fa-face-smiling-hands"){
	  label = _U("\ue396"); }

	else if(iconName == "fa-broccoli"){
	  label = _U("\ue3e2"); }

	else if(iconName == "fa-route-interstate"){
	  label = _U("\uf61b"); }

	else if(iconName == "fa-ear-muffs"){
	  label = _U("\uf795"); }

	else if(iconName == "fa-hotdog"){
	  label = _U("\uf80f"); }

	else if(iconName == "fa-transporter-empty"){
	  label = _U("\ue046"); }

	else if(iconName == "fa-person-walking-with-cane"){
	  label = _U("\uf29d"); }

	else if(iconName == "fa-blind"){
	  label = _U("\uf29d"); }

	else if(iconName == "fa-angle-90"){
	  label = _U("\ue08d"); }

	else if(iconName == "fa-rectangle-terminal"){
	  label = _U("\ue236"); }

	else if(iconName == "fa-kite"){
	  label = _U("\uf6f4"); }

	else if(iconName == "fa-drum"){
	  label = _U("\uf569"); }

	else if(iconName == "fa-scrubber"){
	  label = _U("\uf2f8"); }

	else if(iconName == "fa-ice-cream"){
	  label = _U("\uf810"); }

	else if(iconName == "fa-heart-circle-bolt"){
	  label = _U("\ue4fc"); }

	else if(iconName == "fa-fish-bones"){
	  label = _U("\ue304"); }

	else if(iconName == "fa-deer-rudolph"){
	  label = _U("\uf78f"); }

	else if(iconName == "fa-fax"){
	  label = _U("\uf1ac"); }

	else if(iconName == "fa-paragraph"){
	  label = _U("\uf1dd"); }

	else if(iconName == "fa-head-side-heart"){
	  label = _U("\ue1aa"); }

	else if(iconName == "fa-square-e"){
	  label = _U("\ue26d"); }

	else if(iconName == "fa-meter-fire"){
	  label = _U("\ue1eb"); }

	else if(iconName == "fa-cloud-hail"){
	  label = _U("\uf739"); }

	else if(iconName == "fa-check-to-slot"){
	  label = _U("\uf772"); }

	else if(iconName == "fa-vote-yea"){
	  label = _U("\uf772"); }

	else if(iconName == "fa-money-from-bracket"){
	  label = _U("\ue312"); }

	else if(iconName == "fa-star-half"){
	  label = _U("\uf089"); }

	else if(iconName == "fa-car-bus"){
	  label = _U("\uf85a"); }

	else if(iconName == "fa-speaker"){
	  label = _U("\uf8df"); }

	else if(iconName == "fa-timer"){
	  label = _U("\ue29e"); }

	else if(iconName == "fa-boxes-stacked"){
	  label = _U("\uf468"); }

	else if(iconName == "fa-boxes"){
	  label = _U("\uf468"); }

	else if(iconName == "fa-boxes-alt"){
	  label = _U("\uf468"); }

	else if(iconName == "fa-landmark-magnifying-glass"){
	  label = _U("\ue622"); }

	else if(iconName == "fa-grill-hot"){
	  label = _U("\ue5a5"); }

	else if(iconName == "fa-ballot-check"){
	  label = _U("\uf733"); }

	else if(iconName == "fa-link"){
	  label = _U("\uf0c1"); }

	else if(iconName == "fa-chain"){
	  label = _U("\uf0c1"); }

	else if(iconName == "fa-ear-listen"){
	  label = _U("\uf2a2"); }

	else if(iconName == "fa-assistive-listening-systems"){
	  label = _U("\uf2a2"); }

	else if(iconName == "fa-file-minus"){
	  label = _U("\uf318"); }

	else if(iconName == "fa-tree-city"){
	  label = _U("\ue587"); }

	else if(iconName == "fa-play"){
	  label = _U("\uf04b"); }

	else if(iconName == "fa-font"){
	  label = _U("\uf031"); }

	else if(iconName == "fa-cup-togo"){
	  label = _U("\uf6c5"); }

	else if(iconName == "fa-coffee-togo"){
	  label = _U("\uf6c5"); }

	else if(iconName == "fa-square-down-left"){
	  label = _U("\ue26b"); }

	else if(iconName == "fa-burger-lettuce"){
	  label = _U("\ue3e3"); }

	else if(iconName == "fa-rupiah-sign"){
	  label = _U("\ue23d"); }

	else if(iconName == "fa-magnifying-glass"){
	  label = _U("\uf002"); }

	else if(iconName == "fa-search"){
	  label = _U("\uf002"); }

	else if(iconName == "fa-table-tennis-paddle-ball"){
	  label = _U("\uf45d"); }

	else if(iconName == "fa-ping-pong-paddle-ball"){
	  label = _U("\uf45d"); }

	else if(iconName == "fa-table-tennis"){
	  label = _U("\uf45d"); }

	else if(iconName == "fa-person-dots-from-line"){
	  label = _U("\uf470"); }

	else if(iconName == "fa-diagnoses"){
	  label = _U("\uf470"); }

	else if(iconName == "fa-chevrons-down"){
	  label = _U("\uf322"); }

	else if(iconName == "fa-chevron-double-down"){
	  label = _U("\uf322"); }

	else if(iconName == "fa-trash-can-arrow-up"){
	  label = _U("\uf82a"); }

	else if(iconName == "fa-trash-restore-alt"){
	  label = _U("\uf82a"); }

	else if(iconName == "fa-signal-good"){
	  label = _U("\uf68e"); }

	else if(iconName == "fa-signal-3"){
	  label = _U("\uf68e"); }

	else if(iconName == "fa-location-question"){
	  label = _U("\uf60b"); }

	else if(iconName == "fa-map-marker-question"){
	  label = _U("\uf60b"); }

	else if(iconName == "fa-floppy-disk-circle-xmark"){
	  label = _U("\ue181"); }

	else if(iconName == "fa-floppy-disk-times"){
	  label = _U("\ue181"); }

	else if(iconName == "fa-save-circle-xmark"){
	  label = _U("\ue181"); }

	else if(iconName == "fa-save-times"){
	  label = _U("\ue181"); }

	else if(iconName == "fa-naira-sign"){
	  label = _U("\ue1f6"); }

	else if(iconName == "fa-peach"){
	  label = _U("\ue20b"); }

	else if(iconName == "fa-taxi-bus"){
	  label = _U("\ue298"); }

	else if(iconName == "fa-bracket-curly"){
	  label = _U("\u007b"); }

	else if(iconName == "fa-bracket-curly-left"){
	  label = _U("\u007b"); }

	else if(iconName == "fa-lobster"){
	  label = _U("\ue421"); }

	else if(iconName == "fa-cart-flatbed-empty"){
	  label = _U("\uf476"); }

	else if(iconName == "fa-dolly-flatbed-empty"){
	  label = _U("\uf476"); }

	else if(iconName == "fa-colon"){
	  label = _U("\u003a"); }

	else if(iconName == "fa-cart-arrow-down"){
	  label = _U("\uf218"); }

	else if(iconName == "fa-wand"){
	  label = _U("\uf72a"); }

	else if(iconName == "fa-walkie-talkie"){
	  label = _U("\uf8ef"); }

	else if(iconName == "fa-file-pen"){
	  label = _U("\uf31c"); }

	else if(iconName == "fa-file-edit"){
	  label = _U("\uf31c"); }

	else if(iconName == "fa-receipt"){
	  label = _U("\uf543"); }

	else if(iconName == "fa-table-picnic"){
	  label = _U("\ue32d"); }

	else if(iconName == "fa-square-pen"){
	  label = _U("\uf14b"); }

	else if(iconName == "fa-pen-square"){
	  label = _U("\uf14b"); }

	else if(iconName == "fa-pencil-square"){
	  label = _U("\uf14b"); }

	else if(iconName == "fa-circle-microphone-lines"){
	  label = _U("\ue117"); }

	else if(iconName == "fa-microphone-circle-alt"){
	  label = _U("\ue117"); }

	else if(iconName == "fa-display-slash"){
	  label = _U("\ue2fa"); }

	else if(iconName == "fa-desktop-slash"){
	  label = _U("\ue2fa"); }

	else if(iconName == "fa-suitcase-rolling"){
	  label = _U("\uf5c1"); }

	else if(iconName == "fa-person-circle-exclamation"){
	  label = _U("\ue53f"); }

	else if(iconName == "fa-transporter-2"){
	  label = _U("\ue044"); }

	else if(iconName == "fa-hands-holding-diamond"){
	  label = _U("\uf47c"); }

	else if(iconName == "fa-hand-receiving"){
	  label = _U("\uf47c"); }

	else if(iconName == "fa-money-bill-simple-wave"){
	  label = _U("\ue1f2"); }

	else if(iconName == "fa-chevron-down"){
	  label = _U("\uf078"); }

	else if(iconName == "fa-battery-full"){
	  label = _U("\uf240"); }

	else if(iconName == "fa-battery"){
	  label = _U("\uf240"); }

	else if(iconName == "fa-battery-5"){
	  label = _U("\uf240"); }

	else if(iconName == "fa-bell-plus"){
	  label = _U("\uf849"); }

	else if(iconName == "fa-book-arrow-right"){
	  label = _U("\ue0b9"); }

	else if(iconName == "fa-hospitals"){
	  label = _U("\uf80e"); }

	else if(iconName == "fa-club"){
	  label = _U("\uf327"); }

	else if(iconName == "fa-skull-crossbones"){
	  label = _U("\uf714"); }

	else if(iconName == "fa-droplet-degree"){
	  label = _U("\uf748"); }

	else if(iconName == "fa-dewpoint"){
	  label = _U("\uf748"); }

	else if(iconName == "fa-code-compare"){
	  label = _U("\ue13a"); }

	else if(iconName == "fa-list-ul"){
	  label = _U("\uf0ca"); }

	else if(iconName == "fa-list-dots"){
	  label = _U("\uf0ca"); }

	else if(iconName == "fa-hand-holding-magic"){
	  label = _U("\uf6e5"); }

	else if(iconName == "fa-watermelon-slice"){
	  label = _U("\ue337"); }

	else if(iconName == "fa-circle-ellipsis"){
	  label = _U("\ue10a"); }

	else if(iconName == "fa-school-lock"){
	  label = _U("\ue56f"); }

	else if(iconName == "fa-tower-cell"){
	  label = _U("\ue585"); }

	else if(iconName == "fa-sd-cards"){
	  label = _U("\ue240"); }

	else if(iconName == "fa-jug-bottle"){
	  label = _U("\ue5fb"); }

	else if(iconName == "fa-down-long"){
	  label = _U("\uf309"); }

	else if(iconName == "fa-long-arrow-alt-down"){
	  label = _U("\uf309"); }

	else if(iconName == "fa-envelopes"){
	  label = _U("\ue170"); }

	else if(iconName == "fa-phone-office"){
	  label = _U("\uf67d"); }

	else if(iconName == "fa-ranking-star"){
	  label = _U("\ue561"); }

	else if(iconName == "fa-chess-king"){
	  label = _U("\uf43f"); }

	else if(iconName == "fa-nfc-pen"){
	  label = _U("\ue1fa"); }

	else if(iconName == "fa-person-harassing"){
	  label = _U("\ue549"); }

	else if(iconName == "fa-magnifying-glass-play"){
	  label = _U("\ue660"); }

	else if(iconName == "fa-hat-winter"){
	  label = _U("\uf7a8"); }

	else if(iconName == "fa-brazilian-real-sign"){
	  label = _U("\ue46c"); }

	else if(iconName == "fa-landmark-dome"){
	  label = _U("\uf752"); }

	else if(iconName == "fa-landmark-alt"){
	  label = _U("\uf752"); }

	else if(iconName == "fa-bone-break"){
	  label = _U("\uf5d8"); }

	else if(iconName == "fa-arrow-up"){
	  label = _U("\uf062"); }

	else if(iconName == "fa-down-from-dotted-line"){
	  label = _U("\ue407"); }

	else if(iconName == "fa-tv"){
	  label = _U("\uf26c"); }

	else if(iconName == "fa-television"){
	  label = _U("\uf26c"); }

	else if(iconName == "fa-tv-alt"){
	  label = _U("\uf26c"); }

	else if(iconName == "fa-border-left"){
	  label = _U("\uf84f"); }

	else if(iconName == "fa-circle-divide"){
	  label = _U("\ue106"); }

	else if(iconName == "fa-shrimp"){
	  label = _U("\ue448"); }

	else if(iconName == "fa-list-check"){
	  label = _U("\uf0ae"); }

	else if(iconName == "fa-tasks"){
	  label = _U("\uf0ae"); }

	else if(iconName == "fa-diagram-subtask"){
	  label = _U("\ue479"); }

	else if(iconName == "fa-jug-detergent"){
	  label = _U("\ue519"); }

	else if(iconName == "fa-circle-user"){
	  label = _U("\uf2bd"); }

	else if(iconName == "fa-user-circle"){
	  label = _U("\uf2bd"); }

	else if(iconName == "fa-square-y"){
	  label = _U("\ue287"); }

	else if(iconName == "fa-user-doctor-hair"){
	  label = _U("\ue458"); }

	else if(iconName == "fa-planet-ringed"){
	  label = _U("\ue020"); }

	else if(iconName == "fa-mushroom"){
	  label = _U("\ue425"); }

	else if(iconName == "fa-user-shield"){
	  label = _U("\uf505"); }

	else if(iconName == "fa-megaphone"){
	  label = _U("\uf675"); }

	else if(iconName == "fa-wreath-laurel"){
	  label = _U("\ue5d2"); }

	else if(iconName == "fa-circle-exclamation-check"){
	  label = _U("\ue10d"); }

	else if(iconName == "fa-wind"){
	  label = _U("\uf72e"); }

	else if(iconName == "fa-box-dollar"){
	  label = _U("\uf4a0"); }

	else if(iconName == "fa-box-usd"){
	  label = _U("\uf4a0"); }

	else if(iconName == "fa-car-burst"){
	  label = _U("\uf5e1"); }

	else if(iconName == "fa-car-crash"){
	  label = _U("\uf5e1"); }

	else if(iconName == "fa-y"){
	  label = _U("\u0059"); }

	else if(iconName == "fa-user-headset"){
	  label = _U("\uf82d"); }

	else if(iconName == "fa-arrows-retweet"){
	  label = _U("\uf361"); }

	else if(iconName == "fa-retweet-alt"){
	  label = _U("\uf361"); }

	else if(iconName == "fa-person-snowboarding"){
	  label = _U("\uf7ce"); }

	else if(iconName == "fa-snowboarding"){
	  label = _U("\uf7ce"); }

	else if(iconName == "fa-square-chevron-right"){
	  label = _U("\uf32b"); }

	else if(iconName == "fa-chevron-square-right"){
	  label = _U("\uf32b"); }

	else if(iconName == "fa-lacrosse-stick-ball"){
	  label = _U("\ue3b6"); }

	else if(iconName == "fa-truck-fast"){
	  label = _U("\uf48b"); }

	else if(iconName == "fa-shipping-fast"){
	  label = _U("\uf48b"); }

	else if(iconName == "fa-user-magnifying-glass"){
	  label = _U("\ue5c5"); }

	else if(iconName == "fa-star-sharp"){
	  label = _U("\ue28b"); }

	else if(iconName == "fa-comment-heart"){
	  label = _U("\ue5c8"); }

	else if(iconName == "fa-circle-1"){
	  label = _U("\ue0ee"); }

	else if(iconName == "fa-circle-star"){
	  label = _U("\ue123"); }

	else if(iconName == "fa-star-circle"){
	  label = _U("\ue123"); }

	else if(iconName == "fa-fish"){
	  label = _U("\uf578"); }

	else if(iconName == "fa-cloud-fog"){
	  label = _U("\uf74e"); }

	else if(iconName == "fa-fog"){
	  label = _U("\uf74e"); }

	else if(iconName == "fa-waffle"){
	  label = _U("\ue466"); }

	else if(iconName == "fa-music-note"){
	  label = _U("\uf8cf"); }

	else if(iconName == "fa-music-alt"){
	  label = _U("\uf8cf"); }

	else if(iconName == "fa-hexagon-exclamation"){
	  label = _U("\ue417"); }

	else if(iconName == "fa-cart-shopping-fast"){
	  label = _U("\ue0dc"); }

	else if(iconName == "fa-object-union"){
	  label = _U("\ue49f"); }

	else if(iconName == "fa-user-graduate"){
	  label = _U("\uf501"); }

	else if(iconName == "fa-starfighter"){
	  label = _U("\ue037"); }

	else if(iconName == "fa-circle-half-stroke"){
	  label = _U("\uf042"); }

	else if(iconName == "fa-adjust"){
	  label = _U("\uf042"); }

	else if(iconName == "fa-arrow-right-long-to-line"){
	  label = _U("\ue3d5"); }

	else if(iconName == "fa-square-arrow-down"){
	  label = _U("\uf339"); }

	else if(iconName == "fa-arrow-square-down"){
	  label = _U("\uf339"); }

	else if(iconName == "fa-diamond-half-stroke"){
	  label = _U("\ue5b8"); }

	else if(iconName == "fa-clapperboard"){
	  label = _U("\ue131"); }

	else if(iconName == "fa-square-chevron-left"){
	  label = _U("\uf32a"); }

	else if(iconName == "fa-chevron-square-left"){
	  label = _U("\uf32a"); }

	else if(iconName == "fa-phone-intercom"){
	  label = _U("\ue434"); }

	else if(iconName == "fa-link-horizontal"){
	  label = _U("\ue1cb"); }

	else if(iconName == "fa-chain-horizontal"){
	  label = _U("\ue1cb"); }

	else if(iconName == "fa-mango"){
	  label = _U("\ue30f"); }

	else if(iconName == "fa-music-note-slash"){
	  label = _U("\uf8d0"); }

	else if(iconName == "fa-music-alt-slash"){
	  label = _U("\uf8d0"); }

	else if(iconName == "fa-circle-radiation"){
	  label = _U("\uf7ba"); }

	else if(iconName == "fa-radiation-alt"){
	  label = _U("\uf7ba"); }

	else if(iconName == "fa-face-tongue-sweat"){
	  label = _U("\ue39e"); }

	else if(iconName == "fa-globe-stand"){
	  label = _U("\uf5f6"); }

	else if(iconName == "fa-baseball"){
	  label = _U("\uf433"); }

	else if(iconName == "fa-baseball-ball"){
	  label = _U("\uf433"); }

	else if(iconName == "fa-circle-p"){
	  label = _U("\ue11a"); }

	else if(iconName == "fa-award-simple"){
	  label = _U("\ue0ab"); }

	else if(iconName == "fa-jet-fighter-up"){
	  label = _U("\ue518"); }

	else if(iconName == "fa-diagram-project"){
	  label = _U("\uf542"); }

	else if(iconName == "fa-project-diagram"){
	  label = _U("\uf542"); }

	else if(iconName == "fa-pedestal"){
	  label = _U("\ue20d"); }

	else if(iconName == "fa-chart-pyramid"){
	  label = _U("\ue0e6"); }

	else if(iconName == "fa-sidebar"){
	  label = _U("\ue24e"); }

	else if(iconName == "fa-snowman-head"){
	  label = _U("\uf79b"); }

	else if(iconName == "fa-frosty-head"){
	  label = _U("\uf79b"); }

	else if(iconName == "fa-copy"){
	  label = _U("\uf0c5"); }

	else if(iconName == "fa-burger-glass"){
	  label = _U("\ue0ce"); }

	else if(iconName == "fa-volume-xmark"){
	  label = _U("\uf6a9"); }

	else if(iconName == "fa-volume-mute"){
	  label = _U("\uf6a9"); }

	else if(iconName == "fa-volume-times"){
	  label = _U("\uf6a9"); }

	else if(iconName == "fa-hand-sparkles"){
	  label = _U("\ue05d"); }

	else if(iconName == "fa-bars-filter"){
	  label = _U("\ue0ad"); }

	else if(iconName == "fa-paintbrush-pencil"){
	  label = _U("\ue206"); }

	else if(iconName == "fa-party-bell"){
	  label = _U("\ue31a"); }

	else if(iconName == "fa-user-vneck-hair"){
	  label = _U("\ue462"); }

	else if(iconName == "fa-jack-o-lantern"){
	  label = _U("\uf30e"); }

	else if(iconName == "fa-grip"){
	  label = _U("\uf58d"); }

	else if(iconName == "fa-grip-horizontal"){
	  label = _U("\uf58d"); }

	else if(iconName == "fa-share-from-square"){
	  label = _U("\uf14d"); }

	else if(iconName == "fa-share-square"){
	  label = _U("\uf14d"); }

	else if(iconName == "fa-keynote"){
	  label = _U("\uf66c"); }

	else if(iconName == "fa-child-combatant"){
	  label = _U("\ue4e0"); }

	else if(iconName == "fa-child-rifle"){
	  label = _U("\ue4e0"); }

	else if(iconName == "fa-gun"){
	  label = _U("\ue19b"); }

	else if(iconName == "fa-square-phone"){
	  label = _U("\uf098"); }

	else if(iconName == "fa-phone-square"){
	  label = _U("\uf098"); }

	else if(iconName == "fa-hat-beach"){
	  label = _U("\ue606"); }

	else if(iconName == "fa-plus"){
	  label = _U("\u002b"); }

	else if(iconName == "fa-add"){
	  label = _U("\u002b"); }

	else if(iconName == "fa-expand"){
	  label = _U("\uf065"); }

	else if(iconName == "fa-computer"){
	  label = _U("\ue4e5"); }

	else if(iconName == "fa-fort"){
	  label = _U("\ue486"); }

	else if(iconName == "fa-cloud-check"){
	  label = _U("\ue35c"); }

	else if(iconName == "fa-xmark"){
	  label = _U("\uf00d"); }

	else if(iconName == "fa-close"){
	  label = _U("\uf00d"); }

	else if(iconName == "fa-multiply"){
	  label = _U("\uf00d"); }

	else if(iconName == "fa-remove"){
	  label = _U("\uf00d"); }

	else if(iconName == "fa-times"){
	  label = _U("\uf00d"); }

	else if(iconName == "fa-face-smirking"){
	  label = _U("\ue397"); }

	else if(iconName == "fa-arrows-up-down-left-right"){
	  label = _U("\uf047"); }

	else if(iconName == "fa-arrows"){
	  label = _U("\uf047"); }

	else if(iconName == "fa-chalkboard-user"){
	  label = _U("\uf51c"); }

	else if(iconName == "fa-chalkboard-teacher"){
	  label = _U("\uf51c"); }

	else if(iconName == "fa-rhombus"){
	  label = _U("\ue23b"); }

	else if(iconName == "fa-claw-marks"){
	  label = _U("\uf6c2"); }

	else if(iconName == "fa-peso-sign"){
	  label = _U("\ue222"); }

	else if(iconName == "fa-face-smile-tongue"){
	  label = _U("\ue394"); }

	else if(iconName == "fa-cart-circle-xmark"){
	  label = _U("\ue3f4"); }

	else if(iconName == "fa-building-shield"){
	  label = _U("\ue4d8"); }

	else if(iconName == "fa-circle-phone-flip"){
	  label = _U("\ue11c"); }

	else if(iconName == "fa-phone-circle-alt"){
	  label = _U("\ue11c"); }

	else if(iconName == "fa-baby"){
	  label = _U("\uf77c"); }

	else if(iconName == "fa-users-line"){
	  label = _U("\ue592"); }

	else if(iconName == "fa-quote-left"){
	  label = _U("\uf10d"); }

	else if(iconName == "fa-quote-left-alt"){
	  label = _U("\uf10d"); }

	else if(iconName == "fa-tractor"){
	  label = _U("\uf722"); }

	else if(iconName == "fa-key-skeleton"){
	  label = _U("\uf6f3"); }

	else if(iconName == "fa-trash-arrow-up"){
	  label = _U("\uf829"); }

	else if(iconName == "fa-trash-restore"){
	  label = _U("\uf829"); }

	else if(iconName == "fa-arrow-down-up-lock"){
	  label = _U("\ue4b0"); }

	else if(iconName == "fa-arrow-down-to-bracket"){
	  label = _U("\ue094"); }

	else if(iconName == "fa-lines-leaning"){
	  label = _U("\ue51e"); }

	else if(iconName == "fa-square-q"){
	  label = _U("\ue27b"); }

	else if(iconName == "fa-ruler-combined"){
	  label = _U("\uf546"); }

	else if(iconName == "fa-symbols"){
	  label = _U("\uf86e"); }

	else if(iconName == "fa-icons-alt"){
	  label = _U("\uf86e"); }

	else if(iconName == "fa-copyright"){
	  label = _U("\uf1f9"); }

	else if(iconName == "fa-flask-gear"){
	  label = _U("\ue5f1"); }

	else if(iconName == "fa-highlighter-line"){
	  label = _U("\ue1af"); }

	else if(iconName == "fa-bracket-square"){
	  label = _U("\u005b"); }

	else if(iconName == "fa-bracket"){
	  label = _U("\u005b"); }

	else if(iconName == "fa-bracket-left"){
	  label = _U("\u005b"); }

	else if(iconName == "fa-island-tropical"){
	  label = _U("\uf811"); }

	else if(iconName == "fa-island-tree-palm"){
	  label = _U("\uf811"); }

	else if(iconName == "fa-arrow-right-from-line"){
	  label = _U("\uf343"); }

	else if(iconName == "fa-arrow-from-left"){
	  label = _U("\uf343"); }

	else if(iconName == "fa-h2"){
	  label = _U("\uf314"); }

	else if(iconName == "fa-equals"){
	  label = _U("\u003d"); }

	else if(iconName == "fa-cake-slice"){
	  label = _U("\ue3e5"); }

	else if(iconName == "fa-shortcake"){
	  label = _U("\ue3e5"); }

	else if(iconName == "fa-building-magnifying-glass"){
	  label = _U("\ue61c"); }

	else if(iconName == "fa-peanut"){
	  label = _U("\ue430"); }

	else if(iconName == "fa-wrench-simple"){
	  label = _U("\ue2d1"); }

	else if(iconName == "fa-blender"){
	  label = _U("\uf517"); }

	else if(iconName == "fa-teeth"){
	  label = _U("\uf62e"); }

	else if(iconName == "fa-tally-2"){
	  label = _U("\ue295"); }

	else if(iconName == "fa-shekel-sign"){
	  label = _U("\uf20b"); }

	else if(iconName == "fa-ils"){
	  label = _U("\uf20b"); }

	else if(iconName == "fa-shekel"){
	  label = _U("\uf20b"); }

	else if(iconName == "fa-sheqel"){
	  label = _U("\uf20b"); }

	else if(iconName == "fa-sheqel-sign"){
	  label = _U("\uf20b"); }

	else if(iconName == "fa-cars"){
	  label = _U("\uf85b"); }

	else if(iconName == "fa-axe-battle"){
	  label = _U("\uf6b3"); }

	else if(iconName == "fa-user-hair-long"){
	  label = _U("\ue45b"); }

	else if(iconName == "fa-map"){
	  label = _U("\uf279"); }

	else if(iconName == "fa-arrow-left-from-arc"){
	  label = _U("\ue615"); }

	else if(iconName == "fa-file-circle-info"){
	  label = _U("\ue493"); }

	else if(iconName == "fa-face-disappointed"){
	  label = _U("\ue36f"); }

	else if(iconName == "fa-lasso-sparkles"){
	  label = _U("\ue1c9"); }

	else if(iconName == "fa-clock-eleven"){
	  label = _U("\ue347"); }

	else if(iconName == "fa-rocket"){
	  label = _U("\uf135"); }

	else if(iconName == "fa-siren-on"){
	  label = _U("\ue02e"); }

	else if(iconName == "fa-clock-ten"){
	  label = _U("\ue354"); }

	else if(iconName == "fa-candle-holder"){
	  label = _U("\uf6bc"); }

	else if(iconName == "fa-video-arrow-down-left"){
	  label = _U("\ue2c8"); }

	else if(iconName == "fa-photo-film"){
	  label = _U("\uf87c"); }

	else if(iconName == "fa-photo-video"){
	  label = _U("\uf87c"); }

	else if(iconName == "fa-floppy-disk-circle-arrow-right"){
	  label = _U("\ue180"); }

	else if(iconName == "fa-save-circle-arrow-right"){
	  label = _U("\ue180"); }

	else if(iconName == "fa-folder-minus"){
	  label = _U("\uf65d"); }

	else if(iconName == "fa-planet-moon"){
	  label = _U("\ue01f"); }

	else if(iconName == "fa-face-eyes-xmarks"){
	  label = _U("\ue374"); }

	else if(iconName == "fa-chart-scatter"){
	  label = _U("\uf7ee"); }

	else if(iconName == "fa-display-arrow-down"){
	  label = _U("\ue164"); }

	else if(iconName == "fa-store"){
	  label = _U("\uf54e"); }

	else if(iconName == "fa-arrow-trend-up"){
	  label = _U("\ue098"); }

	else if(iconName == "fa-plug-circle-minus"){
	  label = _U("\ue55e"); }

	else if(iconName == "fa-olive-branch"){
	  label = _U("\ue317"); }

	else if(iconName == "fa-angle"){
	  label = _U("\ue08c"); }

	else if(iconName == "fa-vacuum-robot"){
	  label = _U("\ue04e"); }

	else if(iconName == "fa-sign-hanging"){
	  label = _U("\uf4d9"); }

	else if(iconName == "fa-sign"){
	  label = _U("\uf4d9"); }

	else if(iconName == "fa-square-divide"){
	  label = _U("\ue26a"); }

	else if(iconName == "fa-folder-check"){
	  label = _U("\ue64e"); }

	else if(iconName == "fa-signal-stream-slash"){
	  label = _U("\ue250"); }

	else if(iconName == "fa-bezier-curve"){
	  label = _U("\uf55b"); }

	else if(iconName == "fa-eye-dropper-half"){
	  label = _U("\ue173"); }

	else if(iconName == "fa-store-lock"){
	  label = _U("\ue4a6"); }

	else if(iconName == "fa-bell-slash"){
	  label = _U("\uf1f6"); }

	else if(iconName == "fa-cloud-bolt-sun"){
	  label = _U("\uf76e"); }

	else if(iconName == "fa-thunderstorm-sun"){
	  label = _U("\uf76e"); }

	else if(iconName == "fa-camera-slash"){
	  label = _U("\ue0d9"); }

	else if(iconName == "fa-comment-quote"){
	  label = _U("\ue14c"); }

	else if(iconName == "fa-tablet"){
	  label = _U("\uf3fb"); }

	else if(iconName == "fa-tablet-android"){
	  label = _U("\uf3fb"); }

	else if(iconName == "fa-school-flag"){
	  label = _U("\ue56e"); }

	else if(iconName == "fa-message-code"){
	  label = _U("\ue1df"); }

	else if(iconName == "fa-glass-half"){
	  label = _U("\ue192"); }

	else if(iconName == "fa-glass-half-empty"){
	  label = _U("\ue192"); }

	else if(iconName == "fa-glass-half-full"){
	  label = _U("\ue192"); }

	else if(iconName == "fa-fill"){
	  label = _U("\uf575"); }

	else if(iconName == "fa-message-minus"){
	  label = _U("\uf4a7"); }

	else if(iconName == "fa-comment-alt-minus"){
	  label = _U("\uf4a7"); }

	else if(iconName == "fa-angle-up"){
	  label = _U("\uf106"); }

	else if(iconName == "fa-dinosaur"){
	  label = _U("\ue5fe"); }

	else if(iconName == "fa-drumstick-bite"){
	  label = _U("\uf6d7"); }

	else if(iconName == "fa-link-horizontal-slash"){
	  label = _U("\ue1cc"); }

	else if(iconName == "fa-chain-horizontal-slash"){
	  label = _U("\ue1cc"); }

	else if(iconName == "fa-holly-berry"){
	  label = _U("\uf7aa"); }

	else if(iconName == "fa-nose"){
	  label = _U("\ue5bd"); }

	else if(iconName == "fa-arrow-left-to-arc"){
	  label = _U("\ue616"); }

	else if(iconName == "fa-chevron-left"){
	  label = _U("\uf053"); }

	else if(iconName == "fa-bacteria"){
	  label = _U("\ue059"); }

	else if(iconName == "fa-clouds"){
	  label = _U("\uf744"); }

	else if(iconName == "fa-money-bill-simple"){
	  label = _U("\ue1f1"); }

	else if(iconName == "fa-hand-lizard"){
	  label = _U("\uf258"); }

	else if(iconName == "fa-table-pivot"){
	  label = _U("\ue291"); }

	else if(iconName == "fa-filter-slash"){
	  label = _U("\ue17d"); }

	else if(iconName == "fa-trash-can-undo"){
	  label = _U("\uf896"); }

	else if(iconName == "fa-trash-can-arrow-turn-left"){
	  label = _U("\uf896"); }

	else if(iconName == "fa-trash-undo-alt"){
	  label = _U("\uf896"); }

	else if(iconName == "fa-notdef"){
	  label = _U("\ue1fe"); }

	else if(iconName == "fa-disease"){
	  label = _U("\uf7fa"); }

	else if(iconName == "fa-person-to-door"){
	  label = _U("\ue433"); }

	else if(iconName == "fa-turntable"){
	  label = _U("\uf8e4"); }

	else if(iconName == "fa-briefcase-medical"){
	  label = _U("\uf469"); }

	else if(iconName == "fa-genderless"){
	  label = _U("\uf22d"); }

	else if(iconName == "fa-chevron-right"){
	  label = _U("\uf054"); }

	else if(iconName == "fa-signal-weak"){
	  label = _U("\uf68c"); }

	else if(iconName == "fa-signal-1"){
	  label = _U("\uf68c"); }

	else if(iconName == "fa-clock-five"){
	  label = _U("\ue349"); }

	else if(iconName == "fa-retweet"){
	  label = _U("\uf079"); }

	else if(iconName == "fa-car-rear"){
	  label = _U("\uf5de"); }

	else if(iconName == "fa-car-alt"){
	  label = _U("\uf5de"); }

	else if(iconName == "fa-pump-soap"){
	  label = _U("\ue06b"); }

	else if(iconName == "fa-computer-classic"){
	  label = _U("\uf8b1"); }

	else if(iconName == "fa-frame"){
	  label = _U("\ue495"); }

	else if(iconName == "fa-video-slash"){
	  label = _U("\uf4e2"); }

	else if(iconName == "fa-battery-quarter"){
	  label = _U("\uf243"); }

	else if(iconName == "fa-battery-2"){
	  label = _U("\uf243"); }

	else if(iconName == "fa-ellipsis-stroke"){
	  label = _U("\uf39b"); }

	else if(iconName == "fa-ellipsis-h-alt"){
	  label = _U("\uf39b"); }

	else if(iconName == "fa-radio"){
	  label = _U("\uf8d7"); }

	else if(iconName == "fa-baby-carriage"){
	  label = _U("\uf77d"); }

	else if(iconName == "fa-carriage-baby"){
	  label = _U("\uf77d"); }

	else if(iconName == "fa-face-expressionless"){
	  label = _U("\ue373"); }

	else if(iconName == "fa-down-to-dotted-line"){
	  label = _U("\ue408"); }

	else if(iconName == "fa-cloud-music"){
	  label = _U("\uf8ae"); }

	else if(iconName == "fa-traffic-light"){
	  label = _U("\uf637"); }

	else if(iconName == "fa-cloud-minus"){
	  label = _U("\ue35d"); }

	else if(iconName == "fa-thermometer"){
	  label = _U("\uf491"); }

	else if(iconName == "fa-shield-minus"){
	  label = _U("\ue249"); }

	else if(iconName == "fa-vr-cardboard"){
	  label = _U("\uf729"); }

	else if(iconName == "fa-car-tilt"){
	  label = _U("\uf5e5"); }

	else if(iconName == "fa-gauge-circle-minus"){
	  label = _U("\ue497"); }

	else if(iconName == "fa-brightness-low"){
	  label = _U("\ue0ca"); }

	else if(iconName == "fa-hand-middle-finger"){
	  label = _U("\uf806"); }

	else if(iconName == "fa-percent"){
	  label = _U("\u0025"); }

	else if(iconName == "fa-percentage"){
	  label = _U("\u0025"); }

	else if(iconName == "fa-truck-moving"){
	  label = _U("\uf4df"); }

	else if(iconName == "fa-glass-water-droplet"){
	  label = _U("\ue4f5"); }

	else if(iconName == "fa-conveyor-belt"){
	  label = _U("\uf46e"); }

	else if(iconName == "fa-location-check"){
	  label = _U("\uf606"); }

	else if(iconName == "fa-map-marker-check"){
	  label = _U("\uf606"); }

	else if(iconName == "fa-coin-vertical"){
	  label = _U("\ue3fd"); }

	else if(iconName == "fa-display"){
	  label = _U("\ue163"); }

	else if(iconName == "fa-person-sign"){
	  label = _U("\uf757"); }

	else if(iconName == "fa-face-smile"){
	  label = _U("\uf118"); }

	else if(iconName == "fa-smile"){
	  label = _U("\uf118"); }

	else if(iconName == "fa-phone-hangup"){
	  label = _U("\ue225"); }

	else if(iconName == "fa-signature-slash"){
	  label = _U("\ue3cb"); }

	else if(iconName == "fa-thumbtack"){
	  label = _U("\uf08d"); }

	else if(iconName == "fa-thumb-tack"){
	  label = _U("\uf08d"); }

	else if(iconName == "fa-wheat-slash"){
	  label = _U("\ue339"); }

	else if(iconName == "fa-trophy"){
	  label = _U("\uf091"); }

	else if(iconName == "fa-clouds-sun"){
	  label = _U("\uf746"); }

	else if(iconName == "fa-person-praying"){
	  label = _U("\uf683"); }

	else if(iconName == "fa-pray"){
	  label = _U("\uf683"); }

	else if(iconName == "fa-hammer"){
	  label = _U("\uf6e3"); }

	else if(iconName == "fa-face-vomit"){
	  label = _U("\ue3a0"); }

	else if(iconName == "fa-speakers"){
	  label = _U("\uf8e0"); }

	else if(iconName == "fa-tty-answer"){
	  label = _U("\ue2b9"); }

	else if(iconName == "fa-teletype-answer"){
	  label = _U("\ue2b9"); }

	else if(iconName == "fa-mug-tea-saucer"){
	  label = _U("\ue1f5"); }

	else if(iconName == "fa-diagram-lean-canvas"){
	  label = _U("\ue156"); }

	else if(iconName == "fa-alt"){
	  label = _U("\ue08a"); }

	else if(iconName == "fa-dial"){
	  label = _U("\ue15b"); }

	else if(iconName == "fa-dial-med-high"){
	  label = _U("\ue15b"); }

	else if(iconName == "fa-hand-peace"){
	  label = _U("\uf25b"); }

	else if(iconName == "fa-circle-trash"){
	  label = _U("\ue126"); }

	else if(iconName == "fa-trash-circle"){
	  label = _U("\ue126"); }

	else if(iconName == "fa-rotate"){
	  label = _U("\uf2f1"); }

	else if(iconName == "fa-sync-alt"){
	  label = _U("\uf2f1"); }

	else if(iconName == "fa-circle-quarters"){
	  label = _U("\ue3f8"); }

	else if(iconName == "fa-spinner"){
	  label = _U("\uf110"); }

	else if(iconName == "fa-tower-control"){
	  label = _U("\ue2a2"); }

	else if(iconName == "fa-arrow-up-triangle-square"){
	  label = _U("\uf88a"); }

	else if(iconName == "fa-sort-shapes-up"){
	  label = _U("\uf88a"); }

	else if(iconName == "fa-whale"){
	  label = _U("\uf72c"); }

	else if(iconName == "fa-robot"){
	  label = _U("\uf544"); }

	else if(iconName == "fa-peace"){
	  label = _U("\uf67c"); }

	else if(iconName == "fa-party-horn"){
	  label = _U("\ue31b"); }

	else if(iconName == "fa-gears"){
	  label = _U("\uf085"); }

	else if(iconName == "fa-cogs"){
	  label = _U("\uf085"); }

	else if(iconName == "fa-sun-bright"){
	  label = _U("\ue28f"); }

	else if(iconName == "fa-sun-alt"){
	  label = _U("\ue28f"); }

	else if(iconName == "fa-warehouse"){
	  label = _U("\uf494"); }

	else if(iconName == "fa-conveyor-belt-arm"){
	  label = _U("\ue5f8"); }

	else if(iconName == "fa-lock-keyhole-open"){
	  label = _U("\uf3c2"); }

	else if(iconName == "fa-lock-open-alt"){
	  label = _U("\uf3c2"); }

	else if(iconName == "fa-square-fragile"){
	  label = _U("\uf49b"); }

	else if(iconName == "fa-box-fragile"){
	  label = _U("\uf49b"); }

	else if(iconName == "fa-square-wine-glass-crack"){
	  label = _U("\uf49b"); }

	else if(iconName == "fa-arrow-up-right-dots"){
	  label = _U("\ue4b7"); }

	else if(iconName == "fa-square-n"){
	  label = _U("\ue277"); }

	else if(iconName == "fa-splotch"){
	  label = _U("\uf5bc"); }

	else if(iconName == "fa-face-grin-hearts"){
	  label = _U("\uf584"); }

	else if(iconName == "fa-grin-hearts"){
	  label = _U("\uf584"); }

	else if(iconName == "fa-meter"){
	  label = _U("\ue1e8"); }

	else if(iconName == "fa-mandolin"){
	  label = _U("\uf6f9"); }

	else if(iconName == "fa-dice-four"){
	  label = _U("\uf524"); }

	else if(iconName == "fa-sim-card"){
	  label = _U("\uf7c4"); }

	else if(iconName == "fa-transgender"){
	  label = _U("\uf225"); }

	else if(iconName == "fa-transgender-alt"){
	  label = _U("\uf225"); }

	else if(iconName == "fa-mercury"){
	  label = _U("\uf223"); }

	else if(iconName == "fa-up-from-bracket"){
	  label = _U("\ue590"); }

	else if(iconName == "fa-knife-kitchen"){
	  label = _U("\uf6f5"); }

	else if(iconName == "fa-border-right"){
	  label = _U("\uf852"); }

	else if(iconName == "fa-arrow-turn-down"){
	  label = _U("\uf149"); }

	else if(iconName == "fa-level-down"){
	  label = _U("\uf149"); }

	else if(iconName == "fa-spade"){
	  label = _U("\uf2f4"); }

	else if(iconName == "fa-card-spade"){
	  label = _U("\ue3ec"); }

	else if(iconName == "fa-line-columns"){
	  label = _U("\uf870"); }

	else if(iconName == "fa-arrow-right-to-line"){
	  label = _U("\uf340"); }

	else if(iconName == "fa-arrow-to-right"){
	  label = _U("\uf340"); }

	else if(iconName == "fa-person-falling-burst"){
	  label = _U("\ue547"); }

	else if(iconName == "fa-flag-pennant"){
	  label = _U("\uf456"); }

	else if(iconName == "fa-pennant"){
	  label = _U("\uf456"); }

	else if(iconName == "fa-conveyor-belt-empty"){
	  label = _U("\ue150"); }

	else if(iconName == "fa-user-group-simple"){
	  label = _U("\ue603"); }

	else if(iconName == "fa-award"){
	  label = _U("\uf559"); }

	else if(iconName == "fa-ticket-simple"){
	  label = _U("\uf3ff"); }

	else if(iconName == "fa-ticket-alt"){
	  label = _U("\uf3ff"); }

	else if(iconName == "fa-building"){
	  label = _U("\uf1ad"); }

	else if(iconName == "fa-angles-left"){
	  label = _U("\uf100"); }

	else if(iconName == "fa-angle-double-left"){
	  label = _U("\uf100"); }

	else if(iconName == "fa-camcorder"){
	  label = _U("\uf8a8"); }

	else if(iconName == "fa-video-handheld"){
	  label = _U("\uf8a8"); }

	else if(iconName == "fa-pancakes"){
	  label = _U("\ue42d"); }

	else if(iconName == "fa-album-circle-user"){
	  label = _U("\ue48d"); }

	else if(iconName == "fa-subtitles-slash"){
	  label = _U("\ue610"); }

	else if(iconName == "fa-qrcode"){
	  label = _U("\uf029"); }

	else if(iconName == "fa-dice-d10"){
	  label = _U("\uf6cd"); }

	else if(iconName == "fa-fireplace"){
	  label = _U("\uf79a"); }

	else if(iconName == "fa-browser"){
	  label = _U("\uf37e"); }

	else if(iconName == "fa-pen-paintbrush"){
	  label = _U("\uf618"); }

	else if(iconName == "fa-pencil-paintbrush"){
	  label = _U("\uf618"); }

	else if(iconName == "fa-fish-cooked"){
	  label = _U("\uf7fe"); }

	else if(iconName == "fa-chair-office"){
	  label = _U("\uf6c1"); }

	else if(iconName == "fa-magnifying-glass-music"){
	  label = _U("\ue65f"); }

	else if(iconName == "fa-nesting-dolls"){
	  label = _U("\ue3ba"); }

	else if(iconName == "fa-clock-rotate-left"){
	  label = _U("\uf1da"); }

	else if(iconName == "fa-history"){
	  label = _U("\uf1da"); }

	else if(iconName == "fa-trumpet"){
	  label = _U("\uf8e3"); }

	else if(iconName == "fa-face-grin-beam-sweat"){
	  label = _U("\uf583"); }

	else if(iconName == "fa-grin-beam-sweat"){
	  label = _U("\uf583"); }

	else if(iconName == "fa-fire-smoke"){
	  label = _U("\uf74b"); }

	else if(iconName == "fa-phone-missed"){
	  label = _U("\ue226"); }

	else if(iconName == "fa-file-export"){
	  label = _U("\uf56e"); }

	else if(iconName == "fa-arrow-right-from-file"){
	  label = _U("\uf56e"); }

	else if(iconName == "fa-shield"){
	  label = _U("\uf132"); }

	else if(iconName == "fa-shield-blank"){
	  label = _U("\uf132"); }

	else if(iconName == "fa-arrow-up-short-wide"){
	  label = _U("\uf885"); }

	else if(iconName == "fa-sort-amount-up-alt"){
	  label = _U("\uf885"); }

	else if(iconName == "fa-arrows-repeat-1"){
	  label = _U("\uf366"); }

	else if(iconName == "fa-repeat-1-alt"){
	  label = _U("\uf366"); }

	else if(iconName == "fa-gun-slash"){
	  label = _U("\ue19c"); }

	else if(iconName == "fa-avocado"){
	  label = _U("\ue0aa"); }

	else if(iconName == "fa-binary"){
	  label = _U("\ue33b"); }

	else if(iconName == "fa-glasses-round"){
	  label = _U("\uf5f5"); }

	else if(iconName == "fa-glasses-alt"){
	  label = _U("\uf5f5"); }

	else if(iconName == "fa-phone-plus"){
	  label = _U("\uf4d2"); }

	else if(iconName == "fa-ditto"){
	  label = _U("\u0022"); }

	else if(iconName == "fa-person-seat"){
	  label = _U("\ue21e"); }

	else if(iconName == "fa-house-medical"){
	  label = _U("\ue3b2"); }

	else if(iconName == "fa-golf-ball-tee"){
	  label = _U("\uf450"); }

	else if(iconName == "fa-golf-ball"){
	  label = _U("\uf450"); }

	else if(iconName == "fa-circle-chevron-left"){
	  label = _U("\uf137"); }

	else if(iconName == "fa-chevron-circle-left"){
	  label = _U("\uf137"); }

	else if(iconName == "fa-house-chimney-window"){
	  label = _U("\ue00d"); }

	else if(iconName == "fa-scythe"){
	  label = _U("\uf710"); }

	else if(iconName == "fa-pen-nib"){
	  label = _U("\uf5ad"); }

	else if(iconName == "fa-ban-parking"){
	  label = _U("\uf616"); }

	else if(iconName == "fa-parking-circle-slash"){
	  label = _U("\uf616"); }

	else if(iconName == "fa-tent-arrow-turn-left"){
	  label = _U("\ue580"); }

	else if(iconName == "fa-face-diagonal-mouth"){
	  label = _U("\ue47e"); }

	else if(iconName == "fa-diagram-cells"){
	  label = _U("\ue475"); }

	else if(iconName == "fa-cricket-bat-ball"){
	  label = _U("\uf449"); }

	else if(iconName == "fa-cricket"){
	  label = _U("\uf449"); }

	else if(iconName == "fa-tents"){
	  label = _U("\ue582"); }

	else if(iconName == "fa-wand-magic"){
	  label = _U("\uf0d0"); }

	else if(iconName == "fa-magic"){
	  label = _U("\uf0d0"); }

	else if(iconName == "fa-dog"){
	  label = _U("\uf6d3"); }

	else if(iconName == "fa-pen-line"){
	  label = _U("\ue212"); }

	else if(iconName == "fa-atom-simple"){
	  label = _U("\uf5d3"); }

	else if(iconName == "fa-atom-alt"){
	  label = _U("\uf5d3"); }

	else if(iconName == "fa-ampersand"){
	  label = _U("\u0026"); }

	else if(iconName == "fa-carrot"){
	  label = _U("\uf787"); }

	else if(iconName == "fa-arrow-up-from-line"){
	  label = _U("\uf342"); }

	else if(iconName == "fa-arrow-from-bottom"){
	  label = _U("\uf342"); }

	else if(iconName == "fa-moon"){
	  label = _U("\uf186"); }

	else if(iconName == "fa-pen-slash"){
	  label = _U("\ue213"); }

	else if(iconName == "fa-wine-glass-empty"){
	  label = _U("\uf5ce"); }

	else if(iconName == "fa-wine-glass-alt"){
	  label = _U("\uf5ce"); }

	else if(iconName == "fa-square-star"){
	  label = _U("\ue27f"); }

	else if(iconName == "fa-cheese"){
	  label = _U("\uf7ef"); }

	else if(iconName == "fa-send-backward"){
	  label = _U("\uf87f"); }

	else if(iconName == "fa-yin-yang"){
	  label = _U("\uf6ad"); }

	else if(iconName == "fa-music"){
	  label = _U("\uf001"); }

	else if(iconName == "fa-compass-slash"){
	  label = _U("\uf5e9"); }

	else if(iconName == "fa-clock-one"){
	  label = _U("\ue34e"); }

	else if(iconName == "fa-file-music"){
	  label = _U("\uf8b6"); }

	else if(iconName == "fa-code-commit"){
	  label = _U("\uf386"); }

	else if(iconName == "fa-temperature-low"){
	  label = _U("\uf76b"); }

	else if(iconName == "fa-person-biking"){
	  label = _U("\uf84a"); }

	else if(iconName == "fa-biking"){
	  label = _U("\uf84a"); }

	else if(iconName == "fa-display-chart-up-circle-currency"){
	  label = _U("\ue5e5"); }

	else if(iconName == "fa-skeleton"){
	  label = _U("\uf620"); }

	else if(iconName == "fa-circle-g"){
	  label = _U("\ue10f"); }

	else if(iconName == "fa-circle-arrow-up-left"){
	  label = _U("\ue0fb"); }

	else if(iconName == "fa-coin-blank"){
	  label = _U("\ue3fb"); }

	else if(iconName == "fa-broom"){
	  label = _U("\uf51a"); }

	else if(iconName == "fa-vacuum"){
	  label = _U("\ue04d"); }

	else if(iconName == "fa-shield-heart"){
	  label = _U("\ue574"); }

	else if(iconName == "fa-card-heart"){
	  label = _U("\ue3eb"); }

	else if(iconName == "fa-lightbulb-cfl-on"){
	  label = _U("\ue5a7"); }

	else if(iconName == "fa-melon"){
	  label = _U("\ue310"); }

	else if(iconName == "fa-gopuram"){
	  label = _U("\uf664"); }

	else if(iconName == "fa-earth-oceania"){
	  label = _U("\ue47b"); }

	else if(iconName == "fa-globe-oceania"){
	  label = _U("\ue47b"); }

	else if(iconName == "fa-container-storage"){
	  label = _U("\uf4b7"); }

	else if(iconName == "fa-face-pouting"){
	  label = _U("\ue387"); }

	else if(iconName == "fa-square-xmark"){
	  label = _U("\uf2d3"); }

	else if(iconName == "fa-times-square"){
	  label = _U("\uf2d3"); }

	else if(iconName == "fa-xmark-square"){
	  label = _U("\uf2d3"); }

	else if(iconName == "fa-face-explode"){
	  label = _U("\ue2fe"); }

	else if(iconName == "fa-exploding-head"){
	  label = _U("\ue2fe"); }

	else if(iconName == "fa-hashtag"){
	  label = _U("\u0023"); }

	else if(iconName == "fa-up-right-and-down-left-from-center"){
	  label = _U("\uf424"); }

	else if(iconName == "fa-expand-alt"){
	  label = _U("\uf424"); }

	else if(iconName == "fa-oil-can"){
	  label = _U("\uf613"); }

	else if(iconName == "fa-t"){
	  label = _U("\u0054"); }

	else if(iconName == "fa-transformer-bolt"){
	  label = _U("\ue2a4"); }

	else if(iconName == "fa-hippo"){
	  label = _U("\uf6ed"); }

	else if(iconName == "fa-chart-column"){
	  label = _U("\ue0e3"); }

	else if(iconName == "fa-cassette-vhs"){
	  label = _U("\uf8ec"); }

	else if(iconName == "fa-vhs"){
	  label = _U("\uf8ec"); }

	else if(iconName == "fa-infinity"){
	  label = _U("\uf534"); }

	else if(iconName == "fa-vial-circle-check"){
	  label = _U("\ue596"); }

	else if(iconName == "fa-chimney"){
	  label = _U("\uf78b"); }

	else if(iconName == "fa-object-intersect"){
	  label = _U("\ue49d"); }

	else if(iconName == "fa-person-arrow-down-to-line"){
	  label = _U("\ue538"); }

	else if(iconName == "fa-voicemail"){
	  label = _U("\uf897"); }

	else if(iconName == "fa-block-brick"){
	  label = _U("\ue3db"); }

	else if(iconName == "fa-wall-brick"){
	  label = _U("\ue3db"); }

	else if(iconName == "fa-fan"){
	  label = _U("\uf863"); }

	else if(iconName == "fa-bags-shopping"){
	  label = _U("\uf847"); }

	else if(iconName == "fa-paragraph-left"){
	  label = _U("\uf878"); }

	else if(iconName == "fa-paragraph-rtl"){
	  label = _U("\uf878"); }

	else if(iconName == "fa-person-walking-luggage"){
	  label = _U("\ue554"); }

	else if(iconName == "fa-caravan-simple"){
	  label = _U("\ue000"); }

	else if(iconName == "fa-caravan-alt"){
	  label = _U("\ue000"); }

	else if(iconName == "fa-turtle"){
	  label = _U("\uf726"); }

	else if(iconName == "fa-pencil-mechanical"){
	  label = _U("\ue5ca"); }

	else if(iconName == "fa-up-down"){
	  label = _U("\uf338"); }

	else if(iconName == "fa-arrows-alt-v"){
	  label = _U("\uf338"); }

	else if(iconName == "fa-cloud-moon-rain"){
	  label = _U("\uf73c"); }

	else if(iconName == "fa-booth-curtain"){
	  label = _U("\uf734"); }

	else if(iconName == "fa-calendar"){
	  label = _U("\uf133"); }

	else if(iconName == "fa-box-heart"){
	  label = _U("\uf49d"); }

	else if(iconName == "fa-trailer"){
	  label = _U("\ue041"); }

	else if(iconName == "fa-user-doctor-message"){
	  label = _U("\uf82e"); }

	else if(iconName == "fa-user-md-chat"){
	  label = _U("\uf82e"); }

	else if(iconName == "fa-bahai"){
	  label = _U("\uf666"); }

	else if(iconName == "fa-haykal"){
	  label = _U("\uf666"); }

	else if(iconName == "fa-lighthouse"){
	  label = _U("\ue612"); }

	else if(iconName == "fa-amp-guitar"){
	  label = _U("\uf8a1"); }

	else if(iconName == "fa-sd-card"){
	  label = _U("\uf7c2"); }

	else if(iconName == "fa-volume-slash"){
	  label = _U("\uf2e2"); }

	else if(iconName == "fa-border-bottom"){
	  label = _U("\uf84d"); }

	else if(iconName == "fa-wifi-weak"){
	  label = _U("\uf6aa"); }

	else if(iconName == "fa-wifi-1"){
	  label = _U("\uf6aa"); }

	else if(iconName == "fa-dragon"){
	  label = _U("\uf6d5"); }

	else if(iconName == "fa-shoe-prints"){
	  label = _U("\uf54b"); }

	else if(iconName == "fa-circle-plus"){
	  label = _U("\uf055"); }

	else if(iconName == "fa-plus-circle"){
	  label = _U("\uf055"); }

	else if(iconName == "fa-face-grin-tongue-wink"){
	  label = _U("\uf58b"); }

	else if(iconName == "fa-grin-tongue-wink"){
	  label = _U("\uf58b"); }

	else if(iconName == "fa-hand-holding"){
	  label = _U("\uf4bd"); }

	else if(iconName == "fa-plug-circle-exclamation"){
	  label = _U("\ue55d"); }

	else if(iconName == "fa-link-slash"){
	  label = _U("\uf127"); }

	else if(iconName == "fa-chain-broken"){
	  label = _U("\uf127"); }

	else if(iconName == "fa-chain-slash"){
	  label = _U("\uf127"); }

	else if(iconName == "fa-unlink"){
	  label = _U("\uf127"); }

	else if(iconName == "fa-clone"){
	  label = _U("\uf24d"); }

	else if(iconName == "fa-person-walking-arrow-loop-left"){
	  label = _U("\ue551"); }

	else if(iconName == "fa-arrow-up-z-a"){
	  label = _U("\uf882"); }

	else if(iconName == "fa-sort-alpha-up-alt"){
	  label = _U("\uf882"); }

	else if(iconName == "fa-fire-flame-curved"){
	  label = _U("\uf7e4"); }

	else if(iconName == "fa-fire-alt"){
	  label = _U("\uf7e4"); }

	else if(iconName == "fa-tornado"){
	  label = _U("\uf76f"); }

	else if(iconName == "fa-file-circle-plus"){
	  label = _U("\ue494"); }

	else if(iconName == "fa-delete-right"){
	  label = _U("\ue154"); }

	else if(iconName == "fa-book-quran"){
	  label = _U("\uf687"); }

	else if(iconName == "fa-quran"){
	  label = _U("\uf687"); }

	else if(iconName == "fa-circle-quarter"){
	  label = _U("\ue11f"); }

	else if(iconName == "fa-anchor"){
	  label = _U("\uf13d"); }

	else if(iconName == "fa-border-all"){
	  label = _U("\uf84c"); }

	else if(iconName == "fa-function"){
	  label = _U("\uf661"); }

	else if(iconName == "fa-face-angry"){
	  label = _U("\uf556"); }

	else if(iconName == "fa-angry"){
	  label = _U("\uf556"); }

	else if(iconName == "fa-people-simple"){
	  label = _U("\ue21b"); }

	else if(iconName == "fa-cookie-bite"){
	  label = _U("\uf564"); }

	else if(iconName == "fa-arrow-trend-down"){
	  label = _U("\ue097"); }

	else if(iconName == "fa-rss"){
	  label = _U("\uf09e"); }

	else if(iconName == "fa-feed"){
	  label = _U("\uf09e"); }

	else if(iconName == "fa-face-monocle"){
	  label = _U("\ue380"); }

	else if(iconName == "fa-draw-polygon"){
	  label = _U("\uf5ee"); }

	else if(iconName == "fa-scale-balanced"){
	  label = _U("\uf24e"); }

	else if(iconName == "fa-balance-scale"){
	  label = _U("\uf24e"); }

	else if(iconName == "fa-calendar-lines"){
	  label = _U("\ue0d5"); }

	else if(iconName == "fa-calendar-note"){
	  label = _U("\ue0d5"); }

	else if(iconName == "fa-arrow-down-big-small"){
	  label = _U("\uf88c"); }

	else if(iconName == "fa-sort-size-down"){
	  label = _U("\uf88c"); }

	else if(iconName == "fa-gauge-simple-high"){
	  label = _U("\uf62a"); }

	else if(iconName == "fa-tachometer"){
	  label = _U("\uf62a"); }

	else if(iconName == "fa-tachometer-fast"){
	  label = _U("\uf62a"); }

	else if(iconName == "fa-do-not-enter"){
	  label = _U("\uf5ec"); }

	else if(iconName == "fa-shower"){
	  label = _U("\uf2cc"); }

	else if(iconName == "fa-dice-d8"){
	  label = _U("\uf6d2"); }

	else if(iconName == "fa-desktop"){
	  label = _U("\uf390"); }

	else if(iconName == "fa-desktop-alt"){
	  label = _U("\uf390"); }

	else if(iconName == "fa-m"){
	  label = _U("\u004d"); }

	else if(iconName == "fa-spinner-scale"){
	  label = _U("\ue62a"); }

	else if(iconName == "fa-grip-dots-vertical"){
	  label = _U("\ue411"); }

	else if(iconName == "fa-face-viewfinder"){
	  label = _U("\ue2ff"); }

	else if(iconName == "fa-soft-serve"){
	  label = _U("\ue400"); }

	else if(iconName == "fa-creemee"){
	  label = _U("\ue400"); }

	else if(iconName == "fa-h5"){
	  label = _U("\ue412"); }

	else if(iconName == "fa-hand-back-point-down"){
	  label = _U("\ue19e"); }

	else if(iconName == "fa-table-list"){
	  label = _U("\uf00b"); }

	else if(iconName == "fa-th-list"){
	  label = _U("\uf00b"); }

	else if(iconName == "fa-basket-shopping-minus"){
	  label = _U("\ue652"); }

	else if(iconName == "fa-comment-sms"){
	  label = _U("\uf7cd"); }

	else if(iconName == "fa-sms"){
	  label = _U("\uf7cd"); }

	else if(iconName == "fa-rectangle"){
	  label = _U("\uf2fa"); }

	else if(iconName == "fa-rectangle-landscape"){
	  label = _U("\uf2fa"); }

	else if(iconName == "fa-clipboard-list-check"){
	  label = _U("\uf737"); }

	else if(iconName == "fa-turkey"){
	  label = _U("\uf725"); }

	else if(iconName == "fa-book"){
	  label = _U("\uf02d"); }

	else if(iconName == "fa-user-plus"){
	  label = _U("\uf234"); }

	else if(iconName == "fa-ice-skate"){
	  label = _U("\uf7ac"); }

	else if(iconName == "fa-check"){
	  label = _U("\uf00c"); }

	else if(iconName == "fa-battery-three-quarters"){
	  label = _U("\uf241"); }

	else if(iconName == "fa-battery-4"){
	  label = _U("\uf241"); }

	else if(iconName == "fa-tomato"){
	  label = _U("\ue330"); }

	else if(iconName == "fa-sword-laser"){
	  label = _U("\ue03b"); }

	else if(iconName == "fa-house-circle-check"){
	  label = _U("\ue509"); }

	else if(iconName == "fa-buildings"){
	  label = _U("\ue0cc"); }

	else if(iconName == "fa-angle-left"){
	  label = _U("\uf104"); }

	else if(iconName == "fa-cart-flatbed-boxes"){
	  label = _U("\uf475"); }

	else if(iconName == "fa-dolly-flatbed-alt"){
	  label = _U("\uf475"); }

	else if(iconName == "fa-diagram-successor"){
	  label = _U("\ue47a"); }

	else if(iconName == "fa-truck-arrow-right"){
	  label = _U("\ue58b"); }

	else if(iconName == "fa-square-w"){
	  label = _U("\ue285"); }

	else if(iconName == "fa-arrows-split-up-and-left"){
	  label = _U("\ue4bc"); }

	else if(iconName == "fa-lamp"){
	  label = _U("\uf4ca"); }

	else if(iconName == "fa-airplay"){
	  label = _U("\ue089"); }

	else if(iconName == "fa-hand-fist"){
	  label = _U("\uf6de"); }

	else if(iconName == "fa-fist-raised"){
	  label = _U("\uf6de"); }

	else if(iconName == "fa-shield-quartered"){
	  label = _U("\ue575"); }

	else if(iconName == "fa-slash-forward"){
	  label = _U("\u002f"); }

	else if(iconName == "fa-location-pen"){
	  label = _U("\uf607"); }

	else if(iconName == "fa-map-marker-edit"){
	  label = _U("\uf607"); }

	else if(iconName == "fa-cloud-moon"){
	  label = _U("\uf6c3"); }

	else if(iconName == "fa-pot-food"){
	  label = _U("\ue43f"); }

	else if(iconName == "fa-briefcase"){
	  label = _U("\uf0b1"); }

	else if(iconName == "fa-person-falling"){
	  label = _U("\ue546"); }

	else if(iconName == "fa-image-portrait"){
	  label = _U("\uf3e0"); }

	else if(iconName == "fa-portrait"){
	  label = _U("\uf3e0"); }

	else if(iconName == "fa-user-tag"){
	  label = _U("\uf507"); }

	else if(iconName == "fa-rug"){
	  label = _U("\ue569"); }

	else if(iconName == "fa-print-slash"){
	  label = _U("\uf686"); }

	else if(iconName == "fa-earth-europe"){
	  label = _U("\uf7a2"); }

	else if(iconName == "fa-globe-europe"){
	  label = _U("\uf7a2"); }

	else if(iconName == "fa-cart-flatbed-suitcase"){
	  label = _U("\uf59d"); }

	else if(iconName == "fa-luggage-cart"){
	  label = _U("\uf59d"); }

	else if(iconName == "fa-hand-back-point-ribbon"){
	  label = _U("\ue1a0"); }

	else if(iconName == "fa-rectangle-xmark"){
	  label = _U("\uf410"); }

	else if(iconName == "fa-rectangle-times"){
	  label = _U("\uf410"); }

	else if(iconName == "fa-times-rectangle"){
	  label = _U("\uf410"); }

	else if(iconName == "fa-window-close"){
	  label = _U("\uf410"); }

	else if(iconName == "fa-tire-rugged"){
	  label = _U("\uf634"); }

	else if(iconName == "fa-lightbulb-dollar"){
	  label = _U("\uf670"); }

	else if(iconName == "fa-cowbell"){
	  label = _U("\uf8b3"); }

	else if(iconName == "fa-baht-sign"){
	  label = _U("\ue0ac"); }

	else if(iconName == "fa-corner"){
	  label = _U("\ue3fe"); }

	else if(iconName == "fa-chevrons-right"){
	  label = _U("\uf324"); }

	else if(iconName == "fa-chevron-double-right"){
	  label = _U("\uf324"); }

	else if(iconName == "fa-book-open"){
	  label = _U("\uf518"); }

	else if(iconName == "fa-book-journal-whills"){
	  label = _U("\uf66a"); }

	else if(iconName == "fa-journal-whills"){
	  label = _U("\uf66a"); }

	else if(iconName == "fa-inhaler"){
	  label = _U("\uf5f9"); }

	else if(iconName == "fa-handcuffs"){
	  label = _U("\ue4f8"); }

	else if(iconName == "fa-snake"){
	  label = _U("\uf716"); }

	else if(iconName == "fa-triangle-exclamation"){
	  label = _U("\uf071"); }

	else if(iconName == "fa-exclamation-triangle"){
	  label = _U("\uf071"); }

	else if(iconName == "fa-warning"){
	  label = _U("\uf071"); }

	else if(iconName == "fa-note-medical"){
	  label = _U("\ue200"); }

	else if(iconName == "fa-database"){
	  label = _U("\uf1c0"); }

	else if(iconName == "fa-down-left"){
	  label = _U("\ue16a"); }

	else if(iconName == "fa-share"){
	  label = _U("\uf064"); }

	else if(iconName == "fa-mail-forward"){
	  label = _U("\uf064"); }

	else if(iconName == "fa-face-thinking"){
	  label = _U("\ue39b"); }

	else if(iconName == "fa-turn-down-right"){
	  label = _U("\ue455"); }

	else if(iconName == "fa-bottle-droplet"){
	  label = _U("\ue4c4"); }

	else if(iconName == "fa-mask-face"){
	  label = _U("\ue1d7"); }

	else if(iconName == "fa-hill-rockslide"){
	  label = _U("\ue508"); }

	else if(iconName == "fa-scanner-keyboard"){
	  label = _U("\uf489"); }

	else if(iconName == "fa-circle-o"){
	  label = _U("\ue119"); }

	else if(iconName == "fa-grid-horizontal"){
	  label = _U("\ue307"); }

	else if(iconName == "fa-message-dollar"){
	  label = _U("\uf650"); }

	else if(iconName == "fa-comment-alt-dollar"){
	  label = _U("\uf650"); }

	else if(iconName == "fa-right-left"){
	  label = _U("\uf362"); }

	else if(iconName == "fa-exchange-alt"){
	  label = _U("\uf362"); }

	else if(iconName == "fa-columns-3"){
	  label = _U("\ue361"); }

	else if(iconName == "fa-paper-plane"){
	  label = _U("\uf1d8"); }

	else if(iconName == "fa-road-circle-exclamation"){
	  label = _U("\ue565"); }

	else if(iconName == "fa-dungeon"){
	  label = _U("\uf6d9"); }

	else if(iconName == "fa-hand-holding-box"){
	  label = _U("\uf47b"); }

	else if(iconName == "fa-input-text"){
	  label = _U("\ue1bf"); }

	else if(iconName == "fa-window-flip"){
	  label = _U("\uf40f"); }

	else if(iconName == "fa-window-alt"){
	  label = _U("\uf40f"); }

	else if(iconName == "fa-align-right"){
	  label = _U("\uf038"); }

	else if(iconName == "fa-scanner-gun"){
	  label = _U("\uf488"); }

	else if(iconName == "fa-scanner"){
	  label = _U("\uf488"); }

	else if(iconName == "fa-tire"){
	  label = _U("\uf631"); }

	else if(iconName == "fa-engine"){
	  label = _U("\ue16e"); }

	else if(iconName == "fa-money-bill-1-wave"){
	  label = _U("\uf53b"); }

	else if(iconName == "fa-money-bill-wave-alt"){
	  label = _U("\uf53b"); }

	else if(iconName == "fa-life-ring"){
	  label = _U("\uf1cd"); }

	else if(iconName == "fa-hands"){
	  label = _U("\uf2a7"); }

	else if(iconName == "fa-sign-language"){
	  label = _U("\uf2a7"); }

	else if(iconName == "fa-signing"){
	  label = _U("\uf2a7"); }

	else if(iconName == "fa-circle-caret-right"){
	  label = _U("\uf330"); }

	else if(iconName == "fa-caret-circle-right"){
	  label = _U("\uf330"); }

	else if(iconName == "fa-turn-left"){
	  label = _U("\ue636"); }

	else if(iconName == "fa-wheat"){
	  label = _U("\uf72d"); }

	else if(iconName == "fa-file-spreadsheet"){
	  label = _U("\uf65b"); }

	else if(iconName == "fa-audio-description-slash"){
	  label = _U("\ue0a8"); }

	else if(iconName == "fa-bell-ring"){
	  label = _U("\ue62c"); }

	else if(iconName == "fa-calendar-day"){
	  label = _U("\uf783"); }

	else if(iconName == "fa-water-ladder"){
	  label = _U("\uf5c5"); }

	else if(iconName == "fa-ladder-water"){
	  label = _U("\uf5c5"); }

	else if(iconName == "fa-swimming-pool"){
	  label = _U("\uf5c5"); }

	else if(iconName == "fa-arrows-up-down"){
	  label = _U("\uf07d"); }

	else if(iconName == "fa-arrows-v"){
	  label = _U("\uf07d"); }

	else if(iconName == "fa-chess-pawn-piece"){
	  label = _U("\uf444"); }

	else if(iconName == "fa-chess-pawn-alt"){
	  label = _U("\uf444"); }

	else if(iconName == "fa-face-grimace"){
	  label = _U("\uf57f"); }

	else if(iconName == "fa-grimace"){
	  label = _U("\uf57f"); }

	else if(iconName == "fa-wheelchair-move"){
	  label = _U("\ue2ce"); }

	else if(iconName == "fa-wheelchair-alt"){
	  label = _U("\ue2ce"); }

	else if(iconName == "fa-turn-down"){
	  label = _U("\uf3be"); }

	else if(iconName == "fa-level-down-alt"){
	  label = _U("\uf3be"); }

	else if(iconName == "fa-square-s"){
	  label = _U("\ue27d"); }

	else if(iconName == "fa-rectangle-barcode"){
	  label = _U("\uf463"); }

	else if(iconName == "fa-barcode-alt"){
	  label = _U("\uf463"); }

	else if(iconName == "fa-person-walking-arrow-right"){
	  label = _U("\ue552"); }

	else if(iconName == "fa-square-envelope"){
	  label = _U("\uf199"); }

	else if(iconName == "fa-envelope-square"){
	  label = _U("\uf199"); }

	else if(iconName == "fa-dice"){
	  label = _U("\uf522"); }

	else if(iconName == "fa-unicorn"){
	  label = _U("\uf727"); }

	else if(iconName == "fa-bowling-ball"){
	  label = _U("\uf436"); }

	else if(iconName == "fa-pompebled"){
	  label = _U("\ue43d"); }

	else if(iconName == "fa-brain"){
	  label = _U("\uf5dc"); }

	else if(iconName == "fa-watch-smart"){
	  label = _U("\ue2cc"); }

	else if(iconName == "fa-book-user"){
	  label = _U("\uf7e7"); }

	else if(iconName == "fa-sensor-cloud"){
	  label = _U("\ue02c"); }

	else if(iconName == "fa-sensor-smoke"){
	  label = _U("\ue02c"); }

	else if(iconName == "fa-clapperboard-play"){
	  label = _U("\ue132"); }

	else if(iconName == "fa-bandage"){
	  label = _U("\uf462"); }

	else if(iconName == "fa-band-aid"){
	  label = _U("\uf462"); }

	else if(iconName == "fa-calendar-minus"){
	  label = _U("\uf272"); }

	else if(iconName == "fa-circle-xmark"){
	  label = _U("\uf057"); }

	else if(iconName == "fa-times-circle"){
	  label = _U("\uf057"); }

	else if(iconName == "fa-xmark-circle"){
	  label = _U("\uf057"); }

	else if(iconName == "fa-circle-4"){
	  label = _U("\ue0f1"); }

	else if(iconName == "fa-gifts"){
	  label = _U("\uf79c"); }

	else if(iconName == "fa-album-collection"){
	  label = _U("\uf8a0"); }

	else if(iconName == "fa-hotel"){
	  label = _U("\uf594"); }

	else if(iconName == "fa-earth-asia"){
	  label = _U("\uf57e"); }

	else if(iconName == "fa-globe-asia"){
	  label = _U("\uf57e"); }

	else if(iconName == "fa-id-card-clip"){
	  label = _U("\uf47f"); }

	else if(iconName == "fa-id-card-alt"){
	  label = _U("\uf47f"); }

	else if(iconName == "fa-magnifying-glass-plus"){
	  label = _U("\uf00e"); }

	else if(iconName == "fa-search-plus"){
	  label = _U("\uf00e"); }

	else if(iconName == "fa-thumbs-up"){
	  label = _U("\uf164"); }

	else if(iconName == "fa-cloud-showers"){
	  label = _U("\uf73f"); }

	else if(iconName == "fa-user-clock"){
	  label = _U("\uf4fd"); }

	else if(iconName == "fa-onion"){
	  label = _U("\ue427"); }

	else if(iconName == "fa-clock-twelve-thirty"){
	  label = _U("\ue359"); }

	else if(iconName == "fa-arrow-down-to-dotted-line"){
	  label = _U("\ue095"); }

	else if(iconName == "fa-hand-dots"){
	  label = _U("\uf461"); }

	else if(iconName == "fa-allergies"){
	  label = _U("\uf461"); }

	else if(iconName == "fa-file-invoice"){
	  label = _U("\uf570"); }

	else if(iconName == "fa-window-minimize"){
	  label = _U("\uf2d1"); }

	else if(iconName == "fa-rectangle-wide"){
	  label = _U("\uf2fc"); }

	else if(iconName == "fa-comment-arrow-up"){
	  label = _U("\ue144"); }

	else if(iconName == "fa-garlic"){
	  label = _U("\ue40e"); }

	else if(iconName == "fa-mug-saucer"){
	  label = _U("\uf0f4"); }

	else if(iconName == "fa-coffee"){
	  label = _U("\uf0f4"); }

	else if(iconName == "fa-brush"){
	  label = _U("\uf55d"); }

	else if(iconName == "fa-tree-decorated"){
	  label = _U("\uf7dc"); }

	else if(iconName == "fa-mask"){
	  label = _U("\uf6fa"); }

	else if(iconName == "fa-calendar-heart"){
	  label = _U("\ue0d3"); }

	else if(iconName == "fa-magnifying-glass-minus"){
	  label = _U("\uf010"); }

	else if(iconName == "fa-search-minus"){
	  label = _U("\uf010"); }

	else if(iconName == "fa-flower"){
	  label = _U("\uf7ff"); }

	else if(iconName == "fa-arrow-down-from-arc"){
	  label = _U("\ue614"); }

	else if(iconName == "fa-right-left-large"){
	  label = _U("\ue5e1"); }

	else if(iconName == "fa-ruler-vertical"){
	  label = _U("\uf548"); }

	else if(iconName == "fa-circles-overlap"){
	  label = _U("\ue600"); }

	else if(iconName == "fa-user-large"){
	  label = _U("\uf406"); }

	else if(iconName == "fa-user-alt"){
	  label = _U("\uf406"); }

	else if(iconName == "fa-starship-freighter"){
	  label = _U("\ue03a"); }

	else if(iconName == "fa-train-tram"){
	  label = _U("\ue5b4"); }

	else if(iconName == "fa-bridge-suspension"){
	  label = _U("\ue4cd"); }

	else if(iconName == "fa-trash-check"){
	  label = _U("\ue2af"); }

	else if(iconName == "fa-user-nurse"){
	  label = _U("\uf82f"); }

	else if(iconName == "fa-boombox"){
	  label = _U("\uf8a5"); }

	else if(iconName == "fa-syringe"){
	  label = _U("\uf48e"); }

	else if(iconName == "fa-cloud-sun"){
	  label = _U("\uf6c4"); }

	else if(iconName == "fa-shield-exclamation"){
	  label = _U("\ue247"); }

	else if(iconName == "fa-stopwatch-20"){
	  label = _U("\ue06f"); }

	else if(iconName == "fa-square-full"){
	  label = _U("\uf45c"); }

	else if(iconName == "fa-grip-dots"){
	  label = _U("\ue410"); }

	else if(iconName == "fa-comment-exclamation"){
	  label = _U("\uf4af"); }

	else if(iconName == "fa-pen-swirl"){
	  label = _U("\ue214"); }

	else if(iconName == "fa-falafel"){
	  label = _U("\ue40a"); }

	else if(iconName == "fa-circle-2"){
	  label = _U("\ue0ef"); }

	else if(iconName == "fa-magnet"){
	  label = _U("\uf076"); }

	else if(iconName == "fa-jar"){
	  label = _U("\ue516"); }

	else if(iconName == "fa-gramophone"){
	  label = _U("\uf8bd"); }

	else if(iconName == "fa-dice-d12"){
	  label = _U("\uf6ce"); }

	else if(iconName == "fa-note-sticky"){
	  label = _U("\uf249"); }

	else if(iconName == "fa-sticky-note"){
	  label = _U("\uf249"); }

	else if(iconName == "fa-down"){
	  label = _U("\uf354"); }

	else if(iconName == "fa-arrow-alt-down"){
	  label = _U("\uf354"); }

	else if(iconName == "fa-hundred-points"){
	  label = _U("\ue41c"); }

	else if(iconName == "fa-100"){
	  label = _U("\ue41c"); }

	else if(iconName == "fa-paperclip-vertical"){
	  label = _U("\ue3c2"); }

	else if(iconName == "fa-wind-warning"){
	  label = _U("\uf776"); }

	else if(iconName == "fa-wind-circle-exclamation"){
	  label = _U("\uf776"); }

	else if(iconName == "fa-location-pin-slash"){
	  label = _U("\uf60c"); }

	else if(iconName == "fa-map-marker-slash"){
	  label = _U("\uf60c"); }

	else if(iconName == "fa-face-sad-sweat"){
	  label = _U("\ue38a"); }

	else if(iconName == "fa-bug-slash"){
	  label = _U("\ue490"); }

	else if(iconName == "fa-cupcake"){
	  label = _U("\ue402"); }

	else if(iconName == "fa-light-switch-off"){
	  label = _U("\ue018"); }

	else if(iconName == "fa-toggle-large-off"){
	  label = _U("\ue5b0"); }

	else if(iconName == "fa-pen-fancy-slash"){
	  label = _U("\ue210"); }

	else if(iconName == "fa-truck-container"){
	  label = _U("\uf4dc"); }

	else if(iconName == "fa-boot"){
	  label = _U("\uf782"); }

	else if(iconName == "fa-arrow-up-from-water-pump"){
	  label = _U("\ue4b6"); }

	else if(iconName == "fa-file-check"){
	  label = _U("\uf316"); }

	else if(iconName == "fa-bone"){
	  label = _U("\uf5d7"); }

	else if(iconName == "fa-cards-blank"){
	  label = _U("\ue4df"); }

	else if(iconName == "fa-circle-3"){
	  label = _U("\ue0f0"); }

	else if(iconName == "fa-bench-tree"){
	  label = _U("\ue2e7"); }

	else if(iconName == "fa-keyboard-brightness-low"){
	  label = _U("\ue1c1"); }

	else if(iconName == "fa-ski-boot-ski"){
	  label = _U("\ue3cd"); }

	else if(iconName == "fa-brain-circuit"){
	  label = _U("\ue0c6"); }

	else if(iconName == "fa-user-injured"){
	  label = _U("\uf728"); }

	else if(iconName == "fa-block-brick-fire"){
	  label = _U("\ue3dc"); }

	else if(iconName == "fa-firewall"){
	  label = _U("\ue3dc"); }

	else if(iconName == "fa-face-sad-tear"){
	  label = _U("\uf5b4"); }

	else if(iconName == "fa-sad-tear"){
	  label = _U("\uf5b4"); }

	else if(iconName == "fa-plane"){
	  label = _U("\uf072"); }

	else if(iconName == "fa-tent-arrows-down"){
	  label = _U("\ue581"); }

	else if(iconName == "fa-exclamation"){
	  label = _U("\u0021"); }

	else if(iconName == "fa-arrows-spin"){
	  label = _U("\ue4bb"); }

	else if(iconName == "fa-face-smile-relaxed"){
	  label = _U("\ue392"); }

	else if(iconName == "fa-comment-xmark"){
	  label = _U("\uf4b5"); }

	else if(iconName == "fa-comment-times"){
	  label = _U("\uf4b5"); }

	else if(iconName == "fa-print"){
	  label = _U("\uf02f"); }

	else if(iconName == "fa-turkish-lira-sign"){
	  label = _U("\ue2bb"); }

	else if(iconName == "fa-try"){
	  label = _U("\ue2bb"); }

	else if(iconName == "fa-turkish-lira"){
	  label = _U("\ue2bb"); }

	else if(iconName == "fa-face-nose-steam"){
	  label = _U("\ue382"); }

	else if(iconName == "fa-circle-waveform-lines"){
	  label = _U("\ue12d"); }

	else if(iconName == "fa-waveform-circle"){
	  label = _U("\ue12d"); }

	else if(iconName == "fa-dollar-sign"){
	  label = _U("\u0024"); }

	else if(iconName == "fa-dollar"){
	  label = _U("\u0024"); }

	else if(iconName == "fa-usd"){
	  label = _U("\u0024"); }

	else if(iconName == "fa-ferris-wheel"){
	  label = _U("\ue174"); }

	else if(iconName == "fa-computer-speaker"){
	  label = _U("\uf8b2"); }

	else if(iconName == "fa-skull-cow"){
	  label = _U("\uf8de"); }

	else if(iconName == "fa-x"){
	  label = _U("\u0058"); }

	else if(iconName == "fa-magnifying-glass-dollar"){
	  label = _U("\uf688"); }

	else if(iconName == "fa-search-dollar"){
	  label = _U("\uf688"); }

	else if(iconName == "fa-users-gear"){
	  label = _U("\uf509"); }

	else if(iconName == "fa-users-cog"){
	  label = _U("\uf509"); }

	else if(iconName == "fa-person-military-pointing"){
	  label = _U("\ue54a"); }

	else if(iconName == "fa-building-columns"){
	  label = _U("\uf19c"); }

	else if(iconName == "fa-bank"){
	  label = _U("\uf19c"); }

	else if(iconName == "fa-institution"){
	  label = _U("\uf19c"); }

	else if(iconName == "fa-museum"){
	  label = _U("\uf19c"); }

	else if(iconName == "fa-university"){
	  label = _U("\uf19c"); }

	else if(iconName == "fa-circle-t"){
	  label = _U("\ue124"); }

	else if(iconName == "fa-sack"){
	  label = _U("\uf81c"); }

	else if(iconName == "fa-grid-2"){
	  label = _U("\ue196"); }

	else if(iconName == "fa-camera-cctv"){
	  label = _U("\uf8ac"); }

	else if(iconName == "fa-cctv"){
	  label = _U("\uf8ac"); }

	else if(iconName == "fa-umbrella"){
	  label = _U("\uf0e9"); }

	else if(iconName == "fa-trowel"){
	  label = _U("\ue589"); }

	else if(iconName == "fa-horizontal-rule"){
	  label = _U("\uf86c"); }

	else if(iconName == "fa-bed-front"){
	  label = _U("\uf8f7"); }

	else if(iconName == "fa-bed-alt"){
	  label = _U("\uf8f7"); }

	else if(iconName == "fa-d"){
	  label = _U("\u0044"); }

	else if(iconName == "fa-stapler"){
	  label = _U("\ue5af"); }

	else if(iconName == "fa-masks-theater"){
	  label = _U("\uf630"); }

	else if(iconName == "fa-theater-masks"){
	  label = _U("\uf630"); }

	else if(iconName == "fa-file-gif"){
	  label = _U("\ue645"); }

	else if(iconName == "fa-kip-sign"){
	  label = _U("\ue1c4"); }

	else if(iconName == "fa-face-woozy"){
	  label = _U("\ue3a2"); }

	else if(iconName == "fa-cloud-question"){
	  label = _U("\ue492"); }

	else if(iconName == "fa-pineapple"){
	  label = _U("\ue31f"); }

	else if(iconName == "fa-hand-point-left"){
	  label = _U("\uf0a5"); }

	else if(iconName == "fa-gallery-thumbnails"){
	  label = _U("\ue3aa"); }

	else if(iconName == "fa-circle-j"){
	  label = _U("\ue112"); }

	else if(iconName == "fa-eyes"){
	  label = _U("\ue367"); }

	else if(iconName == "fa-handshake-simple"){
	  label = _U("\uf4c6"); }

	else if(iconName == "fa-handshake-alt"){
	  label = _U("\uf4c6"); }

	else if(iconName == "fa-page-caret-up"){
	  label = _U("\ue42a"); }

	else if(iconName == "fa-file-caret-up"){
	  label = _U("\ue42a"); }

	else if(iconName == "fa-jet-fighter"){
	  label = _U("\uf0fb"); }

	else if(iconName == "fa-fighter-jet"){
	  label = _U("\uf0fb"); }

	else if(iconName == "fa-comet"){
	  label = _U("\ue003"); }

	else if(iconName == "fa-square-share-nodes"){
	  label = _U("\uf1e1"); }

	else if(iconName == "fa-share-alt-square"){
	  label = _U("\uf1e1"); }

	else if(iconName == "fa-reflect-vertical"){
	  label = _U("\ue665"); }

	else if(iconName == "fa-shield-keyhole"){
	  label = _U("\ue248"); }

	else if(iconName == "fa-file-mp4"){
	  label = _U("\ue649"); }

	else if(iconName == "fa-barcode"){
	  label = _U("\uf02a"); }

	else if(iconName == "fa-bulldozer"){
	  label = _U("\ue655"); }

	else if(iconName == "fa-plus-minus"){
	  label = _U("\ue43c"); }

	else if(iconName == "fa-square-sliders-vertical"){
	  label = _U("\uf3f2"); }

	else if(iconName == "fa-sliders-v-square"){
	  label = _U("\uf3f2"); }

	else if(iconName == "fa-video"){
	  label = _U("\uf03d"); }

	else if(iconName == "fa-video-camera"){
	  label = _U("\uf03d"); }

	else if(iconName == "fa-message-middle"){
	  label = _U("\ue1e1"); }

	else if(iconName == "fa-comment-middle-alt"){
	  label = _U("\ue1e1"); }

	else if(iconName == "fa-graduation-cap"){
	  label = _U("\uf19d"); }

	else if(iconName == "fa-mortar-board"){
	  label = _U("\uf19d"); }

	else if(iconName == "fa-hand-holding-medical"){
	  label = _U("\ue05c"); }

	else if(iconName == "fa-person-circle-check"){
	  label = _U("\ue53e"); }

	else if(iconName == "fa-square-z"){
	  label = _U("\ue288"); }

	else if(iconName == "fa-message-text"){
	  label = _U("\ue1e6"); }

	else if(iconName == "fa-comment-alt-text"){
	  label = _U("\ue1e6"); }

	else if(iconName == "fa-turn-up"){
	  label = _U("\uf3bf"); }

	else if(iconName == "fa-level-up-alt"){
	  label = _U("\uf3bf"); }

	else if(iconName == "fa-monero") {
	  label = _U("\uf3d0"); }

	else if(iconName == "fa-hooli") {
	  label = _U("\uf427"); }

	else if(iconName == "fa-yelp") {
	  label = _U("\uf1e9"); }

	else if(iconName == "fa-cc-visa") {
	  label = _U("\uf1f0"); }

	else if(iconName == "fa-lastfm") {
	  label = _U("\uf202"); }

	else if(iconName == "fa-shopware") {
	  label = _U("\uf5b5"); }

	else if(iconName == "fa-creative-commons-nc") {
	  label = _U("\uf4e8"); }

	else if(iconName == "fa-aws") {
	  label = _U("\uf375"); }

	else if(iconName == "fa-redhat") {
	  label = _U("\uf7bc"); }

	else if(iconName == "fa-yoast") {
	  label = _U("\uf2b1"); }

	else if(iconName == "fa-cloudflare") {
	  label = _U("\ue07d"); }

	else if(iconName == "fa-ups") {
	  label = _U("\uf7e0"); }

	else if(iconName == "fa-pixiv") {
	  label = _U("\ue640"); }

	else if(iconName == "fa-wpexplorer") {
	  label = _U("\uf2de"); }

	else if(iconName == "fa-dyalog") {
	  label = _U("\uf399"); }

	else if(iconName == "fa-bity") {
	  label = _U("\uf37a"); }

	else if(iconName == "fa-stackpath") {
	  label = _U("\uf842"); }

	else if(iconName == "fa-buysellads") {
	  label = _U("\uf20d"); }

	else if(iconName == "fa-first-order") {
	  label = _U("\uf2b0"); }

	else if(iconName == "fa-modx") {
	  label = _U("\uf285"); }

	else if(iconName == "fa-guilded") {
	  label = _U("\ue07e"); }

	else if(iconName == "fa-vnv") {
	  label = _U("\uf40b"); }

	else if(iconName == "fa-square-js") {
	  label = _U("\uf3b9"); }

	else if(iconName == "fa-js-square") {
	  label = _U("\uf3b9"); }

	else if(iconName == "fa-microsoft") {
	  label = _U("\uf3ca"); }

	else if(iconName == "fa-qq") {
	  label = _U("\uf1d6"); }

	else if(iconName == "fa-orcid") {
	  label = _U("\uf8d2"); }

	else if(iconName == "fa-java") {
	  label = _U("\uf4e4"); }

	else if(iconName == "fa-invision") {
	  label = _U("\uf7b0"); }

	else if(iconName == "fa-creative-commons-pd-alt") {
	  label = _U("\uf4ed"); }

	else if(iconName == "fa-centercode") {
	  label = _U("\uf380"); }

	else if(iconName == "fa-glide-g") {
	  label = _U("\uf2a6"); }

	else if(iconName == "fa-drupal") {
	  label = _U("\uf1a9"); }

	else if(iconName == "fa-hire-a-helper") {
	  label = _U("\uf3b0"); }

	else if(iconName == "fa-creative-commons-by") {
	  label = _U("\uf4e7"); }

	else if(iconName == "fa-unity") {
	  label = _U("\ue049"); }

	else if(iconName == "fa-whmcs") {
	  label = _U("\uf40d"); }

	else if(iconName == "fa-rocketchat") {
	  label = _U("\uf3e8"); }

	else if(iconName == "fa-vk") {
	  label = _U("\uf189"); }

	else if(iconName == "fa-untappd") {
	  label = _U("\uf405"); }

	else if(iconName == "fa-mailchimp") {
	  label = _U("\uf59e"); }

	else if(iconName == "fa-css3-alt") {
	  label = _U("\uf38b"); }

	else if(iconName == "fa-square-reddit") {
	  label = _U("\uf1a2"); }

	else if(iconName == "fa-reddit-square") {
	  label = _U("\uf1a2"); }

	else if(iconName == "fa-vimeo-v") {
	  label = _U("\uf27d"); }

	else if(iconName == "fa-contao") {
	  label = _U("\uf26d"); }

	else if(iconName == "fa-square-font-awesome") {
	  label = _U("\ue5ad"); }

	else if(iconName == "fa-deskpro") {
	  label = _U("\uf38f"); }

	else if(iconName == "fa-brave") {
	  label = _U("\ue63c"); }

	else if(iconName == "fa-sistrix") {
	  label = _U("\uf3ee"); }

	else if(iconName == "fa-square-instagram") {
	  label = _U("\ue055"); }

	else if(iconName == "fa-instagram-square") {
	  label = _U("\ue055"); }

	else if(iconName == "fa-battle-net") {
	  label = _U("\uf835"); }

	else if(iconName == "fa-the-red-yeti") {
	  label = _U("\uf69d"); }

	else if(iconName == "fa-square-hacker-news") {
	  label = _U("\uf3af"); }

	else if(iconName == "fa-hacker-news-square") {
	  label = _U("\uf3af"); }

	else if(iconName == "fa-edge") {
	  label = _U("\uf282"); }

	else if(iconName == "fa-threads") {
	  label = _U("\ue618"); }

	else if(iconName == "fa-napster") {
	  label = _U("\uf3d2"); }

	else if(iconName == "fa-square-snapchat") {
	  label = _U("\uf2ad"); }

	else if(iconName == "fa-snapchat-square") {
	  label = _U("\uf2ad"); }

	else if(iconName == "fa-google-plus-g") {
	  label = _U("\uf0d5"); }

	else if(iconName == "fa-artstation") {
	  label = _U("\uf77a"); }

	else if(iconName == "fa-markdown") {
	  label = _U("\uf60f"); }

	else if(iconName == "fa-sourcetree") {
	  label = _U("\uf7d3"); }

	else if(iconName == "fa-google-plus") {
	  label = _U("\uf2b3"); }

	else if(iconName == "fa-diaspora") {
	  label = _U("\uf791"); }

	else if(iconName == "fa-foursquare") {
	  label = _U("\uf180"); }

	else if(iconName == "fa-stack-overflow") {
	  label = _U("\uf16c"); }

	else if(iconName == "fa-github-alt") {
	  label = _U("\uf113"); }

	else if(iconName == "fa-phoenix-squadron") {
	  label = _U("\uf511"); }

	else if(iconName == "fa-pagelines") {
	  label = _U("\uf18c"); }

	else if(iconName == "fa-algolia") {
	  label = _U("\uf36c"); }

	else if(iconName == "fa-red-river") {
	  label = _U("\uf3e3"); }

	else if(iconName == "fa-creative-commons-sa") {
	  label = _U("\uf4ef"); }

	else if(iconName == "fa-safari") {
	  label = _U("\uf267"); }

	else if(iconName == "fa-google") {
	  label = _U("\uf1a0"); }

	else if(iconName == "fa-square-font-awesome-stroke") {
	  label = _U("\uf35c"); }

	else if(iconName == "fa-font-awesome-alt") {
	  label = _U("\uf35c"); }

	else if(iconName == "fa-atlassian") {
	  label = _U("\uf77b"); }

	else if(iconName == "fa-linkedin-in") {
	  label = _U("\uf0e1"); }

	else if(iconName == "fa-digital-ocean") {
	  label = _U("\uf391"); }

	else if(iconName == "fa-nimblr") {
	  label = _U("\uf5a8"); }

	else if(iconName == "fa-chromecast") {
	  label = _U("\uf838"); }

	else if(iconName == "fa-evernote") {
	  label = _U("\uf839"); }

	else if(iconName == "fa-hacker-news") {
	  label = _U("\uf1d4"); }

	else if(iconName == "fa-creative-commons-sampling") {
	  label = _U("\uf4f0"); }

	else if(iconName == "fa-adversal") {
	  label = _U("\uf36a"); }

	else if(iconName == "fa-creative-commons") {
	  label = _U("\uf25e"); }

	else if(iconName == "fa-watchman-monitoring") {
	  label = _U("\ue087"); }

	else if(iconName == "fa-fonticons") {
	  label = _U("\uf280"); }

	else if(iconName == "fa-weixin") {
	  label = _U("\uf1d7"); }

	else if(iconName == "fa-shirtsinbulk") {
	  label = _U("\uf214"); }

	else if(iconName == "fa-codepen") {
	  label = _U("\uf1cb"); }

	else if(iconName == "fa-git-alt") {
	  label = _U("\uf841"); }

	else if(iconName == "fa-lyft") {
	  label = _U("\uf3c3"); }

	else if(iconName == "fa-rev") {
	  label = _U("\uf5b2"); }

	else if(iconName == "fa-windows") {
	  label = _U("\uf17a"); }

	else if(iconName == "fa-wizards-of-the-coast") {
	  label = _U("\uf730"); }

	else if(iconName == "fa-square-viadeo") {
	  label = _U("\uf2aa"); }

	else if(iconName == "fa-viadeo-square") {
	  label = _U("\uf2aa"); }

	else if(iconName == "fa-meetup") {
	  label = _U("\uf2e0"); }

	else if(iconName == "fa-centos") {
	  label = _U("\uf789"); }

	else if(iconName == "fa-adn") {
	  label = _U("\uf170"); }

	else if(iconName == "fa-cloudsmith") {
	  label = _U("\uf384"); }

	else if(iconName == "fa-opensuse") {
	  label = _U("\ue62b"); }

	else if(iconName == "fa-pied-piper-alt") {
	  label = _U("\uf1a8"); }

	else if(iconName == "fa-square-dribbble") {
	  label = _U("\uf397"); }

	else if(iconName == "fa-dribbble-square") {
	  label = _U("\uf397"); }

	else if(iconName == "fa-codiepie") {
	  label = _U("\uf284"); }

	else if(iconName == "fa-node") {
	  label = _U("\uf419"); }

	else if(iconName == "fa-mix") {
	  label = _U("\uf3cb"); }

	else if(iconName == "fa-steam") {
	  label = _U("\uf1b6"); }

	else if(iconName == "fa-cc-apple-pay") {
	  label = _U("\uf416"); }

	else if(iconName == "fa-scribd") {
	  label = _U("\uf28a"); }

	else if(iconName == "fa-debian") {
	  label = _U("\ue60b"); }

	else if(iconName == "fa-openid") {
	  label = _U("\uf19b"); }

	else if(iconName == "fa-instalod") {
	  label = _U("\ue081"); }

	else if(iconName == "fa-expeditedssl") {
	  label = _U("\uf23e"); }

	else if(iconName == "fa-sellcast") {
	  label = _U("\uf2da"); }

	else if(iconName == "fa-square-twitter") {
	  label = _U("\uf081"); }

	else if(iconName == "fa-twitter-square") {
	  label = _U("\uf081"); }

	else if(iconName == "fa-r-project") {
	  label = _U("\uf4f7"); }

	else if(iconName == "fa-delicious") {
	  label = _U("\uf1a5"); }

	else if(iconName == "fa-freebsd") {
	  label = _U("\uf3a4"); }

	else if(iconName == "fa-vuejs") {
	  label = _U("\uf41f"); }

	else if(iconName == "fa-accusoft") {
	  label = _U("\uf369"); }

	else if(iconName == "fa-ioxhost") {
	  label = _U("\uf208"); }

	else if(iconName == "fa-fonticons-fi") {
	  label = _U("\uf3a2"); }

	else if(iconName == "fa-app-store") {
	  label = _U("\uf36f"); }

	else if(iconName == "fa-cc-mastercard") {
	  label = _U("\uf1f1"); }

	else if(iconName == "fa-itunes-note") {
	  label = _U("\uf3b5"); }

	else if(iconName == "fa-golang") {
	  label = _U("\ue40f"); }

	else if(iconName == "fa-kickstarter") {
	  label = _U("\uf3bb"); }

	else if(iconName == "fa-grav") {
	  label = _U("\uf2d6"); }

	else if(iconName == "fa-weibo") {
	  label = _U("\uf18a"); }

	else if(iconName == "fa-uncharted") {
	  label = _U("\ue084"); }

	else if(iconName == "fa-firstdraft") {
	  label = _U("\uf3a1"); }

	else if(iconName == "fa-square-youtube") {
	  label = _U("\uf431"); }

	else if(iconName == "fa-youtube-square") {
	  label = _U("\uf431"); }

	else if(iconName == "fa-wikipedia-w") {
	  label = _U("\uf266"); }

	else if(iconName == "fa-wpressr") {
	  label = _U("\uf3e4"); }

	else if(iconName == "fa-rendact") {
	  label = _U("\uf3e4"); }

	else if(iconName == "fa-angellist") {
	  label = _U("\uf209"); }

	else if(iconName == "fa-galactic-republic") {
	  label = _U("\uf50c"); }

	else if(iconName == "fa-nfc-directional") {
	  label = _U("\ue530"); }

	else if(iconName == "fa-skype") {
	  label = _U("\uf17e"); }

	else if(iconName == "fa-joget") {
	  label = _U("\uf3b7"); }

	else if(iconName == "fa-fedora") {
	  label = _U("\uf798"); }

	else if(iconName == "fa-stripe-s") {
	  label = _U("\uf42a"); }

	else if(iconName == "fa-meta") {
	  label = _U("\ue49b"); }

	else if(iconName == "fa-laravel") {
	  label = _U("\uf3bd"); }

	else if(iconName == "fa-hotjar") {
	  label = _U("\uf3b1"); }

	else if(iconName == "fa-bluetooth-b") {
	  label = _U("\uf294"); }

	else if(iconName == "fa-square-letterboxd") {
	  label = _U("\ue62e"); }

	else if(iconName == "fa-sticker-mule") {
	  label = _U("\uf3f7"); }

	else if(iconName == "fa-creative-commons-zero") {
	  label = _U("\uf4f3"); }

	else if(iconName == "fa-hips") {
	  label = _U("\uf452"); }

	else if(iconName == "fa-behance") {
	  label = _U("\uf1b4"); }

	else if(iconName == "fa-reddit") {
	  label = _U("\uf1a1"); }

	else if(iconName == "fa-discord") {
	  label = _U("\uf392"); }

	else if(iconName == "fa-chrome") {
	  label = _U("\uf268"); }

	else if(iconName == "fa-app-store-ios") {
	  label = _U("\uf370"); }

	else if(iconName == "fa-cc-discover") {
	  label = _U("\uf1f2"); }

	else if(iconName == "fa-wpbeginner") {
	  label = _U("\uf297"); }

	else if(iconName == "fa-confluence") {
	  label = _U("\uf78d"); }

	else if(iconName == "fa-shoelace") {
	  label = _U("\ue60c"); }

	else if(iconName == "fa-mdb") {
	  label = _U("\uf8ca"); }

	else if(iconName == "fa-dochub") {
	  label = _U("\uf394"); }

	else if(iconName == "fa-accessible-icon") {
	  label = _U("\uf368"); }

	else if(iconName == "fa-ebay") {
	  label = _U("\uf4f4"); }

	else if(iconName == "fa-amazon") {
	  label = _U("\uf270"); }

	else if(iconName == "fa-unsplash") {
	  label = _U("\ue07c"); }

	else if(iconName == "fa-yarn") {
	  label = _U("\uf7e3"); }

	else if(iconName == "fa-square-steam") {
	  label = _U("\uf1b7"); }

	else if(iconName == "fa-steam-square") {
	  label = _U("\uf1b7"); }

	else if(iconName == "fa-500px") {
	  label = _U("\uf26e"); }

	else if(iconName == "fa-square-vimeo") {
	  label = _U("\uf194"); }

	else if(iconName == "fa-vimeo-square") {
	  label = _U("\uf194"); }

	else if(iconName == "fa-asymmetrik") {
	  label = _U("\uf372"); }

	else if(iconName == "fa-font-awesome") {
	  label = _U("\uf2b4"); }

	else if(iconName == "fa-font-awesome-flag") {
	  label = _U("\uf2b4"); }

	else if(iconName == "fa-font-awesome-logo-full") {
	  label = _U("\uf2b4"); }

	else if(iconName == "fa-gratipay") {
	  label = _U("\uf184"); }

	else if(iconName == "fa-apple") {
	  label = _U("\uf179"); }

	else if(iconName == "fa-hive") {
	  label = _U("\ue07f"); }

	else if(iconName == "fa-gitkraken") {
	  label = _U("\uf3a6"); }

	else if(iconName == "fa-keybase") {
	  label = _U("\uf4f5"); }

	else if(iconName == "fa-apple-pay") {
	  label = _U("\uf415"); }

	else if(iconName == "fa-padlet") {
	  label = _U("\ue4a0"); }

	else if(iconName == "fa-amazon-pay") {
	  label = _U("\uf42c"); }

	else if(iconName == "fa-square-github") {
	  label = _U("\uf092"); }

	else if(iconName == "fa-github-square") {
	  label = _U("\uf092"); }

	else if(iconName == "fa-stumbleupon") {
	  label = _U("\uf1a4"); }

	else if(iconName == "fa-fedex") {
	  label = _U("\uf797"); }

	else if(iconName == "fa-phoenix-framework") {
	  label = _U("\uf3dc"); }

	else if(iconName == "fa-shopify") {
	  label = _U("\ue057"); }

	else if(iconName == "fa-neos") {
	  label = _U("\uf612"); }

	else if(iconName == "fa-square-threads") {
	  label = _U("\ue619"); }

	else if(iconName == "fa-hackerrank") {
	  label = _U("\uf5f7"); }

	else if(iconName == "fa-researchgate") {
	  label = _U("\uf4f8"); }

	else if(iconName == "fa-swift") {
	  label = _U("\uf8e1"); }

	else if(iconName == "fa-angular") {
	  label = _U("\uf420"); }

	else if(iconName == "fa-speakap") {
	  label = _U("\uf3f3"); }

	else if(iconName == "fa-angrycreative") {
	  label = _U("\uf36e"); }

	else if(iconName == "fa-y-combinator") {
	  label = _U("\uf23b"); }

	else if(iconName == "fa-empire") {
	  label = _U("\uf1d1"); }

	else if(iconName == "fa-envira") {
	  label = _U("\uf299"); }

	else if(iconName == "fa-google-scholar") {
	  label = _U("\ue63b"); }

	else if(iconName == "fa-square-gitlab") {
	  label = _U("\ue5ae"); }

	else if(iconName == "fa-gitlab-square") {
	  label = _U("\ue5ae"); }

	else if(iconName == "fa-studiovinari") {
	  label = _U("\uf3f8"); }

	else if(iconName == "fa-pied-piper") {
	  label = _U("\uf2ae"); }

	else if(iconName == "fa-wordpress") {
	  label = _U("\uf19a"); }

	else if(iconName == "fa-product-hunt") {
	  label = _U("\uf288"); }

	else if(iconName == "fa-firefox") {
	  label = _U("\uf269"); }

	else if(iconName == "fa-linode") {
	  label = _U("\uf2b8"); }

	else if(iconName == "fa-goodreads") {
	  label = _U("\uf3a8"); }

	else if(iconName == "fa-square-odnoklassniki") {
	  label = _U("\uf264"); }

	else if(iconName == "fa-odnoklassniki-square") {
	  label = _U("\uf264"); }

	else if(iconName == "fa-jsfiddle") {
	  label = _U("\uf1cc"); }

	else if(iconName == "fa-sith") {
	  label = _U("\uf512"); }

	else if(iconName == "fa-themeisle") {
	  label = _U("\uf2b2"); }

	else if(iconName == "fa-page4") {
	  label = _U("\uf3d7"); }

	else if(iconName == "fa-hashnode") {
	  label = _U("\ue499"); }

	else if(iconName == "fa-react") {
	  label = _U("\uf41b"); }

	else if(iconName == "fa-cc-paypal") {
	  label = _U("\uf1f4"); }

	else if(iconName == "fa-squarespace") {
	  label = _U("\uf5be"); }

	else if(iconName == "fa-cc-stripe") {
	  label = _U("\uf1f5"); }

	else if(iconName == "fa-creative-commons-share") {
	  label = _U("\uf4f2"); }

	else if(iconName == "fa-bitcoin") {
	  label = _U("\uf379"); }

	else if(iconName == "fa-keycdn") {
	  label = _U("\uf3ba"); }

	else if(iconName == "fa-opera") {
	  label = _U("\uf26a"); }

	else if(iconName == "fa-itch-io") {
	  label = _U("\uf83a"); }

	else if(iconName == "fa-umbraco") {
	  label = _U("\uf8e8"); }

	else if(iconName == "fa-galactic-senate") {
	  label = _U("\uf50d"); }

	else if(iconName == "fa-ubuntu") {
	  label = _U("\uf7df"); }

	else if(iconName == "fa-draft2digital") {
	  label = _U("\uf396"); }

	else if(iconName == "fa-stripe") {
	  label = _U("\uf429"); }

	else if(iconName == "fa-houzz") {
	  label = _U("\uf27c"); }

	else if(iconName == "fa-gg") {
	  label = _U("\uf260"); }

	else if(iconName == "fa-dhl") {
	  label = _U("\uf790"); }

	else if(iconName == "fa-square-pinterest") {
	  label = _U("\uf0d3"); }

	else if(iconName == "fa-pinterest-square") {
	  label = _U("\uf0d3"); }

	else if(iconName == "fa-xing") {
	  label = _U("\uf168"); }

	else if(iconName == "fa-blackberry") {
	  label = _U("\uf37b"); }

	else if(iconName == "fa-creative-commons-pd") {
	  label = _U("\uf4ec"); }

	else if(iconName == "fa-playstation") {
	  label = _U("\uf3df"); }

	else if(iconName == "fa-quinscape") {
	  label = _U("\uf459"); }

	else if(iconName == "fa-less") {
	  label = _U("\uf41d"); }

	else if(iconName == "fa-blogger-b") {
	  label = _U("\uf37d"); }

	else if(iconName == "fa-opencart") {
	  label = _U("\uf23d"); }

	else if(iconName == "fa-vine") {
	  label = _U("\uf1ca"); }

	else if(iconName == "fa-signal-messenger") {
	  label = _U("\ue663"); }

	else if(iconName == "fa-paypal") {
	  label = _U("\uf1ed"); }

	else if(iconName == "fa-gitlab") {
	  label = _U("\uf296"); }

	else if(iconName == "fa-typo3") {
	  label = _U("\uf42b"); }

	else if(iconName == "fa-reddit-alien") {
	  label = _U("\uf281"); }

	else if(iconName == "fa-yahoo") {
	  label = _U("\uf19e"); }

	else if(iconName == "fa-dailymotion") {
	  label = _U("\ue052"); }

	else if(iconName == "fa-affiliatetheme") {
	  label = _U("\uf36b"); }

	else if(iconName == "fa-pied-piper-pp") {
	  label = _U("\uf1a7"); }

	else if(iconName == "fa-bootstrap") {
	  label = _U("\uf836"); }

	else if(iconName == "fa-odnoklassniki") {
	  label = _U("\uf263"); }

	else if(iconName == "fa-nfc-symbol") {
	  label = _U("\ue531"); }

	else if(iconName == "fa-mintbit") {
	  label = _U("\ue62f"); }

	else if(iconName == "fa-ethereum") {
	  label = _U("\uf42e"); }

	else if(iconName == "fa-speaker-deck") {
	  label = _U("\uf83c"); }

	else if(iconName == "fa-creative-commons-nc-eu") {
	  label = _U("\uf4e9"); }

	else if(iconName == "fa-patreon") {
	  label = _U("\uf3d9"); }

	else if(iconName == "fa-avianex") {
	  label = _U("\uf374"); }

	else if(iconName == "fa-ello") {
	  label = _U("\uf5f1"); }

	else if(iconName == "fa-gofore") {
	  label = _U("\uf3a7"); }

	else if(iconName == "fa-bimobject") {
	  label = _U("\uf378"); }

	else if(iconName == "fa-brave-reverse") {
	  label = _U("\ue63d"); }

	else if(iconName == "fa-facebook-f") {
	  label = _U("\uf39e"); }

	else if(iconName == "fa-square-google-plus") {
	  label = _U("\uf0d4"); }

	else if(iconName == "fa-google-plus-square") {
	  label = _U("\uf0d4"); }

	else if(iconName == "fa-mandalorian") {
	  label = _U("\uf50f"); }

	else if(iconName == "fa-first-order-alt") {
	  label = _U("\uf50a"); }

	else if(iconName == "fa-osi") {
	  label = _U("\uf41a"); }

	else if(iconName == "fa-google-wallet") {
	  label = _U("\uf1ee"); }

	else if(iconName == "fa-d-and-d-beyond") {
	  label = _U("\uf6ca"); }

	else if(iconName == "fa-periscope") {
	  label = _U("\uf3da"); }

	else if(iconName == "fa-fulcrum") {
	  label = _U("\uf50b"); }

	else if(iconName == "fa-cloudscale") {
	  label = _U("\uf383"); }

	else if(iconName == "fa-forumbee") {
	  label = _U("\uf211"); }

	else if(iconName == "fa-mizuni") {
	  label = _U("\uf3cc"); }

	else if(iconName == "fa-schlix") {
	  label = _U("\uf3ea"); }

	else if(iconName == "fa-square-xing") {
	  label = _U("\uf169"); }

	else if(iconName == "fa-xing-square") {
	  label = _U("\uf169"); }

	else if(iconName == "fa-bandcamp") {
	  label = _U("\uf2d5"); }

	else if(iconName == "fa-wpforms") {
	  label = _U("\uf298"); }

	else if(iconName == "fa-cloudversify") {
	  label = _U("\uf385"); }

	else if(iconName == "fa-usps") {
	  label = _U("\uf7e1"); }

	else if(iconName == "fa-megaport") {
	  label = _U("\uf5a3"); }

	else if(iconName == "fa-magento") {
	  label = _U("\uf3c4"); }

	else if(iconName == "fa-spotify") {
	  label = _U("\uf1bc"); }

	else if(iconName == "fa-optin-monster") {
	  label = _U("\uf23c"); }

	else if(iconName == "fa-fly") {
	  label = _U("\uf417"); }

	else if(iconName == "fa-aviato") {
	  label = _U("\uf421"); }

	else if(iconName == "fa-itunes") {
	  label = _U("\uf3b4"); }

	else if(iconName == "fa-cuttlefish") {
	  label = _U("\uf38c"); }

	else if(iconName == "fa-blogger") {
	  label = _U("\uf37c"); }

	else if(iconName == "fa-flickr") {
	  label = _U("\uf16e"); }

	else if(iconName == "fa-viber") {
	  label = _U("\uf409"); }

	else if(iconName == "fa-soundcloud") {
	  label = _U("\uf1be"); }

	else if(iconName == "fa-digg") {
	  label = _U("\uf1a6"); }

	else if(iconName == "fa-tencent-weibo") {
	  label = _U("\uf1d5"); }

	else if(iconName == "fa-letterboxd") {
	  label = _U("\ue62d"); }

	else if(iconName == "fa-symfony") {
	  label = _U("\uf83d"); }

	else if(iconName == "fa-maxcdn") {
	  label = _U("\uf136"); }

	else if(iconName == "fa-etsy") {
	  label = _U("\uf2d7"); }

	else if(iconName == "fa-facebook-messenger") {
	  label = _U("\uf39f"); }

	else if(iconName == "fa-audible") {
	  label = _U("\uf373"); }

	else if(iconName == "fa-think-peaks") {
	  label = _U("\uf731"); }

	else if(iconName == "fa-bilibili") {
	  label = _U("\ue3d9"); }

	else if(iconName == "fa-erlang") {
	  label = _U("\uf39d"); }

	else if(iconName == "fa-x-twitter") {
	  label = _U("\ue61b"); }

	else if(iconName == "fa-cotton-bureau") {
	  label = _U("\uf89e"); }

	else if(iconName == "fa-dashcube") {
	  label = _U("\uf210"); }

	else if(iconName == "fa-42-group") {
	  label = _U("\ue080"); }

	else if(iconName == "fa-innosoft") {
	  label = _U("\ue080"); }

	else if(iconName == "fa-stack-exchange") {
	  label = _U("\uf18d"); }

	else if(iconName == "fa-elementor") {
	  label = _U("\uf430"); }

	else if(iconName == "fa-square-pied-piper") {
	  label = _U("\ue01e"); }

	else if(iconName == "fa-pied-piper-square") {
	  label = _U("\ue01e"); }

	else if(iconName == "fa-creative-commons-nd") {
	  label = _U("\uf4eb"); }

	else if(iconName == "fa-palfed") {
	  label = _U("\uf3d8"); }

	else if(iconName == "fa-superpowers") {
	  label = _U("\uf2dd"); }

	else if(iconName == "fa-resolving") {
	  label = _U("\uf3e7"); }

	else if(iconName == "fa-xbox") {
	  label = _U("\uf412"); }

	else if(iconName == "fa-searchengin") {
	  label = _U("\uf3eb"); }

	else if(iconName == "fa-tiktok") {
	  label = _U("\ue07b"); }

	else if(iconName == "fa-square-facebook") {
	  label = _U("\uf082"); }

	else if(iconName == "fa-facebook-square") {
	  label = _U("\uf082"); }

	else if(iconName == "fa-renren") {
	  label = _U("\uf18b"); }

	else if(iconName == "fa-linux") {
	  label = _U("\uf17c"); }

	else if(iconName == "fa-glide") {
	  label = _U("\uf2a5"); }

	else if(iconName == "fa-linkedin") {
	  label = _U("\uf08c"); }

	else if(iconName == "fa-hubspot") {
	  label = _U("\uf3b2"); }

	else if(iconName == "fa-deploydog") {
	  label = _U("\uf38e"); }

	else if(iconName == "fa-twitch") {
	  label = _U("\uf1e8"); }

	else if(iconName == "fa-ravelry") {
	  label = _U("\uf2d9"); }

	else if(iconName == "fa-mixer") {
	  label = _U("\ue056"); }

	else if(iconName == "fa-square-lastfm") {
	  label = _U("\uf203"); }

	else if(iconName == "fa-lastfm-square") {
	  label = _U("\uf203"); }

	else if(iconName == "fa-vimeo") {
	  label = _U("\uf40a"); }

	else if(iconName == "fa-mendeley") {
	  label = _U("\uf7b3"); }

	else if(iconName == "fa-uniregistry") {
	  label = _U("\uf404"); }

	else if(iconName == "fa-figma") {
	  label = _U("\uf799"); }

	else if(iconName == "fa-creative-commons-remix") {
	  label = _U("\uf4ee"); }

	else if(iconName == "fa-cc-amazon-pay") {
	  label = _U("\uf42d"); }

	else if(iconName == "fa-dropbox") {
	  label = _U("\uf16b"); }

	else if(iconName == "fa-instagram") {
	  label = _U("\uf16d"); }

	else if(iconName == "fa-cmplid") {
	  label = _U("\ue360"); }

	else if(iconName == "fa-upwork") {
	  label = _U("\ue641"); }

	else if(iconName == "fa-facebook") {
	  label = _U("\uf09a"); }

	else if(iconName == "fa-gripfire") {
	  label = _U("\uf3ac"); }

	else if(iconName == "fa-jedi-order") {
	  label = _U("\uf50e"); }

	else if(iconName == "fa-uikit") {
	  label = _U("\uf403"); }

	else if(iconName == "fa-fort-awesome-alt") {
	  label = _U("\uf3a3"); }

	else if(iconName == "fa-phabricator") {
	  label = _U("\uf3db"); }

	else if(iconName == "fa-ussunnah") {
	  label = _U("\uf407"); }

	else if(iconName == "fa-earlybirds") {
	  label = _U("\uf39a"); }

	else if(iconName == "fa-trade-federation") {
	  label = _U("\uf513"); }

	else if(iconName == "fa-autoprefixer") {
	  label = _U("\uf41c"); }

	else if(iconName == "fa-whatsapp") {
	  label = _U("\uf232"); }

	else if(iconName == "fa-slideshare") {
	  label = _U("\uf1e7"); }

	else if(iconName == "fa-google-play") {
	  label = _U("\uf3ab"); }

	else if(iconName == "fa-viadeo") {
	  label = _U("\uf2a9"); }

	else if(iconName == "fa-line") {
	  label = _U("\uf3c0"); }

	else if(iconName == "fa-google-drive") {
	  label = _U("\uf3aa"); }

	else if(iconName == "fa-servicestack") {
	  label = _U("\uf3ec"); }

	else if(iconName == "fa-simplybuilt") {
	  label = _U("\uf215"); }

	else if(iconName == "fa-bitbucket") {
	  label = _U("\uf171"); }

	else if(iconName == "fa-imdb") {
	  label = _U("\uf2d8"); }

	else if(iconName == "fa-deezer") {
	  label = _U("\ue077"); }

	else if(iconName == "fa-raspberry-pi") {
	  label = _U("\uf7bb"); }

	else if(iconName == "fa-jira") {
	  label = _U("\uf7b1"); }

	else if(iconName == "fa-docker") {
	  label = _U("\uf395"); }

	else if(iconName == "fa-screenpal") {
	  label = _U("\ue570"); }

	else if(iconName == "fa-bluetooth") {
	  label = _U("\uf293"); }

	else if(iconName == "fa-gitter") {
	  label = _U("\uf426"); }

	else if(iconName == "fa-d-and-d") {
	  label = _U("\uf38d"); }

	else if(iconName == "fa-microblog") {
	  label = _U("\ue01a"); }

	else if(iconName == "fa-cc-diners-club") {
	  label = _U("\uf24c"); }

	else if(iconName == "fa-gg-circle") {
	  label = _U("\uf261"); }

	else if(iconName == "fa-pied-piper-hat") {
	  label = _U("\uf4e5"); }

	else if(iconName == "fa-kickstarter-k") {
	  label = _U("\uf3bc"); }

	else if(iconName == "fa-yandex") {
	  label = _U("\uf413"); }

	else if(iconName == "fa-readme") {
	  label = _U("\uf4d5"); }

	else if(iconName == "fa-html5") {
	  label = _U("\uf13b"); }

	else if(iconName == "fa-sellsy") {
	  label = _U("\uf213"); }

	else if(iconName == "fa-sass") {
	  label = _U("\uf41e"); }

	else if(iconName == "fa-wirsindhandwerk") {
	  label = _U("\ue2d0"); }

	else if(iconName == "fa-wsh") {
	  label = _U("\ue2d0"); }

	else if(iconName == "fa-buromobelexperte") {
	  label = _U("\uf37f"); }

	else if(iconName == "fa-salesforce") {
	  label = _U("\uf83b"); }

	else if(iconName == "fa-octopus-deploy") {
	  label = _U("\ue082"); }

	else if(iconName == "fa-medapps") {
	  label = _U("\uf3c6"); }

	else if(iconName == "fa-ns8") {
	  label = _U("\uf3d5"); }

	else if(iconName == "fa-pinterest-p") {
	  label = _U("\uf231"); }

	else if(iconName == "fa-apper") {
	  label = _U("\uf371"); }

	else if(iconName == "fa-fort-awesome") {
	  label = _U("\uf286"); }

	else if(iconName == "fa-waze") {
	  label = _U("\uf83f"); }

	else if(iconName == "fa-cc-jcb") {
	  label = _U("\uf24b"); }

	else if(iconName == "fa-snapchat") {
	  label = _U("\uf2ab"); }

	else if(iconName == "fa-snapchat-ghost") {
	  label = _U("\uf2ab"); }

	else if(iconName == "fa-fantasy-flight-games") {
	  label = _U("\uf6dc"); }

	else if(iconName == "fa-rust") {
	  label = _U("\ue07a"); }

	else if(iconName == "fa-wix") {
	  label = _U("\uf5cf"); }

	else if(iconName == "fa-square-behance") {
	  label = _U("\uf1b5"); }

	else if(iconName == "fa-behance-square") {
	  label = _U("\uf1b5"); }

	else if(iconName == "fa-supple") {
	  label = _U("\uf3f9"); }

	else if(iconName == "fa-webflow") {
	  label = _U("\ue65c"); }

	else if(iconName == "fa-rebel") {
	  label = _U("\uf1d0"); }

	else if(iconName == "fa-css3") {
	  label = _U("\uf13c"); }

	else if(iconName == "fa-staylinked") {
	  label = _U("\uf3f5"); }

	else if(iconName == "fa-kaggle") {
	  label = _U("\uf5fa"); }

	else if(iconName == "fa-space-awesome") {
	  label = _U("\ue5ac"); }

	else if(iconName == "fa-deviantart") {
	  label = _U("\uf1bd"); }

	else if(iconName == "fa-cpanel") {
	  label = _U("\uf388"); }

	else if(iconName == "fa-goodreads-g") {
	  label = _U("\uf3a9"); }

	else if(iconName == "fa-square-git") {
	  label = _U("\uf1d2"); }

	else if(iconName == "fa-git-square") {
	  label = _U("\uf1d2"); }

	else if(iconName == "fa-square-tumblr") {
	  label = _U("\uf174"); }

	else if(iconName == "fa-tumblr-square") {
	  label = _U("\uf174"); }

	else if(iconName == "fa-trello") {
	  label = _U("\uf181"); }

	else if(iconName == "fa-creative-commons-nc-jp") {
	  label = _U("\uf4ea"); }

	else if(iconName == "fa-get-pocket") {
	  label = _U("\uf265"); }

	else if(iconName == "fa-perbyte") {
	  label = _U("\ue083"); }

	else if(iconName == "fa-grunt") {
	  label = _U("\uf3ad"); }

	else if(iconName == "fa-weebly") {
	  label = _U("\uf5cc"); }

	else if(iconName == "fa-connectdevelop") {
	  label = _U("\uf20e"); }

	else if(iconName == "fa-leanpub") {
	  label = _U("\uf212"); }

	else if(iconName == "fa-black-tie") {
	  label = _U("\uf27e"); }

	else if(iconName == "fa-themeco") {
	  label = _U("\uf5c6"); }

	else if(iconName == "fa-python") {
	  label = _U("\uf3e2"); }

	else if(iconName == "fa-android") {
	  label = _U("\uf17b"); }

	else if(iconName == "fa-bots") {
	  label = _U("\ue340"); }

	else if(iconName == "fa-free-code-camp") {
	  label = _U("\uf2c5"); }

	else if(iconName == "fa-hornbill") {
	  label = _U("\uf592"); }

	else if(iconName == "fa-js") {
	  label = _U("\uf3b8"); }

	else if(iconName == "fa-ideal") {
	  label = _U("\ue013"); }

	else if(iconName == "fa-git") {
	  label = _U("\uf1d3"); }

	else if(iconName == "fa-dev") {
	  label = _U("\uf6cc"); }

	else if(iconName == "fa-sketch") {
	  label = _U("\uf7c6"); }

	else if(iconName == "fa-yandex-international") {
	  label = _U("\uf414"); }

	else if(iconName == "fa-cc-amex") {
	  label = _U("\uf1f3"); }

	else if(iconName == "fa-uber") {
	  label = _U("\uf402"); }

	else if(iconName == "fa-github") {
	  label = _U("\uf09b"); }

	else if(iconName == "fa-php") {
	  label = _U("\uf457"); }

	else if(iconName == "fa-alipay") {
	  label = _U("\uf642"); }

	else if(iconName == "fa-youtube") {
	  label = _U("\uf167"); }

	else if(iconName == "fa-skyatlas") {
	  label = _U("\uf216"); }

	else if(iconName == "fa-firefox-browser") {
	  label = _U("\ue007"); }

	else if(iconName == "fa-replyd") {
	  label = _U("\uf3e6"); }

	else if(iconName == "fa-suse") {
	  label = _U("\uf7d6"); }

	else if(iconName == "fa-jenkins") {
	  label = _U("\uf3b6"); }

	else if(iconName == "fa-twitter") {
	  label = _U("\uf099"); }

	else if(iconName == "fa-rockrms") {
	  label = _U("\uf3e9"); }

	else if(iconName == "fa-pinterest") {
	  label = _U("\uf0d2"); }

	else if(iconName == "fa-buffer") {
	  label = _U("\uf837"); }

	else if(iconName == "fa-npm") {
	  label = _U("\uf3d4"); }

	else if(iconName == "fa-yammer") {
	  label = _U("\uf840"); }

	else if(iconName == "fa-btc") {
	  label = _U("\uf15a"); }

	else if(iconName == "fa-dribbble") {
	  label = _U("\uf17d"); }

	else if(iconName == "fa-stumbleupon-circle") {
	  label = _U("\uf1a3"); }

	else if(iconName == "fa-internet-explorer") {
	  label = _U("\uf26b"); }

	else if(iconName == "fa-stubber") {
	  label = _U("\ue5c7"); }

	else if(iconName == "fa-telegram") {
	  label = _U("\uf2c6"); }

	else if(iconName == "fa-telegram-plane") {
	  label = _U("\uf2c6"); }

	else if(iconName == "fa-old-republic") {
	  label = _U("\uf510"); }

	else if(iconName == "fa-odysee") {
	  label = _U("\ue5c6"); }

	else if(iconName == "fa-square-whatsapp") {
	  label = _U("\uf40c"); }

	else if(iconName == "fa-whatsapp-square") {
	  label = _U("\uf40c"); }

	else if(iconName == "fa-node-js") {
	  label = _U("\uf3d3"); }

	else if(iconName == "fa-edge-legacy") {
	  label = _U("\ue078"); }

	else if(iconName == "fa-slack") {
	  label = _U("\uf198"); }

	else if(iconName == "fa-slack-hash") {
	  label = _U("\uf198"); }

	else if(iconName == "fa-medrt") {
	  label = _U("\uf3c8"); }

	else if(iconName == "fa-usb") {
	  label = _U("\uf287"); }

	else if(iconName == "fa-tumblr") {
	  label = _U("\uf173"); }

	else if(iconName == "fa-vaadin") {
	  label = _U("\uf408"); }

	else if(iconName == "fa-quora") {
	  label = _U("\uf2c4"); }

	else if(iconName == "fa-square-x-twitter") {
	  label = _U("\ue61a"); }

	else if(iconName == "fa-reacteurope") {
	  label = _U("\uf75d"); }

	else if(iconName == "fa-medium") {
	  label = _U("\uf23a"); }

	else if(iconName == "fa-medium-m") {
	  label = _U("\uf23a"); }

	else if(iconName == "fa-amilia") {
	  label = _U("\uf36d"); }

	else if(iconName == "fa-mixcloud") {
	  label = _U("\uf289"); }

	else if(iconName == "fa-flipboard") {
	  label = _U("\uf44d"); }

	else if(iconName == "fa-viacoin") {
	  label = _U("\uf237"); }

	else if(iconName == "fa-critical-role") {
	  label = _U("\uf6c9"); }

	else if(iconName == "fa-sitrox") {
	  label = _U("\ue44a"); }

	else if(iconName == "fa-discourse") {
	  label = _U("\uf393"); }

	else if(iconName == "fa-joomla") {
	  label = _U("\uf1aa"); }

	else if(iconName == "fa-mastodon") {
	  label = _U("\uf4f6"); }

	else if(iconName == "fa-airbnb") {
	  label = _U("\uf834"); }

	else if(iconName == "fa-wolf-pack-battalion") {
	  label = _U("\uf514"); }

	else if(iconName == "fa-buy-n-large") {
	  label = _U("\uf8a6"); }

	else if(iconName == "fa-gulp") {
	  label = _U("\uf3ae"); }

	else if(iconName == "fa-creative-commons-sampling-plus") {
	  label = _U("\uf4f1"); }

	else if(iconName == "fa-strava") {
	  label = _U("\uf428"); }

	else if(iconName == "fa-ember") {
	  label = _U("\uf423"); }

	else if(iconName == "fa-canadian-maple-leaf") {
	  label = _U("\uf785"); }

	else if(iconName == "fa-teamspeak") {
	  label = _U("\uf4f9"); }

	else if(iconName == "fa-pushed") {
	  label = _U("\uf3e1"); }

	else if(iconName == "fa-wordpress-simple") {
	  label = _U("\uf411"); }

	else if(iconName == "fa-nutritionix") {
	  label = _U("\uf3d6"); }

	else if(iconName == "fa-wodu") {
	  label = _U("\ue088"); }

	else if(iconName == "fa-google-pay") {
	  label = _U("\ue079"); }

	else if(iconName == "fa-intercom") {
	  label = _U("\uf7af"); }

	else if(iconName == "fa-zhihu") {
	  label = _U("\uf63f"); }

	else if(iconName == "fa-korvue") {
	  label = _U("\uf42f"); }

	else if(iconName == "fa-pix") {
	  label = _U("\ue43a"); }

	if (!label.empty())
	{
		auto text = std::make_shared<TextComponent>(window, label, theme->Text.font, theme->Text.color, ALIGN_CENTER);
		row.addElement(text, false);

		// spacer between icon and text
		auto spacer = std::make_shared<GuiComponent>(window);
		spacer->setSize(10, 0);
		row.addElement(spacer, false);
	}
}

void MenuComponent::addWithLabel(const std::string& label, const std::shared_ptr<GuiComponent>& comp, const std::function<void()>& func, const std::string& iconName, bool setCursorHere)
{
	auto theme = ThemeData::getMenuTheme();

	ComponentListRow row;

	addMenuIcon(mWindow, row, iconName);

	auto text = std::make_shared<TextComponent>(mWindow, Utils::String::toUpper(label), theme->Text.font, theme->Text.color);
	row.addElement(text, true);

	if (EsLocale::isRTL())
		text->setHorizontalAlignment(Alignment::ALIGN_RIGHT);

	row.addElement(comp, false);

	if (func != nullptr)
		row.makeAcceptInputHandler(func);

	addRow(row, setCursorHere);
}

void MenuComponent::addWithDescription(const std::string& label, const std::string& description, const std::shared_ptr<GuiComponent>& comp, const std::function<void()>& func, const std::string& iconName, bool setCursorHere, bool multiLine, const std::string& userData, bool doUpdateSize)
{
	auto theme = ThemeData::getMenuTheme();

	ComponentListRow row;

	addMenuIcon(mWindow, row, iconName);

	if (!description.empty())
	{
		if (!multiLine)
			mList->setUpdateType(ComponentListFlags::UpdateType::UPDATE_ALWAYS);

		row.addElement(std::make_shared<MultiLineMenuEntry>(mWindow, Utils::String::toUpper(label), description, multiLine), true);
	}
	else
		row.addElement(std::make_shared<TextComponent>(mWindow, Utils::String::toUpper(label), theme->Text.font, theme->Text.color), true);

	if (comp != nullptr)
		row.addElement(comp, false);

	if (func != nullptr)
		row.makeAcceptInputHandler(func);

	addRow(row, setCursorHere, doUpdateSize, userData);
}


void MenuComponent::addEntry(const std::string& name, bool add_arrow, const std::function<void()>& func, const std::string& iconName, bool setCursorHere, bool onButtonRelease, const std::string& userData, bool doUpdateSize)
{
	auto theme = ThemeData::getMenuTheme();
	std::shared_ptr<Font> font = theme->Text.font;
	unsigned int color = theme->Text.color;

	// populate the list
	ComponentListRow row;

	addMenuIcon(mWindow, row, iconName);

	auto text = std::make_shared<TextComponent>(mWindow, name, font, color);
	row.addElement(text, true);

	if (EsLocale::isRTL())
		text->setHorizontalAlignment(Alignment::ALIGN_RIGHT);

	if (add_arrow)
	{
		auto arrow = makeArrow(mWindow);

		if (EsLocale::isRTL())
			arrow->setFlipX(true);

		row.addElement(arrow, false);
	}

	if (func != nullptr)
		row.makeAcceptInputHandler(func, onButtonRelease);

	addRow(row, setCursorHere, doUpdateSize, userData);
}
/*
void MenuComponent::addEntryLabel(const std::string& name, const std::string& label, const std::function<void()>& func, const std::string& iconName, bool setCursorHere, bool onButtonRelease, const std::string& userData, bool doUpdateSize)
{
	auto theme = ThemeData::getMenuTheme();
	std::shared_ptr<Font> font = theme->Text.font;
	unsigned int color = theme->Text.color;

	// populate the list
	ComponentListRow row;

	addMenuIcon(mWindow, row, iconName);

	auto text = std::make_shared<TextComponent>(mWindow, name, font, color);
	row.addElement(text, true);

	if (EsLocale::isRTL())
		text->setHorizontalAlignment(Alignment::ALIGN_RIGHT);

	auto textlabel = std::make_shared<TextComponent>(mWindow, label, font, color);
	row.addElement(textlabel, false);

	if (func != nullptr)
		row.makeAcceptInputHandler(func, onButtonRelease);

	addRow(row, setCursorHere, doUpdateSize, userData);
}
*/
void MenuComponent::setTitle(const std::string& title, const std::shared_ptr<Font>& font)
{
	mTitle->setText(title/*Utils::String::toUpper(title)*/);

	if (font != nullptr)
		mTitle->setFont(font);
}

void MenuComponent::setTitleImage(std::shared_ptr<ImageComponent> titleImage, bool replaceTitle)
{
	if (titleImage == nullptr)
	{
		if (mTitleImage != nullptr)
		{
			mHeaderGrid->removeEntry(mTitleImage);
			mTitleImage = nullptr;
		}

		mHeaderGrid->setColWidthPerc(0, 1);
		mHeaderGrid->setColWidthPerc(1, 0);

		return;
	}

	mTitleImage = titleImage;
	mTitleImage->setPadding(TITLE_HEIGHT * 0.15f);

	if (replaceTitle)
	{
		mTitleImage->setMaxSize(mSize.x() * 0.7f, TITLE_HEIGHT);

		mHeaderGrid->setColWidthPerc(0, 0);
		mHeaderGrid->setColWidthPerc(1, 1);
		mHeaderGrid->setEntry(mTitleImage, Vector2i(0, 0), false, false, Vector2i(2, 2));

		if (mTitle)
			mTitle->setVisible(false);

		if (mSubtitle)
			mSubtitle->setVisible(false);
	}
	else
	{
		float width = (float)Math::min((int)Renderer::getScreenHeight(), (int)(Renderer::getScreenWidth() * 0.90f));
		float iw = TITLE_HEIGHT / width;

		mTitleImage->setMaxSize(1.3f * iw * mSize.x(), TITLE_HEIGHT);

		mHeaderGrid->setColWidthPerc(0, 1 - iw);
		mHeaderGrid->setColWidthPerc(1, iw);
		mHeaderGrid->setEntry(mTitleImage, Vector2i(1, 0), false, false, Vector2i(1, 2));
	}

	updateSize();
}

void MenuComponent::setSubTitle(const std::string& text)
{
	if (text.empty())
	{
		if (mSubtitle != nullptr)
		{
			mHeaderGrid->removeEntry(mSubtitle);
			mSubtitle = nullptr;
		}

		mHeaderGrid->setRowHeightPerc(0, 1);
		mHeaderGrid->setRowHeightPerc(1, 0);

		return;
	}

	if (mSubtitle == nullptr)
	{
		auto theme = ThemeData::getMenuTheme();

		mSubtitle = std::make_shared<TextComponent>(mWindow,
			Utils::String::toUpper(Utils::FileSystem::getFileName(text)),
			theme->TextSmall.font, theme->TextSmall.color, ALIGN_CENTER);

		mHeaderGrid->setEntry(mSubtitle, Vector2i(0, 1), false, true);
	}

	mSubtitle->setText(text);
	mSubtitle->setVerticalAlignment(Alignment::ALIGN_TOP);
	mSubtitle->setSize(Renderer::getScreenWidth() * 0.88f, 0);
	mSubtitle->setLineSpacing(1.1);

	const float titleHeight = mTitle->getFont()->getLetterHeight() + (mSubtitle ? TITLE_WITHSUB_VERT_PADDING : TITLE_VERT_PADDING);
	const float subtitleHeight = mSubtitle->getSize().y() + SUBTITLE_VERT_PADDING;

	mHeaderGrid->setRowHeightPerc(0, titleHeight / TITLE_HEIGHT);

	if (mTitleImage != nullptr)
		setTitleImage(mTitleImage);

	updateSize();
}

float MenuComponent::getTitleHeight() const
{
	return TITLE_HEIGHT;
}

float MenuComponent::getHeaderGridHeight() const
{
	return mHeaderGrid->getRowHeight(0);
}

float MenuComponent::getButtonGridHeight() const
{
	auto menuTheme = ThemeData::getMenuTheme();
	return (mButtonGrid ? mButtonGrid->getSize().y() : menuTheme->Text.font->getHeight() + BUTTON_GRID_VERT_PADDING);
}

void MenuComponent::updateSize()
{
	// GPI
	if (Renderer::isSmallScreen())
	{
		setSize(Renderer::getScreenWidth(), Renderer::getScreenHeight());
		return;
	}

	const float maxHeight = mMaxHeight <= 0 ? Renderer::getScreenHeight() * 0.75f : mMaxHeight;

	float height = TITLE_HEIGHT + mList->getTotalRowHeight() + getButtonGridHeight() + 2;
	if(height > maxHeight)
	{
		height = TITLE_HEIGHT + getButtonGridHeight();
		int i = 0;
		while(i < mList->size())
		{
			float rowHeight = mList->getRowHeight(i);
			if(height + rowHeight < maxHeight)
				height += rowHeight;
			else
				break;
			i++;
		}
	}

	float width = (float)Math::min((int)Renderer::getScreenHeight(), (int)(Renderer::getScreenWidth() * 0.90f));
	setSize(width, height);

	if (mTitleImage != nullptr && mTitle != nullptr && mTitle->isVisible())
	{
		float pad = Renderer::getScreenWidth() * 0.012;
		mTitle->setPadding(Vector4f(pad, 0.0f, pad, 0.0f));
		mTitle->setHorizontalAlignment(ALIGN_LEFT);

		if (mSubtitle != nullptr)
		{
			mSubtitle->setPadding(Vector4f(pad, 0.0f, pad, 0.0f));
			mSubtitle->setHorizontalAlignment(ALIGN_LEFT);
		}
	}
}

void MenuComponent::onSizeChanged()
{
	GuiComponent::onSizeChanged();

	mBackground.fitTo(mSize, Vector3f::Zero(), Vector2f(-32, -32));

	// update grid row/col sizes
	mGrid.setRowHeightPerc(0, TITLE_HEIGHT / mSize.y(), false);
	mGrid.setRowHeightPerc(2, getButtonGridHeight() / mSize.y(), false);

	mGrid.setSize(mSize);
	/*
	if (mTitleImage != nullptr)
	{
		mTitleImage->setMaxSize(mSize.x() * 0.80, TITLE_HEIGHT * 0.85); // replaceTitle

		if (mTitle != nullptr && mTitle->isVisible())
		{
			float iw = mTitleImage->getSize().x() / mSize.x();
			mHeaderGrid->setColWidthPerc(0, 1 - iw);
			mHeaderGrid->setColWidthPerc(1, iw);
		}
	}*/
}

void MenuComponent::clearButtons()
{
	mButtons.clear();
	updateGrid();
	updateSize();
}

void MenuComponent::addButton(const std::string& name, const std::string& helpText, const std::function<void()>& callback)
{
	mButtons.push_back(std::make_shared<ButtonComponent>(mWindow, Utils::String::toUpper(name), helpText, callback));
	updateGrid();
	updateSize();
}

void MenuComponent::updateGrid()
{
	if(mButtonGrid)
		mGrid.removeEntry(mButtonGrid);

	mButtonGrid.reset();

	if(mButtons.size())
	{
		mButtonGrid = makeButtonGrid(mWindow, mButtons);
		mGrid.setEntry(mButtonGrid, Vector2i(0, 2), true, false);
	}
}

std::vector<HelpPrompt> MenuComponent::getHelpPrompts()
{
	return mGrid.getHelpPrompts();
}

void MenuComponent::onShow()
{
  GuiComponent::onShow();
  //TextToSpeech::getInstance()->say(mTitle->getText());
  getList()->saySelectedLine();
}

std::shared_ptr<ComponentGrid> makeButtonGrid(Window* window, const std::vector< std::shared_ptr<ButtonComponent> >& buttons)
{
	std::shared_ptr<ComponentGrid> buttonGrid = std::make_shared<ComponentGrid>(window, Vector2i((int)buttons.size(), 2));

	float buttonGridWidth = (float)BUTTON_GRID_HORIZ_PADDING * buttons.size(); // initialize to padding
	for(int i = 0; i < (int)buttons.size(); i++)
	{
		buttonGrid->setEntry(buttons.at(i), Vector2i(i, 0), true, false);
		buttonGridWidth += buttons.at(i)->getSize().x();
	}
	for(unsigned int i = 0; i < buttons.size(); i++)
	{
		buttonGrid->setColWidthPerc(i, (buttons.at(i)->getSize().x() + BUTTON_GRID_HORIZ_PADDING) / buttonGridWidth);
	}

	buttonGrid->setSize(buttonGridWidth, buttons.at(0)->getSize().y() + BUTTON_GRID_VERT_PADDING + 2);
	buttonGrid->setRowHeightPerc(1, 2 / buttonGrid->getSize().y()); // spacer row to deal with dropshadow to make buttons look centered

	return buttonGrid;
}

std::shared_ptr<ImageComponent> makeArrow(Window* window)
{
	auto menuTheme = ThemeData::getMenuTheme();

	auto bracket = std::make_shared<ImageComponent>(window);
	bracket->setImage(ThemeData::getMenuTheme()->Icons.arrow); // ":/arrow.svg");
	bracket->setColorShift(menuTheme->Text.color);
	bracket->setResize(0, round(menuTheme->Text.font->getLetterHeight()));

	return bracket;
}
