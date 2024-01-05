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

		else if(iconName == "fa-500px"){label = _U("\uf26e");}
	else if(iconName == "fa-address-book"){label = _U("\uf2b9");}
	else if(iconName == "fa-address-book-o"){label = _U("\uf2ba");}
	else if(iconName == "fa-address-card"){label = _U("\uf2bb");}
	else if(iconName == "fa-address-card-o"){label = _U("\uf2bc");}
	else if(iconName == "fa-adjust"){label = _U("\uf042");}
	else if(iconName == "fa-adn"){label = _U("\uf170");}
	else if(iconName == "fa-align-center"){label = _U("\uf037");}
	else if(iconName == "fa-align-justify"){label = _U("\uf039");}
	else if(iconName == "fa-align-left"){label = _U("\uf036");}
	else if(iconName == "fa-align-right"){label = _U("\uf038");}
	else if(iconName == "fa-amazon"){label = _U("\uf270");}
	else if(iconName == "fa-ambulance"){label = _U("\uf0f9");}
	else if(iconName == "fa-american-sign-language-interpreting"){label = _U("\uf2a3");}
	else if(iconName == "fa-anchor"){label = _U("\uf13d");}
	else if(iconName == "fa-android"){label = _U("\uf17b");}
	else if(iconName == "fa-angellist"){label = _U("\uf209");}
	else if(iconName == "fa-angle-double-down"){label = _U("\uf103");}
	else if(iconName == "fa-angle-double-left"){label = _U("\uf100");}
	else if(iconName == "fa-angle-double-right"){label = _U("\uf101");}
	else if(iconName == "fa-angle-double-up"){label = _U("\uf102");}
	else if(iconName == "fa-angle-down"){label = _U("\uf107");}
	else if(iconName == "fa-angle-left"){label = _U("\uf104");}
	else if(iconName == "fa-angle-right"){label = _U("\uf105");}
	else if(iconName == "fa-angle-up"){label = _U("\uf106");}
	else if(iconName == "fa-apple"){label = _U("\uf179");}
	else if(iconName == "fa-archive"){label = _U("\uf187");}
	else if(iconName == "fa-area-chart"){label = _U("\uf1fe");}
	else if(iconName == "fa-arrow-circle-down"){label = _U("\uf0ab");}
	else if(iconName == "fa-arrow-circle-left"){label = _U("\uf0a8");}
	else if(iconName == "fa-arrow-circle-o-down"){label = _U("\uf01a");}
	else if(iconName == "fa-arrow-circle-o-left"){label = _U("\uf190");}
	else if(iconName == "fa-arrow-circle-o-right"){label = _U("\uf18e");}
	else if(iconName == "fa-arrow-circle-o-up"){label = _U("\uf01b");}
	else if(iconName == "fa-arrow-circle-right"){label = _U("\uf0a9");}
	else if(iconName == "fa-arrow-circle-up"){label = _U("\uf0aa");}
	else if(iconName == "fa-arrow-down"){label = _U("\uf063");}
	else if(iconName == "fa-arrow-left"){label = _U("\uf060");}
	else if(iconName == "fa-arrow-right"){label = _U("\uf061");}
	else if(iconName == "fa-arrow-up"){label = _U("\uf062");}
	else if(iconName == "fa-arrows"){label = _U("\uf047");}
	else if(iconName == "fa-arrows-alt"){label = _U("\uf0b2");}
	else if(iconName == "fa-arrows-h"){label = _U("\uf07e");}
	else if(iconName == "fa-arrows-v"){label = _U("\uf07d");}
	else if(iconName == "fa-asl-interpreting (alias)"){label = _U("\uf2a3");}
	else if(iconName == "fa-assistive-listening-systems"){label = _U("\uf2a2");}
	else if(iconName == "fa-asterisk"){label = _U("\uf069");}
	else if(iconName == "fa-at"){label = _U("\uf1fa");}
	else if(iconName == "fa-audio-description"){label = _U("\uf29e");}
	else if(iconName == "fa-automobile (alias)"){label = _U("\uf1b9");}
	else if(iconName == "fa-backward"){label = _U("\uf04a");}
	else if(iconName == "fa-balance-scale"){label = _U("\uf24e");}
	else if(iconName == "fa-ban"){label = _U("\uf05e");}
	else if(iconName == "fa-bandcamp"){label = _U("\uf2d5");}
	else if(iconName == "fa-bank (alias)"){label = _U("\uf19c");}
	else if(iconName == "fa-bar-chart"){label = _U("\uf080");}
	else if(iconName == "fa-bar-chart-o (alias)"){label = _U("\uf080");}
	else if(iconName == "fa-barcode"){label = _U("\uf02a");}
	else if(iconName == "fa-bars"){label = _U("\uf0c9");}
	else if(iconName == "fa-bath"){label = _U("\uf2cd");}
	else if(iconName == "fa-bathtub (alias)"){label = _U("\uf2cd");}
	else if(iconName == "fa-battery (alias)"){label = _U("\uf240");}
	else if(iconName == "fa-battery-0 (alias)"){label = _U("\uf244");}
	else if(iconName == "fa-battery-1 (alias)"){label = _U("\uf243");}
	else if(iconName == "fa-battery-2 (alias)"){label = _U("\uf242");}
	else if(iconName == "fa-battery-3 (alias)"){label = _U("\uf241");}
	else if(iconName == "fa-battery-4 (alias)"){label = _U("\uf240");}
	else if(iconName == "fa-battery-empty"){label = _U("\uf244");}
	else if(iconName == "fa-battery-full"){label = _U("\uf240");}
	else if(iconName == "fa-battery-half"){label = _U("\uf242");}
	else if(iconName == "fa-battery-quarter"){label = _U("\uf243");}
	else if(iconName == "fa-battery-three-quarters"){label = _U("\uf241");}
	else if(iconName == "fa-bed"){label = _U("\uf236");}
	else if(iconName == "fa-beer"){label = _U("\uf0fc");}
	else if(iconName == "fa-behance"){label = _U("\uf1b4");}
	else if(iconName == "fa-behance-square"){label = _U("\uf1b5");}
	else if(iconName == "fa-bell"){label = _U("\uf0f3");}
	else if(iconName == "fa-bell-o"){label = _U("\uf0a2");}
	else if(iconName == "fa-bell-slash"){label = _U("\uf1f6");}
	else if(iconName == "fa-bell-slash-o"){label = _U("\uf1f7");}
	else if(iconName == "fa-bicycle"){label = _U("\uf206");}
	else if(iconName == "fa-binoculars"){label = _U("\uf1e5");}
	else if(iconName == "fa-birthday-cake"){label = _U("\uf1fd");}
	else if(iconName == "fa-bitbucket"){label = _U("\uf171");}
	else if(iconName == "fa-bitbucket-square"){label = _U("\uf172");}
	else if(iconName == "fa-bitcoin (alias)"){label = _U("\uf15a");}
	else if(iconName == "fa-black-tie"){label = _U("\uf27e");}
	else if(iconName == "fa-blind"){label = _U("\uf29d");}
	else if(iconName == "fa-bluetooth"){label = _U("\uf293");}
	else if(iconName == "fa-bluetooth-b"){label = _U("\uf294");}
	else if(iconName == "fa-bold"){label = _U("\uf032");}
	else if(iconName == "fa-bolt"){label = _U("\uf0e7");}
	else if(iconName == "fa-bomb"){label = _U("\uf1e2");}
	else if(iconName == "fa-book"){label = _U("\uf02d");}
	else if(iconName == "fa-bookmark"){label = _U("\uf02e");}
	else if(iconName == "fa-bookmark-o"){label = _U("\uf097");}
	else if(iconName == "fa-braille"){label = _U("\uf2a1");}
	else if(iconName == "fa-briefcase"){label = _U("\uf0b1");}
	else if(iconName == "fa-btc"){label = _U("\uf15a");}
	else if(iconName == "fa-bug"){label = _U("\uf188");}
	else if(iconName == "fa-building"){label = _U("\uf1ad");}
	else if(iconName == "fa-building-o"){label = _U("\uf0f7");}
	else if(iconName == "fa-bullhorn"){label = _U("\uf0a1");}
	else if(iconName == "fa-bullseye"){label = _U("\uf140");}
	else if(iconName == "fa-bus"){label = _U("\uf207");}
	else if(iconName == "fa-buysellads"){label = _U("\uf20d");}
	else if(iconName == "fa-cab (alias)"){label = _U("\uf1ba");}
	else if(iconName == "fa-calculator"){label = _U("\uf1ec");}
	else if(iconName == "fa-calendar"){label = _U("\uf073");}
	else if(iconName == "fa-calendar-check-o"){label = _U("\uf274");}
	else if(iconName == "fa-calendar-minus-o"){label = _U("\uf272");}
	else if(iconName == "fa-calendar-o"){label = _U("\uf133");}
	else if(iconName == "fa-calendar-plus-o"){label = _U("\uf271");}
	else if(iconName == "fa-calendar-times-o"){label = _U("\uf273");}
	else if(iconName == "fa-camera"){label = _U("\uf030");}
	else if(iconName == "fa-camera-retro"){label = _U("\uf083");}
	else if(iconName == "fa-car"){label = _U("\uf1b9");}
	else if(iconName == "fa-caret-down"){label = _U("\uf0d7");}
	else if(iconName == "fa-caret-left"){label = _U("\uf0d9");}
	else if(iconName == "fa-caret-right"){label = _U("\uf0da");}
	else if(iconName == "fa-caret-square-o-down"){label = _U("\uf150");}
	else if(iconName == "fa-caret-square-o-left"){label = _U("\uf191");}
	else if(iconName == "fa-caret-square-o-right"){label = _U("\uf152");}
	else if(iconName == "fa-caret-square-o-up"){label = _U("\uf151");}
	else if(iconName == "fa-caret-up"){label = _U("\uf0d8");}
	else if(iconName == "fa-cart-arrow-down"){label = _U("\uf218");}
	else if(iconName == "fa-cart-plus"){label = _U("\uf217");}
	else if(iconName == "fa-cc"){label = _U("\uf20a");}
	else if(iconName == "fa-cc-amex"){label = _U("\uf1f3");}
	else if(iconName == "fa-cc-diners-club"){label = _U("\uf24c");}
	else if(iconName == "fa-cc-discover"){label = _U("\uf1f2");}
	else if(iconName == "fa-cc-jcb"){label = _U("\uf24b");}
	else if(iconName == "fa-cc-mastercard"){label = _U("\uf1f1");}
	else if(iconName == "fa-cc-paypal"){label = _U("\uf1f4");}
	else if(iconName == "fa-cc-stripe"){label = _U("\uf1f5");}
	else if(iconName == "fa-cc-visa"){label = _U("\uf1f0");}
	else if(iconName == "fa-certificate"){label = _U("\uf0a3");}
	else if(iconName == "fa-chain (alias)"){label = _U("\uf0c1");}
	else if(iconName == "fa-chain-broken"){label = _U("\uf127");}
	else if(iconName == "fa-check"){label = _U("\uf00c");}
	else if(iconName == "fa-check-circle"){label = _U("\uf058");}
	else if(iconName == "fa-check-circle-o"){label = _U("\uf05d");}
	else if(iconName == "fa-check-square"){label = _U("\uf14a");}
	else if(iconName == "fa-check-square-o"){label = _U("\uf046");}
	else if(iconName == "fa-chevron-circle-down"){label = _U("\uf13a");}
	else if(iconName == "fa-chevron-circle-left"){label = _U("\uf137");}
	else if(iconName == "fa-chevron-circle-right"){label = _U("\uf138");}
	else if(iconName == "fa-chevron-circle-up"){label = _U("\uf139");}
	else if(iconName == "fa-chevron-down"){label = _U("\uf078");}
	else if(iconName == "fa-chevron-left"){label = _U("\uf053");}
	else if(iconName == "fa-chevron-right"){label = _U("\uf054");}
	else if(iconName == "fa-chevron-up"){label = _U("\uf077");}
	else if(iconName == "fa-child"){label = _U("\uf1ae");}
	else if(iconName == "fa-chrome"){label = _U("\uf268");}
	else if(iconName == "fa-circle"){label = _U("\uf111");}
	else if(iconName == "fa-circle-o"){label = _U("\uf10c");}
	else if(iconName == "fa-circle-o-notch"){label = _U("\uf1ce");}
	else if(iconName == "fa-circle-thin"){label = _U("\uf1db");}
	else if(iconName == "fa-clipboard"){label = _U("\uf0ea");}
	else if(iconName == "fa-clock-o"){label = _U("\uf017");}
	else if(iconName == "fa-clone"){label = _U("\uf24d");}
	else if(iconName == "fa-close (alias)"){label = _U("\uf00d");}
	else if(iconName == "fa-cloud"){label = _U("\uf0c2");}
	else if(iconName == "fa-cloud-download"){label = _U("\uf0ed");}
	else if(iconName == "fa-cloud-upload"){label = _U("\uf0ee");}
	else if(iconName == "fa-cny (alias)"){label = _U("\uf157");}
	else if(iconName == "fa-code"){label = _U("\uf121");}
	else if(iconName == "fa-code-fork"){label = _U("\uf126");}
	else if(iconName == "fa-codepen"){label = _U("\uf1cb");}
	else if(iconName == "fa-codiepie"){label = _U("\uf284");}
	else if(iconName == "fa-coffee"){label = _U("\uf0f4");}
	else if(iconName == "fa-cog"){label = _U("\uf013");}
	else if(iconName == "fa-cogs"){label = _U("\uf085");}
	else if(iconName == "fa-columns"){label = _U("\uf0db");}
	else if(iconName == "fa-comment"){label = _U("\uf075");}
	else if(iconName == "fa-comment-o"){label = _U("\uf0e5");}
	else if(iconName == "fa-commenting"){label = _U("\uf27a");}
	else if(iconName == "fa-commenting-o"){label = _U("\uf27b");}
	else if(iconName == "fa-comments"){label = _U("\uf086");}
	else if(iconName == "fa-comments-o"){label = _U("\uf0e6");}
	else if(iconName == "fa-compass"){label = _U("\uf14e");}
	else if(iconName == "fa-compress"){label = _U("\uf066");}
	else if(iconName == "fa-connectdevelop"){label = _U("\uf20e");}
	else if(iconName == "fa-contao"){label = _U("\uf26d");}
	else if(iconName == "fa-copy (alias)"){label = _U("\uf0c5");}
	else if(iconName == "fa-copyright"){label = _U("\uf1f9");}
	else if(iconName == "fa-creative-commons"){label = _U("\uf25e");}
	else if(iconName == "fa-credit-card"){label = _U("\uf09d");}
	else if(iconName == "fa-credit-card-alt"){label = _U("\uf283");}
	else if(iconName == "fa-crop"){label = _U("\uf125");}
	else if(iconName == "fa-crosshairs"){label = _U("\uf05b");}
	else if(iconName == "fa-css3"){label = _U("\uf13c");}
	else if(iconName == "fa-cube"){label = _U("\uf1b2");}
	else if(iconName == "fa-cubes"){label = _U("\uf1b3");}
	else if(iconName == "fa-cut (alias)"){label = _U("\uf0c4");}
	else if(iconName == "fa-cutlery"){label = _U("\uf0f5");}
	else if(iconName == "fa-dashboard (alias)"){label = _U("\uf0e4");}
	else if(iconName == "fa-dashcube"){label = _U("\uf210");}
	else if(iconName == "fa-database"){label = _U("\uf1c0");}
	else if(iconName == "fa-deaf"){label = _U("\uf2a4");}
	else if(iconName == "fa-deafness (alias)"){label = _U("\uf2a4");}
	else if(iconName == "fa-dedent (alias)"){label = _U("\uf03b");}
	else if(iconName == "fa-delicious"){label = _U("\uf1a5");}
	else if(iconName == "fa-desktop"){label = _U("\uf108");}
	else if(iconName == "fa-deviantart"){label = _U("\uf1bd");}
	else if(iconName == "fa-diamond"){label = _U("\uf219");}
	else if(iconName == "fa-digg"){label = _U("\uf1a6");}
	else if(iconName == "fa-dollar (alias)"){label = _U("\uf155");}
	else if(iconName == "fa-dot-circle-o"){label = _U("\uf192");}
	else if(iconName == "fa-download"){label = _U("\uf019");}
	else if(iconName == "fa-dribbble"){label = _U("\uf17d");}
	else if(iconName == "fa-drivers-license (alias)"){label = _U("\uf2c2");}
	else if(iconName == "fa-drivers-license-o (alias)"){label = _U("\uf2c3");}
	else if(iconName == "fa-dropbox"){label = _U("\uf16b");}
	else if(iconName == "fa-drupal"){label = _U("\uf1a9");}
	else if(iconName == "fa-edge"){label = _U("\uf282");}
	else if(iconName == "fa-edit (alias)"){label = _U("\uf044");}
	else if(iconName == "fa-eercast"){label = _U("\uf2da");}
	else if(iconName == "fa-eject"){label = _U("\uf052");}
	else if(iconName == "fa-ellipsis-h"){label = _U("\uf141");}
	else if(iconName == "fa-ellipsis-v"){label = _U("\uf142");}
	else if(iconName == "fa-empire"){label = _U("\uf1d1");}
	else if(iconName == "fa-envelope"){label = _U("\uf0e0");}
	else if(iconName == "fa-envelope-o"){label = _U("\uf003");}
	else if(iconName == "fa-envelope-open"){label = _U("\uf2b6");}
	else if(iconName == "fa-envelope-open-o"){label = _U("\uf2b7");}
	else if(iconName == "fa-envelope-square"){label = _U("\uf199");}
	else if(iconName == "fa-envira"){label = _U("\uf299");}
	else if(iconName == "fa-eraser"){label = _U("\uf12d");}
	else if(iconName == "fa-etsy"){label = _U("\uf2d7");}
	else if(iconName == "fa-eur"){label = _U("\uf153");}
	else if(iconName == "fa-euro (alias)"){label = _U("\uf153");}
	else if(iconName == "fa-exchange"){label = _U("\uf0ec");}
	else if(iconName == "fa-exclamation"){label = _U("\uf12a");}
	else if(iconName == "fa-exclamation-circle"){label = _U("\uf06a");}
	else if(iconName == "fa-exclamation-triangle"){label = _U("\uf071");}
	else if(iconName == "fa-expand"){label = _U("\uf065");}
	else if(iconName == "fa-expeditedssl"){label = _U("\uf23e");}
	else if(iconName == "fa-external-link"){label = _U("\uf08e");}
	else if(iconName == "fa-external-link-square"){label = _U("\uf14c");}
	else if(iconName == "fa-eye"){label = _U("\uf06e");}
	else if(iconName == "fa-eye-slash"){label = _U("\uf070");}
	else if(iconName == "fa-eyedropper"){label = _U("\uf1fb");}
	else if(iconName == "fa-fa (alias)"){label = _U("\uf2b4");}
	else if(iconName == "fa-facebook"){label = _U("\uf09a");}
	else if(iconName == "fa-facebook-f (alias)"){label = _U("\uf09a");}
	else if(iconName == "fa-facebook-official"){label = _U("\uf230");}
	else if(iconName == "fa-facebook-square"){label = _U("\uf082");}
	else if(iconName == "fa-fast-backward"){label = _U("\uf049");}
	else if(iconName == "fa-fast-forward"){label = _U("\uf050");}
	else if(iconName == "fa-fax"){label = _U("\uf1ac");}
	else if(iconName == "fa-feed (alias)"){label = _U("\uf09e");}
	else if(iconName == "fa-female"){label = _U("\uf182");}
	else if(iconName == "fa-fighter-jet"){label = _U("\uf0fb");}
	else if(iconName == "fa-file"){label = _U("\uf15b");}
	else if(iconName == "fa-file-archive-o"){label = _U("\uf1c6");}
	else if(iconName == "fa-file-audio-o"){label = _U("\uf1c7");}
	else if(iconName == "fa-file-code-o"){label = _U("\uf1c9");}
	else if(iconName == "fa-file-excel-o"){label = _U("\uf1c3");}
	else if(iconName == "fa-file-image-o"){label = _U("\uf1c5");}
	else if(iconName == "fa-file-movie-o (alias)"){label = _U("\uf1c8");}
	else if(iconName == "fa-file-o"){label = _U("\uf016");}
	else if(iconName == "fa-file-pdf-o"){label = _U("\uf1c1");}
	else if(iconName == "fa-file-photo-o (alias)"){label = _U("\uf1c5");}
	else if(iconName == "fa-file-picture-o (alias)"){label = _U("\uf1c5");}
	else if(iconName == "fa-file-powerpoint-o"){label = _U("\uf1c4");}
	else if(iconName == "fa-file-sound-o (alias)"){label = _U("\uf1c7");}
	else if(iconName == "fa-file-text"){label = _U("\uf15c");}
	else if(iconName == "fa-file-text-o"){label = _U("\uf0f6");}
	else if(iconName == "fa-file-video-o"){label = _U("\uf1c8");}
	else if(iconName == "fa-file-word-o"){label = _U("\uf1c2");}
	else if(iconName == "fa-file-zip-o (alias)"){label = _U("\uf1c6");}
	else if(iconName == "fa-files-o"){label = _U("\uf0c5");}
	else if(iconName == "fa-film"){label = _U("\uf008");}
	else if(iconName == "fa-filter"){label = _U("\uf0b0");}
	else if(iconName == "fa-fire"){label = _U("\uf06d");}
	else if(iconName == "fa-fire-extinguisher"){label = _U("\uf134");}
	else if(iconName == "fa-firefox"){label = _U("\uf269");}
	else if(iconName == "fa-first-order"){label = _U("\uf2b0");}
	else if(iconName == "fa-flag"){label = _U("\uf024");}
	else if(iconName == "fa-flag-checkered"){label = _U("\uf11e");}
	else if(iconName == "fa-flag-o"){label = _U("\uf11d");}
	else if(iconName == "fa-flash (alias)"){label = _U("\uf0e7");}
	else if(iconName == "fa-flask"){label = _U("\uf0c3");}
	else if(iconName == "fa-flickr"){label = _U("\uf16e");}
	else if(iconName == "fa-floppy-o"){label = _U("\uf0c7");}
	else if(iconName == "fa-folder"){label = _U("\uf07b");}
	else if(iconName == "fa-folder-o"){label = _U("\uf114");}
	else if(iconName == "fa-folder-open"){label = _U("\uf07c");}
	else if(iconName == "fa-folder-open-o"){label = _U("\uf115");}
	else if(iconName == "fa-font"){label = _U("\uf031");}
	else if(iconName == "fa-font-awesome"){label = _U("\uf2b4");}
	else if(iconName == "fa-fonticons"){label = _U("\uf280");}
	else if(iconName == "fa-fort-awesome"){label = _U("\uf286");}
	else if(iconName == "fa-forumbee"){label = _U("\uf211");}
	else if(iconName == "fa-forward"){label = _U("\uf04e");}
	else if(iconName == "fa-foursquare"){label = _U("\uf180");}
	else if(iconName == "fa-free-code-camp"){label = _U("\uf2c5");}
	else if(iconName == "fa-frown-o"){label = _U("\uf119");}
	else if(iconName == "fa-futbol-o"){label = _U("\uf1e3");}
	else if(iconName == "fa-gamepad"){label = _U("\uf11b");}
	else if(iconName == "fa-gavel"){label = _U("\uf0e3");}
	else if(iconName == "fa-gbp"){label = _U("\uf154");}
	else if(iconName == "fa-ge (alias)"){label = _U("\uf1d1");}
	else if(iconName == "fa-gear (alias)"){label = _U("\uf013");}
	else if(iconName == "fa-gears (alias)"){label = _U("\uf085");}
	else if(iconName == "fa-genderless"){label = _U("\uf22d");}
	else if(iconName == "fa-get-pocket"){label = _U("\uf265");}
	else if(iconName == "fa-gg"){label = _U("\uf260");}
	else if(iconName == "fa-gg-circle"){label = _U("\uf261");}
	else if(iconName == "fa-gift"){label = _U("\uf06b");}
	else if(iconName == "fa-git"){label = _U("\uf1d3");}
	else if(iconName == "fa-git-square"){label = _U("\uf1d2");}
	else if(iconName == "fa-github"){label = _U("\uf09b");}
	else if(iconName == "fa-github-alt"){label = _U("\uf113");}
	else if(iconName == "fa-github-square"){label = _U("\uf092");}
	else if(iconName == "fa-gitlab"){label = _U("\uf296");}
	else if(iconName == "fa-gittip (alias)"){label = _U("\uf184");}
	else if(iconName == "fa-glass"){label = _U("\uf000");}
	else if(iconName == "fa-glide"){label = _U("\uf2a5");}
	else if(iconName == "fa-glide-g"){label = _U("\uf2a6");}
	else if(iconName == "fa-globe"){label = _U("\uf0ac");}
	else if(iconName == "fa-google"){label = _U("\uf1a0");}
	else if(iconName == "fa-google-plus"){label = _U("\uf0d5");}
	else if(iconName == "fa-google-plus-circle (alias)"){label = _U("\uf2b3");}
	else if(iconName == "fa-google-plus-official"){label = _U("\uf2b3");}
	else if(iconName == "fa-google-plus-square"){label = _U("\uf0d4");}
	else if(iconName == "fa-google-wallet"){label = _U("\uf1ee");}
	else if(iconName == "fa-graduation-cap"){label = _U("\uf19d");}
	else if(iconName == "fa-gratipay"){label = _U("\uf184");}
	else if(iconName == "fa-grav"){label = _U("\uf2d6");}
	else if(iconName == "fa-group (alias)"){label = _U("\uf0c0");}
	else if(iconName == "fa-h-square"){label = _U("\uf0fd");}
	else if(iconName == "fa-hacker-news"){label = _U("\uf1d4");}
	else if(iconName == "fa-hand-grab-o (alias)"){label = _U("\uf255");}
	else if(iconName == "fa-hand-lizard-o"){label = _U("\uf258");}
	else if(iconName == "fa-hand-o-down"){label = _U("\uf0a7");}
	else if(iconName == "fa-hand-o-left"){label = _U("\uf0a5");}
	else if(iconName == "fa-hand-o-right"){label = _U("\uf0a4");}
	else if(iconName == "fa-hand-o-up"){label = _U("\uf0a6");}
	else if(iconName == "fa-hand-paper-o"){label = _U("\uf256");}
	else if(iconName == "fa-hand-peace-o"){label = _U("\uf25b");}
	else if(iconName == "fa-hand-pointer-o"){label = _U("\uf25a");}
	else if(iconName == "fa-hand-rock-o"){label = _U("\uf255");}
	else if(iconName == "fa-hand-scissors-o"){label = _U("\uf257");}
	else if(iconName == "fa-hand-spock-o"){label = _U("\uf259");}
	else if(iconName == "fa-hand-stop-o (alias)"){label = _U("\uf256");}
	else if(iconName == "fa-handshake-o"){label = _U("\uf2b5");}
	else if(iconName == "fa-hard-of-hearing (alias)"){label = _U("\uf2a4");}
	else if(iconName == "fa-hashtag"){label = _U("\uf292");}
	else if(iconName == "fa-hdd-o"){label = _U("\uf0a0");}
	else if(iconName == "fa-header"){label = _U("\uf1dc");}
	else if(iconName == "fa-headphones"){label = _U("\uf025");}
	else if(iconName == "fa-heart"){label = _U("\uf004");}
	else if(iconName == "fa-heart-o"){label = _U("\uf08a");}
	else if(iconName == "fa-heartbeat"){label = _U("\uf21e");}
	else if(iconName == "fa-history"){label = _U("\uf1da");}
	else if(iconName == "fa-home"){label = _U("\uf015");}
	else if(iconName == "fa-hospital-o"){label = _U("\uf0f8");}
	else if(iconName == "fa-hotel (alias)"){label = _U("\uf236");}
	else if(iconName == "fa-hourglass"){label = _U("\uf254");}
	else if(iconName == "fa-hourglass-1 (alias)"){label = _U("\uf251");}
	else if(iconName == "fa-hourglass-2 (alias)"){label = _U("\uf252");}
	else if(iconName == "fa-hourglass-3 (alias)"){label = _U("\uf253");}
	else if(iconName == "fa-hourglass-end"){label = _U("\uf253");}
	else if(iconName == "fa-hourglass-half"){label = _U("\uf252");}
	else if(iconName == "fa-hourglass-o"){label = _U("\uf250");}
	else if(iconName == "fa-hourglass-start"){label = _U("\uf251");}
	else if(iconName == "fa-houzz"){label = _U("\uf27c");}
	else if(iconName == "fa-html5"){label = _U("\uf13b");}
	else if(iconName == "fa-i-cursor"){label = _U("\uf246");}
	else if(iconName == "fa-id-badge"){label = _U("\uf2c1");}
	else if(iconName == "fa-id-card"){label = _U("\uf2c2");}
	else if(iconName == "fa-id-card-o"){label = _U("\uf2c3");}
	else if(iconName == "fa-ils"){label = _U("\uf20b");}
	else if(iconName == "fa-image (alias)"){label = _U("\uf03e");}
	else if(iconName == "fa-imdb"){label = _U("\uf2d8");}
	else if(iconName == "fa-inbox"){label = _U("\uf01c");}
	else if(iconName == "fa-indent"){label = _U("\uf03c");}
	else if(iconName == "fa-industry"){label = _U("\uf275");}
	else if(iconName == "fa-info"){label = _U("\uf129");}
	else if(iconName == "fa-info-circle"){label = _U("\uf05a");}
	else if(iconName == "fa-inr"){label = _U("\uf156");}
	else if(iconName == "fa-instagram"){label = _U("\uf16d");}
	else if(iconName == "fa-institution (alias)"){label = _U("\uf19c");}
	else if(iconName == "fa-internet-explorer"){label = _U("\uf26b");}
	else if(iconName == "fa-intersex (alias)"){label = _U("\uf224");}
	else if(iconName == "fa-ioxhost"){label = _U("\uf208");}
	else if(iconName == "fa-italic"){label = _U("\uf033");}
	else if(iconName == "fa-joomla"){label = _U("\uf1aa");}
	else if(iconName == "fa-jpy"){label = _U("\uf157");}
	else if(iconName == "fa-jsfiddle"){label = _U("\uf1cc");}
	else if(iconName == "fa-key"){label = _U("\uf084");}
	else if(iconName == "fa-keyboard-o"){label = _U("\uf11c");}
	else if(iconName == "fa-krw"){label = _U("\uf159");}
	else if(iconName == "fa-language"){label = _U("\uf1ab");}
	else if(iconName == "fa-laptop"){label = _U("\uf109");}
	else if(iconName == "fa-lastfm"){label = _U("\uf202");}
	else if(iconName == "fa-lastfm-square"){label = _U("\uf203");}
	else if(iconName == "fa-leaf"){label = _U("\uf06c");}
	else if(iconName == "fa-leanpub"){label = _U("\uf212");}
	else if(iconName == "fa-legal (alias)"){label = _U("\uf0e3");}
	else if(iconName == "fa-lemon-o"){label = _U("\uf094");}
	else if(iconName == "fa-level-down"){label = _U("\uf149");}
	else if(iconName == "fa-level-up"){label = _U("\uf148");}
	else if(iconName == "fa-life-bouy (alias)"){label = _U("\uf1cd");}
	else if(iconName == "fa-life-buoy (alias)"){label = _U("\uf1cd");}
	else if(iconName == "fa-life-ring"){label = _U("\uf1cd");}
	else if(iconName == "fa-life-saver (alias)"){label = _U("\uf1cd");}
	else if(iconName == "fa-lightbulb-o"){label = _U("\uf0eb");}
	else if(iconName == "fa-line-chart"){label = _U("\uf201");}
	else if(iconName == "fa-link"){label = _U("\uf0c1");}
	else if(iconName == "fa-linkedin"){label = _U("\uf0e1");}
	else if(iconName == "fa-linkedin-square"){label = _U("\uf08c");}
	else if(iconName == "fa-linode"){label = _U("\uf2b8");}
	else if(iconName == "fa-linux"){label = _U("\uf17c");}
	else if(iconName == "fa-list"){label = _U("\uf03a");}
	else if(iconName == "fa-list-alt"){label = _U("\uf022");}
	else if(iconName == "fa-list-ol"){label = _U("\uf0cb");}
	else if(iconName == "fa-list-ul"){label = _U("\uf0ca");}
	else if(iconName == "fa-location-arrow"){label = _U("\uf124");}
	else if(iconName == "fa-lock"){label = _U("\uf023");}
	else if(iconName == "fa-long-arrow-down"){label = _U("\uf175");}
	else if(iconName == "fa-long-arrow-left"){label = _U("\uf177");}
	else if(iconName == "fa-long-arrow-right"){label = _U("\uf178");}
	else if(iconName == "fa-long-arrow-up"){label = _U("\uf176");}
	else if(iconName == "fa-low-vision"){label = _U("\uf2a8");}
	else if(iconName == "fa-magic"){label = _U("\uf0d0");}
	else if(iconName == "fa-magnet"){label = _U("\uf076");}
	else if(iconName == "fa-mail-forward (alias)"){label = _U("\uf064");}
	else if(iconName == "fa-mail-reply (alias)"){label = _U("\uf112");}
	else if(iconName == "fa-mail-reply-all (alias)"){label = _U("\uf122");}
	else if(iconName == "fa-male"){label = _U("\uf183");}
	else if(iconName == "fa-map"){label = _U("\uf279");}
	else if(iconName == "fa-map-marker"){label = _U("\uf041");}
	else if(iconName == "fa-map-o"){label = _U("\uf278");}
	else if(iconName == "fa-map-pin"){label = _U("\uf276");}
	else if(iconName == "fa-map-signs"){label = _U("\uf277");}
	else if(iconName == "fa-mars"){label = _U("\uf222");}
	else if(iconName == "fa-mars-double"){label = _U("\uf227");}
	else if(iconName == "fa-mars-stroke"){label = _U("\uf229");}
	else if(iconName == "fa-mars-stroke-h"){label = _U("\uf22b");}
	else if(iconName == "fa-mars-stroke-v"){label = _U("\uf22a");}
	else if(iconName == "fa-maxcdn"){label = _U("\uf136");}
	else if(iconName == "fa-meanpath"){label = _U("\uf20c");}
	else if(iconName == "fa-medium"){label = _U("\uf23a");}
	else if(iconName == "fa-medkit"){label = _U("\uf0fa");}
	else if(iconName == "fa-meetup"){label = _U("\uf2e0");}
	else if(iconName == "fa-meh-o"){label = _U("\uf11a");}
	else if(iconName == "fa-mercury"){label = _U("\uf223");}
	else if(iconName == "fa-microchip"){label = _U("\uf2db");}
	else if(iconName == "fa-microphone"){label = _U("\uf130");}
	else if(iconName == "fa-microphone-slash"){label = _U("\uf131");}
	else if(iconName == "fa-minus"){label = _U("\uf068");}
	else if(iconName == "fa-minus-circle"){label = _U("\uf056");}
	else if(iconName == "fa-minus-square"){label = _U("\uf146");}
	else if(iconName == "fa-minus-square-o"){label = _U("\uf147");}
	else if(iconName == "fa-mixcloud"){label = _U("\uf289");}
	else if(iconName == "fa-mobile"){label = _U("\uf10b");}
	else if(iconName == "fa-mobile-phone (alias)"){label = _U("\uf10b");}
	else if(iconName == "fa-modx"){label = _U("\uf285");}
	else if(iconName == "fa-money"){label = _U("\uf0d6");}
	else if(iconName == "fa-moon-o"){label = _U("\uf186");}
	else if(iconName == "fa-mortar-board (alias)"){label = _U("\uf19d");}
	else if(iconName == "fa-motorcycle"){label = _U("\uf21c");}
	else if(iconName == "fa-mouse-pointer"){label = _U("\uf245");}
	else if(iconName == "fa-music"){label = _U("\uf001");}
	else if(iconName == "fa-navicon (alias)"){label = _U("\uf0c9");}
	else if(iconName == "fa-neuter"){label = _U("\uf22c");}
	else if(iconName == "fa-newspaper-o"){label = _U("\uf1ea");}
	else if(iconName == "fa-object-group"){label = _U("\uf247");}
	else if(iconName == "fa-object-ungroup"){label = _U("\uf248");}
	else if(iconName == "fa-odnoklassniki"){label = _U("\uf263");}
	else if(iconName == "fa-odnoklassniki-square"){label = _U("\uf264");}
	else if(iconName == "fa-opencart"){label = _U("\uf23d");}
	else if(iconName == "fa-openid"){label = _U("\uf19b");}
	else if(iconName == "fa-opera"){label = _U("\uf26a");}
	else if(iconName == "fa-optin-monster"){label = _U("\uf23c");}
	else if(iconName == "fa-outdent"){label = _U("\uf03b");}
	else if(iconName == "fa-pagelines"){label = _U("\uf18c");}
	else if(iconName == "fa-paint-brush"){label = _U("\uf1fc");}
	else if(iconName == "fa-paper-plane"){label = _U("\uf1d8");}
	else if(iconName == "fa-paper-plane-o"){label = _U("\uf1d9");}
	else if(iconName == "fa-paperclip"){label = _U("\uf0c6");}
	else if(iconName == "fa-paragraph"){label = _U("\uf1dd");}
	else if(iconName == "fa-paste (alias)"){label = _U("\uf0ea");}
	else if(iconName == "fa-pause"){label = _U("\uf04c");}
	else if(iconName == "fa-pause-circle"){label = _U("\uf28b");}
	else if(iconName == "fa-pause-circle-o"){label = _U("\uf28c");}
	else if(iconName == "fa-paw"){label = _U("\uf1b0");}
	else if(iconName == "fa-paypal"){label = _U("\uf1ed");}
	else if(iconName == "fa-pencil"){label = _U("\uf040");}
	else if(iconName == "fa-pencil-square"){label = _U("\uf14b");}
	else if(iconName == "fa-pencil-square-o"){label = _U("\uf044");}
	else if(iconName == "fa-percent"){label = _U("\uf295");}
	else if(iconName == "fa-phone"){label = _U("\uf095");}
	else if(iconName == "fa-phone-square"){label = _U("\uf098");}
	else if(iconName == "fa-photo (alias)"){label = _U("\uf03e");}
	else if(iconName == "fa-picture-o"){label = _U("\uf03e");}
	else if(iconName == "fa-pie-chart"){label = _U("\uf200");}
	else if(iconName == "fa-pied-piper"){label = _U("\uf2ae");}
	else if(iconName == "fa-pied-piper-alt"){label = _U("\uf1a8");}
	else if(iconName == "fa-pied-piper-pp"){label = _U("\uf1a7");}
	else if(iconName == "fa-pinterest"){label = _U("\uf0d2");}
	else if(iconName == "fa-pinterest-p"){label = _U("\uf231");}
	else if(iconName == "fa-pinterest-square"){label = _U("\uf0d3");}
	else if(iconName == "fa-plane"){label = _U("\uf072");}
	else if(iconName == "fa-play"){label = _U("\uf04b");}
	else if(iconName == "fa-play-circle"){label = _U("\uf144");}
	else if(iconName == "fa-play-circle-o"){label = _U("\uf01d");}
	else if(iconName == "fa-plug"){label = _U("\uf1e6");}
	else if(iconName == "fa-plus"){label = _U("\uf067");}
	else if(iconName == "fa-plus-circle"){label = _U("\uf055");}
	else if(iconName == "fa-plus-square"){label = _U("\uf0fe");}
	else if(iconName == "fa-plus-square-o"){label = _U("\uf196");}
	else if(iconName == "fa-podcast"){label = _U("\uf2ce");}
	else if(iconName == "fa-power-off"){label = _U("\uf011");}
	else if(iconName == "fa-print"){label = _U("\uf02f");}
	else if(iconName == "fa-product-hunt"){label = _U("\uf288");}
	else if(iconName == "fa-puzzle-piece"){label = _U("\uf12e");}
	else if(iconName == "fa-qq"){label = _U("\uf1d6");}
	else if(iconName == "fa-qrcode"){label = _U("\uf029");}
	else if(iconName == "fa-question"){label = _U("\uf128");}
	else if(iconName == "fa-question-circle"){label = _U("\uf059");}
	else if(iconName == "fa-question-circle-o"){label = _U("\uf29c");}
	else if(iconName == "fa-quora"){label = _U("\uf2c4");}
	else if(iconName == "fa-quote-left"){label = _U("\uf10d");}
	else if(iconName == "fa-quote-right"){label = _U("\uf10e");}
	else if(iconName == "fa-ra (alias)"){label = _U("\uf1d0");}
	else if(iconName == "fa-random"){label = _U("\uf074");}
	else if(iconName == "fa-ravelry"){label = _U("\uf2d9");}
	else if(iconName == "fa-rebel"){label = _U("\uf1d0");}
	else if(iconName == "fa-recycle"){label = _U("\uf1b8");}
	else if(iconName == "fa-reddit"){label = _U("\uf1a1");}
	else if(iconName == "fa-reddit-alien"){label = _U("\uf281");}
	else if(iconName == "fa-reddit-square"){label = _U("\uf1a2");}
	else if(iconName == "fa-refresh"){label = _U("\uf021");}
	else if(iconName == "fa-registered"){label = _U("\uf25d");}
	else if(iconName == "fa-remove (alias)"){label = _U("\uf00d");}
	else if(iconName == "fa-renren"){label = _U("\uf18b");}
	else if(iconName == "fa-reorder (alias)"){label = _U("\uf0c9");}
	else if(iconName == "fa-repeat"){label = _U("\uf01e");}
	else if(iconName == "fa-reply"){label = _U("\uf112");}
	else if(iconName == "fa-reply-all"){label = _U("\uf122");}
	else if(iconName == "fa-resistance (alias)"){label = _U("\uf1d0");}
	else if(iconName == "fa-retweet"){label = _U("\uf079");}
	else if(iconName == "fa-rmb (alias)"){label = _U("\uf157");}
	else if(iconName == "fa-road"){label = _U("\uf018");}
	else if(iconName == "fa-rocket"){label = _U("\uf135");}
	else if(iconName == "fa-rotate-left (alias)"){label = _U("\uf0e2");}
	else if(iconName == "fa-rotate-right (alias)"){label = _U("\uf01e");}
	else if(iconName == "fa-rouble (alias)"){label = _U("\uf158");}
	else if(iconName == "fa-rss"){label = _U("\uf09e");}
	else if(iconName == "fa-rss-square"){label = _U("\uf143");}
	else if(iconName == "fa-rub"){label = _U("\uf158");}
	else if(iconName == "fa-ruble (alias)"){label = _U("\uf158");}
	else if(iconName == "fa-rupee (alias)"){label = _U("\uf156");}
	else if(iconName == "fa-s15 (alias)"){label = _U("\uf2cd");}
	else if(iconName == "fa-safari"){label = _U("\uf267");}
	else if(iconName == "fa-save (alias)"){label = _U("\uf0c7");}
	else if(iconName == "fa-scissors"){label = _U("\uf0c4");}
	else if(iconName == "fa-scribd"){label = _U("\uf28a");}
	else if(iconName == "fa-search"){label = _U("\uf002");}
	else if(iconName == "fa-search-minus"){label = _U("\uf010");}
	else if(iconName == "fa-search-plus"){label = _U("\uf00e");}
	else if(iconName == "fa-sellsy"){label = _U("\uf213");}
	else if(iconName == "fa-send (alias)"){label = _U("\uf1d8");}
	else if(iconName == "fa-send-o (alias)"){label = _U("\uf1d9");}
	else if(iconName == "fa-server"){label = _U("\uf233");}
	else if(iconName == "fa-share"){label = _U("\uf064");}
	else if(iconName == "fa-share-alt"){label = _U("\uf1e0");}
	else if(iconName == "fa-share-alt-square"){label = _U("\uf1e1");}
	else if(iconName == "fa-share-square"){label = _U("\uf14d");}
	else if(iconName == "fa-share-square-o"){label = _U("\uf045");}
	else if(iconName == "fa-shekel (alias)"){label = _U("\uf20b");}
	else if(iconName == "fa-sheqel (alias)"){label = _U("\uf20b");}
	else if(iconName == "fa-shield"){label = _U("\uf132");}
	else if(iconName == "fa-ship"){label = _U("\uf21a");}
	else if(iconName == "fa-shirtsinbulk"){label = _U("\uf214");}
	else if(iconName == "fa-shopping-bag"){label = _U("\uf290");}
	else if(iconName == "fa-shopping-basket"){label = _U("\uf291");}
	else if(iconName == "fa-shopping-cart"){label = _U("\uf07a");}
	else if(iconName == "fa-shower"){label = _U("\uf2cc");}
	else if(iconName == "fa-sign-in"){label = _U("\uf090");}
	else if(iconName == "fa-sign-language"){label = _U("\uf2a7");}
	else if(iconName == "fa-sign-out"){label = _U("\uf08b");}
	else if(iconName == "fa-signal"){label = _U("\uf012");}
	else if(iconName == "fa-signing (alias)"){label = _U("\uf2a7");}
	else if(iconName == "fa-simplybuilt"){label = _U("\uf215");}
	else if(iconName == "fa-sitemap"){label = _U("\uf0e8");}
	else if(iconName == "fa-skyatlas"){label = _U("\uf216");}
	else if(iconName == "fa-skype"){label = _U("\uf17e");}
	else if(iconName == "fa-slack"){label = _U("\uf198");}
	else if(iconName == "fa-sliders"){label = _U("\uf1de");}
	else if(iconName == "fa-slideshare"){label = _U("\uf1e7");}
	else if(iconName == "fa-smile-o"){label = _U("\uf118");}
	else if(iconName == "fa-snapchat"){label = _U("\uf2ab");}
	else if(iconName == "fa-snapchat-ghost"){label = _U("\uf2ac");}
	else if(iconName == "fa-snapchat-square"){label = _U("\uf2ad");}
	else if(iconName == "fa-snowflake-o"){label = _U("\uf2dc");}
	else if(iconName == "fa-soccer-ball-o (alias)"){label = _U("\uf1e3");}
	else if(iconName == "fa-sort"){label = _U("\uf0dc");}
	else if(iconName == "fa-sort-alpha-asc"){label = _U("\uf15d");}
	else if(iconName == "fa-sort-alpha-desc"){label = _U("\uf15e");}
	else if(iconName == "fa-sort-amount-asc"){label = _U("\uf160");}
	else if(iconName == "fa-sort-amount-desc"){label = _U("\uf161");}
	else if(iconName == "fa-sort-asc"){label = _U("\uf0de");}
	else if(iconName == "fa-sort-desc"){label = _U("\uf0dd");}
	else if(iconName == "fa-sort-down (alias)"){label = _U("\uf0dd");}
	else if(iconName == "fa-sort-numeric-asc"){label = _U("\uf162");}
	else if(iconName == "fa-sort-numeric-desc"){label = _U("\uf163");}
	else if(iconName == "fa-sort-up (alias)"){label = _U("\uf0de");}
	else if(iconName == "fa-soundcloud"){label = _U("\uf1be");}
	else if(iconName == "fa-space-shuttle"){label = _U("\uf197");}
	else if(iconName == "fa-spinner"){label = _U("\uf110");}
	else if(iconName == "fa-spoon"){label = _U("\uf1b1");}
	else if(iconName == "fa-spotify"){label = _U("\uf1bc");}
	else if(iconName == "fa-square"){label = _U("\uf0c8");}
	else if(iconName == "fa-square-o"){label = _U("\uf096");}
	else if(iconName == "fa-stack-exchange"){label = _U("\uf18d");}
	else if(iconName == "fa-stack-overflow"){label = _U("\uf16c");}
	else if(iconName == "fa-star"){label = _U("\uf005");}
	else if(iconName == "fa-star-half"){label = _U("\uf089");}
	else if(iconName == "fa-star-half-empty (alias)"){label = _U("\uf123");}
	else if(iconName == "fa-star-half-full (alias)"){label = _U("\uf123");}
	else if(iconName == "fa-star-half-o"){label = _U("\uf123");}
	else if(iconName == "fa-star-o"){label = _U("\uf006");}
	else if(iconName == "fa-steam"){label = _U("\uf1b6");}
	else if(iconName == "fa-steam-square"){label = _U("\uf1b7");}
	else if(iconName == "fa-step-backward"){label = _U("\uf048");}
	else if(iconName == "fa-step-forward"){label = _U("\uf051");}
	else if(iconName == "fa-stethoscope"){label = _U("\uf0f1");}
	else if(iconName == "fa-sticky-note"){label = _U("\uf249");}
	else if(iconName == "fa-sticky-note-o"){label = _U("\uf24a");}
	else if(iconName == "fa-stop"){label = _U("\uf04d");}
	else if(iconName == "fa-stop-circle"){label = _U("\uf28d");}
	else if(iconName == "fa-stop-circle-o"){label = _U("\uf28e");}
	else if(iconName == "fa-street-view"){label = _U("\uf21d");}
	else if(iconName == "fa-strikethrough"){label = _U("\uf0cc");}
	else if(iconName == "fa-stumbleupon"){label = _U("\uf1a4");}
	else if(iconName == "fa-stumbleupon-circle"){label = _U("\uf1a3");}
	else if(iconName == "fa-subscript"){label = _U("\uf12c");}
	else if(iconName == "fa-subway"){label = _U("\uf239");}
	else if(iconName == "fa-suitcase"){label = _U("\uf0f2");}
	else if(iconName == "fa-sun-o"){label = _U("\uf185");}
	else if(iconName == "fa-superpowers"){label = _U("\uf2dd");}
	else if(iconName == "fa-superscript"){label = _U("\uf12b");}
	else if(iconName == "fa-support (alias)"){label = _U("\uf1cd");}
	else if(iconName == "fa-table"){label = _U("\uf0ce");}
	else if(iconName == "fa-tablet"){label = _U("\uf10a");}
	else if(iconName == "fa-tachometer"){label = _U("\uf0e4");}
	else if(iconName == "fa-tag"){label = _U("\uf02b");}
	else if(iconName == "fa-tags"){label = _U("\uf02c");}
	else if(iconName == "fa-tasks"){label = _U("\uf0ae");}
	else if(iconName == "fa-taxi"){label = _U("\uf1ba");}
	else if(iconName == "fa-telegram"){label = _U("\uf2c6");}
	else if(iconName == "fa-television"){label = _U("\uf26c");}
	else if(iconName == "fa-tencent-weibo"){label = _U("\uf1d5");}
	else if(iconName == "fa-terminal"){label = _U("\uf120");}
	else if(iconName == "fa-text-height"){label = _U("\uf034");}
	else if(iconName == "fa-text-width"){label = _U("\uf035");}
	else if(iconName == "fa-th"){label = _U("\uf00a");}
	else if(iconName == "fa-th-large"){label = _U("\uf009");}
	else if(iconName == "fa-th-list"){label = _U("\uf00b");}
	else if(iconName == "fa-themeisle"){label = _U("\uf2b2");}
	else if(iconName == "fa-thermometer (alias)"){label = _U("\uf2c7");}
	else if(iconName == "fa-thermometer-0 (alias)"){label = _U("\uf2cb");}
	else if(iconName == "fa-thermometer-1 (alias)"){label = _U("\uf2ca");}
	else if(iconName == "fa-thermometer-2 (alias)"){label = _U("\uf2c9");}
	else if(iconName == "fa-thermometer-3 (alias)"){label = _U("\uf2c8");}
	else if(iconName == "fa-thermometer-4 (alias)"){label = _U("\uf2c7");}
	else if(iconName == "fa-thermometer-empty"){label = _U("\uf2cb");}
	else if(iconName == "fa-thermometer-full"){label = _U("\uf2c7");}
	else if(iconName == "fa-thermometer-half"){label = _U("\uf2c9");}
	else if(iconName == "fa-thermometer-quarter"){label = _U("\uf2ca");}
	else if(iconName == "fa-thermometer-three-quarters"){label = _U("\uf2c8");}
	else if(iconName == "fa-thumb-tack"){label = _U("\uf08d");}
	else if(iconName == "fa-thumbs-down"){label = _U("\uf165");}
	else if(iconName == "fa-thumbs-o-down"){label = _U("\uf088");}
	else if(iconName == "fa-thumbs-o-up"){label = _U("\uf087");}
	else if(iconName == "fa-thumbs-up"){label = _U("\uf164");}
	else if(iconName == "fa-ticket"){label = _U("\uf145");}
	else if(iconName == "fa-times"){label = _U("\uf00d");}
	else if(iconName == "fa-times-circle"){label = _U("\uf057");}
	else if(iconName == "fa-times-circle-o"){label = _U("\uf05c");}
	else if(iconName == "fa-times-rectangle (alias)"){label = _U("\uf2d3");}
	else if(iconName == "fa-times-rectangle-o (alias)"){label = _U("\uf2d4");}
	else if(iconName == "fa-tint"){label = _U("\uf043");}
	else if(iconName == "fa-toggle-down (alias)"){label = _U("\uf150");}
	else if(iconName == "fa-toggle-left (alias)"){label = _U("\uf191");}
	else if(iconName == "fa-toggle-off"){label = _U("\uf204");}
	else if(iconName == "fa-toggle-on"){label = _U("\uf205");}
	else if(iconName == "fa-toggle-right (alias)"){label = _U("\uf152");}
	else if(iconName == "fa-toggle-up (alias)"){label = _U("\uf151");}
	else if(iconName == "fa-trademark"){label = _U("\uf25c");}
	else if(iconName == "fa-train"){label = _U("\uf238");}
	else if(iconName == "fa-transgender"){label = _U("\uf224");}
	else if(iconName == "fa-transgender-alt"){label = _U("\uf225");}
	else if(iconName == "fa-trash"){label = _U("\uf1f8");}
	else if(iconName == "fa-trash-o"){label = _U("\uf014");}
	else if(iconName == "fa-tree"){label = _U("\uf1bb");}
	else if(iconName == "fa-trello"){label = _U("\uf181");}
	else if(iconName == "fa-tripadvisor"){label = _U("\uf262");}
	else if(iconName == "fa-trophy"){label = _U("\uf091");}
	else if(iconName == "fa-truck"){label = _U("\uf0d1");}
	else if(iconName == "fa-try"){label = _U("\uf195");}
	else if(iconName == "fa-tty"){label = _U("\uf1e4");}
	else if(iconName == "fa-tumblr"){label = _U("\uf173");}
	else if(iconName == "fa-tumblr-square"){label = _U("\uf174");}
	else if(iconName == "fa-turkish-lira (alias)"){label = _U("\uf195");}
	else if(iconName == "fa-tv (alias)"){label = _U("\uf26c");}
	else if(iconName == "fa-twitch"){label = _U("\uf1e8");}
	else if(iconName == "fa-twitter"){label = _U("\uf099");}
	else if(iconName == "fa-twitter-square"){label = _U("\uf081");}
	else if(iconName == "fa-umbrella"){label = _U("\uf0e9");}
	else if(iconName == "fa-underline"){label = _U("\uf0cd");}
	else if(iconName == "fa-undo"){label = _U("\uf0e2");}
	else if(iconName == "fa-universal-access"){label = _U("\uf29a");}
	else if(iconName == "fa-university"){label = _U("\uf19c");}
	else if(iconName == "fa-unlink (alias)"){label = _U("\uf127");}
	else if(iconName == "fa-unlock"){label = _U("\uf09c");}
	else if(iconName == "fa-unlock-alt"){label = _U("\uf13e");}
	else if(iconName == "fa-unsorted (alias)"){label = _U("\uf0dc");}
	else if(iconName == "fa-upload"){label = _U("\uf093");}
	else if(iconName == "fa-usb"){label = _U("\uf287");}
	else if(iconName == "fa-usd"){label = _U("\uf155");}
	else if(iconName == "fa-user"){label = _U("\uf007");}
	else if(iconName == "fa-user-circle"){label = _U("\uf2bd");}
	else if(iconName == "fa-user-circle-o"){label = _U("\uf2be");}
	else if(iconName == "fa-user-md"){label = _U("\uf0f0");}
	else if(iconName == "fa-user-o"){label = _U("\uf2c0");}
	else if(iconName == "fa-user-plus"){label = _U("\uf234");}
	else if(iconName == "fa-user-secret"){label = _U("\uf21b");}
	else if(iconName == "fa-user-times"){label = _U("\uf235");}
	else if(iconName == "fa-users"){label = _U("\uf0c0");}
	else if(iconName == "fa-vcard (alias)"){label = _U("\uf2bb");}
	else if(iconName == "fa-vcard-o (alias)"){label = _U("\uf2bc");}
	else if(iconName == "fa-venus"){label = _U("\uf221");}
	else if(iconName == "fa-venus-double"){label = _U("\uf226");}
	else if(iconName == "fa-venus-mars"){label = _U("\uf228");}
	else if(iconName == "fa-viacoin"){label = _U("\uf237");}
	else if(iconName == "fa-viadeo"){label = _U("\uf2a9");}
	else if(iconName == "fa-viadeo-square"){label = _U("\uf2aa");}
	else if(iconName == "fa-video-camera"){label = _U("\uf03d");}
	else if(iconName == "fa-vimeo"){label = _U("\uf27d");}
	else if(iconName == "fa-vimeo-square"){label = _U("\uf194");}
	else if(iconName == "fa-vine"){label = _U("\uf1ca");}
	else if(iconName == "fa-vk"){label = _U("\uf189");}
	else if(iconName == "fa-volume-control-phone"){label = _U("\uf2a0");}
	else if(iconName == "fa-volume-down"){label = _U("\uf027");}
	else if(iconName == "fa-volume-off"){label = _U("\uf026");}
	else if(iconName == "fa-volume-up"){label = _U("\uf028");}
	else if(iconName == "fa-warning (alias)"){label = _U("\uf071");}
	else if(iconName == "fa-wechat (alias)"){label = _U("\uf1d7");}
	else if(iconName == "fa-weibo"){label = _U("\uf18a");}
	else if(iconName == "fa-weixin"){label = _U("\uf1d7");}
	else if(iconName == "fa-whatsapp"){label = _U("\uf232");}
	else if(iconName == "fa-wheelchair"){label = _U("\uf193");}
	else if(iconName == "fa-wheelchair-alt"){label = _U("\uf29b");}
	else if(iconName == "fa-wifi"){label = _U("\uf1eb");}
	else if(iconName == "fa-wikipedia-w"){label = _U("\uf266");}
	else if(iconName == "fa-window-close"){label = _U("\uf2d3");}
	else if(iconName == "fa-window-close-o"){label = _U("\uf2d4");}
	else if(iconName == "fa-window-maximize"){label = _U("\uf2d0");}
	else if(iconName == "fa-window-minimize"){label = _U("\uf2d1");}
	else if(iconName == "fa-window-restore"){label = _U("\uf2d2");}
	else if(iconName == "fa-windows"){label = _U("\uf17a");}
	else if(iconName == "fa-won (alias)"){label = _U("\uf159");}
	else if(iconName == "fa-wordpress"){label = _U("\uf19a");}
	else if(iconName == "fa-wpbeginner"){label = _U("\uf297");}
	else if(iconName == "fa-wpexplorer"){label = _U("\uf2de");}
	else if(iconName == "fa-wpforms"){label = _U("\uf298");}
	else if(iconName == "fa-wrench"){label = _U("\uf0ad");}
	else if(iconName == "fa-xing"){label = _U("\uf168");}
	else if(iconName == "fa-xing-square"){label = _U("\uf169");}
	else if(iconName == "fa-y-combinator"){label = _U("\uf23b");}
	else if(iconName == "fa-y-combinator-square (alias)"){label = _U("\uf1d4");}
	else if(iconName == "fa-yahoo"){label = _U("\uf19e");}
	else if(iconName == "fa-yc (alias)"){label = _U("\uf23b");}
	else if(iconName == "fa-yc-square (alias)"){label = _U("\uf1d4");}
	else if(iconName == "fa-yelp"){label = _U("\uf1e9");}
	else if(iconName == "fa-yen (alias)"){label = _U("\uf157");}
	else if(iconName == "fa-yoast"){label = _U("\uf2b1");}
	else if(iconName == "fa-youtube"){label = _U("\uf167");}
	else if(iconName == "fa-youtube-play"){label = _U("\uf16a");}
	else if(iconName == "fa-youtube-square"){label = _U("\uf166");}


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
