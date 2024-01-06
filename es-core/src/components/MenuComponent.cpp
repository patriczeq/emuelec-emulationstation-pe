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

		else if(iconName == "fa-fill-drip"){
	  label = _U("\uf576"); }

	else if(iconName == "fa-arrows-to-circle"){
	  label = _U("\ue4bd"); }

	else if(iconName == "fa-circle-chevron-right"){
	  label = _U("\uf138"); }

	else if(iconName == "fa-chevron-circle-right"){
	  label = _U("\uf138"); }

	else if(iconName == "fa-trash-can"){
	  label = _U("\uf2ed"); }

	else if(iconName == "fa-trash-alt"){
	  label = _U("\uf2ed"); }

	else if(iconName == "fa-text-height"){
	  label = _U("\uf034"); }

	else if(iconName == "fa-user-xmark"){
	  label = _U("\uf235"); }

	else if(iconName == "fa-user-times"){
	  label = _U("\uf235"); }

	else if(iconName == "fa-stethoscope"){
	  label = _U("\uf0f1"); }

	else if(iconName == "fa-message"){
	  label = _U("\uf27a"); }

	else if(iconName == "fa-comment-alt"){
	  label = _U("\uf27a"); }

	else if(iconName == "fa-info"){
	  label = _U("\uf129"); }

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

	else if(iconName == "fa-calendar-days"){
	  label = _U("\uf073"); }

	else if(iconName == "fa-calendar-alt"){
	  label = _U("\uf073"); }

	else if(iconName == "fa-anchor-circle-check"){
	  label = _U("\ue4aa"); }

	else if(iconName == "fa-building-circle-arrow-right"){
	  label = _U("\ue4d1"); }

	else if(iconName == "fa-volleyball"){
	  label = _U("\uf45f"); }

	else if(iconName == "fa-volleyball-ball"){
	  label = _U("\uf45f"); }

	else if(iconName == "fa-arrows-up-to-line"){
	  label = _U("\ue4c2"); }

	else if(iconName == "fa-sort-down"){
	  label = _U("\uf0dd"); }

	else if(iconName == "fa-sort-desc"){
	  label = _U("\uf0dd"); }

	else if(iconName == "fa-circle-minus"){
	  label = _U("\uf056"); }

	else if(iconName == "fa-minus-circle"){
	  label = _U("\uf056"); }

	else if(iconName == "fa-door-open"){
	  label = _U("\uf52b"); }

	else if(iconName == "fa-right-from-bracket"){
	  label = _U("\uf2f5"); }

	else if(iconName == "fa-sign-out-alt"){
	  label = _U("\uf2f5"); }

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

	else if(iconName == "fa-bridge-circle-check"){
	  label = _U("\ue4c9"); }

	else if(iconName == "fa-pump-medical"){
	  label = _U("\ue06a"); }

	else if(iconName == "fa-fingerprint"){
	  label = _U("\uf577"); }

	else if(iconName == "fa-hand-point-right"){
	  label = _U("\uf0a4"); }

	else if(iconName == "fa-magnifying-glass-location"){
	  label = _U("\uf689"); }

	else if(iconName == "fa-search-location"){
	  label = _U("\uf689"); }

	else if(iconName == "fa-forward-step"){
	  label = _U("\uf051"); }

	else if(iconName == "fa-step-forward"){
	  label = _U("\uf051"); }

	else if(iconName == "fa-face-smile-beam"){
	  label = _U("\uf5b8"); }

	else if(iconName == "fa-smile-beam"){
	  label = _U("\uf5b8"); }

	else if(iconName == "fa-flag-checkered"){
	  label = _U("\uf11e"); }

	else if(iconName == "fa-football"){
	  label = _U("\uf44e"); }

	else if(iconName == "fa-football-ball"){
	  label = _U("\uf44e"); }

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

	else if(iconName == "fa-people-line"){
	  label = _U("\ue534"); }

	else if(iconName == "fa-beer-mug-empty"){
	  label = _U("\uf0fc"); }

	else if(iconName == "fa-beer"){
	  label = _U("\uf0fc"); }

	else if(iconName == "fa-diagram-predecessor"){
	  label = _U("\ue477"); }

	else if(iconName == "fa-arrow-up-long"){
	  label = _U("\uf176"); }

	else if(iconName == "fa-long-arrow-up"){
	  label = _U("\uf176"); }

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

	else if(iconName == "fa-truck-plane"){
	  label = _U("\ue58f"); }

	else if(iconName == "fa-record-vinyl"){
	  label = _U("\uf8d9"); }

	else if(iconName == "fa-face-grin-stars"){
	  label = _U("\uf587"); }

	else if(iconName == "fa-grin-stars"){
	  label = _U("\uf587"); }

	else if(iconName == "fa-bong"){
	  label = _U("\uf55c"); }

	else if(iconName == "fa-spaghetti-monster-flying"){
	  label = _U("\uf67b"); }

	else if(iconName == "fa-pastafarianism"){
	  label = _U("\uf67b"); }

	else if(iconName == "fa-arrow-down-up-across-line"){
	  label = _U("\ue4af"); }

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

	else if(iconName == "fa-circle-h"){
	  label = _U("\uf47e"); }

	else if(iconName == "fa-hospital-symbol"){
	  label = _U("\uf47e"); }

	else if(iconName == "fa-pager"){
	  label = _U("\uf815"); }

	else if(iconName == "fa-address-book"){
	  label = _U("\uf2b9"); }

	else if(iconName == "fa-contact-book"){
	  label = _U("\uf2b9"); }

	else if(iconName == "fa-strikethrough"){
	  label = _U("\uf0cc"); }

	else if(iconName == "fa-landmark-flag"){
	  label = _U("\ue51c"); }

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

	else if(iconName == "fa-code-pull-request"){
	  label = _U("\ue13c"); }

	else if(iconName == "fa-clipboard-list"){
	  label = _U("\uf46d"); }

	else if(iconName == "fa-truck-ramp-box"){
	  label = _U("\uf4de"); }

	else if(iconName == "fa-truck-loading"){
	  label = _U("\uf4de"); }

	else if(iconName == "fa-user-check"){
	  label = _U("\uf4fc"); }

	else if(iconName == "fa-vial-virus"){
	  label = _U("\ue597"); }

	else if(iconName == "fa-sheet-plastic"){
	  label = _U("\ue571"); }

	else if(iconName == "fa-blog"){
	  label = _U("\uf781"); }

	else if(iconName == "fa-user-ninja"){
	  label = _U("\uf504"); }

	else if(iconName == "fa-person-arrow-up-from-line"){
	  label = _U("\ue539"); }

	else if(iconName == "fa-scroll-torah"){
	  label = _U("\uf6a0"); }

	else if(iconName == "fa-torah"){
	  label = _U("\uf6a0"); }

	else if(iconName == "fa-broom-ball"){
	  label = _U("\uf458"); }

	else if(iconName == "fa-quidditch"){
	  label = _U("\uf458"); }

	else if(iconName == "fa-quidditch-broom-ball"){
	  label = _U("\uf458"); }

	else if(iconName == "fa-toggle-off"){
	  label = _U("\uf204"); }

	else if(iconName == "fa-box-archive"){
	  label = _U("\uf187"); }

	else if(iconName == "fa-archive"){
	  label = _U("\uf187"); }

	else if(iconName == "fa-person-drowning"){
	  label = _U("\ue545"); }

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

	else if(iconName == "fa-spray-can"){
	  label = _U("\uf5bd"); }

	else if(iconName == "fa-truck-monster"){
	  label = _U("\uf63b"); }

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

	else if(iconName == "fa-cloud"){
	  label = _U("\uf0c2"); }

	else if(iconName == "fa-trowel-bricks"){
	  label = _U("\ue58a"); }

	else if(iconName == "fa-face-flushed"){
	  label = _U("\uf579"); }

	else if(iconName == "fa-flushed"){
	  label = _U("\uf579"); }

	else if(iconName == "fa-hospital-user"){
	  label = _U("\uf80d"); }

	else if(iconName == "fa-tent-arrow-left-right"){
	  label = _U("\ue57f"); }

	else if(iconName == "fa-gavel"){
	  label = _U("\uf0e3"); }

	else if(iconName == "fa-legal"){
	  label = _U("\uf0e3"); }

	else if(iconName == "fa-binoculars"){
	  label = _U("\uf1e5"); }

	else if(iconName == "fa-microphone-slash"){
	  label = _U("\uf131"); }

	else if(iconName == "fa-box-tissue"){
	  label = _U("\ue05b"); }

	else if(iconName == "fa-motorcycle"){
	  label = _U("\uf21c"); }

	else if(iconName == "fa-bell-concierge"){
	  label = _U("\uf562"); }

	else if(iconName == "fa-concierge-bell"){
	  label = _U("\uf562"); }

	else if(iconName == "fa-pen-ruler"){
	  label = _U("\uf5ae"); }

	else if(iconName == "fa-pencil-ruler"){
	  label = _U("\uf5ae"); }

	else if(iconName == "fa-people-arrows"){
	  label = _U("\ue068"); }

	else if(iconName == "fa-people-arrows-left-right"){
	  label = _U("\ue068"); }

	else if(iconName == "fa-mars-and-venus-burst"){
	  label = _U("\ue523"); }

	else if(iconName == "fa-square-caret-right"){
	  label = _U("\uf152"); }

	else if(iconName == "fa-caret-square-right"){
	  label = _U("\uf152"); }

	else if(iconName == "fa-scissors"){
	  label = _U("\uf0c4"); }

	else if(iconName == "fa-cut"){
	  label = _U("\uf0c4"); }

	else if(iconName == "fa-sun-plant-wilt"){
	  label = _U("\ue57a"); }

	else if(iconName == "fa-toilets-portable"){
	  label = _U("\ue584"); }

	else if(iconName == "fa-hockey-puck"){
	  label = _U("\uf453"); }

	else if(iconName == "fa-table"){
	  label = _U("\uf0ce"); }

	else if(iconName == "fa-magnifying-glass-arrow-right"){
	  label = _U("\ue521"); }

	else if(iconName == "fa-tachograph-digital"){
	  label = _U("\uf566"); }

	else if(iconName == "fa-digital-tachograph"){
	  label = _U("\uf566"); }

	else if(iconName == "fa-users-slash"){
	  label = _U("\ue073"); }

	else if(iconName == "fa-clover"){
	  label = _U("\ue139"); }

	else if(iconName == "fa-reply"){
	  label = _U("\uf3e5"); }

	else if(iconName == "fa-mail-reply"){
	  label = _U("\uf3e5"); }

	else if(iconName == "fa-star-and-crescent"){
	  label = _U("\uf699"); }

	else if(iconName == "fa-house-fire"){
	  label = _U("\ue50c"); }

	else if(iconName == "fa-square-minus"){
	  label = _U("\uf146"); }

	else if(iconName == "fa-minus-square"){
	  label = _U("\uf146"); }

	else if(iconName == "fa-helicopter"){
	  label = _U("\uf533"); }

	else if(iconName == "fa-compass"){
	  label = _U("\uf14e"); }

	else if(iconName == "fa-square-caret-down"){
	  label = _U("\uf150"); }

	else if(iconName == "fa-caret-square-down"){
	  label = _U("\uf150"); }

	else if(iconName == "fa-file-circle-question"){
	  label = _U("\ue4ef"); }

	else if(iconName == "fa-laptop-code"){
	  label = _U("\uf5fc"); }

	else if(iconName == "fa-swatchbook"){
	  label = _U("\uf5c3"); }

	else if(iconName == "fa-prescription-bottle"){
	  label = _U("\uf485"); }

	else if(iconName == "fa-bars"){
	  label = _U("\uf0c9"); }

	else if(iconName == "fa-navicon"){
	  label = _U("\uf0c9"); }

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

	else if(iconName == "fa-square-up-right"){
	  label = _U("\uf360"); }

	else if(iconName == "fa-external-link-square-alt"){
	  label = _U("\uf360"); }

	else if(iconName == "fa-face-kiss-beam"){
	  label = _U("\uf597"); }

	else if(iconName == "fa-kiss-beam"){
	  label = _U("\uf597"); }

	else if(iconName == "fa-film"){
	  label = _U("\uf008"); }

	else if(iconName == "fa-ruler-horizontal"){
	  label = _U("\uf547"); }

	else if(iconName == "fa-people-robbery"){
	  label = _U("\ue536"); }

	else if(iconName == "fa-lightbulb"){
	  label = _U("\uf0eb"); }

	else if(iconName == "fa-caret-left"){
	  label = _U("\uf0d9"); }

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

	else if(iconName == "fa-circle-chevron-down"){
	  label = _U("\uf13a"); }

	else if(iconName == "fa-chevron-circle-down"){
	  label = _U("\uf13a"); }

	else if(iconName == "fa-unlock-keyhole"){
	  label = _U("\uf13e"); }

	else if(iconName == "fa-unlock-alt"){
	  label = _U("\uf13e"); }

	else if(iconName == "fa-cloud-showers-heavy"){
	  label = _U("\uf740"); }

	else if(iconName == "fa-headphones-simple"){
	  label = _U("\uf58f"); }

	else if(iconName == "fa-headphones-alt"){
	  label = _U("\uf58f"); }

	else if(iconName == "fa-sitemap"){
	  label = _U("\uf0e8"); }

	else if(iconName == "fa-circle-dollar-to-slot"){
	  label = _U("\uf4b9"); }

	else if(iconName == "fa-donate"){
	  label = _U("\uf4b9"); }

	else if(iconName == "fa-memory"){
	  label = _U("\uf538"); }

	else if(iconName == "fa-road-spikes"){
	  label = _U("\ue568"); }

	else if(iconName == "fa-fire-burner"){
	  label = _U("\ue4f1"); }

	else if(iconName == "fa-flag"){
	  label = _U("\uf024"); }

	else if(iconName == "fa-hanukiah"){
	  label = _U("\uf6e6"); }

	else if(iconName == "fa-feather"){
	  label = _U("\uf52d"); }

	else if(iconName == "fa-volume-low"){
	  label = _U("\uf027"); }

	else if(iconName == "fa-volume-down"){
	  label = _U("\uf027"); }

	else if(iconName == "fa-comment-slash"){
	  label = _U("\uf4b3"); }

	else if(iconName == "fa-cloud-sun-rain"){
	  label = _U("\uf743"); }

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

	else if(iconName == "fa-square-check"){
	  label = _U("\uf14a"); }

	else if(iconName == "fa-check-square"){
	  label = _U("\uf14a"); }

	else if(iconName == "fa-peseta-sign"){
	  label = _U("\ue221"); }

	else if(iconName == "fa-heading"){
	  label = _U("\uf1dc"); }

	else if(iconName == "fa-header"){
	  label = _U("\uf1dc"); }

	else if(iconName == "fa-ghost"){
	  label = _U("\uf6e2"); }

	else if(iconName == "fa-list"){
	  label = _U("\uf03a"); }

	else if(iconName == "fa-list-squares"){
	  label = _U("\uf03a"); }

	else if(iconName == "fa-square-phone-flip"){
	  label = _U("\uf87b"); }

	else if(iconName == "fa-phone-square-alt"){
	  label = _U("\uf87b"); }

	else if(iconName == "fa-cart-plus"){
	  label = _U("\uf217"); }

	else if(iconName == "fa-gamepad"){
	  label = _U("\uf11b"); }

	else if(iconName == "fa-circle-dot"){
	  label = _U("\uf192"); }

	else if(iconName == "fa-dot-circle"){
	  label = _U("\uf192"); }

	else if(iconName == "fa-face-dizzy"){
	  label = _U("\uf567"); }

	else if(iconName == "fa-dizzy"){
	  label = _U("\uf567"); }

	else if(iconName == "fa-egg"){
	  label = _U("\uf7fb"); }

	else if(iconName == "fa-house-medical-circle-xmark"){
	  label = _U("\ue513"); }

	else if(iconName == "fa-campground"){
	  label = _U("\uf6bb"); }

	else if(iconName == "fa-folder-plus"){
	  label = _U("\uf65e"); }

	else if(iconName == "fa-futbol"){
	  label = _U("\uf1e3"); }

	else if(iconName == "fa-futbol-ball"){
	  label = _U("\uf1e3"); }

	else if(iconName == "fa-soccer-ball"){
	  label = _U("\uf1e3"); }

	else if(iconName == "fa-paintbrush"){
	  label = _U("\uf1fc"); }

	else if(iconName == "fa-paint-brush"){
	  label = _U("\uf1fc"); }

	else if(iconName == "fa-lock"){
	  label = _U("\uf023"); }

	else if(iconName == "fa-gas-pump"){
	  label = _U("\uf52f"); }

	else if(iconName == "fa-hot-tub-person"){
	  label = _U("\uf593"); }

	else if(iconName == "fa-hot-tub"){
	  label = _U("\uf593"); }

	else if(iconName == "fa-map-location"){
	  label = _U("\uf59f"); }

	else if(iconName == "fa-map-marked"){
	  label = _U("\uf59f"); }

	else if(iconName == "fa-house-flood-water"){
	  label = _U("\ue50e"); }

	else if(iconName == "fa-tree"){
	  label = _U("\uf1bb"); }

	else if(iconName == "fa-bridge-lock"){
	  label = _U("\ue4cc"); }

	else if(iconName == "fa-sack-dollar"){
	  label = _U("\uf81d"); }

	else if(iconName == "fa-pen-to-square"){
	  label = _U("\uf044"); }

	else if(iconName == "fa-edit"){
	  label = _U("\uf044"); }

	else if(iconName == "fa-car-side"){
	  label = _U("\uf5e4"); }

	else if(iconName == "fa-share-nodes"){
	  label = _U("\uf1e0"); }

	else if(iconName == "fa-share-alt"){
	  label = _U("\uf1e0"); }

	else if(iconName == "fa-heart-circle-minus"){
	  label = _U("\ue4ff"); }

	else if(iconName == "fa-hourglass-half"){
	  label = _U("\uf252"); }

	else if(iconName == "fa-hourglass-2"){
	  label = _U("\uf252"); }

	else if(iconName == "fa-microscope"){
	  label = _U("\uf610"); }

	else if(iconName == "fa-sink"){
	  label = _U("\ue06d"); }

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

	else if(iconName == "fa-person-rays"){
	  label = _U("\ue54d"); }

	else if(iconName == "fa-users"){
	  label = _U("\uf0c0"); }

	else if(iconName == "fa-eye-slash"){
	  label = _U("\uf070"); }

	else if(iconName == "fa-flask-vial"){
	  label = _U("\ue4f3"); }

	else if(iconName == "fa-hand"){
	  label = _U("\uf256"); }

	else if(iconName == "fa-hand-paper"){
	  label = _U("\uf256"); }

	else if(iconName == "fa-om"){
	  label = _U("\uf679"); }

	else if(iconName == "fa-worm"){
	  label = _U("\ue599"); }

	else if(iconName == "fa-house-circle-xmark"){
	  label = _U("\ue50b"); }

	else if(iconName == "fa-plug"){
	  label = _U("\uf1e6"); }

	else if(iconName == "fa-chevron-up"){
	  label = _U("\uf077"); }

	else if(iconName == "fa-hand-spock"){
	  label = _U("\uf259"); }

	else if(iconName == "fa-stopwatch"){
	  label = _U("\uf2f2"); }

	else if(iconName == "fa-face-kiss"){
	  label = _U("\uf596"); }

	else if(iconName == "fa-kiss"){
	  label = _U("\uf596"); }

	else if(iconName == "fa-bridge-circle-xmark"){
	  label = _U("\ue4cb"); }

	else if(iconName == "fa-face-grin-tongue"){
	  label = _U("\uf589"); }

	else if(iconName == "fa-grin-tongue"){
	  label = _U("\uf589"); }

	else if(iconName == "fa-chess-bishop"){
	  label = _U("\uf43a"); }

	else if(iconName == "fa-face-grin-wink"){
	  label = _U("\uf58c"); }

	else if(iconName == "fa-grin-wink"){
	  label = _U("\uf58c"); }

	else if(iconName == "fa-ear-deaf"){
	  label = _U("\uf2a4"); }

	else if(iconName == "fa-deaf"){
	  label = _U("\uf2a4"); }

	else if(iconName == "fa-deafness"){
	  label = _U("\uf2a4"); }

	else if(iconName == "fa-hard-of-hearing"){
	  label = _U("\uf2a4"); }

	else if(iconName == "fa-road-circle-check"){
	  label = _U("\ue564"); }

	else if(iconName == "fa-dice-five"){
	  label = _U("\uf523"); }

	else if(iconName == "fa-square-rss"){
	  label = _U("\uf143"); }

	else if(iconName == "fa-rss-square"){
	  label = _U("\uf143"); }

	else if(iconName == "fa-land-mine-on"){
	  label = _U("\ue51b"); }

	else if(iconName == "fa-i-cursor"){
	  label = _U("\uf246"); }

	else if(iconName == "fa-stamp"){
	  label = _U("\uf5bf"); }

	else if(iconName == "fa-stairs"){
	  label = _U("\ue289"); }

	else if(iconName == "fa-hryvnia-sign"){
	  label = _U("\uf6f2"); }

	else if(iconName == "fa-hryvnia"){
	  label = _U("\uf6f2"); }

	else if(iconName == "fa-pills"){
	  label = _U("\uf484"); }

	else if(iconName == "fa-face-grin-wide"){
	  label = _U("\uf581"); }

	else if(iconName == "fa-grin-alt"){
	  label = _U("\uf581"); }

	else if(iconName == "fa-tooth"){
	  label = _U("\uf5c9"); }

	else if(iconName == "fa-bangladeshi-taka-sign"){
	  label = _U("\ue2e6"); }

	else if(iconName == "fa-bicycle"){
	  label = _U("\uf206"); }

	else if(iconName == "fa-staff-snake"){
	  label = _U("\ue579"); }

	else if(iconName == "fa-rod-asclepius"){
	  label = _U("\ue579"); }

	else if(iconName == "fa-rod-snake"){
	  label = _U("\ue579"); }

	else if(iconName == "fa-staff-aesculapius"){
	  label = _U("\ue579"); }

	else if(iconName == "fa-head-side-cough-slash"){
	  label = _U("\ue062"); }

	else if(iconName == "fa-truck-medical"){
	  label = _U("\uf0f9"); }

	else if(iconName == "fa-ambulance"){
	  label = _U("\uf0f9"); }

	else if(iconName == "fa-wheat-awn-circle-exclamation"){
	  label = _U("\ue598"); }

	else if(iconName == "fa-snowman"){
	  label = _U("\uf7d0"); }

	else if(iconName == "fa-mortar-pestle"){
	  label = _U("\uf5a7"); }

	else if(iconName == "fa-road-barrier"){
	  label = _U("\ue562"); }

	else if(iconName == "fa-school"){
	  label = _U("\uf549"); }

	else if(iconName == "fa-igloo"){
	  label = _U("\uf7ae"); }

	else if(iconName == "fa-joint"){
	  label = _U("\uf595"); }

	else if(iconName == "fa-angle-right"){
	  label = _U("\uf105"); }

	else if(iconName == "fa-horse"){
	  label = _U("\uf6f0"); }

	else if(iconName == "fa-notes-medical"){
	  label = _U("\uf481"); }

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

	else if(iconName == "fa-capsules"){
	  label = _U("\uf46b"); }

	else if(iconName == "fa-poo-storm"){
	  label = _U("\uf75a"); }

	else if(iconName == "fa-poo-bolt"){
	  label = _U("\uf75a"); }

	else if(iconName == "fa-face-frown-open"){
	  label = _U("\uf57a"); }

	else if(iconName == "fa-frown-open"){
	  label = _U("\uf57a"); }

	else if(iconName == "fa-hand-point-up"){
	  label = _U("\uf0a6"); }

	else if(iconName == "fa-money-bill"){
	  label = _U("\uf0d6"); }

	else if(iconName == "fa-bookmark"){
	  label = _U("\uf02e"); }

	else if(iconName == "fa-align-justify"){
	  label = _U("\uf039"); }

	else if(iconName == "fa-umbrella-beach"){
	  label = _U("\uf5ca"); }

	else if(iconName == "fa-helmet-un"){
	  label = _U("\ue503"); }

	else if(iconName == "fa-bullseye"){
	  label = _U("\uf140"); }

	else if(iconName == "fa-bacon"){
	  label = _U("\uf7e5"); }

	else if(iconName == "fa-hand-point-down"){
	  label = _U("\uf0a7"); }

	else if(iconName == "fa-arrow-up-from-bracket"){
	  label = _U("\ue09a"); }

	else if(iconName == "fa-folder"){
	  label = _U("\uf07b"); }

	else if(iconName == "fa-folder-blank"){
	  label = _U("\uf07b"); }

	else if(iconName == "fa-file-waveform"){
	  label = _U("\uf478"); }

	else if(iconName == "fa-file-medical-alt"){
	  label = _U("\uf478"); }

	else if(iconName == "fa-radiation"){
	  label = _U("\uf7b9"); }

	else if(iconName == "fa-chart-simple"){
	  label = _U("\ue473"); }

	else if(iconName == "fa-mars-stroke"){
	  label = _U("\uf229"); }

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

	else if(iconName == "fa-pen-clip"){
	  label = _U("\uf305"); }

	else if(iconName == "fa-pen-alt"){
	  label = _U("\uf305"); }

	else if(iconName == "fa-bridge-circle-exclamation"){
	  label = _U("\ue4ca"); }

	else if(iconName == "fa-user"){
	  label = _U("\uf007"); }

	else if(iconName == "fa-school-circle-check"){
	  label = _U("\ue56b"); }

	else if(iconName == "fa-dumpster"){
	  label = _U("\uf793"); }

	else if(iconName == "fa-van-shuttle"){
	  label = _U("\uf5b6"); }

	else if(iconName == "fa-shuttle-van"){
	  label = _U("\uf5b6"); }

	else if(iconName == "fa-building-user"){
	  label = _U("\ue4da"); }

	else if(iconName == "fa-square-caret-left"){
	  label = _U("\uf191"); }

	else if(iconName == "fa-caret-square-left"){
	  label = _U("\uf191"); }

	else if(iconName == "fa-highlighter"){
	  label = _U("\uf591"); }

	else if(iconName == "fa-key"){
	  label = _U("\uf084"); }

	else if(iconName == "fa-bullhorn"){
	  label = _U("\uf0a1"); }

	else if(iconName == "fa-globe"){
	  label = _U("\uf0ac"); }

	else if(iconName == "fa-synagogue"){
	  label = _U("\uf69b"); }

	else if(iconName == "fa-person-half-dress"){
	  label = _U("\ue548"); }

	else if(iconName == "fa-road-bridge"){
	  label = _U("\ue563"); }

	else if(iconName == "fa-location-arrow"){
	  label = _U("\uf124"); }

	else if(iconName == "fa-tablet-button"){
	  label = _U("\uf10a"); }

	else if(iconName == "fa-building-lock"){
	  label = _U("\ue4d6"); }

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

	else if(iconName == "fa-person-circle-minus"){
	  label = _U("\ue540"); }

	else if(iconName == "fa-ban"){
	  label = _U("\uf05e"); }

	else if(iconName == "fa-cancel"){
	  label = _U("\uf05e"); }

	else if(iconName == "fa-camera-rotate"){
	  label = _U("\ue0d8"); }

	else if(iconName == "fa-spray-can-sparkles"){
	  label = _U("\uf5d0"); }

	else if(iconName == "fa-air-freshener"){
	  label = _U("\uf5d0"); }

	else if(iconName == "fa-star"){
	  label = _U("\uf005"); }

	else if(iconName == "fa-repeat"){
	  label = _U("\uf363"); }

	else if(iconName == "fa-cross"){
	  label = _U("\uf654"); }

	else if(iconName == "fa-box"){
	  label = _U("\uf466"); }

	else if(iconName == "fa-venus-mars"){
	  label = _U("\uf228"); }

	else if(iconName == "fa-arrow-pointer"){
	  label = _U("\uf245"); }

	else if(iconName == "fa-mouse-pointer"){
	  label = _U("\uf245"); }

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

	else if(iconName == "fa-spider"){
	  label = _U("\uf717"); }

	else if(iconName == "fa-hands-bound"){
	  label = _U("\ue4f9"); }

	else if(iconName == "fa-file-invoice-dollar"){
	  label = _U("\uf571"); }

	else if(iconName == "fa-plane-circle-exclamation"){
	  label = _U("\ue556"); }

	else if(iconName == "fa-x-ray"){
	  label = _U("\uf497"); }

	else if(iconName == "fa-spell-check"){
	  label = _U("\uf891"); }

	else if(iconName == "fa-slash"){
	  label = _U("\uf715"); }

	else if(iconName == "fa-computer-mouse"){
	  label = _U("\uf8cc"); }

	else if(iconName == "fa-mouse"){
	  label = _U("\uf8cc"); }

	else if(iconName == "fa-arrow-right-to-bracket"){
	  label = _U("\uf090"); }

	else if(iconName == "fa-sign-in"){
	  label = _U("\uf090"); }

	else if(iconName == "fa-shop-slash"){
	  label = _U("\ue070"); }

	else if(iconName == "fa-store-alt-slash"){
	  label = _U("\ue070"); }

	else if(iconName == "fa-server"){
	  label = _U("\uf233"); }

	else if(iconName == "fa-virus-covid-slash"){
	  label = _U("\ue4a9"); }

	else if(iconName == "fa-shop-lock"){
	  label = _U("\ue4a5"); }

	else if(iconName == "fa-hourglass-start"){
	  label = _U("\uf251"); }

	else if(iconName == "fa-hourglass-1"){
	  label = _U("\uf251"); }

	else if(iconName == "fa-blender-phone"){
	  label = _U("\uf6b6"); }

	else if(iconName == "fa-building-wheat"){
	  label = _U("\ue4db"); }

	else if(iconName == "fa-person-breastfeeding"){
	  label = _U("\ue53a"); }

	else if(iconName == "fa-right-to-bracket"){
	  label = _U("\uf2f6"); }

	else if(iconName == "fa-sign-in-alt"){
	  label = _U("\uf2f6"); }

	else if(iconName == "fa-venus"){
	  label = _U("\uf221"); }

	else if(iconName == "fa-passport"){
	  label = _U("\uf5ab"); }

	else if(iconName == "fa-heart-pulse"){
	  label = _U("\uf21e"); }

	else if(iconName == "fa-heartbeat"){
	  label = _U("\uf21e"); }

	else if(iconName == "fa-people-carry-box"){
	  label = _U("\uf4ce"); }

	else if(iconName == "fa-people-carry"){
	  label = _U("\uf4ce"); }

	else if(iconName == "fa-temperature-high"){
	  label = _U("\uf769"); }

	else if(iconName == "fa-microchip"){
	  label = _U("\uf2db"); }

	else if(iconName == "fa-crown"){
	  label = _U("\uf521"); }

	else if(iconName == "fa-weight-hanging"){
	  label = _U("\uf5cd"); }

	else if(iconName == "fa-xmarks-lines"){
	  label = _U("\ue59a"); }

	else if(iconName == "fa-file-prescription"){
	  label = _U("\uf572"); }

	else if(iconName == "fa-weight-scale"){
	  label = _U("\uf496"); }

	else if(iconName == "fa-weight"){
	  label = _U("\uf496"); }

	else if(iconName == "fa-user-group"){
	  label = _U("\uf500"); }

	else if(iconName == "fa-user-friends"){
	  label = _U("\uf500"); }

	else if(iconName == "fa-arrow-up-a-z"){
	  label = _U("\uf15e"); }

	else if(iconName == "fa-sort-alpha-up"){
	  label = _U("\uf15e"); }

	else if(iconName == "fa-chess-knight"){
	  label = _U("\uf441"); }

	else if(iconName == "fa-face-laugh-squint"){
	  label = _U("\uf59b"); }

	else if(iconName == "fa-laugh-squint"){
	  label = _U("\uf59b"); }

	else if(iconName == "fa-wheelchair"){
	  label = _U("\uf193"); }

	else if(iconName == "fa-circle-arrow-up"){
	  label = _U("\uf0aa"); }

	else if(iconName == "fa-arrow-circle-up"){
	  label = _U("\uf0aa"); }

	else if(iconName == "fa-toggle-on"){
	  label = _U("\uf205"); }

	else if(iconName == "fa-person-walking"){
	  label = _U("\uf554"); }

	else if(iconName == "fa-walking"){
	  label = _U("\uf554"); }

	else if(iconName == "fa-fire"){
	  label = _U("\uf06d"); }

	else if(iconName == "fa-bed-pulse"){
	  label = _U("\uf487"); }

	else if(iconName == "fa-procedures"){
	  label = _U("\uf487"); }

	else if(iconName == "fa-shuttle-space"){
	  label = _U("\uf197"); }

	else if(iconName == "fa-space-shuttle"){
	  label = _U("\uf197"); }

	else if(iconName == "fa-face-laugh"){
	  label = _U("\uf599"); }

	else if(iconName == "fa-laugh"){
	  label = _U("\uf599"); }

	else if(iconName == "fa-folder-open"){
	  label = _U("\uf07c"); }

	else if(iconName == "fa-heart-circle-plus"){
	  label = _U("\ue500"); }

	else if(iconName == "fa-code-fork"){
	  label = _U("\ue13b"); }

	else if(iconName == "fa-city"){
	  label = _U("\uf64f"); }

	else if(iconName == "fa-microphone-lines"){
	  label = _U("\uf3c9"); }

	else if(iconName == "fa-microphone-alt"){
	  label = _U("\uf3c9"); }

	else if(iconName == "fa-pepper-hot"){
	  label = _U("\uf816"); }

	else if(iconName == "fa-unlock"){
	  label = _U("\uf09c"); }

	else if(iconName == "fa-colon-sign"){
	  label = _U("\ue140"); }

	else if(iconName == "fa-headset"){
	  label = _U("\uf590"); }

	else if(iconName == "fa-store-slash"){
	  label = _U("\ue071"); }

	else if(iconName == "fa-road-circle-xmark"){
	  label = _U("\ue566"); }

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

	else if(iconName == "fa-bath"){
	  label = _U("\uf2cd"); }

	else if(iconName == "fa-bathtub"){
	  label = _U("\uf2cd"); }

	else if(iconName == "fa-underline"){
	  label = _U("\uf0cd"); }

	else if(iconName == "fa-user-pen"){
	  label = _U("\uf4ff"); }

	else if(iconName == "fa-user-edit"){
	  label = _U("\uf4ff"); }

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

	else if(iconName == "fa-manat-sign"){
	  label = _U("\ue1d5"); }

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

	else if(iconName == "fa-jedi"){
	  label = _U("\uf669"); }

	else if(iconName == "fa-square-poll-vertical"){
	  label = _U("\uf681"); }

	else if(iconName == "fa-poll"){
	  label = _U("\uf681"); }

	else if(iconName == "fa-mug-hot"){
	  label = _U("\uf7b6"); }

	else if(iconName == "fa-car-battery"){
	  label = _U("\uf5df"); }

	else if(iconName == "fa-battery-car"){
	  label = _U("\uf5df"); }

	else if(iconName == "fa-gift"){
	  label = _U("\uf06b"); }

	else if(iconName == "fa-dice-two"){
	  label = _U("\uf528"); }

	else if(iconName == "fa-chess-queen"){
	  label = _U("\uf445"); }

	else if(iconName == "fa-glasses"){
	  label = _U("\uf530"); }

	else if(iconName == "fa-chess-board"){
	  label = _U("\uf43c"); }

	else if(iconName == "fa-building-circle-check"){
	  label = _U("\ue4d2"); }

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

	else if(iconName == "fa-eye-low-vision"){
	  label = _U("\uf2a8"); }

	else if(iconName == "fa-low-vision"){
	  label = _U("\uf2a8"); }

	else if(iconName == "fa-crow"){
	  label = _U("\uf520"); }

	else if(iconName == "fa-sailboat"){
	  label = _U("\ue445"); }

	else if(iconName == "fa-window-restore"){
	  label = _U("\uf2d2"); }

	else if(iconName == "fa-square-plus"){
	  label = _U("\uf0fe"); }

	else if(iconName == "fa-plus-square"){
	  label = _U("\uf0fe"); }

	else if(iconName == "fa-torii-gate"){
	  label = _U("\uf6a1"); }

	else if(iconName == "fa-frog"){
	  label = _U("\uf52e"); }

	else if(iconName == "fa-bucket"){
	  label = _U("\ue4cf"); }

	else if(iconName == "fa-image"){
	  label = _U("\uf03e"); }

	else if(iconName == "fa-microphone"){
	  label = _U("\uf130"); }

	else if(iconName == "fa-cow"){
	  label = _U("\uf6c8"); }

	else if(iconName == "fa-caret-up"){
	  label = _U("\uf0d8"); }

	else if(iconName == "fa-screwdriver"){
	  label = _U("\uf54a"); }

	else if(iconName == "fa-folder-closed"){
	  label = _U("\ue185"); }

	else if(iconName == "fa-house-tsunami"){
	  label = _U("\ue515"); }

	else if(iconName == "fa-square-nfi"){
	  label = _U("\ue576"); }

	else if(iconName == "fa-arrow-up-from-ground-water"){
	  label = _U("\ue4b5"); }

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

	else if(iconName == "fa-monument"){
	  label = _U("\uf5a6"); }

	else if(iconName == "fa-snowplow"){
	  label = _U("\uf7d2"); }

	else if(iconName == "fa-angles-right"){
	  label = _U("\uf101"); }

	else if(iconName == "fa-angle-double-right"){
	  label = _U("\uf101"); }

	else if(iconName == "fa-cannabis"){
	  label = _U("\uf55f"); }

	else if(iconName == "fa-circle-play"){
	  label = _U("\uf144"); }

	else if(iconName == "fa-play-circle"){
	  label = _U("\uf144"); }

	else if(iconName == "fa-tablets"){
	  label = _U("\uf490"); }

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

	else if(iconName == "fa-circle-stop"){
	  label = _U("\uf28d"); }

	else if(iconName == "fa-stop-circle"){
	  label = _U("\uf28d"); }

	else if(iconName == "fa-compass-drafting"){
	  label = _U("\uf568"); }

	else if(iconName == "fa-drafting-compass"){
	  label = _U("\uf568"); }

	else if(iconName == "fa-plate-wheat"){
	  label = _U("\ue55a"); }

	else if(iconName == "fa-icicles"){
	  label = _U("\uf7ad"); }

	else if(iconName == "fa-person-shelter"){
	  label = _U("\ue54f"); }

	else if(iconName == "fa-neuter"){
	  label = _U("\uf22c"); }

	else if(iconName == "fa-id-badge"){
	  label = _U("\uf2c1"); }

	else if(iconName == "fa-marker"){
	  label = _U("\uf5a1"); }

	else if(iconName == "fa-face-laugh-beam"){
	  label = _U("\uf59a"); }

	else if(iconName == "fa-laugh-beam"){
	  label = _U("\uf59a"); }

	else if(iconName == "fa-helicopter-symbol"){
	  label = _U("\ue502"); }

	else if(iconName == "fa-universal-access"){
	  label = _U("\uf29a"); }

	else if(iconName == "fa-circle-chevron-up"){
	  label = _U("\uf139"); }

	else if(iconName == "fa-chevron-circle-up"){
	  label = _U("\uf139"); }

	else if(iconName == "fa-lari-sign"){
	  label = _U("\ue1c8"); }

	else if(iconName == "fa-volcano"){
	  label = _U("\uf770"); }

	else if(iconName == "fa-person-walking-dashed-line-arrow-right"){
	  label = _U("\ue553"); }

	else if(iconName == "fa-sterling-sign"){
	  label = _U("\uf154"); }

	else if(iconName == "fa-gbp"){
	  label = _U("\uf154"); }

	else if(iconName == "fa-pound-sign"){
	  label = _U("\uf154"); }

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

	else if(iconName == "fa-file-import"){
	  label = _U("\uf56f"); }

	else if(iconName == "fa-arrow-right-to-file"){
	  label = _U("\uf56f"); }

	else if(iconName == "fa-square-arrow-up-right"){
	  label = _U("\uf14c"); }

	else if(iconName == "fa-external-link-square"){
	  label = _U("\uf14c"); }

	else if(iconName == "fa-box-open"){
	  label = _U("\uf49e"); }

	else if(iconName == "fa-scroll"){
	  label = _U("\uf70e"); }

	else if(iconName == "fa-spa"){
	  label = _U("\uf5bb"); }

	else if(iconName == "fa-location-pin-lock"){
	  label = _U("\ue51f"); }

	else if(iconName == "fa-pause"){
	  label = _U("\uf04c"); }

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

	else if(iconName == "fa-registered"){
	  label = _U("\uf25d"); }

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

	else if(iconName == "fa-subscript"){
	  label = _U("\uf12c"); }

	else if(iconName == "fa-diamond-turn-right"){
	  label = _U("\uf5eb"); }

	else if(iconName == "fa-directions"){
	  label = _U("\uf5eb"); }

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

	else if(iconName == "fa-smog"){
	  label = _U("\uf75f"); }

	else if(iconName == "fa-crutch"){
	  label = _U("\uf7f7"); }

	else if(iconName == "fa-cloud-arrow-up"){
	  label = _U("\uf0ee"); }

	else if(iconName == "fa-cloud-upload"){
	  label = _U("\uf0ee"); }

	else if(iconName == "fa-cloud-upload-alt"){
	  label = _U("\uf0ee"); }

	else if(iconName == "fa-palette"){
	  label = _U("\uf53f"); }

	else if(iconName == "fa-arrows-turn-right"){
	  label = _U("\ue4c0"); }

	else if(iconName == "fa-vest"){
	  label = _U("\ue085"); }

	else if(iconName == "fa-ferry"){
	  label = _U("\ue4ea"); }

	else if(iconName == "fa-arrows-down-to-people"){
	  label = _U("\ue4b9"); }

	else if(iconName == "fa-seedling"){
	  label = _U("\uf4d8"); }

	else if(iconName == "fa-sprout"){
	  label = _U("\uf4d8"); }

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

	else if(iconName == "fa-group-arrows-rotate"){
	  label = _U("\ue4f6"); }

	else if(iconName == "fa-bowl-food"){
	  label = _U("\ue4c6"); }

	else if(iconName == "fa-candy-cane"){
	  label = _U("\uf786"); }

	else if(iconName == "fa-arrow-down-wide-short"){
	  label = _U("\uf160"); }

	else if(iconName == "fa-sort-amount-asc"){
	  label = _U("\uf160"); }

	else if(iconName == "fa-sort-amount-down"){
	  label = _U("\uf160"); }

	else if(iconName == "fa-cloud-bolt"){
	  label = _U("\uf76c"); }

	else if(iconName == "fa-thunderstorm"){
	  label = _U("\uf76c"); }

	else if(iconName == "fa-text-slash"){
	  label = _U("\uf87d"); }

	else if(iconName == "fa-remove-format"){
	  label = _U("\uf87d"); }

	else if(iconName == "fa-face-smile-wink"){
	  label = _U("\uf4da"); }

	else if(iconName == "fa-smile-wink"){
	  label = _U("\uf4da"); }

	else if(iconName == "fa-file-word"){
	  label = _U("\uf1c2"); }

	else if(iconName == "fa-file-powerpoint"){
	  label = _U("\uf1c4"); }

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

	else if(iconName == "fa-children"){
	  label = _U("\ue4e1"); }

	else if(iconName == "fa-chalkboard"){
	  label = _U("\uf51b"); }

	else if(iconName == "fa-blackboard"){
	  label = _U("\uf51b"); }

	else if(iconName == "fa-user-large-slash"){
	  label = _U("\uf4fa"); }

	else if(iconName == "fa-user-alt-slash"){
	  label = _U("\uf4fa"); }

	else if(iconName == "fa-envelope-open"){
	  label = _U("\uf2b6"); }

	else if(iconName == "fa-handshake-simple-slash"){
	  label = _U("\ue05f"); }

	else if(iconName == "fa-handshake-alt-slash"){
	  label = _U("\ue05f"); }

	else if(iconName == "fa-mattress-pillow"){
	  label = _U("\ue525"); }

	else if(iconName == "fa-guarani-sign"){
	  label = _U("\ue19a"); }

	else if(iconName == "fa-arrows-rotate"){
	  label = _U("\uf021"); }

	else if(iconName == "fa-refresh"){
	  label = _U("\uf021"); }

	else if(iconName == "fa-sync"){
	  label = _U("\uf021"); }

	else if(iconName == "fa-fire-extinguisher"){
	  label = _U("\uf134"); }

	else if(iconName == "fa-cruzeiro-sign"){
	  label = _U("\ue152"); }

	else if(iconName == "fa-greater-than-equal"){
	  label = _U("\uf532"); }

	else if(iconName == "fa-shield-halved"){
	  label = _U("\uf3ed"); }

	else if(iconName == "fa-shield-alt"){
	  label = _U("\uf3ed"); }

	else if(iconName == "fa-book-atlas"){
	  label = _U("\uf558"); }

	else if(iconName == "fa-atlas"){
	  label = _U("\uf558"); }

	else if(iconName == "fa-virus"){
	  label = _U("\ue074"); }

	else if(iconName == "fa-envelope-circle-check"){
	  label = _U("\ue4e8"); }

	else if(iconName == "fa-layer-group"){
	  label = _U("\uf5fd"); }

	else if(iconName == "fa-arrows-to-dot"){
	  label = _U("\ue4be"); }

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

	else if(iconName == "fa-square"){
	  label = _U("\uf0c8"); }

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

	else if(iconName == "fa-church"){
	  label = _U("\uf51d"); }

	else if(iconName == "fa-comments-dollar"){
	  label = _U("\uf653"); }

	else if(iconName == "fa-democrat"){
	  label = _U("\uf747"); }

	else if(iconName == "fa-person-skiing"){
	  label = _U("\uf7c9"); }

	else if(iconName == "fa-skiing"){
	  label = _U("\uf7c9"); }

	else if(iconName == "fa-road-lock"){
	  label = _U("\ue567"); }

	else if(iconName == "fa-temperature-arrow-down"){
	  label = _U("\ue03f"); }

	else if(iconName == "fa-temperature-down"){
	  label = _U("\ue03f"); }

	else if(iconName == "fa-feather-pointed"){
	  label = _U("\uf56b"); }

	else if(iconName == "fa-feather-alt"){
	  label = _U("\uf56b"); }

	else if(iconName == "fa-snowflake"){
	  label = _U("\uf2dc"); }

	else if(iconName == "fa-newspaper"){
	  label = _U("\uf1ea"); }

	else if(iconName == "fa-rectangle-ad"){
	  label = _U("\uf641"); }

	else if(iconName == "fa-ad"){
	  label = _U("\uf641"); }

	else if(iconName == "fa-circle-arrow-right"){
	  label = _U("\uf0a9"); }

	else if(iconName == "fa-arrow-circle-right"){
	  label = _U("\uf0a9"); }

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

	else if(iconName == "fa-person-dress-burst"){
	  label = _U("\ue544"); }

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

	else if(iconName == "fa-face-kiss-wink-heart"){
	  label = _U("\uf598"); }

	else if(iconName == "fa-kiss-wink-heart"){
	  label = _U("\uf598"); }

	else if(iconName == "fa-filter"){
	  label = _U("\uf0b0"); }

	else if(iconName == "fa-file-signature"){
	  label = _U("\uf573"); }

	else if(iconName == "fa-up-down-left-right"){
	  label = _U("\uf0b2"); }

	else if(iconName == "fa-arrows-alt"){
	  label = _U("\uf0b2"); }

	else if(iconName == "fa-house-chimney-user"){
	  label = _U("\ue065"); }

	else if(iconName == "fa-hand-holding-heart"){
	  label = _U("\uf4be"); }

	else if(iconName == "fa-puzzle-piece"){
	  label = _U("\uf12e"); }

	else if(iconName == "fa-money-check"){
	  label = _U("\uf53c"); }

	else if(iconName == "fa-star-half-stroke"){
	  label = _U("\uf5c0"); }

	else if(iconName == "fa-star-half-alt"){
	  label = _U("\uf5c0"); }

	else if(iconName == "fa-code"){
	  label = _U("\uf121"); }

	else if(iconName == "fa-whiskey-glass"){
	  label = _U("\uf7a0"); }

	else if(iconName == "fa-glass-whiskey"){
	  label = _U("\uf7a0"); }

	else if(iconName == "fa-building-circle-exclamation"){
	  label = _U("\ue4d3"); }

	else if(iconName == "fa-magnifying-glass-chart"){
	  label = _U("\ue522"); }

	else if(iconName == "fa-arrow-up-right-from-square"){
	  label = _U("\uf08e"); }

	else if(iconName == "fa-external-link"){
	  label = _U("\uf08e"); }

	else if(iconName == "fa-cubes-stacked"){
	  label = _U("\ue4e6"); }

	else if(iconName == "fa-won-sign"){
	  label = _U("\uf159"); }

	else if(iconName == "fa-krw"){
	  label = _U("\uf159"); }

	else if(iconName == "fa-won"){
	  label = _U("\uf159"); }

	else if(iconName == "fa-virus-covid"){
	  label = _U("\ue4a8"); }

	else if(iconName == "fa-austral-sign"){
	  label = _U("\ue0a9"); }

	else if(iconName == "fa-leaf"){
	  label = _U("\uf06c"); }

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

	else if(iconName == "fa-sack-xmark"){
	  label = _U("\ue56a"); }

	else if(iconName == "fa-file-excel"){
	  label = _U("\uf1c3"); }

	else if(iconName == "fa-file-contract"){
	  label = _U("\uf56c"); }

	else if(iconName == "fa-fish-fins"){
	  label = _U("\ue4f2"); }

	else if(iconName == "fa-building-flag"){
	  label = _U("\ue4d5"); }

	else if(iconName == "fa-face-grin-beam"){
	  label = _U("\uf582"); }

	else if(iconName == "fa-grin-beam"){
	  label = _U("\uf582"); }

	else if(iconName == "fa-object-ungroup"){
	  label = _U("\uf248"); }

	else if(iconName == "fa-poop"){
	  label = _U("\uf619"); }

	else if(iconName == "fa-location-pin"){
	  label = _U("\uf041"); }

	else if(iconName == "fa-map-marker"){
	  label = _U("\uf041"); }

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

	else if(iconName == "fa-eject"){
	  label = _U("\uf052"); }

	else if(iconName == "fa-circle-right"){
	  label = _U("\uf35a"); }

	else if(iconName == "fa-arrow-alt-circle-right"){
	  label = _U("\uf35a"); }

	else if(iconName == "fa-plane-circle-check"){
	  label = _U("\ue555"); }

	else if(iconName == "fa-face-rolling-eyes"){
	  label = _U("\uf5a5"); }

	else if(iconName == "fa-meh-rolling-eyes"){
	  label = _U("\uf5a5"); }

	else if(iconName == "fa-object-group"){
	  label = _U("\uf247"); }

	else if(iconName == "fa-chart-line"){
	  label = _U("\uf201"); }

	else if(iconName == "fa-line-chart"){
	  label = _U("\uf201"); }

	else if(iconName == "fa-mask-ventilator"){
	  label = _U("\ue524"); }

	else if(iconName == "fa-arrow-right"){
	  label = _U("\uf061"); }

	else if(iconName == "fa-signs-post"){
	  label = _U("\uf277"); }

	else if(iconName == "fa-map-signs"){
	  label = _U("\uf277"); }

	else if(iconName == "fa-cash-register"){
	  label = _U("\uf788"); }

	else if(iconName == "fa-person-circle-question"){
	  label = _U("\ue542"); }

	else if(iconName == "fa-tarp"){
	  label = _U("\ue57b"); }

	else if(iconName == "fa-screwdriver-wrench"){
	  label = _U("\uf7d9"); }

	else if(iconName == "fa-tools"){
	  label = _U("\uf7d9"); }

	else if(iconName == "fa-arrows-to-eye"){
	  label = _U("\ue4bf"); }

	else if(iconName == "fa-plug-circle-bolt"){
	  label = _U("\ue55b"); }

	else if(iconName == "fa-heart"){
	  label = _U("\uf004"); }

	else if(iconName == "fa-mars-and-venus"){
	  label = _U("\uf224"); }

	else if(iconName == "fa-house-user"){
	  label = _U("\ue1b0"); }

	else if(iconName == "fa-home-user"){
	  label = _U("\ue1b0"); }

	else if(iconName == "fa-dumpster-fire"){
	  label = _U("\uf794"); }

	else if(iconName == "fa-house-crack"){
	  label = _U("\ue3b1"); }

	else if(iconName == "fa-martini-glass-citrus"){
	  label = _U("\uf561"); }

	else if(iconName == "fa-cocktail"){
	  label = _U("\uf561"); }

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

	else if(iconName == "fa-toilet-paper-slash"){
	  label = _U("\ue072"); }

	else if(iconName == "fa-apple-whole"){
	  label = _U("\uf5d1"); }

	else if(iconName == "fa-apple-alt"){
	  label = _U("\uf5d1"); }

	else if(iconName == "fa-kitchen-set"){
	  label = _U("\ue51a"); }

	else if(iconName == "fa-temperature-quarter"){
	  label = _U("\uf2ca"); }

	else if(iconName == "fa-temperature-1"){
	  label = _U("\uf2ca"); }

	else if(iconName == "fa-thermometer-1"){
	  label = _U("\uf2ca"); }

	else if(iconName == "fa-thermometer-quarter"){
	  label = _U("\uf2ca"); }

	else if(iconName == "fa-cube"){
	  label = _U("\uf1b2"); }

	else if(iconName == "fa-bitcoin-sign"){
	  label = _U("\ue0b4"); }

	else if(iconName == "fa-shield-dog"){
	  label = _U("\ue573"); }

	else if(iconName == "fa-solar-panel"){
	  label = _U("\uf5ba"); }

	else if(iconName == "fa-lock-open"){
	  label = _U("\uf3c1"); }

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

	else if(iconName == "fa-backward-fast"){
	  label = _U("\uf049"); }

	else if(iconName == "fa-fast-backward"){
	  label = _U("\uf049"); }

	else if(iconName == "fa-recycle"){
	  label = _U("\uf1b8"); }

	else if(iconName == "fa-user-astronaut"){
	  label = _U("\uf4fb"); }

	else if(iconName == "fa-plane-slash"){
	  label = _U("\ue069"); }

	else if(iconName == "fa-trademark"){
	  label = _U("\uf25c"); }

	else if(iconName == "fa-basketball"){
	  label = _U("\uf434"); }

	else if(iconName == "fa-basketball-ball"){
	  label = _U("\uf434"); }

	else if(iconName == "fa-satellite-dish"){
	  label = _U("\uf7c0"); }

	else if(iconName == "fa-circle-up"){
	  label = _U("\uf35b"); }

	else if(iconName == "fa-arrow-alt-circle-up"){
	  label = _U("\uf35b"); }

	else if(iconName == "fa-mobile-screen-button"){
	  label = _U("\uf3cd"); }

	else if(iconName == "fa-mobile-alt"){
	  label = _U("\uf3cd"); }

	else if(iconName == "fa-volume-high"){
	  label = _U("\uf028"); }

	else if(iconName == "fa-volume-up"){
	  label = _U("\uf028"); }

	else if(iconName == "fa-users-rays"){
	  label = _U("\ue593"); }

	else if(iconName == "fa-wallet"){
	  label = _U("\uf555"); }

	else if(iconName == "fa-clipboard-check"){
	  label = _U("\uf46c"); }

	else if(iconName == "fa-file-audio"){
	  label = _U("\uf1c7"); }

	else if(iconName == "fa-burger"){
	  label = _U("\uf805"); }

	else if(iconName == "fa-hamburger"){
	  label = _U("\uf805"); }

	else if(iconName == "fa-wrench"){
	  label = _U("\uf0ad"); }

	else if(iconName == "fa-bugs"){
	  label = _U("\ue4d0"); }

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

	else if(iconName == "fa-plane-departure"){
	  label = _U("\uf5b0"); }

	else if(iconName == "fa-handshake-slash"){
	  label = _U("\ue060"); }

	else if(iconName == "fa-book-bookmark"){
	  label = _U("\ue0bb"); }

	else if(iconName == "fa-code-branch"){
	  label = _U("\uf126"); }

	else if(iconName == "fa-hat-cowboy"){
	  label = _U("\uf8c0"); }

	else if(iconName == "fa-bridge"){
	  label = _U("\ue4c8"); }

	else if(iconName == "fa-phone-flip"){
	  label = _U("\uf879"); }

	else if(iconName == "fa-phone-alt"){
	  label = _U("\uf879"); }

	else if(iconName == "fa-truck-front"){
	  label = _U("\ue2b7"); }

	else if(iconName == "fa-cat"){
	  label = _U("\uf6be"); }

	else if(iconName == "fa-anchor-circle-exclamation"){
	  label = _U("\ue4ab"); }

	else if(iconName == "fa-truck-field"){
	  label = _U("\ue58d"); }

	else if(iconName == "fa-route"){
	  label = _U("\uf4d7"); }

	else if(iconName == "fa-clipboard-question"){
	  label = _U("\ue4e3"); }

	else if(iconName == "fa-panorama"){
	  label = _U("\ue209"); }

	else if(iconName == "fa-comment-medical"){
	  label = _U("\uf7f5"); }

	else if(iconName == "fa-teeth-open"){
	  label = _U("\uf62f"); }

	else if(iconName == "fa-file-circle-minus"){
	  label = _U("\ue4ed"); }

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

	else if(iconName == "fa-square-parking"){
	  label = _U("\uf540"); }

	else if(iconName == "fa-parking"){
	  label = _U("\uf540"); }

	else if(iconName == "fa-house-signal"){
	  label = _U("\ue012"); }

	else if(iconName == "fa-bars-progress"){
	  label = _U("\uf828"); }

	else if(iconName == "fa-tasks-alt"){
	  label = _U("\uf828"); }

	else if(iconName == "fa-faucet-drip"){
	  label = _U("\ue006"); }

	else if(iconName == "fa-cart-flatbed"){
	  label = _U("\uf474"); }

	else if(iconName == "fa-dolly-flatbed"){
	  label = _U("\uf474"); }

	else if(iconName == "fa-ban-smoking"){
	  label = _U("\uf54d"); }

	else if(iconName == "fa-smoking-ban"){
	  label = _U("\uf54d"); }

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

	else if(iconName == "fa-audio-description"){
	  label = _U("\uf29e"); }

	else if(iconName == "fa-person-military-to-person"){
	  label = _U("\ue54c"); }

	else if(iconName == "fa-file-shield"){
	  label = _U("\ue4f0"); }

	else if(iconName == "fa-user-slash"){
	  label = _U("\uf506"); }

	else if(iconName == "fa-pen"){
	  label = _U("\uf304"); }

	else if(iconName == "fa-tower-observation"){
	  label = _U("\ue586"); }

	else if(iconName == "fa-file-code"){
	  label = _U("\uf1c9"); }

	else if(iconName == "fa-signal"){
	  label = _U("\uf012"); }

	else if(iconName == "fa-signal-5"){
	  label = _U("\uf012"); }

	else if(iconName == "fa-signal-perfect"){
	  label = _U("\uf012"); }

	else if(iconName == "fa-bus"){
	  label = _U("\uf207"); }

	else if(iconName == "fa-heart-circle-xmark"){
	  label = _U("\ue501"); }

	else if(iconName == "fa-house-chimney"){
	  label = _U("\ue3af"); }

	else if(iconName == "fa-home-lg"){
	  label = _U("\ue3af"); }

	else if(iconName == "fa-window-maximize"){
	  label = _U("\uf2d0"); }

	else if(iconName == "fa-face-frown"){
	  label = _U("\uf119"); }

	else if(iconName == "fa-frown"){
	  label = _U("\uf119"); }

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

	else if(iconName == "fa-scale-unbalanced"){
	  label = _U("\uf515"); }

	else if(iconName == "fa-balance-scale-left"){
	  label = _U("\uf515"); }

	else if(iconName == "fa-sort-up"){
	  label = _U("\uf0de"); }

	else if(iconName == "fa-sort-asc"){
	  label = _U("\uf0de"); }

	else if(iconName == "fa-comment-dots"){
	  label = _U("\uf4ad"); }

	else if(iconName == "fa-commenting"){
	  label = _U("\uf4ad"); }

	else if(iconName == "fa-plant-wilt"){
	  label = _U("\ue5aa"); }

	else if(iconName == "fa-diamond"){
	  label = _U("\uf219"); }

	else if(iconName == "fa-face-grin-squint"){
	  label = _U("\uf585"); }

	else if(iconName == "fa-grin-squint"){
	  label = _U("\uf585"); }

	else if(iconName == "fa-hand-holding-dollar"){
	  label = _U("\uf4c0"); }

	else if(iconName == "fa-hand-holding-usd"){
	  label = _U("\uf4c0"); }

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

	else if(iconName == "fa-arrow-rotate-right"){
	  label = _U("\uf01e"); }

	else if(iconName == "fa-arrow-right-rotate"){
	  label = _U("\uf01e"); }

	else if(iconName == "fa-arrow-rotate-forward"){
	  label = _U("\uf01e"); }

	else if(iconName == "fa-redo"){
	  label = _U("\uf01e"); }

	else if(iconName == "fa-biohazard"){
	  label = _U("\uf780"); }

	else if(iconName == "fa-location-crosshairs"){
	  label = _U("\uf601"); }

	else if(iconName == "fa-location"){
	  label = _U("\uf601"); }

	else if(iconName == "fa-mars-double"){
	  label = _U("\uf227"); }

	else if(iconName == "fa-child-dress"){
	  label = _U("\ue59c"); }

	else if(iconName == "fa-users-between-lines"){
	  label = _U("\ue591"); }

	else if(iconName == "fa-lungs-virus"){
	  label = _U("\ue067"); }

	else if(iconName == "fa-face-grin-tears"){
	  label = _U("\uf588"); }

	else if(iconName == "fa-grin-tears"){
	  label = _U("\uf588"); }

	else if(iconName == "fa-phone"){
	  label = _U("\uf095"); }

	else if(iconName == "fa-calendar-xmark"){
	  label = _U("\uf273"); }

	else if(iconName == "fa-calendar-times"){
	  label = _U("\uf273"); }

	else if(iconName == "fa-child-reaching"){
	  label = _U("\ue59d"); }

	else if(iconName == "fa-head-side-virus"){
	  label = _U("\ue064"); }

	else if(iconName == "fa-user-gear"){
	  label = _U("\uf4fe"); }

	else if(iconName == "fa-user-cog"){
	  label = _U("\uf4fe"); }

	else if(iconName == "fa-arrow-up-1-9"){
	  label = _U("\uf163"); }

	else if(iconName == "fa-sort-numeric-up"){
	  label = _U("\uf163"); }

	else if(iconName == "fa-door-closed"){
	  label = _U("\uf52a"); }

	else if(iconName == "fa-shield-virus"){
	  label = _U("\ue06c"); }

	else if(iconName == "fa-dice-six"){
	  label = _U("\uf526"); }

	else if(iconName == "fa-mosquito-net"){
	  label = _U("\ue52c"); }

	else if(iconName == "fa-bridge-water"){
	  label = _U("\ue4ce"); }

	else if(iconName == "fa-person-booth"){
	  label = _U("\uf756"); }

	else if(iconName == "fa-text-width"){
	  label = _U("\uf035"); }

	else if(iconName == "fa-hat-wizard"){
	  label = _U("\uf6e8"); }

	else if(iconName == "fa-pen-fancy"){
	  label = _U("\uf5ac"); }

	else if(iconName == "fa-person-digging"){
	  label = _U("\uf85e"); }

	else if(iconName == "fa-digging"){
	  label = _U("\uf85e"); }

	else if(iconName == "fa-trash"){
	  label = _U("\uf1f8"); }

	else if(iconName == "fa-gauge-simple"){
	  label = _U("\uf629"); }

	else if(iconName == "fa-gauge-simple-med"){
	  label = _U("\uf629"); }

	else if(iconName == "fa-tachometer-average"){
	  label = _U("\uf629"); }

	else if(iconName == "fa-book-medical"){
	  label = _U("\uf7e6"); }

	else if(iconName == "fa-poo"){
	  label = _U("\uf2fe"); }

	else if(iconName == "fa-quote-right"){
	  label = _U("\uf10e"); }

	else if(iconName == "fa-quote-right-alt"){
	  label = _U("\uf10e"); }

	else if(iconName == "fa-shirt"){
	  label = _U("\uf553"); }

	else if(iconName == "fa-t-shirt"){
	  label = _U("\uf553"); }

	else if(iconName == "fa-tshirt"){
	  label = _U("\uf553"); }

	else if(iconName == "fa-cubes"){
	  label = _U("\uf1b3"); }

	else if(iconName == "fa-divide"){
	  label = _U("\uf529"); }

	else if(iconName == "fa-tenge-sign"){
	  label = _U("\uf7d7"); }

	else if(iconName == "fa-tenge"){
	  label = _U("\uf7d7"); }

	else if(iconName == "fa-headphones"){
	  label = _U("\uf025"); }

	else if(iconName == "fa-hands-holding"){
	  label = _U("\uf4c2"); }

	else if(iconName == "fa-hands-clapping"){
	  label = _U("\ue1a8"); }

	else if(iconName == "fa-republican"){
	  label = _U("\uf75e"); }

	else if(iconName == "fa-arrow-left"){
	  label = _U("\uf060"); }

	else if(iconName == "fa-person-circle-xmark"){
	  label = _U("\ue543"); }

	else if(iconName == "fa-ruler"){
	  label = _U("\uf545"); }

	else if(iconName == "fa-align-left"){
	  label = _U("\uf036"); }

	else if(iconName == "fa-dice-d6"){
	  label = _U("\uf6d1"); }

	else if(iconName == "fa-restroom"){
	  label = _U("\uf7bd"); }

	else if(iconName == "fa-users-viewfinder"){
	  label = _U("\ue595"); }

	else if(iconName == "fa-file-video"){
	  label = _U("\uf1c8"); }

	else if(iconName == "fa-up-right-from-square"){
	  label = _U("\uf35d"); }

	else if(iconName == "fa-external-link-alt"){
	  label = _U("\uf35d"); }

	else if(iconName == "fa-table-cells"){
	  label = _U("\uf00a"); }

	else if(iconName == "fa-th"){
	  label = _U("\uf00a"); }

	else if(iconName == "fa-file-pdf"){
	  label = _U("\uf1c1"); }

	else if(iconName == "fa-book-bible"){
	  label = _U("\uf647"); }

	else if(iconName == "fa-bible"){
	  label = _U("\uf647"); }

	else if(iconName == "fa-suitcase-medical"){
	  label = _U("\uf0fa"); }

	else if(iconName == "fa-medkit"){
	  label = _U("\uf0fa"); }

	else if(iconName == "fa-user-secret"){
	  label = _U("\uf21b"); }

	else if(iconName == "fa-otter"){
	  label = _U("\uf700"); }

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

	else if(iconName == "fa-table-cells-large"){
	  label = _U("\uf009"); }

	else if(iconName == "fa-th-large"){
	  label = _U("\uf009"); }

	else if(iconName == "fa-book-tanakh"){
	  label = _U("\uf827"); }

	else if(iconName == "fa-tanakh"){
	  label = _U("\uf827"); }

	else if(iconName == "fa-phone-volume"){
	  label = _U("\uf2a0"); }

	else if(iconName == "fa-volume-control-phone"){
	  label = _U("\uf2a0"); }

	else if(iconName == "fa-hat-cowboy-side"){
	  label = _U("\uf8c1"); }

	else if(iconName == "fa-clipboard-user"){
	  label = _U("\uf7f3"); }

	else if(iconName == "fa-child"){
	  label = _U("\uf1ae"); }

	else if(iconName == "fa-lira-sign"){
	  label = _U("\uf195"); }

	else if(iconName == "fa-satellite"){
	  label = _U("\uf7bf"); }

	else if(iconName == "fa-plane-lock"){
	  label = _U("\ue558"); }

	else if(iconName == "fa-tag"){
	  label = _U("\uf02b"); }

	else if(iconName == "fa-comment"){
	  label = _U("\uf075"); }

	else if(iconName == "fa-cake-candles"){
	  label = _U("\uf1fd"); }

	else if(iconName == "fa-birthday-cake"){
	  label = _U("\uf1fd"); }

	else if(iconName == "fa-cake"){
	  label = _U("\uf1fd"); }

	else if(iconName == "fa-envelope"){
	  label = _U("\uf0e0"); }

	else if(iconName == "fa-angles-up"){
	  label = _U("\uf102"); }

	else if(iconName == "fa-angle-double-up"){
	  label = _U("\uf102"); }

	else if(iconName == "fa-paperclip"){
	  label = _U("\uf0c6"); }

	else if(iconName == "fa-arrow-right-to-city"){
	  label = _U("\ue4b3"); }

	else if(iconName == "fa-ribbon"){
	  label = _U("\uf4d6"); }

	else if(iconName == "fa-lungs"){
	  label = _U("\uf604"); }

	else if(iconName == "fa-arrow-up-9-1"){
	  label = _U("\uf887"); }

	else if(iconName == "fa-sort-numeric-up-alt"){
	  label = _U("\uf887"); }

	else if(iconName == "fa-litecoin-sign"){
	  label = _U("\ue1d3"); }

	else if(iconName == "fa-border-none"){
	  label = _U("\uf850"); }

	else if(iconName == "fa-circle-nodes"){
	  label = _U("\ue4e2"); }

	else if(iconName == "fa-parachute-box"){
	  label = _U("\uf4cd"); }

	else if(iconName == "fa-indent"){
	  label = _U("\uf03c"); }

	else if(iconName == "fa-truck-field-un"){
	  label = _U("\ue58e"); }

	else if(iconName == "fa-hourglass"){
	  label = _U("\uf254"); }

	else if(iconName == "fa-hourglass-empty"){
	  label = _U("\uf254"); }

	else if(iconName == "fa-mountain"){
	  label = _U("\uf6fc"); }

	else if(iconName == "fa-user-doctor"){
	  label = _U("\uf0f0"); }

	else if(iconName == "fa-user-md"){
	  label = _U("\uf0f0"); }

	else if(iconName == "fa-circle-info"){
	  label = _U("\uf05a"); }

	else if(iconName == "fa-info-circle"){
	  label = _U("\uf05a"); }

	else if(iconName == "fa-cloud-meatball"){
	  label = _U("\uf73b"); }

	else if(iconName == "fa-camera"){
	  label = _U("\uf030"); }

	else if(iconName == "fa-camera-alt"){
	  label = _U("\uf030"); }

	else if(iconName == "fa-square-virus"){
	  label = _U("\ue578"); }

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

	else if(iconName == "fa-hand-holding-droplet"){
	  label = _U("\uf4c1"); }

	else if(iconName == "fa-hand-holding-water"){
	  label = _U("\uf4c1"); }

	else if(iconName == "fa-water"){
	  label = _U("\uf773"); }

	else if(iconName == "fa-calendar-check"){
	  label = _U("\uf274"); }

	else if(iconName == "fa-braille"){
	  label = _U("\uf2a1"); }

	else if(iconName == "fa-prescription-bottle-medical"){
	  label = _U("\uf486"); }

	else if(iconName == "fa-prescription-bottle-alt"){
	  label = _U("\uf486"); }

	else if(iconName == "fa-landmark"){
	  label = _U("\uf66f"); }

	else if(iconName == "fa-truck"){
	  label = _U("\uf0d1"); }

	else if(iconName == "fa-crosshairs"){
	  label = _U("\uf05b"); }

	else if(iconName == "fa-person-cane"){
	  label = _U("\ue53c"); }

	else if(iconName == "fa-tent"){
	  label = _U("\ue57d"); }

	else if(iconName == "fa-vest-patches"){
	  label = _U("\ue086"); }

	else if(iconName == "fa-check-double"){
	  label = _U("\uf560"); }

	else if(iconName == "fa-arrow-down-a-z"){
	  label = _U("\uf15d"); }

	else if(iconName == "fa-sort-alpha-asc"){
	  label = _U("\uf15d"); }

	else if(iconName == "fa-sort-alpha-down"){
	  label = _U("\uf15d"); }

	else if(iconName == "fa-money-bill-wheat"){
	  label = _U("\ue52a"); }

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

	else if(iconName == "fa-hard-drive"){
	  label = _U("\uf0a0"); }

	else if(iconName == "fa-hdd"){
	  label = _U("\uf0a0"); }

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

	else if(iconName == "fa-house-medical-circle-check"){
	  label = _U("\ue511"); }

	else if(iconName == "fa-person-skiing-nordic"){
	  label = _U("\uf7ca"); }

	else if(iconName == "fa-skiing-nordic"){
	  label = _U("\uf7ca"); }

	else if(iconName == "fa-calendar-plus"){
	  label = _U("\uf271"); }

	else if(iconName == "fa-plane-arrival"){
	  label = _U("\uf5af"); }

	else if(iconName == "fa-circle-left"){
	  label = _U("\uf359"); }

	else if(iconName == "fa-arrow-alt-circle-left"){
	  label = _U("\uf359"); }

	else if(iconName == "fa-train-subway"){
	  label = _U("\uf239"); }

	else if(iconName == "fa-subway"){
	  label = _U("\uf239"); }

	else if(iconName == "fa-chart-gantt"){
	  label = _U("\ue0e4"); }

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

	else if(iconName == "fa-dna"){
	  label = _U("\uf471"); }

	else if(iconName == "fa-virus-slash"){
	  label = _U("\ue075"); }

	else if(iconName == "fa-minus"){
	  label = _U("\uf068"); }

	else if(iconName == "fa-subtract"){
	  label = _U("\uf068"); }

	else if(iconName == "fa-chess"){
	  label = _U("\uf439"); }

	else if(iconName == "fa-arrow-left-long"){
	  label = _U("\uf177"); }

	else if(iconName == "fa-long-arrow-left"){
	  label = _U("\uf177"); }

	else if(iconName == "fa-plug-circle-check"){
	  label = _U("\ue55c"); }

	else if(iconName == "fa-street-view"){
	  label = _U("\uf21d"); }

	else if(iconName == "fa-franc-sign"){
	  label = _U("\ue18f"); }

	else if(iconName == "fa-volume-off"){
	  label = _U("\uf026"); }

	else if(iconName == "fa-hands-asl-interpreting"){
	  label = _U("\uf2a3"); }

	else if(iconName == "fa-american-sign-language-interpreting"){
	  label = _U("\uf2a3"); }

	else if(iconName == "fa-asl-interpreting"){
	  label = _U("\uf2a3"); }

	else if(iconName == "fa-hands-american-sign-language-interpreting"){
	  label = _U("\uf2a3"); }

	else if(iconName == "fa-gear"){
	  label = _U("\uf013"); }

	else if(iconName == "fa-cog"){
	  label = _U("\uf013"); }

	else if(iconName == "fa-droplet-slash"){
	  label = _U("\uf5c7"); }

	else if(iconName == "fa-tint-slash"){
	  label = _U("\uf5c7"); }

	else if(iconName == "fa-mosque"){
	  label = _U("\uf678"); }

	else if(iconName == "fa-mosquito"){
	  label = _U("\ue52b"); }

	else if(iconName == "fa-star-of-david"){
	  label = _U("\uf69a"); }

	else if(iconName == "fa-person-military-rifle"){
	  label = _U("\ue54b"); }

	else if(iconName == "fa-cart-shopping"){
	  label = _U("\uf07a"); }

	else if(iconName == "fa-shopping-cart"){
	  label = _U("\uf07a"); }

	else if(iconName == "fa-vials"){
	  label = _U("\uf493"); }

	else if(iconName == "fa-plug-circle-plus"){
	  label = _U("\ue55f"); }

	else if(iconName == "fa-place-of-worship"){
	  label = _U("\uf67f"); }

	else if(iconName == "fa-grip-vertical"){
	  label = _U("\uf58e"); }

	else if(iconName == "fa-arrow-turn-up"){
	  label = _U("\uf148"); }

	else if(iconName == "fa-level-up"){
	  label = _U("\uf148"); }

	else if(iconName == "fa-square-root-variable"){
	  label = _U("\uf698"); }

	else if(iconName == "fa-square-root-alt"){
	  label = _U("\uf698"); }

	else if(iconName == "fa-clock"){
	  label = _U("\uf017"); }

	else if(iconName == "fa-clock-four"){
	  label = _U("\uf017"); }

	else if(iconName == "fa-backward-step"){
	  label = _U("\uf048"); }

	else if(iconName == "fa-step-backward"){
	  label = _U("\uf048"); }

	else if(iconName == "fa-pallet"){
	  label = _U("\uf482"); }

	else if(iconName == "fa-faucet"){
	  label = _U("\ue005"); }

	else if(iconName == "fa-baseball-bat-ball"){
	  label = _U("\uf432"); }

	else if(iconName == "fa-timeline"){
	  label = _U("\ue29c"); }

	else if(iconName == "fa-keyboard"){
	  label = _U("\uf11c"); }

	else if(iconName == "fa-caret-down"){
	  label = _U("\uf0d7"); }

	else if(iconName == "fa-house-chimney-medical"){
	  label = _U("\uf7f2"); }

	else if(iconName == "fa-clinic-medical"){
	  label = _U("\uf7f2"); }

	else if(iconName == "fa-temperature-three-quarters"){
	  label = _U("\uf2c8"); }

	else if(iconName == "fa-temperature-3"){
	  label = _U("\uf2c8"); }

	else if(iconName == "fa-thermometer-3"){
	  label = _U("\uf2c8"); }

	else if(iconName == "fa-thermometer-three-quarters"){
	  label = _U("\uf2c8"); }

	else if(iconName == "fa-mobile-screen"){
	  label = _U("\uf3cf"); }

	else if(iconName == "fa-mobile-android-alt"){
	  label = _U("\uf3cf"); }

	else if(iconName == "fa-plane-up"){
	  label = _U("\ue22d"); }

	else if(iconName == "fa-piggy-bank"){
	  label = _U("\uf4d3"); }

	else if(iconName == "fa-battery-half"){
	  label = _U("\uf242"); }

	else if(iconName == "fa-battery-3"){
	  label = _U("\uf242"); }

	else if(iconName == "fa-mountain-city"){
	  label = _U("\ue52e"); }

	else if(iconName == "fa-coins"){
	  label = _U("\uf51e"); }

	else if(iconName == "fa-khanda"){
	  label = _U("\uf66d"); }

	else if(iconName == "fa-sliders"){
	  label = _U("\uf1de"); }

	else if(iconName == "fa-sliders-h"){
	  label = _U("\uf1de"); }

	else if(iconName == "fa-folder-tree"){
	  label = _U("\uf802"); }

	else if(iconName == "fa-network-wired"){
	  label = _U("\uf6ff"); }

	else if(iconName == "fa-map-pin"){
	  label = _U("\uf276"); }

	else if(iconName == "fa-hamsa"){
	  label = _U("\uf665"); }

	else if(iconName == "fa-cent-sign"){
	  label = _U("\ue3f5"); }

	else if(iconName == "fa-flask"){
	  label = _U("\uf0c3"); }

	else if(iconName == "fa-person-pregnant"){
	  label = _U("\ue31e"); }

	else if(iconName == "fa-wand-sparkles"){
	  label = _U("\uf72b"); }

	else if(iconName == "fa-ellipsis-vertical"){
	  label = _U("\uf142"); }

	else if(iconName == "fa-ellipsis-v"){
	  label = _U("\uf142"); }

	else if(iconName == "fa-ticket"){
	  label = _U("\uf145"); }

	else if(iconName == "fa-power-off"){
	  label = _U("\uf011"); }

	else if(iconName == "fa-right-long"){
	  label = _U("\uf30b"); }

	else if(iconName == "fa-long-arrow-alt-right"){
	  label = _U("\uf30b"); }

	else if(iconName == "fa-flag-usa"){
	  label = _U("\uf74d"); }

	else if(iconName == "fa-laptop-file"){
	  label = _U("\ue51d"); }

	else if(iconName == "fa-tty"){
	  label = _U("\uf1e4"); }

	else if(iconName == "fa-teletype"){
	  label = _U("\uf1e4"); }

	else if(iconName == "fa-diagram-next"){
	  label = _U("\ue476"); }

	else if(iconName == "fa-person-rifle"){
	  label = _U("\ue54e"); }

	else if(iconName == "fa-house-medical-circle-exclamation"){
	  label = _U("\ue512"); }

	else if(iconName == "fa-closed-captioning"){
	  label = _U("\uf20a"); }

	else if(iconName == "fa-person-hiking"){
	  label = _U("\uf6ec"); }

	else if(iconName == "fa-hiking"){
	  label = _U("\uf6ec"); }

	else if(iconName == "fa-venus-double"){
	  label = _U("\uf226"); }

	else if(iconName == "fa-images"){
	  label = _U("\uf302"); }

	else if(iconName == "fa-calculator"){
	  label = _U("\uf1ec"); }

	else if(iconName == "fa-people-pulling"){
	  label = _U("\ue535"); }

	else if(iconName == "fa-cable-car"){
	  label = _U("\uf7da"); }

	else if(iconName == "fa-tram"){
	  label = _U("\uf7da"); }

	else if(iconName == "fa-cloud-rain"){
	  label = _U("\uf73d"); }

	else if(iconName == "fa-building-circle-xmark"){
	  label = _U("\ue4d4"); }

	else if(iconName == "fa-ship"){
	  label = _U("\uf21a"); }

	else if(iconName == "fa-arrows-down-to-line"){
	  label = _U("\ue4b8"); }

	else if(iconName == "fa-download"){
	  label = _U("\uf019"); }

	else if(iconName == "fa-face-grin"){
	  label = _U("\uf580"); }

	else if(iconName == "fa-grin"){
	  label = _U("\uf580"); }

	else if(iconName == "fa-delete-left"){
	  label = _U("\uf55a"); }

	else if(iconName == "fa-backspace"){
	  label = _U("\uf55a"); }

	else if(iconName == "fa-eye-dropper"){
	  label = _U("\uf1fb"); }

	else if(iconName == "fa-eye-dropper-empty"){
	  label = _U("\uf1fb"); }

	else if(iconName == "fa-eyedropper"){
	  label = _U("\uf1fb"); }

	else if(iconName == "fa-file-circle-check"){
	  label = _U("\ue5a0"); }

	else if(iconName == "fa-forward"){
	  label = _U("\uf04e"); }

	else if(iconName == "fa-mobile"){
	  label = _U("\uf3ce"); }

	else if(iconName == "fa-mobile-android"){
	  label = _U("\uf3ce"); }

	else if(iconName == "fa-mobile-phone"){
	  label = _U("\uf3ce"); }

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

	else if(iconName == "fa-outdent"){
	  label = _U("\uf03b"); }

	else if(iconName == "fa-dedent"){
	  label = _U("\uf03b"); }

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

	else if(iconName == "fa-calendar-week"){
	  label = _U("\uf784"); }

	else if(iconName == "fa-laptop-medical"){
	  label = _U("\uf812"); }

	else if(iconName == "fa-file-medical"){
	  label = _U("\uf477"); }

	else if(iconName == "fa-dice-one"){
	  label = _U("\uf525"); }

	else if(iconName == "fa-kiwi-bird"){
	  label = _U("\uf535"); }

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

	else if(iconName == "fa-mill-sign"){
	  label = _U("\ue1ed"); }

	else if(iconName == "fa-bowl-rice"){
	  label = _U("\ue2eb"); }

	else if(iconName == "fa-skull"){
	  label = _U("\uf54c"); }

	else if(iconName == "fa-tower-broadcast"){
	  label = _U("\uf519"); }

	else if(iconName == "fa-broadcast-tower"){
	  label = _U("\uf519"); }

	else if(iconName == "fa-truck-pickup"){
	  label = _U("\uf63c"); }

	else if(iconName == "fa-up-long"){
	  label = _U("\uf30c"); }

	else if(iconName == "fa-long-arrow-alt-up"){
	  label = _U("\uf30c"); }

	else if(iconName == "fa-stop"){
	  label = _U("\uf04d"); }

	else if(iconName == "fa-code-merge"){
	  label = _U("\uf387"); }

	else if(iconName == "fa-upload"){
	  label = _U("\uf093"); }

	else if(iconName == "fa-hurricane"){
	  label = _U("\uf751"); }

	else if(iconName == "fa-mound"){
	  label = _U("\ue52d"); }

	else if(iconName == "fa-toilet-portable"){
	  label = _U("\ue583"); }

	else if(iconName == "fa-compact-disc"){
	  label = _U("\uf51f"); }

	else if(iconName == "fa-file-arrow-down"){
	  label = _U("\uf56d"); }

	else if(iconName == "fa-file-download"){
	  label = _U("\uf56d"); }

	else if(iconName == "fa-caravan"){
	  label = _U("\uf8ff"); }

	else if(iconName == "fa-shield-cat"){
	  label = _U("\ue572"); }

	else if(iconName == "fa-bolt"){
	  label = _U("\uf0e7"); }

	else if(iconName == "fa-zap"){
	  label = _U("\uf0e7"); }

	else if(iconName == "fa-glass-water"){
	  label = _U("\ue4f4"); }

	else if(iconName == "fa-oil-well"){
	  label = _U("\ue532"); }

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

	else if(iconName == "fa-ruble-sign"){
	  label = _U("\uf158"); }

	else if(iconName == "fa-rouble"){
	  label = _U("\uf158"); }

	else if(iconName == "fa-rub"){
	  label = _U("\uf158"); }

	else if(iconName == "fa-ruble"){
	  label = _U("\uf158"); }

	else if(iconName == "fa-sun"){
	  label = _U("\uf185"); }

	else if(iconName == "fa-guitar"){
	  label = _U("\uf7a6"); }

	else if(iconName == "fa-face-laugh-wink"){
	  label = _U("\uf59c"); }

	else if(iconName == "fa-laugh-wink"){
	  label = _U("\uf59c"); }

	else if(iconName == "fa-horse-head"){
	  label = _U("\uf7ab"); }

	else if(iconName == "fa-bore-hole"){
	  label = _U("\ue4c3"); }

	else if(iconName == "fa-industry"){
	  label = _U("\uf275"); }

	else if(iconName == "fa-circle-down"){
	  label = _U("\uf358"); }

	else if(iconName == "fa-arrow-alt-circle-down"){
	  label = _U("\uf358"); }

	else if(iconName == "fa-arrows-turn-to-dots"){
	  label = _U("\ue4c1"); }

	else if(iconName == "fa-florin-sign"){
	  label = _U("\ue184"); }

	else if(iconName == "fa-arrow-down-short-wide"){
	  label = _U("\uf884"); }

	else if(iconName == "fa-sort-amount-desc"){
	  label = _U("\uf884"); }

	else if(iconName == "fa-sort-amount-down-alt"){
	  label = _U("\uf884"); }

	else if(iconName == "fa-angle-down"){
	  label = _U("\uf107"); }

	else if(iconName == "fa-car-tunnel"){
	  label = _U("\ue4de"); }

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

	else if(iconName == "fa-anchor-circle-xmark"){
	  label = _U("\ue4ac"); }

	else if(iconName == "fa-ellipsis"){
	  label = _U("\uf141"); }

	else if(iconName == "fa-ellipsis-h"){
	  label = _U("\uf141"); }

	else if(iconName == "fa-chess-pawn"){
	  label = _U("\uf443"); }

	else if(iconName == "fa-kit-medical"){
	  label = _U("\uf479"); }

	else if(iconName == "fa-first-aid"){
	  label = _U("\uf479"); }

	else if(iconName == "fa-person-through-window"){
	  label = _U("\ue5a9"); }

	else if(iconName == "fa-toolbox"){
	  label = _U("\uf552"); }

	else if(iconName == "fa-hands-holding-circle"){
	  label = _U("\ue4fb"); }

	else if(iconName == "fa-bug"){
	  label = _U("\uf188"); }

	else if(iconName == "fa-credit-card"){
	  label = _U("\uf09d"); }

	else if(iconName == "fa-credit-card-alt"){
	  label = _U("\uf09d"); }

	else if(iconName == "fa-car"){
	  label = _U("\uf1b9"); }

	else if(iconName == "fa-automobile"){
	  label = _U("\uf1b9"); }

	else if(iconName == "fa-hand-holding-hand"){
	  label = _U("\ue4f7"); }

	else if(iconName == "fa-book-open-reader"){
	  label = _U("\uf5da"); }

	else if(iconName == "fa-book-reader"){
	  label = _U("\uf5da"); }

	else if(iconName == "fa-mountain-sun"){
	  label = _U("\ue52f"); }

	else if(iconName == "fa-arrows-left-right-to-line"){
	  label = _U("\ue4ba"); }

	else if(iconName == "fa-dice-d20"){
	  label = _U("\uf6cf"); }

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

	else if(iconName == "fa-bed"){
	  label = _U("\uf236"); }

	else if(iconName == "fa-square-h"){
	  label = _U("\uf0fd"); }

	else if(iconName == "fa-h-square"){
	  label = _U("\uf0fd"); }

	else if(iconName == "fa-podcast"){
	  label = _U("\uf2ce"); }

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

	else if(iconName == "fa-superscript"){
	  label = _U("\uf12b"); }

	else if(iconName == "fa-plug-circle-xmark"){
	  label = _U("\ue560"); }

	else if(iconName == "fa-star-of-life"){
	  label = _U("\uf621"); }

	else if(iconName == "fa-phone-slash"){
	  label = _U("\uf3dd"); }

	else if(iconName == "fa-paint-roller"){
	  label = _U("\uf5aa"); }

	else if(iconName == "fa-handshake-angle"){
	  label = _U("\uf4c4"); }

	else if(iconName == "fa-hands-helping"){
	  label = _U("\uf4c4"); }

	else if(iconName == "fa-location-dot"){
	  label = _U("\uf3c5"); }

	else if(iconName == "fa-map-marker-alt"){
	  label = _U("\uf3c5"); }

	else if(iconName == "fa-file"){
	  label = _U("\uf15b"); }

	else if(iconName == "fa-person-swimming"){
	  label = _U("\uf5c4"); }

	else if(iconName == "fa-swimmer"){
	  label = _U("\uf5c4"); }

	else if(iconName == "fa-arrow-down"){
	  label = _U("\uf063"); }

	else if(iconName == "fa-droplet"){
	  label = _U("\uf043"); }

	else if(iconName == "fa-tint"){
	  label = _U("\uf043"); }

	else if(iconName == "fa-eraser"){
	  label = _U("\uf12d"); }

	else if(iconName == "fa-earth-americas"){
	  label = _U("\uf57d"); }

	else if(iconName == "fa-earth"){
	  label = _U("\uf57d"); }

	else if(iconName == "fa-earth-america"){
	  label = _U("\uf57d"); }

	else if(iconName == "fa-globe-americas"){
	  label = _U("\uf57d"); }

	else if(iconName == "fa-person-burst"){
	  label = _U("\ue53b"); }

	else if(iconName == "fa-dove"){
	  label = _U("\uf4ba"); }

	else if(iconName == "fa-battery-empty"){
	  label = _U("\uf244"); }

	else if(iconName == "fa-battery-0"){
	  label = _U("\uf244"); }

	else if(iconName == "fa-socks"){
	  label = _U("\uf696"); }

	else if(iconName == "fa-inbox"){
	  label = _U("\uf01c"); }

	else if(iconName == "fa-section"){
	  label = _U("\ue447"); }

	else if(iconName == "fa-gauge-high"){
	  label = _U("\uf625"); }

	else if(iconName == "fa-tachometer-alt"){
	  label = _U("\uf625"); }

	else if(iconName == "fa-tachometer-alt-fast"){
	  label = _U("\uf625"); }

	else if(iconName == "fa-envelope-open-text"){
	  label = _U("\uf658"); }

	else if(iconName == "fa-hospital"){
	  label = _U("\uf0f8"); }

	else if(iconName == "fa-hospital-alt"){
	  label = _U("\uf0f8"); }

	else if(iconName == "fa-hospital-wide"){
	  label = _U("\uf0f8"); }

	else if(iconName == "fa-wine-bottle"){
	  label = _U("\uf72f"); }

	else if(iconName == "fa-chess-rook"){
	  label = _U("\uf447"); }

	else if(iconName == "fa-bars-staggered"){
	  label = _U("\uf550"); }

	else if(iconName == "fa-reorder"){
	  label = _U("\uf550"); }

	else if(iconName == "fa-stream"){
	  label = _U("\uf550"); }

	else if(iconName == "fa-dharmachakra"){
	  label = _U("\uf655"); }

	else if(iconName == "fa-hotdog"){
	  label = _U("\uf80f"); }

	else if(iconName == "fa-person-walking-with-cane"){
	  label = _U("\uf29d"); }

	else if(iconName == "fa-blind"){
	  label = _U("\uf29d"); }

	else if(iconName == "fa-drum"){
	  label = _U("\uf569"); }

	else if(iconName == "fa-ice-cream"){
	  label = _U("\uf810"); }

	else if(iconName == "fa-heart-circle-bolt"){
	  label = _U("\ue4fc"); }

	else if(iconName == "fa-fax"){
	  label = _U("\uf1ac"); }

	else if(iconName == "fa-paragraph"){
	  label = _U("\uf1dd"); }

	else if(iconName == "fa-check-to-slot"){
	  label = _U("\uf772"); }

	else if(iconName == "fa-vote-yea"){
	  label = _U("\uf772"); }

	else if(iconName == "fa-star-half"){
	  label = _U("\uf089"); }

	else if(iconName == "fa-boxes-stacked"){
	  label = _U("\uf468"); }

	else if(iconName == "fa-boxes"){
	  label = _U("\uf468"); }

	else if(iconName == "fa-boxes-alt"){
	  label = _U("\uf468"); }

	else if(iconName == "fa-link"){
	  label = _U("\uf0c1"); }

	else if(iconName == "fa-chain"){
	  label = _U("\uf0c1"); }

	else if(iconName == "fa-ear-listen"){
	  label = _U("\uf2a2"); }

	else if(iconName == "fa-assistive-listening-systems"){
	  label = _U("\uf2a2"); }

	else if(iconName == "fa-tree-city"){
	  label = _U("\ue587"); }

	else if(iconName == "fa-play"){
	  label = _U("\uf04b"); }

	else if(iconName == "fa-font"){
	  label = _U("\uf031"); }

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

	else if(iconName == "fa-trash-can-arrow-up"){
	  label = _U("\uf82a"); }

	else if(iconName == "fa-trash-restore-alt"){
	  label = _U("\uf82a"); }

	else if(iconName == "fa-naira-sign"){
	  label = _U("\ue1f6"); }

	else if(iconName == "fa-cart-arrow-down"){
	  label = _U("\uf218"); }

	else if(iconName == "fa-walkie-talkie"){
	  label = _U("\uf8ef"); }

	else if(iconName == "fa-file-pen"){
	  label = _U("\uf31c"); }

	else if(iconName == "fa-file-edit"){
	  label = _U("\uf31c"); }

	else if(iconName == "fa-receipt"){
	  label = _U("\uf543"); }

	else if(iconName == "fa-square-pen"){
	  label = _U("\uf14b"); }

	else if(iconName == "fa-pen-square"){
	  label = _U("\uf14b"); }

	else if(iconName == "fa-pencil-square"){
	  label = _U("\uf14b"); }

	else if(iconName == "fa-suitcase-rolling"){
	  label = _U("\uf5c1"); }

	else if(iconName == "fa-person-circle-exclamation"){
	  label = _U("\ue53f"); }

	else if(iconName == "fa-chevron-down"){
	  label = _U("\uf078"); }

	else if(iconName == "fa-battery-full"){
	  label = _U("\uf240"); }

	else if(iconName == "fa-battery"){
	  label = _U("\uf240"); }

	else if(iconName == "fa-battery-5"){
	  label = _U("\uf240"); }

	else if(iconName == "fa-skull-crossbones"){
	  label = _U("\uf714"); }

	else if(iconName == "fa-code-compare"){
	  label = _U("\ue13a"); }

	else if(iconName == "fa-list-ul"){
	  label = _U("\uf0ca"); }

	else if(iconName == "fa-list-dots"){
	  label = _U("\uf0ca"); }

	else if(iconName == "fa-school-lock"){
	  label = _U("\ue56f"); }

	else if(iconName == "fa-tower-cell"){
	  label = _U("\ue585"); }

	else if(iconName == "fa-down-long"){
	  label = _U("\uf309"); }

	else if(iconName == "fa-long-arrow-alt-down"){
	  label = _U("\uf309"); }

	else if(iconName == "fa-ranking-star"){
	  label = _U("\ue561"); }

	else if(iconName == "fa-chess-king"){
	  label = _U("\uf43f"); }

	else if(iconName == "fa-person-harassing"){
	  label = _U("\ue549"); }

	else if(iconName == "fa-brazilian-real-sign"){
	  label = _U("\ue46c"); }

	else if(iconName == "fa-landmark-dome"){
	  label = _U("\uf752"); }

	else if(iconName == "fa-landmark-alt"){
	  label = _U("\uf752"); }

	else if(iconName == "fa-arrow-up"){
	  label = _U("\uf062"); }

	else if(iconName == "fa-tv"){
	  label = _U("\uf26c"); }

	else if(iconName == "fa-television"){
	  label = _U("\uf26c"); }

	else if(iconName == "fa-tv-alt"){
	  label = _U("\uf26c"); }

	else if(iconName == "fa-shrimp"){
	  label = _U("\ue448"); }

	else if(iconName == "fa-list-check"){
	  label = _U("\uf0ae"); }

	else if(iconName == "fa-tasks"){
	  label = _U("\uf0ae"); }

	else if(iconName == "fa-jug-detergent"){
	  label = _U("\ue519"); }

	else if(iconName == "fa-circle-user"){
	  label = _U("\uf2bd"); }

	else if(iconName == "fa-user-circle"){
	  label = _U("\uf2bd"); }

	else if(iconName == "fa-user-shield"){
	  label = _U("\uf505"); }

	else if(iconName == "fa-wind"){
	  label = _U("\uf72e"); }

	else if(iconName == "fa-car-burst"){
	  label = _U("\uf5e1"); }

	else if(iconName == "fa-car-crash"){
	  label = _U("\uf5e1"); }

	else if(iconName == "fa-person-snowboarding"){
	  label = _U("\uf7ce"); }

	else if(iconName == "fa-snowboarding"){
	  label = _U("\uf7ce"); }

	else if(iconName == "fa-truck-fast"){
	  label = _U("\uf48b"); }

	else if(iconName == "fa-shipping-fast"){
	  label = _U("\uf48b"); }

	else if(iconName == "fa-fish"){
	  label = _U("\uf578"); }

	else if(iconName == "fa-user-graduate"){
	  label = _U("\uf501"); }

	else if(iconName == "fa-circle-half-stroke"){
	  label = _U("\uf042"); }

	else if(iconName == "fa-adjust"){
	  label = _U("\uf042"); }

	else if(iconName == "fa-clapperboard"){
	  label = _U("\ue131"); }

	else if(iconName == "fa-circle-radiation"){
	  label = _U("\uf7ba"); }

	else if(iconName == "fa-radiation-alt"){
	  label = _U("\uf7ba"); }

	else if(iconName == "fa-baseball"){
	  label = _U("\uf433"); }

	else if(iconName == "fa-baseball-ball"){
	  label = _U("\uf433"); }

	else if(iconName == "fa-jet-fighter-up"){
	  label = _U("\ue518"); }

	else if(iconName == "fa-diagram-project"){
	  label = _U("\uf542"); }

	else if(iconName == "fa-project-diagram"){
	  label = _U("\uf542"); }

	else if(iconName == "fa-copy"){
	  label = _U("\uf0c5"); }

	else if(iconName == "fa-volume-xmark"){
	  label = _U("\uf6a9"); }

	else if(iconName == "fa-volume-mute"){
	  label = _U("\uf6a9"); }

	else if(iconName == "fa-volume-times"){
	  label = _U("\uf6a9"); }

	else if(iconName == "fa-hand-sparkles"){
	  label = _U("\ue05d"); }

	else if(iconName == "fa-grip"){
	  label = _U("\uf58d"); }

	else if(iconName == "fa-grip-horizontal"){
	  label = _U("\uf58d"); }

	else if(iconName == "fa-share-from-square"){
	  label = _U("\uf14d"); }

	else if(iconName == "fa-share-square"){
	  label = _U("\uf14d"); }

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

	else if(iconName == "fa-expand"){
	  label = _U("\uf065"); }

	else if(iconName == "fa-computer"){
	  label = _U("\ue4e5"); }

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

	else if(iconName == "fa-arrows-up-down-left-right"){
	  label = _U("\uf047"); }

	else if(iconName == "fa-arrows"){
	  label = _U("\uf047"); }

	else if(iconName == "fa-chalkboard-user"){
	  label = _U("\uf51c"); }

	else if(iconName == "fa-chalkboard-teacher"){
	  label = _U("\uf51c"); }

	else if(iconName == "fa-peso-sign"){
	  label = _U("\ue222"); }

	else if(iconName == "fa-building-shield"){
	  label = _U("\ue4d8"); }

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

	else if(iconName == "fa-trash-arrow-up"){
	  label = _U("\uf829"); }

	else if(iconName == "fa-trash-restore"){
	  label = _U("\uf829"); }

	else if(iconName == "fa-arrow-down-up-lock"){
	  label = _U("\ue4b0"); }

	else if(iconName == "fa-lines-leaning"){
	  label = _U("\ue51e"); }

	else if(iconName == "fa-ruler-combined"){
	  label = _U("\uf546"); }

	else if(iconName == "fa-copyright"){
	  label = _U("\uf1f9"); }

	else if(iconName == "fa-blender"){
	  label = _U("\uf517"); }

	else if(iconName == "fa-teeth"){
	  label = _U("\uf62e"); }

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

	else if(iconName == "fa-map"){
	  label = _U("\uf279"); }

	else if(iconName == "fa-rocket"){
	  label = _U("\uf135"); }

	else if(iconName == "fa-photo-film"){
	  label = _U("\uf87c"); }

	else if(iconName == "fa-photo-video"){
	  label = _U("\uf87c"); }

	else if(iconName == "fa-folder-minus"){
	  label = _U("\uf65d"); }

	else if(iconName == "fa-store"){
	  label = _U("\uf54e"); }

	else if(iconName == "fa-arrow-trend-up"){
	  label = _U("\ue098"); }

	else if(iconName == "fa-plug-circle-minus"){
	  label = _U("\ue55e"); }

	else if(iconName == "fa-sign-hanging"){
	  label = _U("\uf4d9"); }

	else if(iconName == "fa-sign"){
	  label = _U("\uf4d9"); }

	else if(iconName == "fa-bezier-curve"){
	  label = _U("\uf55b"); }

	else if(iconName == "fa-bell-slash"){
	  label = _U("\uf1f6"); }

	else if(iconName == "fa-tablet"){
	  label = _U("\uf3fb"); }

	else if(iconName == "fa-tablet-android"){
	  label = _U("\uf3fb"); }

	else if(iconName == "fa-school-flag"){
	  label = _U("\ue56e"); }

	else if(iconName == "fa-fill"){
	  label = _U("\uf575"); }

	else if(iconName == "fa-angle-up"){
	  label = _U("\uf106"); }

	else if(iconName == "fa-drumstick-bite"){
	  label = _U("\uf6d7"); }

	else if(iconName == "fa-holly-berry"){
	  label = _U("\uf7aa"); }

	else if(iconName == "fa-chevron-left"){
	  label = _U("\uf053"); }

	else if(iconName == "fa-bacteria"){
	  label = _U("\ue059"); }

	else if(iconName == "fa-hand-lizard"){
	  label = _U("\uf258"); }

	else if(iconName == "fa-notdef"){
	  label = _U("\ue1fe"); }

	else if(iconName == "fa-disease"){
	  label = _U("\uf7fa"); }

	else if(iconName == "fa-briefcase-medical"){
	  label = _U("\uf469"); }

	else if(iconName == "fa-genderless"){
	  label = _U("\uf22d"); }

	else if(iconName == "fa-chevron-right"){
	  label = _U("\uf054"); }

	else if(iconName == "fa-retweet"){
	  label = _U("\uf079"); }

	else if(iconName == "fa-car-rear"){
	  label = _U("\uf5de"); }

	else if(iconName == "fa-car-alt"){
	  label = _U("\uf5de"); }

	else if(iconName == "fa-pump-soap"){
	  label = _U("\ue06b"); }

	else if(iconName == "fa-video-slash"){
	  label = _U("\uf4e2"); }

	else if(iconName == "fa-battery-quarter"){
	  label = _U("\uf243"); }

	else if(iconName == "fa-battery-2"){
	  label = _U("\uf243"); }

	else if(iconName == "fa-radio"){
	  label = _U("\uf8d7"); }

	else if(iconName == "fa-baby-carriage"){
	  label = _U("\uf77d"); }

	else if(iconName == "fa-carriage-baby"){
	  label = _U("\uf77d"); }

	else if(iconName == "fa-traffic-light"){
	  label = _U("\uf637"); }

	else if(iconName == "fa-thermometer"){
	  label = _U("\uf491"); }

	else if(iconName == "fa-vr-cardboard"){
	  label = _U("\uf729"); }

	else if(iconName == "fa-hand-middle-finger"){
	  label = _U("\uf806"); }

	else if(iconName == "fa-truck-moving"){
	  label = _U("\uf4df"); }

	else if(iconName == "fa-glass-water-droplet"){
	  label = _U("\ue4f5"); }

	else if(iconName == "fa-display"){
	  label = _U("\ue163"); }

	else if(iconName == "fa-face-smile"){
	  label = _U("\uf118"); }

	else if(iconName == "fa-smile"){
	  label = _U("\uf118"); }

	else if(iconName == "fa-thumbtack"){
	  label = _U("\uf08d"); }

	else if(iconName == "fa-thumb-tack"){
	  label = _U("\uf08d"); }

	else if(iconName == "fa-trophy"){
	  label = _U("\uf091"); }

	else if(iconName == "fa-person-praying"){
	  label = _U("\uf683"); }

	else if(iconName == "fa-pray"){
	  label = _U("\uf683"); }

	else if(iconName == "fa-hammer"){
	  label = _U("\uf6e3"); }

	else if(iconName == "fa-hand-peace"){
	  label = _U("\uf25b"); }

	else if(iconName == "fa-rotate"){
	  label = _U("\uf2f1"); }

	else if(iconName == "fa-sync-alt"){
	  label = _U("\uf2f1"); }

	else if(iconName == "fa-spinner"){
	  label = _U("\uf110"); }

	else if(iconName == "fa-robot"){
	  label = _U("\uf544"); }

	else if(iconName == "fa-peace"){
	  label = _U("\uf67c"); }

	else if(iconName == "fa-gears"){
	  label = _U("\uf085"); }

	else if(iconName == "fa-cogs"){
	  label = _U("\uf085"); }

	else if(iconName == "fa-warehouse"){
	  label = _U("\uf494"); }

	else if(iconName == "fa-arrow-up-right-dots"){
	  label = _U("\ue4b7"); }

	else if(iconName == "fa-splotch"){
	  label = _U("\uf5bc"); }

	else if(iconName == "fa-face-grin-hearts"){
	  label = _U("\uf584"); }

	else if(iconName == "fa-grin-hearts"){
	  label = _U("\uf584"); }

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

	else if(iconName == "fa-arrow-turn-down"){
	  label = _U("\uf149"); }

	else if(iconName == "fa-level-down"){
	  label = _U("\uf149"); }

	else if(iconName == "fa-person-falling-burst"){
	  label = _U("\ue547"); }

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

	else if(iconName == "fa-qrcode"){
	  label = _U("\uf029"); }

	else if(iconName == "fa-clock-rotate-left"){
	  label = _U("\uf1da"); }

	else if(iconName == "fa-history"){
	  label = _U("\uf1da"); }

	else if(iconName == "fa-face-grin-beam-sweat"){
	  label = _U("\uf583"); }

	else if(iconName == "fa-grin-beam-sweat"){
	  label = _U("\uf583"); }

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

	else if(iconName == "fa-pen-nib"){
	  label = _U("\uf5ad"); }

	else if(iconName == "fa-tent-arrow-turn-left"){
	  label = _U("\ue580"); }

	else if(iconName == "fa-tents"){
	  label = _U("\ue582"); }

	else if(iconName == "fa-wand-magic"){
	  label = _U("\uf0d0"); }

	else if(iconName == "fa-magic"){
	  label = _U("\uf0d0"); }

	else if(iconName == "fa-dog"){
	  label = _U("\uf6d3"); }

	else if(iconName == "fa-carrot"){
	  label = _U("\uf787"); }

	else if(iconName == "fa-moon"){
	  label = _U("\uf186"); }

	else if(iconName == "fa-wine-glass-empty"){
	  label = _U("\uf5ce"); }

	else if(iconName == "fa-wine-glass-alt"){
	  label = _U("\uf5ce"); }

	else if(iconName == "fa-cheese"){
	  label = _U("\uf7ef"); }

	else if(iconName == "fa-yin-yang"){
	  label = _U("\uf6ad"); }

	else if(iconName == "fa-music"){
	  label = _U("\uf001"); }

	else if(iconName == "fa-code-commit"){
	  label = _U("\uf386"); }

	else if(iconName == "fa-temperature-low"){
	  label = _U("\uf76b"); }

	else if(iconName == "fa-person-biking"){
	  label = _U("\uf84a"); }

	else if(iconName == "fa-biking"){
	  label = _U("\uf84a"); }

	else if(iconName == "fa-broom"){
	  label = _U("\uf51a"); }

	else if(iconName == "fa-shield-heart"){
	  label = _U("\ue574"); }

	else if(iconName == "fa-gopuram"){
	  label = _U("\uf664"); }

	else if(iconName == "fa-earth-oceania"){
	  label = _U("\ue47b"); }

	else if(iconName == "fa-globe-oceania"){
	  label = _U("\ue47b"); }

	else if(iconName == "fa-square-xmark"){
	  label = _U("\uf2d3"); }

	else if(iconName == "fa-times-square"){
	  label = _U("\uf2d3"); }

	else if(iconName == "fa-xmark-square"){
	  label = _U("\uf2d3"); }

	else if(iconName == "fa-up-right-and-down-left-from-center"){
	  label = _U("\uf424"); }

	else if(iconName == "fa-expand-alt"){
	  label = _U("\uf424"); }

	else if(iconName == "fa-oil-can"){
	  label = _U("\uf613"); }

	else if(iconName == "fa-hippo"){
	  label = _U("\uf6ed"); }

	else if(iconName == "fa-chart-column"){
	  label = _U("\ue0e3"); }

	else if(iconName == "fa-infinity"){
	  label = _U("\uf534"); }

	else if(iconName == "fa-vial-circle-check"){
	  label = _U("\ue596"); }

	else if(iconName == "fa-person-arrow-down-to-line"){
	  label = _U("\ue538"); }

	else if(iconName == "fa-voicemail"){
	  label = _U("\uf897"); }

	else if(iconName == "fa-fan"){
	  label = _U("\uf863"); }

	else if(iconName == "fa-person-walking-luggage"){
	  label = _U("\ue554"); }

	else if(iconName == "fa-up-down"){
	  label = _U("\uf338"); }

	else if(iconName == "fa-arrows-alt-v"){
	  label = _U("\uf338"); }

	else if(iconName == "fa-cloud-moon-rain"){
	  label = _U("\uf73c"); }

	else if(iconName == "fa-calendar"){
	  label = _U("\uf133"); }

	else if(iconName == "fa-trailer"){
	  label = _U("\ue041"); }

	else if(iconName == "fa-bahai"){
	  label = _U("\uf666"); }

	else if(iconName == "fa-haykal"){
	  label = _U("\uf666"); }

	else if(iconName == "fa-sd-card"){
	  label = _U("\uf7c2"); }

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

	else if(iconName == "fa-book-quran"){
	  label = _U("\uf687"); }

	else if(iconName == "fa-quran"){
	  label = _U("\uf687"); }

	else if(iconName == "fa-anchor"){
	  label = _U("\uf13d"); }

	else if(iconName == "fa-border-all"){
	  label = _U("\uf84c"); }

	else if(iconName == "fa-face-angry"){
	  label = _U("\uf556"); }

	else if(iconName == "fa-angry"){
	  label = _U("\uf556"); }

	else if(iconName == "fa-cookie-bite"){
	  label = _U("\uf564"); }

	else if(iconName == "fa-arrow-trend-down"){
	  label = _U("\ue097"); }

	else if(iconName == "fa-rss"){
	  label = _U("\uf09e"); }

	else if(iconName == "fa-feed"){
	  label = _U("\uf09e"); }

	else if(iconName == "fa-draw-polygon"){
	  label = _U("\uf5ee"); }

	else if(iconName == "fa-scale-balanced"){
	  label = _U("\uf24e"); }

	else if(iconName == "fa-balance-scale"){
	  label = _U("\uf24e"); }

	else if(iconName == "fa-gauge-simple-high"){
	  label = _U("\uf62a"); }

	else if(iconName == "fa-tachometer"){
	  label = _U("\uf62a"); }

	else if(iconName == "fa-tachometer-fast"){
	  label = _U("\uf62a"); }

	else if(iconName == "fa-shower"){
	  label = _U("\uf2cc"); }

	else if(iconName == "fa-desktop"){
	  label = _U("\uf390"); }

	else if(iconName == "fa-desktop-alt"){
	  label = _U("\uf390"); }

	else if(iconName == "fa-table-list"){
	  label = _U("\uf00b"); }

	else if(iconName == "fa-th-list"){
	  label = _U("\uf00b"); }

	else if(iconName == "fa-comment-sms"){
	  label = _U("\uf7cd"); }

	else if(iconName == "fa-sms"){
	  label = _U("\uf7cd"); }

	else if(iconName == "fa-book"){
	  label = _U("\uf02d"); }

	else if(iconName == "fa-user-plus"){
	  label = _U("\uf234"); }

	else if(iconName == "fa-check"){
	  label = _U("\uf00c"); }

	else if(iconName == "fa-battery-three-quarters"){
	  label = _U("\uf241"); }

	else if(iconName == "fa-battery-4"){
	  label = _U("\uf241"); }

	else if(iconName == "fa-house-circle-check"){
	  label = _U("\ue509"); }

	else if(iconName == "fa-angle-left"){
	  label = _U("\uf104"); }

	else if(iconName == "fa-diagram-successor"){
	  label = _U("\ue47a"); }

	else if(iconName == "fa-truck-arrow-right"){
	  label = _U("\ue58b"); }

	else if(iconName == "fa-arrows-split-up-and-left"){
	  label = _U("\ue4bc"); }

	else if(iconName == "fa-hand-fist"){
	  label = _U("\uf6de"); }

	else if(iconName == "fa-fist-raised"){
	  label = _U("\uf6de"); }

	else if(iconName == "fa-cloud-moon"){
	  label = _U("\uf6c3"); }

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

	else if(iconName == "fa-earth-europe"){
	  label = _U("\uf7a2"); }

	else if(iconName == "fa-globe-europe"){
	  label = _U("\uf7a2"); }

	else if(iconName == "fa-cart-flatbed-suitcase"){
	  label = _U("\uf59d"); }

	else if(iconName == "fa-luggage-cart"){
	  label = _U("\uf59d"); }

	else if(iconName == "fa-rectangle-xmark"){
	  label = _U("\uf410"); }

	else if(iconName == "fa-rectangle-times"){
	  label = _U("\uf410"); }

	else if(iconName == "fa-times-rectangle"){
	  label = _U("\uf410"); }

	else if(iconName == "fa-window-close"){
	  label = _U("\uf410"); }

	else if(iconName == "fa-baht-sign"){
	  label = _U("\ue0ac"); }

	else if(iconName == "fa-book-open"){
	  label = _U("\uf518"); }

	else if(iconName == "fa-book-journal-whills"){
	  label = _U("\uf66a"); }

	else if(iconName == "fa-journal-whills"){
	  label = _U("\uf66a"); }

	else if(iconName == "fa-handcuffs"){
	  label = _U("\ue4f8"); }

	else if(iconName == "fa-triangle-exclamation"){
	  label = _U("\uf071"); }

	else if(iconName == "fa-exclamation-triangle"){
	  label = _U("\uf071"); }

	else if(iconName == "fa-warning"){
	  label = _U("\uf071"); }

	else if(iconName == "fa-database"){
	  label = _U("\uf1c0"); }

	else if(iconName == "fa-share"){
	  label = _U("\uf064"); }

	else if(iconName == "fa-mail-forward"){
	  label = _U("\uf064"); }

	else if(iconName == "fa-bottle-droplet"){
	  label = _U("\ue4c4"); }

	else if(iconName == "fa-mask-face"){
	  label = _U("\ue1d7"); }

	else if(iconName == "fa-hill-rockslide"){
	  label = _U("\ue508"); }

	else if(iconName == "fa-right-left"){
	  label = _U("\uf362"); }

	else if(iconName == "fa-exchange-alt"){
	  label = _U("\uf362"); }

	else if(iconName == "fa-paper-plane"){
	  label = _U("\uf1d8"); }

	else if(iconName == "fa-road-circle-exclamation"){
	  label = _U("\ue565"); }

	else if(iconName == "fa-dungeon"){
	  label = _U("\uf6d9"); }

	else if(iconName == "fa-align-right"){
	  label = _U("\uf038"); }

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

	else if(iconName == "fa-person-walking-arrow-right"){
	  label = _U("\ue552"); }

	else if(iconName == "fa-square-envelope"){
	  label = _U("\uf199"); }

	else if(iconName == "fa-envelope-square"){
	  label = _U("\uf199"); }

	else if(iconName == "fa-dice"){
	  label = _U("\uf522"); }

	else if(iconName == "fa-bowling-ball"){
	  label = _U("\uf436"); }

	else if(iconName == "fa-brain"){
	  label = _U("\uf5dc"); }

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

	else if(iconName == "fa-gifts"){
	  label = _U("\uf79c"); }

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

	else if(iconName == "fa-user-clock"){
	  label = _U("\uf4fd"); }

	else if(iconName == "fa-hand-dots"){
	  label = _U("\uf461"); }

	else if(iconName == "fa-allergies"){
	  label = _U("\uf461"); }

	else if(iconName == "fa-file-invoice"){
	  label = _U("\uf570"); }

	else if(iconName == "fa-window-minimize"){
	  label = _U("\uf2d1"); }

	else if(iconName == "fa-mug-saucer"){
	  label = _U("\uf0f4"); }

	else if(iconName == "fa-coffee"){
	  label = _U("\uf0f4"); }

	else if(iconName == "fa-brush"){
	  label = _U("\uf55d"); }

	else if(iconName == "fa-mask"){
	  label = _U("\uf6fa"); }

	else if(iconName == "fa-magnifying-glass-minus"){
	  label = _U("\uf010"); }

	else if(iconName == "fa-search-minus"){
	  label = _U("\uf010"); }

	else if(iconName == "fa-ruler-vertical"){
	  label = _U("\uf548"); }

	else if(iconName == "fa-user-large"){
	  label = _U("\uf406"); }

	else if(iconName == "fa-user-alt"){
	  label = _U("\uf406"); }

	else if(iconName == "fa-train-tram"){
	  label = _U("\ue5b4"); }

	else if(iconName == "fa-user-nurse"){
	  label = _U("\uf82f"); }

	else if(iconName == "fa-syringe"){
	  label = _U("\uf48e"); }

	else if(iconName == "fa-cloud-sun"){
	  label = _U("\uf6c4"); }

	else if(iconName == "fa-stopwatch-20"){
	  label = _U("\ue06f"); }

	else if(iconName == "fa-square-full"){
	  label = _U("\uf45c"); }

	else if(iconName == "fa-magnet"){
	  label = _U("\uf076"); }

	else if(iconName == "fa-jar"){
	  label = _U("\ue516"); }

	else if(iconName == "fa-note-sticky"){
	  label = _U("\uf249"); }

	else if(iconName == "fa-sticky-note"){
	  label = _U("\uf249"); }

	else if(iconName == "fa-bug-slash"){
	  label = _U("\ue490"); }

	else if(iconName == "fa-arrow-up-from-water-pump"){
	  label = _U("\ue4b6"); }

	else if(iconName == "fa-bone"){
	  label = _U("\uf5d7"); }

	else if(iconName == "fa-user-injured"){
	  label = _U("\uf728"); }

	else if(iconName == "fa-face-sad-tear"){
	  label = _U("\uf5b4"); }

	else if(iconName == "fa-sad-tear"){
	  label = _U("\uf5b4"); }

	else if(iconName == "fa-plane"){
	  label = _U("\uf072"); }

	else if(iconName == "fa-tent-arrows-down"){
	  label = _U("\ue581"); }

	else if(iconName == "fa-arrows-spin"){
	  label = _U("\ue4bb"); }

	else if(iconName == "fa-print"){
	  label = _U("\uf02f"); }

	else if(iconName == "fa-turkish-lira-sign"){
	  label = _U("\ue2bb"); }

	else if(iconName == "fa-try"){
	  label = _U("\ue2bb"); }

	else if(iconName == "fa-turkish-lira"){
	  label = _U("\ue2bb"); }

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

	else if(iconName == "fa-umbrella"){
	  label = _U("\uf0e9"); }

	else if(iconName == "fa-trowel"){
	  label = _U("\ue589"); }

	else if(iconName == "fa-stapler"){
	  label = _U("\ue5af"); }

	else if(iconName == "fa-masks-theater"){
	  label = _U("\uf630"); }

	else if(iconName == "fa-theater-masks"){
	  label = _U("\uf630"); }

	else if(iconName == "fa-kip-sign"){
	  label = _U("\ue1c4"); }

	else if(iconName == "fa-hand-point-left"){
	  label = _U("\uf0a5"); }

	else if(iconName == "fa-handshake-simple"){
	  label = _U("\uf4c6"); }

	else if(iconName == "fa-handshake-alt"){
	  label = _U("\uf4c6"); }

	else if(iconName == "fa-jet-fighter"){
	  label = _U("\uf0fb"); }

	else if(iconName == "fa-fighter-jet"){
	  label = _U("\uf0fb"); }

	else if(iconName == "fa-square-share-nodes"){
	  label = _U("\uf1e1"); }

	else if(iconName == "fa-share-alt-square"){
	  label = _U("\uf1e1"); }

	else if(iconName == "fa-barcode"){
	  label = _U("\uf02a"); }

	else if(iconName == "fa-plus-minus"){
	  label = _U("\ue43c"); }

	else if(iconName == "fa-video"){
	  label = _U("\uf03d"); }

	else if(iconName == "fa-video-camera"){
	  label = _U("\uf03d"); }

	else if(iconName == "fa-graduation-cap"){
	  label = _U("\uf19d"); }

	else if(iconName == "fa-mortar-board"){
	  label = _U("\uf19d"); }

	else if(iconName == "fa-hand-holding-medical"){
	  label = _U("\ue05c"); }

	else if(iconName == "fa-person-circle-check"){
	  label = _U("\ue53e"); }

	else if(iconName == "fa-turn-up"){
	  label = _U("\uf3bf"); }

	else if(iconName == "fa-level-up-alt"){
	  label = _U("\uf3bf"); }

	else if(iconName == "fa-monero"){
	  label = _U("\uf3d0"); }

	else if(iconName == "fa-hooli"){
	  label = _U("\uf427"); }

	else if(iconName == "fa-yelp"){
	  label = _U("\uf1e9"); }

	else if(iconName == "fa-cc-visa"){
	  label = _U("\uf1f0"); }

	else if(iconName == "fa-lastfm"){
	  label = _U("\uf202"); }

	else if(iconName == "fa-shopware"){
	  label = _U("\uf5b5"); }

	else if(iconName == "fa-creative-commons-nc"){
	  label = _U("\uf4e8"); }

	else if(iconName == "fa-aws"){
	  label = _U("\uf375"); }

	else if(iconName == "fa-redhat"){
	  label = _U("\uf7bc"); }

	else if(iconName == "fa-yoast"){
	  label = _U("\uf2b1"); }

	else if(iconName == "fa-cloudflare"){
	  label = _U("\ue07d"); }

	else if(iconName == "fa-ups"){
	  label = _U("\uf7e0"); }

	else if(iconName == "fa-pixiv"){
	  label = _U("\ue640"); }

	else if(iconName == "fa-wpexplorer"){
	  label = _U("\uf2de"); }

	else if(iconName == "fa-dyalog"){
	  label = _U("\uf399"); }

	else if(iconName == "fa-bity"){
	  label = _U("\uf37a"); }

	else if(iconName == "fa-stackpath"){
	  label = _U("\uf842"); }

	else if(iconName == "fa-buysellads"){
	  label = _U("\uf20d"); }

	else if(iconName == "fa-first-order"){
	  label = _U("\uf2b0"); }

	else if(iconName == "fa-modx"){
	  label = _U("\uf285"); }

	else if(iconName == "fa-guilded"){
	  label = _U("\ue07e"); }

	else if(iconName == "fa-vnv"){
	  label = _U("\uf40b"); }

	else if(iconName == "fa-square-js"){
	  label = _U("\uf3b9"); }

	else if(iconName == "fa-js-square"){
	  label = _U("\uf3b9"); }

	else if(iconName == "fa-microsoft"){
	  label = _U("\uf3ca"); }

	else if(iconName == "fa-qq"){
	  label = _U("\uf1d6"); }

	else if(iconName == "fa-orcid"){
	  label = _U("\uf8d2"); }

	else if(iconName == "fa-java"){
	  label = _U("\uf4e4"); }

	else if(iconName == "fa-invision"){
	  label = _U("\uf7b0"); }

	else if(iconName == "fa-creative-commons-pd-alt"){
	  label = _U("\uf4ed"); }

	else if(iconName == "fa-centercode"){
	  label = _U("\uf380"); }

	else if(iconName == "fa-glide-g"){
	  label = _U("\uf2a6"); }

	else if(iconName == "fa-drupal"){
	  label = _U("\uf1a9"); }

	else if(iconName == "fa-hire-a-helper"){
	  label = _U("\uf3b0"); }

	else if(iconName == "fa-creative-commons-by"){
	  label = _U("\uf4e7"); }

	else if(iconName == "fa-unity"){
	  label = _U("\ue049"); }

	else if(iconName == "fa-whmcs"){
	  label = _U("\uf40d"); }

	else if(iconName == "fa-rocketchat"){
	  label = _U("\uf3e8"); }

	else if(iconName == "fa-vk"){
	  label = _U("\uf189"); }

	else if(iconName == "fa-untappd"){
	  label = _U("\uf405"); }

	else if(iconName == "fa-mailchimp"){
	  label = _U("\uf59e"); }

	else if(iconName == "fa-css3-alt"){
	  label = _U("\uf38b"); }

	else if(iconName == "fa-square-reddit"){
	  label = _U("\uf1a2"); }

	else if(iconName == "fa-reddit-square"){
	  label = _U("\uf1a2"); }

	else if(iconName == "fa-vimeo-v"){
	  label = _U("\uf27d"); }

	else if(iconName == "fa-contao"){
	  label = _U("\uf26d"); }

	else if(iconName == "fa-square-font-awesome"){
	  label = _U("\ue5ad"); }

	else if(iconName == "fa-deskpro"){
	  label = _U("\uf38f"); }

	else if(iconName == "fa-brave"){
	  label = _U("\ue63c"); }

	else if(iconName == "fa-sistrix"){
	  label = _U("\uf3ee"); }

	else if(iconName == "fa-square-instagram"){
	  label = _U("\ue055"); }

	else if(iconName == "fa-instagram-square"){
	  label = _U("\ue055"); }

	else if(iconName == "fa-battle-net"){
	  label = _U("\uf835"); }

	else if(iconName == "fa-the-red-yeti"){
	  label = _U("\uf69d"); }

	else if(iconName == "fa-square-hacker-news"){
	  label = _U("\uf3af"); }

	else if(iconName == "fa-hacker-news-square"){
	  label = _U("\uf3af"); }

	else if(iconName == "fa-edge"){
	  label = _U("\uf282"); }

	else if(iconName == "fa-threads"){
	  label = _U("\ue618"); }

	else if(iconName == "fa-napster"){
	  label = _U("\uf3d2"); }

	else if(iconName == "fa-square-snapchat"){
	  label = _U("\uf2ad"); }

	else if(iconName == "fa-snapchat-square"){
	  label = _U("\uf2ad"); }

	else if(iconName == "fa-google-plus-g"){
	  label = _U("\uf0d5"); }

	else if(iconName == "fa-artstation"){
	  label = _U("\uf77a"); }

	else if(iconName == "fa-markdown"){
	  label = _U("\uf60f"); }

	else if(iconName == "fa-sourcetree"){
	  label = _U("\uf7d3"); }

	else if(iconName == "fa-google-plus"){
	  label = _U("\uf2b3"); }

	else if(iconName == "fa-diaspora"){
	  label = _U("\uf791"); }

	else if(iconName == "fa-foursquare"){
	  label = _U("\uf180"); }

	else if(iconName == "fa-stack-overflow"){
	  label = _U("\uf16c"); }

	else if(iconName == "fa-github-alt"){
	  label = _U("\uf113"); }

	else if(iconName == "fa-phoenix-squadron"){
	  label = _U("\uf511"); }

	else if(iconName == "fa-pagelines"){
	  label = _U("\uf18c"); }

	else if(iconName == "fa-algolia"){
	  label = _U("\uf36c"); }

	else if(iconName == "fa-red-river"){
	  label = _U("\uf3e3"); }

	else if(iconName == "fa-creative-commons-sa"){
	  label = _U("\uf4ef"); }

	else if(iconName == "fa-safari"){
	  label = _U("\uf267"); }

	else if(iconName == "fa-google"){
	  label = _U("\uf1a0"); }

	else if(iconName == "fa-square-font-awesome-stroke"){
	  label = _U("\uf35c"); }

	else if(iconName == "fa-font-awesome-alt"){
	  label = _U("\uf35c"); }

	else if(iconName == "fa-atlassian"){
	  label = _U("\uf77b"); }

	else if(iconName == "fa-linkedin-in"){
	  label = _U("\uf0e1"); }

	else if(iconName == "fa-digital-ocean"){
	  label = _U("\uf391"); }

	else if(iconName == "fa-nimblr"){
	  label = _U("\uf5a8"); }

	else if(iconName == "fa-chromecast"){
	  label = _U("\uf838"); }

	else if(iconName == "fa-evernote"){
	  label = _U("\uf839"); }

	else if(iconName == "fa-hacker-news"){
	  label = _U("\uf1d4"); }

	else if(iconName == "fa-creative-commons-sampling"){
	  label = _U("\uf4f0"); }

	else if(iconName == "fa-adversal"){
	  label = _U("\uf36a"); }

	else if(iconName == "fa-creative-commons"){
	  label = _U("\uf25e"); }

	else if(iconName == "fa-watchman-monitoring"){
	  label = _U("\ue087"); }

	else if(iconName == "fa-fonticons"){
	  label = _U("\uf280"); }

	else if(iconName == "fa-weixin"){
	  label = _U("\uf1d7"); }

	else if(iconName == "fa-shirtsinbulk"){
	  label = _U("\uf214"); }

	else if(iconName == "fa-codepen"){
	  label = _U("\uf1cb"); }

	else if(iconName == "fa-git-alt"){
	  label = _U("\uf841"); }

	else if(iconName == "fa-lyft"){
	  label = _U("\uf3c3"); }

	else if(iconName == "fa-rev"){
	  label = _U("\uf5b2"); }

	else if(iconName == "fa-windows"){
	  label = _U("\uf17a"); }

	else if(iconName == "fa-wizards-of-the-coast"){
	  label = _U("\uf730"); }

	else if(iconName == "fa-square-viadeo"){
	  label = _U("\uf2aa"); }

	else if(iconName == "fa-viadeo-square"){
	  label = _U("\uf2aa"); }

	else if(iconName == "fa-meetup"){
	  label = _U("\uf2e0"); }

	else if(iconName == "fa-centos"){
	  label = _U("\uf789"); }

	else if(iconName == "fa-adn"){
	  label = _U("\uf170"); }

	else if(iconName == "fa-cloudsmith"){
	  label = _U("\uf384"); }

	else if(iconName == "fa-opensuse"){
	  label = _U("\ue62b"); }

	else if(iconName == "fa-pied-piper-alt"){
	  label = _U("\uf1a8"); }

	else if(iconName == "fa-square-dribbble"){
	  label = _U("\uf397"); }

	else if(iconName == "fa-dribbble-square"){
	  label = _U("\uf397"); }

	else if(iconName == "fa-codiepie"){
	  label = _U("\uf284"); }

	else if(iconName == "fa-node"){
	  label = _U("\uf419"); }

	else if(iconName == "fa-mix"){
	  label = _U("\uf3cb"); }

	else if(iconName == "fa-steam"){
	  label = _U("\uf1b6"); }

	else if(iconName == "fa-cc-apple-pay"){
	  label = _U("\uf416"); }

	else if(iconName == "fa-scribd"){
	  label = _U("\uf28a"); }

	else if(iconName == "fa-debian"){
	  label = _U("\ue60b"); }

	else if(iconName == "fa-openid"){
	  label = _U("\uf19b"); }

	else if(iconName == "fa-instalod"){
	  label = _U("\ue081"); }

	else if(iconName == "fa-expeditedssl"){
	  label = _U("\uf23e"); }

	else if(iconName == "fa-sellcast"){
	  label = _U("\uf2da"); }

	else if(iconName == "fa-square-twitter"){
	  label = _U("\uf081"); }

	else if(iconName == "fa-twitter-square"){
	  label = _U("\uf081"); }

	else if(iconName == "fa-r-project"){
	  label = _U("\uf4f7"); }

	else if(iconName == "fa-delicious"){
	  label = _U("\uf1a5"); }

	else if(iconName == "fa-freebsd"){
	  label = _U("\uf3a4"); }

	else if(iconName == "fa-vuejs"){
	  label = _U("\uf41f"); }

	else if(iconName == "fa-accusoft"){
	  label = _U("\uf369"); }

	else if(iconName == "fa-ioxhost"){
	  label = _U("\uf208"); }

	else if(iconName == "fa-fonticons-fi"){
	  label = _U("\uf3a2"); }

	else if(iconName == "fa-app-store"){
	  label = _U("\uf36f"); }

	else if(iconName == "fa-cc-mastercard"){
	  label = _U("\uf1f1"); }

	else if(iconName == "fa-itunes-note"){
	  label = _U("\uf3b5"); }

	else if(iconName == "fa-golang"){
	  label = _U("\ue40f"); }

	else if(iconName == "fa-kickstarter"){
	  label = _U("\uf3bb"); }

	else if(iconName == "fa-grav"){
	  label = _U("\uf2d6"); }

	else if(iconName == "fa-weibo"){
	  label = _U("\uf18a"); }

	else if(iconName == "fa-uncharted"){
	  label = _U("\ue084"); }

	else if(iconName == "fa-firstdraft"){
	  label = _U("\uf3a1"); }

	else if(iconName == "fa-square-youtube"){
	  label = _U("\uf431"); }

	else if(iconName == "fa-youtube-square"){
	  label = _U("\uf431"); }

	else if(iconName == "fa-wikipedia-w"){
	  label = _U("\uf266"); }

	else if(iconName == "fa-wpressr"){
	  label = _U("\uf3e4"); }

	else if(iconName == "fa-rendact"){
	  label = _U("\uf3e4"); }

	else if(iconName == "fa-angellist"){
	  label = _U("\uf209"); }

	else if(iconName == "fa-galactic-republic"){
	  label = _U("\uf50c"); }

	else if(iconName == "fa-nfc-directional"){
	  label = _U("\ue530"); }

	else if(iconName == "fa-skype"){
	  label = _U("\uf17e"); }

	else if(iconName == "fa-joget"){
	  label = _U("\uf3b7"); }

	else if(iconName == "fa-fedora"){
	  label = _U("\uf798"); }

	else if(iconName == "fa-stripe-s"){
	  label = _U("\uf42a"); }

	else if(iconName == "fa-meta"){
	  label = _U("\ue49b"); }

	else if(iconName == "fa-laravel"){
	  label = _U("\uf3bd"); }

	else if(iconName == "fa-hotjar"){
	  label = _U("\uf3b1"); }

	else if(iconName == "fa-bluetooth-b"){
	  label = _U("\uf294"); }

	else if(iconName == "fa-square-letterboxd"){
	  label = _U("\ue62e"); }

	else if(iconName == "fa-sticker-mule"){
	  label = _U("\uf3f7"); }

	else if(iconName == "fa-creative-commons-zero"){
	  label = _U("\uf4f3"); }

	else if(iconName == "fa-hips"){
	  label = _U("\uf452"); }

	else if(iconName == "fa-behance"){
	  label = _U("\uf1b4"); }

	else if(iconName == "fa-reddit"){
	  label = _U("\uf1a1"); }

	else if(iconName == "fa-discord"){
	  label = _U("\uf392"); }

	else if(iconName == "fa-chrome"){
	  label = _U("\uf268"); }

	else if(iconName == "fa-app-store-ios"){
	  label = _U("\uf370"); }

	else if(iconName == "fa-cc-discover"){
	  label = _U("\uf1f2"); }

	else if(iconName == "fa-wpbeginner"){
	  label = _U("\uf297"); }

	else if(iconName == "fa-confluence"){
	  label = _U("\uf78d"); }

	else if(iconName == "fa-shoelace"){
	  label = _U("\ue60c"); }

	else if(iconName == "fa-mdb"){
	  label = _U("\uf8ca"); }

	else if(iconName == "fa-dochub"){
	  label = _U("\uf394"); }

	else if(iconName == "fa-accessible-icon"){
	  label = _U("\uf368"); }

	else if(iconName == "fa-ebay"){
	  label = _U("\uf4f4"); }

	else if(iconName == "fa-amazon"){
	  label = _U("\uf270"); }

	else if(iconName == "fa-unsplash"){
	  label = _U("\ue07c"); }

	else if(iconName == "fa-yarn"){
	  label = _U("\uf7e3"); }

	else if(iconName == "fa-square-steam"){
	  label = _U("\uf1b7"); }

	else if(iconName == "fa-steam-square"){
	  label = _U("\uf1b7"); }

	else if(iconName == "fa-500px"){
	  label = _U("\uf26e"); }

	else if(iconName == "fa-square-vimeo"){
	  label = _U("\uf194"); }

	else if(iconName == "fa-vimeo-square"){
	  label = _U("\uf194"); }

	else if(iconName == "fa-asymmetrik"){
	  label = _U("\uf372"); }

	else if(iconName == "fa-font-awesome"){
	  label = _U("\uf2b4"); }

	else if(iconName == "fa-font-awesome-flag"){
	  label = _U("\uf2b4"); }

	else if(iconName == "fa-font-awesome-logo-full"){
	  label = _U("\uf2b4"); }

	else if(iconName == "fa-gratipay"){
	  label = _U("\uf184"); }

	else if(iconName == "fa-apple"){
	  label = _U("\uf179"); }

	else if(iconName == "fa-hive"){
	  label = _U("\ue07f"); }

	else if(iconName == "fa-gitkraken"){
	  label = _U("\uf3a6"); }

	else if(iconName == "fa-keybase"){
	  label = _U("\uf4f5"); }

	else if(iconName == "fa-apple-pay"){
	  label = _U("\uf415"); }

	else if(iconName == "fa-padlet"){
	  label = _U("\ue4a0"); }

	else if(iconName == "fa-amazon-pay"){
	  label = _U("\uf42c"); }

	else if(iconName == "fa-square-github"){
	  label = _U("\uf092"); }

	else if(iconName == "fa-github-square"){
	  label = _U("\uf092"); }

	else if(iconName == "fa-stumbleupon"){
	  label = _U("\uf1a4"); }

	else if(iconName == "fa-fedex"){
	  label = _U("\uf797"); }

	else if(iconName == "fa-phoenix-framework"){
	  label = _U("\uf3dc"); }

	else if(iconName == "fa-shopify"){
	  label = _U("\ue057"); }

	else if(iconName == "fa-neos"){
	  label = _U("\uf612"); }

	else if(iconName == "fa-square-threads"){
	  label = _U("\ue619"); }

	else if(iconName == "fa-hackerrank"){
	  label = _U("\uf5f7"); }

	else if(iconName == "fa-researchgate"){
	  label = _U("\uf4f8"); }

	else if(iconName == "fa-swift"){
	  label = _U("\uf8e1"); }

	else if(iconName == "fa-angular"){
	  label = _U("\uf420"); }

	else if(iconName == "fa-speakap"){
	  label = _U("\uf3f3"); }

	else if(iconName == "fa-angrycreative"){
	  label = _U("\uf36e"); }

	else if(iconName == "fa-y-combinator"){
	  label = _U("\uf23b"); }

	else if(iconName == "fa-empire"){
	  label = _U("\uf1d1"); }

	else if(iconName == "fa-envira"){
	  label = _U("\uf299"); }

	else if(iconName == "fa-google-scholar"){
	  label = _U("\ue63b"); }

	else if(iconName == "fa-square-gitlab"){
	  label = _U("\ue5ae"); }

	else if(iconName == "fa-gitlab-square"){
	  label = _U("\ue5ae"); }

	else if(iconName == "fa-studiovinari"){
	  label = _U("\uf3f8"); }

	else if(iconName == "fa-pied-piper"){
	  label = _U("\uf2ae"); }

	else if(iconName == "fa-wordpress"){
	  label = _U("\uf19a"); }

	else if(iconName == "fa-product-hunt"){
	  label = _U("\uf288"); }

	else if(iconName == "fa-firefox"){
	  label = _U("\uf269"); }

	else if(iconName == "fa-linode"){
	  label = _U("\uf2b8"); }

	else if(iconName == "fa-goodreads"){
	  label = _U("\uf3a8"); }

	else if(iconName == "fa-square-odnoklassniki"){
	  label = _U("\uf264"); }

	else if(iconName == "fa-odnoklassniki-square"){
	  label = _U("\uf264"); }

	else if(iconName == "fa-jsfiddle"){
	  label = _U("\uf1cc"); }

	else if(iconName == "fa-sith"){
	  label = _U("\uf512"); }

	else if(iconName == "fa-themeisle"){
	  label = _U("\uf2b2"); }

	else if(iconName == "fa-page4"){
	  label = _U("\uf3d7"); }

	else if(iconName == "fa-hashnode"){
	  label = _U("\ue499"); }

	else if(iconName == "fa-react"){
	  label = _U("\uf41b"); }

	else if(iconName == "fa-cc-paypal"){
	  label = _U("\uf1f4"); }

	else if(iconName == "fa-squarespace"){
	  label = _U("\uf5be"); }

	else if(iconName == "fa-cc-stripe"){
	  label = _U("\uf1f5"); }

	else if(iconName == "fa-creative-commons-share"){
	  label = _U("\uf4f2"); }

	else if(iconName == "fa-bitcoin"){
	  label = _U("\uf379"); }

	else if(iconName == "fa-keycdn"){
	  label = _U("\uf3ba"); }

	else if(iconName == "fa-opera"){
	  label = _U("\uf26a"); }

	else if(iconName == "fa-itch-io"){
	  label = _U("\uf83a"); }

	else if(iconName == "fa-umbraco"){
	  label = _U("\uf8e8"); }

	else if(iconName == "fa-galactic-senate"){
	  label = _U("\uf50d"); }

	else if(iconName == "fa-ubuntu"){
	  label = _U("\uf7df"); }

	else if(iconName == "fa-draft2digital"){
	  label = _U("\uf396"); }

	else if(iconName == "fa-stripe"){
	  label = _U("\uf429"); }

	else if(iconName == "fa-houzz"){
	  label = _U("\uf27c"); }

	else if(iconName == "fa-gg"){
	  label = _U("\uf260"); }

	else if(iconName == "fa-dhl"){
	  label = _U("\uf790"); }

	else if(iconName == "fa-square-pinterest"){
	  label = _U("\uf0d3"); }

	else if(iconName == "fa-pinterest-square"){
	  label = _U("\uf0d3"); }

	else if(iconName == "fa-xing"){
	  label = _U("\uf168"); }

	else if(iconName == "fa-blackberry"){
	  label = _U("\uf37b"); }

	else if(iconName == "fa-creative-commons-pd"){
	  label = _U("\uf4ec"); }

	else if(iconName == "fa-playstation"){
	  label = _U("\uf3df"); }

	else if(iconName == "fa-quinscape"){
	  label = _U("\uf459"); }

	else if(iconName == "fa-less"){
	  label = _U("\uf41d"); }

	else if(iconName == "fa-blogger-b"){
	  label = _U("\uf37d"); }

	else if(iconName == "fa-opencart"){
	  label = _U("\uf23d"); }

	else if(iconName == "fa-vine"){
	  label = _U("\uf1ca"); }

	else if(iconName == "fa-signal-messenger"){
	  label = _U("\ue663"); }

	else if(iconName == "fa-paypal"){
	  label = _U("\uf1ed"); }

	else if(iconName == "fa-gitlab"){
	  label = _U("\uf296"); }

	else if(iconName == "fa-typo3"){
	  label = _U("\uf42b"); }

	else if(iconName == "fa-reddit-alien"){
	  label = _U("\uf281"); }

	else if(iconName == "fa-yahoo"){
	  label = _U("\uf19e"); }

	else if(iconName == "fa-dailymotion"){
	  label = _U("\ue052"); }

	else if(iconName == "fa-affiliatetheme"){
	  label = _U("\uf36b"); }

	else if(iconName == "fa-pied-piper-pp"){
	  label = _U("\uf1a7"); }

	else if(iconName == "fa-bootstrap"){
	  label = _U("\uf836"); }

	else if(iconName == "fa-odnoklassniki"){
	  label = _U("\uf263"); }

	else if(iconName == "fa-nfc-symbol"){
	  label = _U("\ue531"); }

	else if(iconName == "fa-mintbit"){
	  label = _U("\ue62f"); }

	else if(iconName == "fa-ethereum"){
	  label = _U("\uf42e"); }

	else if(iconName == "fa-speaker-deck"){
	  label = _U("\uf83c"); }

	else if(iconName == "fa-creative-commons-nc-eu"){
	  label = _U("\uf4e9"); }

	else if(iconName == "fa-patreon"){
	  label = _U("\uf3d9"); }

	else if(iconName == "fa-avianex"){
	  label = _U("\uf374"); }

	else if(iconName == "fa-ello"){
	  label = _U("\uf5f1"); }

	else if(iconName == "fa-gofore"){
	  label = _U("\uf3a7"); }

	else if(iconName == "fa-bimobject"){
	  label = _U("\uf378"); }

	else if(iconName == "fa-brave-reverse"){
	  label = _U("\ue63d"); }

	else if(iconName == "fa-facebook-f"){
	  label = _U("\uf39e"); }

	else if(iconName == "fa-square-google-plus"){
	  label = _U("\uf0d4"); }

	else if(iconName == "fa-google-plus-square"){
	  label = _U("\uf0d4"); }

	else if(iconName == "fa-mandalorian"){
	  label = _U("\uf50f"); }

	else if(iconName == "fa-first-order-alt"){
	  label = _U("\uf50a"); }

	else if(iconName == "fa-osi"){
	  label = _U("\uf41a"); }

	else if(iconName == "fa-google-wallet"){
	  label = _U("\uf1ee"); }

	else if(iconName == "fa-d-and-d-beyond"){
	  label = _U("\uf6ca"); }

	else if(iconName == "fa-periscope"){
	  label = _U("\uf3da"); }

	else if(iconName == "fa-fulcrum"){
	  label = _U("\uf50b"); }

	else if(iconName == "fa-cloudscale"){
	  label = _U("\uf383"); }

	else if(iconName == "fa-forumbee"){
	  label = _U("\uf211"); }

	else if(iconName == "fa-mizuni"){
	  label = _U("\uf3cc"); }

	else if(iconName == "fa-schlix"){
	  label = _U("\uf3ea"); }

	else if(iconName == "fa-square-xing"){
	  label = _U("\uf169"); }

	else if(iconName == "fa-xing-square"){
	  label = _U("\uf169"); }

	else if(iconName == "fa-bandcamp"){
	  label = _U("\uf2d5"); }

	else if(iconName == "fa-wpforms"){
	  label = _U("\uf298"); }

	else if(iconName == "fa-cloudversify"){
	  label = _U("\uf385"); }

	else if(iconName == "fa-usps"){
	  label = _U("\uf7e1"); }

	else if(iconName == "fa-megaport"){
	  label = _U("\uf5a3"); }

	else if(iconName == "fa-magento"){
	  label = _U("\uf3c4"); }

	else if(iconName == "fa-spotify"){
	  label = _U("\uf1bc"); }

	else if(iconName == "fa-optin-monster"){
	  label = _U("\uf23c"); }

	else if(iconName == "fa-fly"){
	  label = _U("\uf417"); }

	else if(iconName == "fa-aviato"){
	  label = _U("\uf421"); }

	else if(iconName == "fa-itunes"){
	  label = _U("\uf3b4"); }

	else if(iconName == "fa-cuttlefish"){
	  label = _U("\uf38c"); }

	else if(iconName == "fa-blogger"){
	  label = _U("\uf37c"); }

	else if(iconName == "fa-flickr"){
	  label = _U("\uf16e"); }

	else if(iconName == "fa-viber"){
	  label = _U("\uf409"); }

	else if(iconName == "fa-soundcloud"){
	  label = _U("\uf1be"); }

	else if(iconName == "fa-digg"){
	  label = _U("\uf1a6"); }

	else if(iconName == "fa-tencent-weibo"){
	  label = _U("\uf1d5"); }

	else if(iconName == "fa-letterboxd"){
	  label = _U("\ue62d"); }

	else if(iconName == "fa-symfony"){
	  label = _U("\uf83d"); }

	else if(iconName == "fa-maxcdn"){
	  label = _U("\uf136"); }

	else if(iconName == "fa-etsy"){
	  label = _U("\uf2d7"); }

	else if(iconName == "fa-facebook-messenger"){
	  label = _U("\uf39f"); }

	else if(iconName == "fa-audible"){
	  label = _U("\uf373"); }

	else if(iconName == "fa-think-peaks"){
	  label = _U("\uf731"); }

	else if(iconName == "fa-bilibili"){
	  label = _U("\ue3d9"); }

	else if(iconName == "fa-erlang"){
	  label = _U("\uf39d"); }

	else if(iconName == "fa-x-twitter"){
	  label = _U("\ue61b"); }

	else if(iconName == "fa-cotton-bureau"){
	  label = _U("\uf89e"); }

	else if(iconName == "fa-dashcube"){
	  label = _U("\uf210"); }

	else if(iconName == "fa-42-group"){
	  label = _U("\ue080"); }

	else if(iconName == "fa-innosoft"){
	  label = _U("\ue080"); }

	else if(iconName == "fa-stack-exchange"){
	  label = _U("\uf18d"); }

	else if(iconName == "fa-elementor"){
	  label = _U("\uf430"); }

	else if(iconName == "fa-square-pied-piper"){
	  label = _U("\ue01e"); }

	else if(iconName == "fa-pied-piper-square"){
	  label = _U("\ue01e"); }

	else if(iconName == "fa-creative-commons-nd"){
	  label = _U("\uf4eb"); }

	else if(iconName == "fa-palfed"){
	  label = _U("\uf3d8"); }

	else if(iconName == "fa-superpowers"){
	  label = _U("\uf2dd"); }

	else if(iconName == "fa-resolving"){
	  label = _U("\uf3e7"); }

	else if(iconName == "fa-xbox"){
	  label = _U("\uf412"); }

	else if(iconName == "fa-searchengin"){
	  label = _U("\uf3eb"); }

	else if(iconName == "fa-tiktok"){
	  label = _U("\ue07b"); }

	else if(iconName == "fa-square-facebook"){
	  label = _U("\uf082"); }

	else if(iconName == "fa-facebook-square"){
	  label = _U("\uf082"); }

	else if(iconName == "fa-renren"){
	  label = _U("\uf18b"); }

	else if(iconName == "fa-linux"){
	  label = _U("\uf17c"); }

	else if(iconName == "fa-glide"){
	  label = _U("\uf2a5"); }

	else if(iconName == "fa-linkedin"){
	  label = _U("\uf08c"); }

	else if(iconName == "fa-hubspot"){
	  label = _U("\uf3b2"); }

	else if(iconName == "fa-deploydog"){
	  label = _U("\uf38e"); }

	else if(iconName == "fa-twitch"){
	  label = _U("\uf1e8"); }

	else if(iconName == "fa-ravelry"){
	  label = _U("\uf2d9"); }

	else if(iconName == "fa-mixer"){
	  label = _U("\ue056"); }

	else if(iconName == "fa-square-lastfm"){
	  label = _U("\uf203"); }

	else if(iconName == "fa-lastfm-square"){
	  label = _U("\uf203"); }

	else if(iconName == "fa-vimeo"){
	  label = _U("\uf40a"); }

	else if(iconName == "fa-mendeley"){
	  label = _U("\uf7b3"); }

	else if(iconName == "fa-uniregistry"){
	  label = _U("\uf404"); }

	else if(iconName == "fa-figma"){
	  label = _U("\uf799"); }

	else if(iconName == "fa-creative-commons-remix"){
	  label = _U("\uf4ee"); }

	else if(iconName == "fa-cc-amazon-pay"){
	  label = _U("\uf42d"); }

	else if(iconName == "fa-dropbox"){
	  label = _U("\uf16b"); }

	else if(iconName == "fa-instagram"){
	  label = _U("\uf16d"); }

	else if(iconName == "fa-cmplid"){
	  label = _U("\ue360"); }

	else if(iconName == "fa-upwork"){
	  label = _U("\ue641"); }

	else if(iconName == "fa-facebook"){
	  label = _U("\uf09a"); }

	else if(iconName == "fa-gripfire"){
	  label = _U("\uf3ac"); }

	else if(iconName == "fa-jedi-order"){
	  label = _U("\uf50e"); }

	else if(iconName == "fa-uikit"){
	  label = _U("\uf403"); }

	else if(iconName == "fa-fort-awesome-alt"){
	  label = _U("\uf3a3"); }

	else if(iconName == "fa-phabricator"){
	  label = _U("\uf3db"); }

	else if(iconName == "fa-ussunnah"){
	  label = _U("\uf407"); }

	else if(iconName == "fa-earlybirds"){
	  label = _U("\uf39a"); }

	else if(iconName == "fa-trade-federation"){
	  label = _U("\uf513"); }

	else if(iconName == "fa-autoprefixer"){
	  label = _U("\uf41c"); }

	else if(iconName == "fa-whatsapp"){
	  label = _U("\uf232"); }

	else if(iconName == "fa-slideshare"){
	  label = _U("\uf1e7"); }

	else if(iconName == "fa-google-play"){
	  label = _U("\uf3ab"); }

	else if(iconName == "fa-viadeo"){
	  label = _U("\uf2a9"); }

	else if(iconName == "fa-line"){
	  label = _U("\uf3c0"); }

	else if(iconName == "fa-google-drive"){
	  label = _U("\uf3aa"); }

	else if(iconName == "fa-servicestack"){
	  label = _U("\uf3ec"); }

	else if(iconName == "fa-simplybuilt"){
	  label = _U("\uf215"); }

	else if(iconName == "fa-bitbucket"){
	  label = _U("\uf171"); }

	else if(iconName == "fa-imdb"){
	  label = _U("\uf2d8"); }

	else if(iconName == "fa-deezer"){
	  label = _U("\ue077"); }

	else if(iconName == "fa-raspberry-pi"){
	  label = _U("\uf7bb"); }

	else if(iconName == "fa-jira"){
	  label = _U("\uf7b1"); }

	else if(iconName == "fa-docker"){
	  label = _U("\uf395"); }

	else if(iconName == "fa-screenpal"){
	  label = _U("\ue570"); }

	else if(iconName == "fa-bluetooth"){
	  label = _U("\uf293"); }

	else if(iconName == "fa-gitter"){
	  label = _U("\uf426"); }

	else if(iconName == "fa-d-and-d"){
	  label = _U("\uf38d"); }

	else if(iconName == "fa-microblog"){
	  label = _U("\ue01a"); }

	else if(iconName == "fa-cc-diners-club"){
	  label = _U("\uf24c"); }

	else if(iconName == "fa-gg-circle"){
	  label = _U("\uf261"); }

	else if(iconName == "fa-pied-piper-hat"){
	  label = _U("\uf4e5"); }

	else if(iconName == "fa-kickstarter-k"){
	  label = _U("\uf3bc"); }

	else if(iconName == "fa-yandex"){
	  label = _U("\uf413"); }

	else if(iconName == "fa-readme"){
	  label = _U("\uf4d5"); }

	else if(iconName == "fa-html5"){
	  label = _U("\uf13b"); }

	else if(iconName == "fa-sellsy"){
	  label = _U("\uf213"); }

	else if(iconName == "fa-sass"){
	  label = _U("\uf41e"); }

	else if(iconName == "fa-wirsindhandwerk"){
	  label = _U("\ue2d0"); }

	else if(iconName == "fa-wsh"){
	  label = _U("\ue2d0"); }

	else if(iconName == "fa-buromobelexperte"){
	  label = _U("\uf37f"); }

	else if(iconName == "fa-salesforce"){
	  label = _U("\uf83b"); }

	else if(iconName == "fa-octopus-deploy"){
	  label = _U("\ue082"); }

	else if(iconName == "fa-medapps"){
	  label = _U("\uf3c6"); }

	else if(iconName == "fa-ns8"){
	  label = _U("\uf3d5"); }

	else if(iconName == "fa-pinterest-p"){
	  label = _U("\uf231"); }

	else if(iconName == "fa-apper"){
	  label = _U("\uf371"); }

	else if(iconName == "fa-fort-awesome"){
	  label = _U("\uf286"); }

	else if(iconName == "fa-waze"){
	  label = _U("\uf83f"); }

	else if(iconName == "fa-cc-jcb"){
	  label = _U("\uf24b"); }

	else if(iconName == "fa-snapchat"){
	  label = _U("\uf2ab"); }

	else if(iconName == "fa-snapchat-ghost"){
	  label = _U("\uf2ab"); }

	else if(iconName == "fa-fantasy-flight-games"){
	  label = _U("\uf6dc"); }

	else if(iconName == "fa-rust"){
	  label = _U("\ue07a"); }

	else if(iconName == "fa-wix"){
	  label = _U("\uf5cf"); }

	else if(iconName == "fa-square-behance"){
	  label = _U("\uf1b5"); }

	else if(iconName == "fa-behance-square"){
	  label = _U("\uf1b5"); }

	else if(iconName == "fa-supple"){
	  label = _U("\uf3f9"); }

	else if(iconName == "fa-webflow"){
	  label = _U("\ue65c"); }

	else if(iconName == "fa-rebel"){
	  label = _U("\uf1d0"); }

	else if(iconName == "fa-css3"){
	  label = _U("\uf13c"); }

	else if(iconName == "fa-staylinked"){
	  label = _U("\uf3f5"); }

	else if(iconName == "fa-kaggle"){
	  label = _U("\uf5fa"); }

	else if(iconName == "fa-space-awesome"){
	  label = _U("\ue5ac"); }

	else if(iconName == "fa-deviantart"){
	  label = _U("\uf1bd"); }

	else if(iconName == "fa-cpanel"){
	  label = _U("\uf388"); }

	else if(iconName == "fa-goodreads-g"){
	  label = _U("\uf3a9"); }

	else if(iconName == "fa-square-git"){
	  label = _U("\uf1d2"); }

	else if(iconName == "fa-git-square"){
	  label = _U("\uf1d2"); }

	else if(iconName == "fa-square-tumblr"){
	  label = _U("\uf174"); }

	else if(iconName == "fa-tumblr-square"){
	  label = _U("\uf174"); }

	else if(iconName == "fa-trello"){
	  label = _U("\uf181"); }

	else if(iconName == "fa-creative-commons-nc-jp"){
	  label = _U("\uf4ea"); }

	else if(iconName == "fa-get-pocket"){
	  label = _U("\uf265"); }

	else if(iconName == "fa-perbyte"){
	  label = _U("\ue083"); }

	else if(iconName == "fa-grunt"){
	  label = _U("\uf3ad"); }

	else if(iconName == "fa-weebly"){
	  label = _U("\uf5cc"); }

	else if(iconName == "fa-connectdevelop"){
	  label = _U("\uf20e"); }

	else if(iconName == "fa-leanpub"){
	  label = _U("\uf212"); }

	else if(iconName == "fa-black-tie"){
	  label = _U("\uf27e"); }

	else if(iconName == "fa-themeco"){
	  label = _U("\uf5c6"); }

	else if(iconName == "fa-python"){
	  label = _U("\uf3e2"); }

	else if(iconName == "fa-android"){
	  label = _U("\uf17b"); }

	else if(iconName == "fa-bots"){
	  label = _U("\ue340"); }

	else if(iconName == "fa-free-code-camp"){
	  label = _U("\uf2c5"); }

	else if(iconName == "fa-hornbill"){
	  label = _U("\uf592"); }

	else if(iconName == "fa-js"){
	  label = _U("\uf3b8"); }

	else if(iconName == "fa-ideal"){
	  label = _U("\ue013"); }

	else if(iconName == "fa-git"){
	  label = _U("\uf1d3"); }

	else if(iconName == "fa-dev"){
	  label = _U("\uf6cc"); }

	else if(iconName == "fa-sketch"){
	  label = _U("\uf7c6"); }

	else if(iconName == "fa-yandex-international"){
	  label = _U("\uf414"); }

	else if(iconName == "fa-cc-amex"){
	  label = _U("\uf1f3"); }

	else if(iconName == "fa-uber"){
	  label = _U("\uf402"); }

	else if(iconName == "fa-github"){
	  label = _U("\uf09b"); }

	else if(iconName == "fa-php"){
	  label = _U("\uf457"); }

	else if(iconName == "fa-alipay"){
	  label = _U("\uf642"); }

	else if(iconName == "fa-youtube"){
	  label = _U("\uf167"); }

	else if(iconName == "fa-skyatlas"){
	  label = _U("\uf216"); }

	else if(iconName == "fa-firefox-browser"){
	  label = _U("\ue007"); }

	else if(iconName == "fa-replyd"){
	  label = _U("\uf3e6"); }

	else if(iconName == "fa-suse"){
	  label = _U("\uf7d6"); }

	else if(iconName == "fa-jenkins"){
	  label = _U("\uf3b6"); }

	else if(iconName == "fa-twitter"){
	  label = _U("\uf099"); }

	else if(iconName == "fa-rockrms"){
	  label = _U("\uf3e9"); }

	else if(iconName == "fa-pinterest"){
	  label = _U("\uf0d2"); }

	else if(iconName == "fa-buffer"){
	  label = _U("\uf837"); }

	else if(iconName == "fa-npm"){
	  label = _U("\uf3d4"); }

	else if(iconName == "fa-yammer"){
	  label = _U("\uf840"); }

	else if(iconName == "fa-btc"){
	  label = _U("\uf15a"); }

	else if(iconName == "fa-dribbble"){
	  label = _U("\uf17d"); }

	else if(iconName == "fa-stumbleupon-circle"){
	  label = _U("\uf1a3"); }

	else if(iconName == "fa-internet-explorer"){
	  label = _U("\uf26b"); }

	else if(iconName == "fa-stubber"){
	  label = _U("\ue5c7"); }

	else if(iconName == "fa-telegram"){
	  label = _U("\uf2c6"); }

	else if(iconName == "fa-telegram-plane"){
	  label = _U("\uf2c6"); }

	else if(iconName == "fa-old-republic"){
	  label = _U("\uf510"); }

	else if(iconName == "fa-odysee"){
	  label = _U("\ue5c6"); }

	else if(iconName == "fa-square-whatsapp"){
	  label = _U("\uf40c"); }

	else if(iconName == "fa-whatsapp-square"){
	  label = _U("\uf40c"); }

	else if(iconName == "fa-node-js"){
	  label = _U("\uf3d3"); }

	else if(iconName == "fa-edge-legacy"){
	  label = _U("\ue078"); }

	else if(iconName == "fa-slack"){
	  label = _U("\uf198"); }

	else if(iconName == "fa-slack-hash"){
	  label = _U("\uf198"); }

	else if(iconName == "fa-medrt"){
	  label = _U("\uf3c8"); }

	else if(iconName == "fa-usb"){
	  label = _U("\uf287"); }

	else if(iconName == "fa-tumblr"){
	  label = _U("\uf173"); }

	else if(iconName == "fa-vaadin"){
	  label = _U("\uf408"); }

	else if(iconName == "fa-quora"){
	  label = _U("\uf2c4"); }

	else if(iconName == "fa-square-x-twitter"){
	  label = _U("\ue61a"); }

	else if(iconName == "fa-reacteurope"){
	  label = _U("\uf75d"); }

	else if(iconName == "fa-medium"){
	  label = _U("\uf23a"); }

	else if(iconName == "fa-medium-m"){
	  label = _U("\uf23a"); }

	else if(iconName == "fa-amilia"){
	  label = _U("\uf36d"); }

	else if(iconName == "fa-mixcloud"){
	  label = _U("\uf289"); }

	else if(iconName == "fa-flipboard"){
	  label = _U("\uf44d"); }

	else if(iconName == "fa-viacoin"){
	  label = _U("\uf237"); }

	else if(iconName == "fa-critical-role"){
	  label = _U("\uf6c9"); }

	else if(iconName == "fa-sitrox"){
	  label = _U("\ue44a"); }

	else if(iconName == "fa-discourse"){
	  label = _U("\uf393"); }

	else if(iconName == "fa-joomla"){
	  label = _U("\uf1aa"); }

	else if(iconName == "fa-mastodon"){
	  label = _U("\uf4f6"); }

	else if(iconName == "fa-airbnb"){
	  label = _U("\uf834"); }

	else if(iconName == "fa-wolf-pack-battalion"){
	  label = _U("\uf514"); }

	else if(iconName == "fa-buy-n-large"){
	  label = _U("\uf8a6"); }

	else if(iconName == "fa-gulp"){
	  label = _U("\uf3ae"); }

	else if(iconName == "fa-creative-commons-sampling-plus"){
	  label = _U("\uf4f1"); }

	else if(iconName == "fa-strava"){
	  label = _U("\uf428"); }

	else if(iconName == "fa-ember"){
	  label = _U("\uf423"); }

	else if(iconName == "fa-canadian-maple-leaf"){
	  label = _U("\uf785"); }

	else if(iconName == "fa-teamspeak"){
	  label = _U("\uf4f9"); }

	else if(iconName == "fa-pushed"){
	  label = _U("\uf3e1"); }

	else if(iconName == "fa-wordpress-simple"){
	  label = _U("\uf411"); }

	else if(iconName == "fa-nutritionix"){
	  label = _U("\uf3d6"); }

	else if(iconName == "fa-wodu"){
	  label = _U("\ue088"); }

	else if(iconName == "fa-google-pay"){
	  label = _U("\ue079"); }

	else if(iconName == "fa-intercom"){
	  label = _U("\uf7af"); }

	else if(iconName == "fa-zhihu"){
	  label = _U("\uf63f"); }

	else if(iconName == "fa-korvue"){
	  label = _U("\uf42f"); }

	else if(iconName == "fa-pix"){
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
	mTitle->setText(Utils::String::toUpper(title));

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
