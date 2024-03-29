#include "guis/GuiMenu.h"

#include "components/OptionListComponent.h"
#include "components/SliderComponent.h"
#include "components/SwitchComponent.h"
#include "guis/GuiCollectionSystemsOptions.h"
#include "guis/GuiDetectDevice.h"
#include "guis/GuiGeneralScreensaverOptions.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiScraperStart.h"
#include "guis/GuiHashStart.h"
#include "guis/GuiThemeInstaller.h"
#include "guis/GuiBezelInstaller.h"
#include "guis/GuiBatoceraStore.h"
#include "guis/GuiSettings.h"
#include "guis/GuiRetroAchievements.h"
#include "guis/GuiGamelistOptions.h"
#include "guis/GuiImageViewer.h"
#include "guis/GuiNetPlaySettings.h"
#include "guis/GuiRetroAchievementsSettings.h"
#include "guis/GuiSystemInformation.h"
#include "guis/GuiScraperSettings.h"
#include "views/UIModeController.h"
#include "views/ViewController.h"
#include "CollectionSystemManager.h"
#include "EmulationStation.h"
#include "Scripting.h"
#include "SystemData.h"
#include "VolumeControl.h"
#include <SDL_events.h>
#include <algorithm>
#include "platform.h"

#include "ThemeData.h"

#include "SystemConf.h"
#include "ApiSystem.h"
#include "InputManager.h"
#include "AudioManager.h"
#include <LibretroRatio.h>
#include "guis/GuiUpdate.h"
#include "guis/GuiInstallStart.h"
#include "guis/GuiTextEditPopupKeyboard.h"
#include "guis/GuiBackupStart.h"
#include "guis/GuiTextEditPopup.h"
#include "guis/GuiWifi.h"


#include "guis/GuiBluetoothPair.h"
#include "guis/GuiBluetoothForget.h"
#include "scrapers/ThreadedScraper.h"
#include "FileSorts.h"
#include "ThreadedHasher.h"
#include "ThreadedBluetooth.h"
#include "views/gamelist/IGameListView.h"
#include "components/MultiLineMenuEntry.h"
#include "components/BatteryIndicatorComponent.h"
#include "GuiLoading.h"
#include "guis/GuiBios.h"
#include "guis/GuiKeyMappingEditor.h"
#include "Gamelist.h"
#include "TextToSpeech.h"
#include "Paths.h"


#include "components/WebImageComponent.h"

#include "oui.h"

#if WIN32
#include "Win32ApiSystem.h"
#endif

#define fake_gettext_fade _("fade")
#define fake_gettext_slide _("slide")
#define fake_gettext_instant _("instant")
#define fake_gettext_fadeslide _("fade & slide")

#define fake_gettext_system       _("System")
#define fake_gettext_architecture _("Architecture")
#define fake_gettext_diskformat   _("Disk format")
#define fake_gettext_temperature  _("Temperature")
#define fake_gettext_avail_memory _("Available memory")
#define fake_gettext_battery      _("Battery")
#define fake_gettext_model        _("Model")
#define fake_gettext_cpu_model    _("Cpu model")
#define fake_gettext_cpu_number   _("Cpu number")
#define fake_gettext_cpu_frequency _("Cpu max frequency")
#define fake_gettext_cpu_feature  _("Cpu feature")

#define fake_gettext_simple_bilinear_simple	pgettext("game_options", "SHARP-BILINEAR-SIMPLE")
#define fake_gettext_scanlines				pgettext("game_options", "SCANLINES")
#define fake_gettext_retro					pgettext("game_options", "RETRO")
#define fake_gettext_enhanced				pgettext("game_options", "ENHANCED")
#define fake_gettext_curvature				pgettext("game_options", "CURVATURE")
#define fake_gettext_zfast					pgettext("game_options", "ZFAST")
#define fake_gettext_flatten_glow			pgettext("game_options", "FLATTEN-GLOW")
#define fake_gettext_rgascaling				pgettext("game_options", "RGA SCALING")

#define fake_gettext_glvendor		_("VENDOR")
#define fake_gettext_glvrenderer	_("RENDERER")
#define fake_gettext_glversion		_("VERSION")
#define fake_gettext_glslversion	_("SHADERS")

#define gettext_controllers_settings				_("CONTROLLER SETTINGS")
#define gettext_controllers_and_bluetooth_settings  _("CONTROLLER & BLUETOOTH SETTINGS")

#define fake_gettext_disk_internal _("INTERNAL")
#define fake_gettext_disk_external _("ANY EXTERNAL")

#define fake_gettext_resolution_max_1K  _("maximum 1920x1080")
#define fake_gettext_resolution_max_640 _("maximum 640x480")

// Windows build does not have bluetooth support, so affect the label for Windows
#if WIN32
#define controllers_settings_label		gettext_controllers_settings
#else
#define controllers_settings_label		gettext_controllers_and_bluetooth_settings
#endif


#ifdef _ENABLEEMUELEC

static std::vector<std::string> explode(std::string sData, char delimeter=',')
{
	std::vector<std::string> arr;
	std::stringstream ssData(sData);
	std::string datum;
	while(std::getline(ssData, datum, delimeter))
	{
			arr.push_back(datum);
	}
	return arr;
}

static std::vector<int> int_explode(std::string sData, char delimeter=',')
{
	std::vector<int> arr;
	std::stringstream ssData(sData);
	std::string datum;
	while(std::getline(ssData, datum, delimeter))
	{
			arr.push_back( atoi(datum.c_str()));
	}
	return arr;
}

static std::string toupper(std::string s)
{
	std::for_each(s.begin(), s.end(), [](char & c){
	    c = ::toupper(c);
	});
	return s;
}

int* getVideoModeDimensions(std::string videomode, std::vector<std::string> reslist)
{
	static int screen[2];

	if (videomode == "480cvbs")
	{
		screen[0] = 720;
		screen[1] = 480;
		return screen;
  }
	else if (videomode == "576cvbs")
	{
		screen[0] = 720;
		screen[1] = 576;
		return screen;
  }

	int pos = videomode.find('x');
	std::string tmp = videomode;

	if (pos >= 0)
	{
		screen[0] = atoi(videomode.substr(0, pos).c_str());
		tmp = videomode.substr(pos+1);
	}

	pos = tmp.find('p');
	if (pos < 0)
		pos = tmp.find('i');
	if (pos >= 0)
	{
		screen[1] = atoi(tmp.substr(0, pos).c_str());
	}

	if (screen[0] == 0) {
		for (auto it = reslist.cbegin(); it != reslist.cend(); it++) {
			int pos = (*it).find(" "+std::to_string(screen[1]));
			if (pos >= 0) {
				screen[0] = atoi((*it).substr(0,pos).c_str());
				break;
			}
		}
	}
	return screen;
}

#endif

GuiMenu::GuiMenu(Window *window, bool animate) : GuiComponent(window), mMenu(window, _("MAIN MENU").c_str()), mVersion(window)
{
	// MAIN MENU
	bool isFullUI = !UIModeController::getInstance()->isUIModeKid() && !UIModeController::getInstance()->isUIModeKiosk();
#ifdef _ENABLEEMUELEC
	bool isKidUI = UIModeController::getInstance()->isUIModeKid();
#endif

	// KODI >
	// GAMES SETTINGS >
	// CONTROLLER & BLUETOOTH >
	// USER INTERFACE SETTINGS >
	// SOUND SETTINGS >
	// NETWORK >
	// SCRAPER >
	// SYSTEM SETTINGS >
	// QUIT >


	/*std::string qTitle = _U("\ue12f");// Y - logout, _U("\ue12f");
							qTitle+= " ";
							qTitle+= _("QUIT");*/

	std::string sTitle = _U("\ue12e");// X - logout, _U("\uF2F5");
							sTitle+= " ";
							sTitle+= _("QUIT");

	/*addButton(qTitle, _("quit"), [this] {
		openQuitMenu();
	});*/
	addButton(sTitle, _("quit"), [this] {
		openQuitMenu();
	});

	mapXcallback([this] {
		openQuitMenu();
		/*mWindow->pushGui(new GuiMsgBox(mWindow, _("REALLY SHUTDOWN?"),
			_("YES"), [] { quitES(QuitMode::SHUTDOWN); },
			_("NO"), nullptr));*/
	});
	if(SystemConf::getInstance()->get("pe_hack.enabled") == "1")
		{
			mapYcallback([this] {
				openESP01Menu();
				//ESP01MenuLoader();
				//openQuitMenu();
			});
		}

	// KODI
#ifdef _ENABLE_KODI_
	if (SystemConf::getInstance()->getBool("kodi.enabled", true) && ApiSystem::getInstance()->isScriptingSupported(ApiSystem::KODI))
		addEntry(_("KODI MEDIA CENTER").c_str(), false, [this]
	{
		Window *window = mWindow;
		delete this;
		if (!ApiSystem::getInstance()->launchKodi(window))
			LOG(LogWarning) << "Shutdown terminated with non-zero result!";

	}, "iconKodi");
#endif

		// pe-emulationstation
		auto ChromecastP = AudioManager::getInstance()->ChromecastData();
		if(ChromecastP.playing)
			{
				std::string basename = ChromecastP.filename;
				std::vector<std::string> bstr = Utils::String::split(basename, '/');
				basename = bstr[bstr.size() - 1];
				addEntry(_("NOW PLAYING") + ": " + basename, true, [this, window, ChromecastP]
				{
					std::vector<AVAHIserviceDetail> gs = getAvahiService("_googlecast._tcp");
					for(auto dev : gs)
						{
							Chromecast device(dev);
							if(device.id == ChromecastP.castID)
								{
									loadChromecastDevice(window, device);
									break;
								}
						}

				}, "fa-chromecast");
				/*addWithDescription(_("CHROMECAST"), _("NOW PLAYING") + ": " + basename, nullptr, [this, window, ChromecastP]
				{
					std::vector<AVAHIserviceDetail> gs = getAvahiService("_googlecast._tcp");
					for(auto dev : gs)
						{
							Chromecast device(dev);
							if(device.id == ChromecastP.castID)
								{
									loadChromecastDevice(window, device);
									break;
								}
						}

				}, "fa-chromecast");*/
			}
		// pe-player
		if(SystemConf::getInstance()->get("pe_femusic.enabled") == "1")
		{
			std::string sname = AudioManager::getInstance()->getSongName();
			if(!sname.empty() && AudioManager::getInstance()->isSongPlaying())
			{
				addEntry(_("NOW PLAYING") + ": " + sname, true, [this]{
					openMusicPlayer();
				}, "fa-volume-high");
				/*addWithDescription(_("MUSIC PLAYER"), _("NOW PLAYING") + ": " + sname, nullptr, [this]
				{
					openMusicPlayer();
				}, "fa-volume-high");*/
			}
		}

		// NEW MUSIC PLAYER
		/*addWithDescription(_("MUSIC GUI"), nullptr, [this]
		{
			openMusicPlayer();
		}, "iconSound");*/

		// pe-hacks
		if(SystemConf::getInstance()->get("pe_hack.enabled") == "1" && SystemConf::getInstance()->get("pe_hack.esp_in_menu") == "1")
			{
				addEntry("H4CK TH3 FK1N W0RLD", true, [this] {
					openESP01Menu();//ESP01MenuLoader();
				}, "fa-skull");
			}

		addEntry(_("2ND PLAYER").c_str(), true, [this] { scanMPServers(); }, "fa-person-circle-plus");

#ifdef _ENABLEEMUELEC
		if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::WIFI))
			addEntry(_("NETWORK SETTINGS").c_str(), true, [this] { openNetworkSettings(); }, "fa-wifi");
#endif
			addEntry(_("NETWORK TOOLS").c_str(), true, [this] { openNetworkTools(); }, "fa-network-wired");
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::RETROACHIVEMENTS) &&
		SystemConf::getInstance()->getBool("global.retroachievements") &&
		Settings::getInstance()->getBool("RetroachievementsMenuitem") &&
		SystemConf::getInstance()->get("global.retroachievements.username") != "")
		addEntry(_("RETROACHIEVEMENTS").c_str(), true, [this] {
				if (!checkNetwork())
					return;
				GuiRetroAchievements::show(mWindow); }, "iconRetroachievements");

	addEntry(_("APPS").c_str(), true, [this] { openAppsMenu(); }, "fa-bars");
	addEntry(_("YouTube"), true, [this]() {	YouTube();}, "fa-youtube");

	if (isFullUI)
	{
		addEntry(_("SCRAPER").c_str(), true, [this] { openScraperSettings(); }, "fa-image");

		if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::BATOCERASTORE) || ApiSystem::getInstance()->isScriptingSupported(ApiSystem::THEMESDOWNLOADER) ||
			(ApiSystem::getInstance()->isScriptingSupported(ApiSystem::THEBEZELPROJECT) && ApiSystem::getInstance()->isScriptingSupported(ApiSystem::DECORATIONS)) ||
			ApiSystem::getInstance()->isScriptingSupported(ApiSystem::UPGRADE))
			addEntry(_("UPDATES & DOWNLOADS"), true, [this] { openUpdatesSettings(); }, "fa-rotate");
		addEntry(_("SETTINGS").c_str(), true, [this] { openAllSettings(); }, "fa-gear");
		addEntry(_("INFORMATION").c_str(), true, [this] { openSysInfo(); }, "fa-circle-info");
	}
	else
	{
		addEntry(_("INFORMATION").c_str(), true, [this] { openSystemInformations(); }, "fa-circle-info");
		addEntry(_("UNLOCK USER INTERFACE MODE").c_str(), true, [this] { exitKidMode(); }, "fa-unlock");
	}

#ifdef WIN32
	addEntry(_("QUIT").c_str(), !Settings::getInstance()->getBool("ShowOnlyExit"), [this] {openQuitMenu(); }, "fa-sign-out");
#else
#ifdef _ENABLEEMUELEC
//if (!isKidUI)
#endif
	//addEntry(_("QUIT").c_str(), true, [this] { openQuitMenu(); }, "fa-sign-out");
	// quit button
#endif




	addChild(&mMenu);
	addVersionInfo();
	setSize(mMenu.getSize());

	if (animate)
	{
		if (Renderer::isSmallScreen())
			animateTo(Vector2f((Renderer::getScreenWidth() - getSize().x()) / 2, (Renderer::getScreenHeight() - getSize().y()) / 2));
		else
			animateTo(Vector2f((Renderer::getScreenWidth() - mSize.x()) / 2, Renderer::getScreenHeight() * 0.15f));
	}
	else
	{
		if (Renderer::isSmallScreen())
			setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, (Renderer::getScreenHeight() - mSize.y()) / 2);
		else
			setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, Renderer::getScreenHeight() * 0.15f);
	}
}

void GuiMenu::openProccesses(std::vector<SysProccess> p)
	{
		Window* window = mWindow;
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		auto s = new GuiSettings(window, _("PROCCESSES").c_str());

		for(auto proc : p)
			{
				s->addWithDescription(proc.PID + ": " + proc.PROC, "",
					std::make_shared<TextComponent>(window, proc.CPU + "%", font, color),
					[this, window, proc]
				{
					std::string procCmd = getShOutput("cat /proc/" + proc.PID + "/cmdline");
					std::string MSG = proc.PID + "\n";
											MSG+= procCmd + "\n";
											MSG+= "CPU: " + proc.CPU + "%, MEM: " + proc.MEM + "%";
					window->pushGui(new GuiMsgBox(window, MSG,_("OK"),nullptr, _("KILL"),[this, proc]{
						runSystemCommand("kill -9 " + proc.PID, "", nullptr);
					}));
				});
			}

		window->pushGui(s);
	}

void GuiMenu::openSysInfo()
	{
		Window* window = mWindow;
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		auto s = new GuiSettings(window, _("SYSTEM INFORMATION").c_str());
			s->addEntry(_("TASK MANAGER"), true, [this, window]{
				mWindow->pushGui(new GuiLoading<std::vector<std::string>>(window, _("LOADING..."),
					[this, window](auto gui)
					{
						mWaitingLoad = true;
						const std::string cmd = "hacks.sh top";
						return ApiSystem::getInstance()->getScriptResults(cmd);
					},
					[this, window](std::vector<std::string> procs)
					{
						mWaitingLoad = false;
						std::vector<SysProccess> Proccesses;
						for(auto raw : procs)
							{
								std::vector<std::string> tokens = Utils::String::split(raw, ';');
								if(tokens.size() == 4)
									{
										SysProccess p;
										p.PID = tokens.at(0);
										p.CPU = tokens.at(1);
										p.MEM = tokens.at(2);
										p.PROC = tokens.at(3);

										Proccesses.push_back(p);
									}
							}
						openProccesses(Proccesses);
					}
				));
			});
		s->addGroup(_("SPACE"));
			s->addWithLabel(_("USER DISK USAGE"), std::make_shared<TextComponent>(window, ApiSystem::getInstance()->getFreeSpaceUserInfo(), font, color));
			s->addWithLabel(_("SYSTEM DISK USAGE"), std::make_shared<TextComponent>(window, ApiSystem::getInstance()->getFreeSpaceSystemInfo(), font, color));
		s->addGroup(_("TEMPERATURE"));
			s->addWithLabel(_("CPU"), std::make_shared<TextComponent>(window, hacksGetString("temp cpu", false), font, color));
			s->addWithLabel(_("GPU"), std::make_shared<TextComponent>(window, hacksGetString("temp gpu", false), font, color));
		s->addGroup(_("POWER"));
			s->addWithLabel(_("BATT. CURRENT"), std::make_shared<TextComponent>(window, hacksGetString("power current", false), font, color));
			s->addWithLabel(_("BATT. VOLTAGE"), std::make_shared<TextComponent>(window, hacksGetString("power voltage", false), font, color));
			s->addWithLabel(_("BATT. STATUS"), std::make_shared<TextComponent>(window, hacksGetString("power status", false), font, color));
			s->addWithLabel(_("BATT. CAPACITY"), std::make_shared<TextComponent>(window, hacksGetString("power capacity", false), font, color));
			auto ac_charger = std::make_shared<SwitchComponent>(mWindow);
			ac_charger->setState(hacksGetString("power ac", false) == "1");
			s->addWithLabel(_("CHARGER CONNECTED"), ac_charger);

			auto ac_usb = std::make_shared<SwitchComponent>(mWindow);
			ac_usb->setState(hacksGetString("power usb", false) == "1");
			s->addWithLabel(_("USB CONNECTED"), ac_usb);


		window->pushGui(s);
	}

void GuiMenu::appLauncher(std::string cmd, bool gpkill)
{
	Window* window = mWindow;
	if(gpkill)
		{
			runSystemCommand("killall -9 gptokeyb", "", nullptr);
			runSystemCommand("gptokeyb 1 '" + cmd + "' -killsignal 9 &", "", nullptr);
		}
	ApiSystem::getInstance()->launchApp(window, cmd);
}

void GuiMenu::openAppsMenu()
{
	Window* window = mWindow;
	auto s = new GuiSettings(window, _("APPS").c_str());
	bool webfilesStatus = apInlineInfo("webfiles") == "1";
	s->addEntry(webfilesStatus ? _("STOP WEB FILE BROWSER") : _("START WEB FILE BROWSER"), false, [window, s, this, webfilesStatus]() {
		if(webfilesStatus)
		{
			runSystemCommand("ap.sh stopwebfiles", "", nullptr);
		}
		else
		{
			runSystemCommand("ap.sh startwebfiles", "", nullptr);
		}
		delete s;
		//openNetworkSettings();
	}, "fa-cloud");
	s->addEntry(_("FILE MANAGER"), false, [this] { appLauncher("file_manager.sh"); }, "fa-folder");
	s->addEntry(_("GMU MUSIC PLAYER").c_str(), false, [this] { appLauncher("gmu_player.sh"); }, "fa-music");
	s->addEntry(_("WESTON DESKTOP").c_str(), false, [this] { appLauncher("weston.sh", true); }, "fa-arrow-pointer");
	s->addGroup(_("EMULATORS"));
		s->addEntry(_("PPSSPP").c_str(), false, [this] { appLauncher("emu-launcher.sh PPSSPPSDL", false); }, "fa-rocket");
		s->addEntry(_("DUCKSTATION").c_str(), false, [this] { appLauncher("emu-launcher.sh duckstation-nogui", false); }, "fa-rocket");
		s->addEntry(_("FLYCAST").c_str(), false, [this] { appLauncher("emu-launcher.sh flycast", false); }, "fa-rocket");
		s->addEntry(_("RETROARCH").c_str(), false, [this] { appLauncher("emu-launcher.sh retroarch", false); }, "fa-rocket");
		s->addEntry(_("RETROARCH 32bit").c_str(), false, [this] { appLauncher("emu-launcher.sh retroarch32", false); }, "fa-rocket");

	std::vector<std::string> scripts = hacksGet("listscripts");//hacks.sh listscripts
	if(scripts.size() > 0)
		{
			s->addGroup(_("SCRIPTS"));
			for(auto script : scripts)
				{
					s->addEntry(Utils::FileSystem::getFileName(script), false, [this, script] { appLauncher(script, false); }, "fa-terminal");
				}
		}

	window->pushGui(s);
}

void GuiMenu::openAllSettings()
{
	Window* window = mWindow;
	auto s = new GuiSettings(window, _("SETTINGS").c_str());
	#if BATOCERA
			s->addEntry(_("GAME SETTINGS").c_str(), true, [this] { openGamesSettings(); }, "iconGames");
			s->addEntry(controllers_settings_label.c_str(), true, [this] { openControllersSettings(); }, "iconControllers");
			s->addEntry(_("USER INTERFACE SETTINGS").c_str(), true, [this] { openUISettings(); }, "iconUI");
			s->addEntry(_("GAME COLLECTION SETTINGS").c_str(), true, [this] { openCollectionSystemSettings(); }, "iconAdvanced");
			s->addEntry(_("SOUND SETTINGS").c_str(), true, [this] { openSoundSettings(); }, "iconSound");

			if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::WIFI))
				s->addEntry(_("NETWORK SETTINGS").c_str(), true, [this] { openNetworkSettings(); }, "iconNetwork");
	#else
			if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::GAMESETTINGS))
				s->addEntry(_("GAME SETTINGS").c_str(), true, [this] { openGamesSettings(); }, "fa-ghost");

			s->addEntry(_("USER INTERFACE SETTINGS").c_str(), true, [this] { openUISettings(); }, "fa-palette");

			if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::GAMESETTINGS))
				s->addEntry(controllers_settings_label.c_str(), true, [this] { openControllersSettings(); }, "fa-gamepad");
			else
				s->addEntry(_("CONFIGURE INPUT"), true, [this] { openConfigInput(); }, "fa-gamepad");

			s->addEntry(_("SOUND SETTINGS").c_str(), true, [this] { openSoundSettings(); }, "fa-volume-high");


			s->addEntry(_("GAME COLLECTION SETTINGS").c_str(), true, [this] { openCollectionSystemSettings(); }, "fa-book");

			if (!ApiSystem::getInstance()->isScriptingSupported(ApiSystem::GAMESETTINGS))
			{
				for (auto system : SystemData::sSystemVector)
				{
					if (system->isCollection() || system->getEmulators().size() == 0 || (system->getEmulators().size() == 1 && system->getEmulators().begin()->cores.size() <= 1))
						continue;

					s->addEntry(_("EMULATOR SETTINGS"), true, [this] { openEmulatorSettings(); }, "iconGames");
					break;
				}
			}
	#endif


			s->addEntry(_("SYSTEM SETTINGS").c_str(), true, [this] { openSystemSettings(); }, "fa-gear");
			s->addEntry(_("CUSTOM SYSTEM SETTINGS").c_str(), true, [this] { openEmuELECSettings(); }, "fa-gears");
			if(SystemConf::getInstance()->get("pe_hack.enabled") == "1")
				{
					s->addEntry(_("DEAUTHER SETTINGS"), true, [this] { openESP01Settings(); }, "fa-skull");
				}

	window->pushGui(s);
}

// gameapconn
void GuiMenu::searchGameAP()
{
	Window* window = mWindow;

	mWindow->pushGui(new GuiLoading<std::vector<std::string>>(window, _("SEARCHING GAME AP..."),
		[this, window](auto gui)
		{
			mWaitingLoad = true;
			const std::string cmd = "ap.sh scanssids";
			return ApiSystem::getInstance()->getScriptResults(cmd);
		},
		[this, window](std::vector<std::string> ssids)
		{
			mWaitingLoad = false;
			bool found = false;
			const std::string wlGameApSSID = apInlineInfo("gameapssid");

			for(auto ssid : ssids)
				{
					if(ssid == wlGameApSSID)
						{
							found = true;
							break;
						}
				}
			if(!found)
				{
					window->pushGui(new GuiMsgBox(window, _("GAME AP IS NOT FOUND"),_("OK"),nullptr));
				}
			else
				{
					runSystemCommand("ap.sh connectgameap", "", nullptr);
					//window->pushGui(new GuiMsgBox(window, _("CONNECTING TO GAME AP..."),	_("OK"),nullptr));

				}
		}
	));
}

/**
 *  MultiPlayer
 */

void GuiMenu::scanMPServers()
	{
		Window* window = mWindow;

		if (ApiSystem::getInstance()->getIpAdress() == "NOT CONNECTED")
			{
				mWindow->pushGui(new GuiMsgBox(mWindow, _("YOU ARE NOT CONNECTED TO A NETWORK"), _("OK"), nullptr));
				return;
			}

		mWindow->pushGui(new GuiLoading<std::vector<AVAHIserviceDetail>>(window, _("SEARCHING 1ST PLAYER..."),
			[this, window](auto gui)
			{
				mWaitingLoad = true;
				/*const std::string cmd = "avahi-browse -d local _oga-mp._udp -t -r -p -l | grep IPv4 | grep =;";
				return ApiSystem::getInstance()->getScriptResults(cmd);*/
				return getAvahiService("_oga-mp._udp");
			},
			[this, window](std::vector<AVAHIserviceDetail> servers)
			{
				mWaitingLoad = false;

				if(servers.size() > 0)
					{
						openMPServers(servers);
					}
				else
					{
						window->pushGui(new GuiMsgBox(window, _("PLAYER 1 NOT FOUND!"),
							_("OK"),nullptr,_("NETWORK SETTINGS"), [this] {
								openNetworkSettings();
							}));
					}
			}
		));
	}
void GuiMenu::openMPServers(std::vector<AVAHIserviceDetail> servers)
	{
		Window* window = mWindow;
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		auto s = new GuiSettings(window, _("SELECT GAME TO JOIN").c_str());

		for (auto server : servers)
		{
			//MPserver _server(server);

			std::string _subtitle	= server.hostname + " (" + server.ip + ")";

			std::string gamename;
			std::string platform;
			std::string image;
			for(auto detail : server.details)
				{
					if(detail.key == "name"){
						gamename = detail.value;
					}
					if(detail.key == "platform"){
						platform = detail.value;
					}
					if(detail.key == "image"){
						image = detail.value;
					}
				}
				if(image.empty() || !Utils::FileSystem::exists(image))
					{
						image = ":/cartridge.svg";
					}
				auto icon = std::make_shared<ImageComponent>(window);
				Vector2f maxSize(64, 64);
				icon->setUpdateColors(false);
				icon->setImage(image, false, maxSize);
				icon->setMaxSize(maxSize);
				icon->setIsLinear(true);
				icon->setPadding(4);

			s->addWithDescription((platform.empty() ? "" : "["+platform+"] ") + (gamename.empty() ? "Unknown game" : gamename),
				_subtitle,
				icon,//std::make_shared<TextComponent>(window, platform.empty() ? "?" : platform, font, color),
				[this, window, server]
			{
					ApiSystem::getInstance()->launchApp(window, "playertoo " + server.ip);
			}, "fa-gamepad");

		}
		window->pushGui(s);
	}
/**
 *
 * Music player
 */
void GuiMenu::openMusicPlayer()
	{
		Window* window = mWindow;
		auto s = new GuiSettings(window, "MUSIC PLAYER");

		/*addWithDescription(_("MUSIC PLAYER"), _("NOW PLAYING") + ": " + sname, nullptr, [this]
				{
					openMusicPlayer();
				}, "iconSound");*/

		// current Song
		std::string sname = AudioManager::getInstance()->getSongName();

		if(!sname.empty())
		{
			// make it reactive!
			//bool paused = AudioManager::getInstance()->isPaused();
			s->addEntry(_("PAUSE / RESUME"), false, [this, s] {
				AudioManager::getInstance()->pause();
			}, "fa-play");

			s->addEntry(_("STOP"), false, [this] {
				AudioManager::getInstance()->stopMusic();
			}, "fa-stop");
		}

		if (AudioManager::getInstance()->myPlaylist.size() > 0)
		{
			s->addEntry(_("CLEAR PLAYLIST"), false, [s] {
				AudioManager::getInstance()->clearPlaylist();
				delete s;
			}, "fa-trash");
			s->addGroup(_("PLAYLIST"));
			for (auto song : AudioManager::getInstance()->myPlaylist)
			{
				s->addEntry(Utils::FileSystem::getStem(song), false, [song] {
					AudioManager::getInstance()->playMySong(song);
				}, AudioManager::getInstance()->isPlaying(song) ? "fa-play" : "fa-file-audio");
			}
		}
		/*else
		{
			s->addGroup(_("OPTIONS"));
			s->addEntry(_("LOAD ALL MUSIC"), false, [s] {
					AudioManager::getInstance()->playDir("/roms/mplayer/");
					delete s;
				});
		}*/

		window->pushGui(s);
	}
/**
 *
 * ESP01 Deauther
 */
void GuiMenu::openESP01Settings()
	{
		Window* window = mWindow;
		auto s = new GuiSettings(window, "DEAUTHER SETTINGS");
		/*s->addEntry(_("START OTA AP"), false, [this, window] {
			mWindow->pushGui(new GuiLoading<std::vector<std::string>>(window, _("PREPARING OTA AP..."),
				[this, window](auto gui)
				{
					mWaitingLoad = true;
					return hacksGet("espota");
				},
				[this, window](std::vector<std::string> ota)
				{
					mWaitingLoad = false;
					if(ota.size() == 1)
						{
							std::vector<std::string> tokens = Utils::String::split(ota.at(0), ';');
							window->pushGui(new GuiMsgBox(window, "SSID: " + tokens.at(0) + "\nPASS: " + tokens.at(1) + "\n\n" + tokens.at(2),_("OK"),nullptr));
						}
					else
						{
							window->pushGui(new GuiMsgBox(window, _("FAILED TO START OTA AP!"),_("OK"),nullptr));
						}
					//ESP01_OTA_SERVER;OTA.8266;https://192.168.4.1/update
				}
			));
		}, "fa-cloud-arrow-up");*/

		/*auto s = new GuiSettings(window, Title);
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;
		std::string oskTitle = _("CUSTOM TITLE");
		std::string currTitle = SystemConf::getInstance()->get("pe_hack.deauth_title");
		std::string currIcon = SystemConf::getInstance()->get("pe_hack.deauth_icon");
		s->addWithDescription(oskTitle, "",
			std::make_shared<TextComponent>(window, currTitle, font, color),
			[this, window]
		{
			if (Settings::getInstance()->getBool("UseOSK"))
			{
				mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, oskTitle, currTitle, [this, window](const std::string& value) {
					Settings::getInstance()->setString("pe_hack.deauth_title", value);
				}, false));
			}
			else
			{
				mWindow->pushGui(new GuiTextEditPopup(mWindow, oskTitle, currTitle, [this, window](const std::string& value) {
					Settings::getInstance()->setString("pe_hack.deauth_title", value);
				}, false));
			}
		}, "fa-pen-to-square");

		s->addEntry("Icon", true, [this]{

		}, SystemConf::getInstance()->get("pe_hack.deauth_icon") == "" ? "fa-skull" : SystemConf::getInstance()->get("pe_hack.deauth_icon"));*/

		auto esp_in_menu = std::make_shared<SwitchComponent>(mWindow);
		esp_in_menu->setState(SystemConf::getInstance()->get("pe_hack.esp_in_menu") == "1");
		s->addWithLabel(_("DEAUTHER IN MAIN MENU"), esp_in_menu);
		s->addSaveFunc([esp_in_menu] {
			if (esp_in_menu->changed()) {
				bool enabled = esp_in_menu->getState();
				SystemConf::getInstance()->set("pe_hack.esp_in_menu", enabled ? "1" : "0");
				SystemConf::getInstance()->saveSystemConf();
			}
		});

		// ----------------------------------------------------------- MAIN SETTINGS
		s->addGroup(_("UART"));
			// PORT
			auto esp_uart = std::make_shared< OptionListComponent<std::string> >(mWindow, "UART PORT", false);
			std::vector<std::string> ports = hacksGet("ports");

			auto port = Settings::getInstance()->getString("pe_hack.uart_port");
			if (port.empty())
				port = "";

			for (auto it = ports.cbegin(); it != ports.cend(); it++)
				esp_uart->add(_(it->c_str()), *it, port == *it);

			s->addWithLabel(_("UART PORT"), esp_uart);
			s->addSaveFunc([this, esp_uart]
			{
				Settings::getInstance()->setString("pe_hack.uart_port", esp_uart->getSelected());
			});
			// baudrate
			auto esp_baudrate = std::make_shared< OptionListComponent<std::string> >(mWindow, "BAUD RATE", false);
			std::vector<std::string> bauds;
			bauds.push_back("115200");
			bauds.push_back("9600");

			auto baud = Settings::getInstance()->getString("pe_hack.uart_baud");
			if (baud.empty())
				baud = "115200";

			for (auto it = bauds.cbegin(); it != bauds.cend(); it++)
				esp_baudrate->add(_(it->c_str()), *it, baud == *it);

			s->addWithLabel(_("BAUD RATE"), esp_baudrate);
			s->addSaveFunc([this, esp_baudrate]
			{
				Settings::getInstance()->setString("pe_hack.uart_baud", esp_baudrate->getSelected());

				std::string port = Settings::getInstance()->getString("pe_hack.uart_port");
				runSystemCommand("hacks.sh " + port + " -b " + esp_baudrate->getSelected(), "", nullptr);
			});
		s->addGroup(_("WIFI SETTINGS"));
			// SCAN
			/*
			auto esp_scan = std::make_shared<SwitchComponent>(mWindow);
			esp_scan->setState(SystemConf::getInstance()->get("pe_hack.scanbyesp") == "1");
			s->addWithLabel(_("SCAN NETWORKS BY ESP"), esp_scan);
			s->addSaveFunc([esp_scan] {
				if (esp_scan->changed()) {
					bool enabled = esp_scan->getState();
					SystemConf::getInstance()->set("pe_hack.scanbyesp", enabled ? "1" : "0");
					SystemConf::getInstance()->saveSystemConf();
				}
			});
			*/
			// snifftime
			auto staScanDur = std::make_shared<SliderComponent>(mWindow, 1.f, 20.f, 1.f, "s");
			staScanDur->setValue(Settings::getInstance()->getInt("pe_hack.stasniffduration"));
			staScanDur->setOnValueChanged([](const float &newVal) { Settings::getInstance()->setInt("pe_hack.stasniffduration", (int)round(newVal)); });
			s->addWithLabel(_("STA SNIFF DURATION"), staScanDur);

			// SCAN DB
			auto scandb = std::make_shared<SwitchComponent>(mWindow);
			scandb->setState(SystemConf::getInstance()->get("pe_hack.scandb") == "1");
			s->addSaveFunc([scandb] {
				if (scandb->changed()) {
					bool enabled = scandb->getState();
					SystemConf::getInstance()->set("pe_hack.scandb", enabled ? "1" : "0");
					SystemConf::getInstance()->saveSystemConf();
				}
			});

		// ----------------------------------------------------------- IR
		s->addGroup(_("IR SETTINGS"));
		// SPACE
			auto irSpace = std::make_shared<SliderComponent>(mWindow, 50.f, 2000.f, 50.f, "ms");
			irSpace->setValue(Settings::getInstance()->getInt("pe_hack.irspace"));
			irSpace->setOnValueChanged([](const float &newVal) { Settings::getInstance()->setInt("pe_hack.irspace", (int)round(newVal)); });
			s->addWithLabel(_("SPACE BETWEEN SENDS"), irSpace);
		// invert LGIC
			/*auto irInvert = std::make_shared<SwitchComponent>(mWindow);
			irInvert->setState(SystemConf::getInstance()->get("pe_hack.irinvert") == "1");
			s->addWithLabel(_("INVERT GPIO2 OUTPUT"), irInvert);
			s->addSaveFunc([this, irInvert] {
				if (irInvert->changed()) {
					bool enabled = irInvert->getState();
					SystemConf::getInstance()->set("pe_hack.irinvert", enabled ? "1" : "0");
					SystemConf::getInstance()->saveSystemConf();
					hacksSend("reboot");
					//runSystemCommand("hacks.sh espconn reboot", "", nullptr);
				}
			});*/
		s->addGroup(_("NEOPIXEL SETTINGS"));
			auto nBright = std::make_shared<SliderComponent>(mWindow, 0.f, 255.f, 1.f, "b");
			nBright->setValue(Settings::getInstance()->getInt("pe_hack.neobright"));
			nBright->setOnValueChanged([this](const float &newVal) {
				Settings::getInstance()->setInt("pe_hack.neobright", (int)round(newVal));
				hacksSend("bright " + std::to_string((int)round(newVal)));
			});
			s->addWithLabel(_("BRIGHTNESS"), nBright);
		s->addGroup(_("CATEGORIES"));

			auto names_cat = std::make_shared<SwitchComponent>(mWindow);
			names_cat->setState(SystemConf::getInstance()->get("pe_hack.names_cat") == "1");
			s->addWithLabel(_("NAMES CATEGORIES"), names_cat);
			s->addSaveFunc([names_cat] {
				if (names_cat->changed()) {
					bool enabled = names_cat->getState();
					SystemConf::getInstance()->set("pe_hack.names_cat", enabled ? "1" : "0");
					SystemConf::getInstance()->saveSystemConf();
				}
			});

			auto sta_cat = std::make_shared<SwitchComponent>(mWindow);
			sta_cat->setState(SystemConf::getInstance()->get("pe_hack.sta_cat") == "1");
			s->addWithLabel(_("SORT STA SCAN BY AP"), sta_cat);
			s->addSaveFunc([sta_cat] {
				if (sta_cat->changed()) {
					bool enabled = sta_cat->getState();
					SystemConf::getInstance()->set("pe_hack.sta_cat", enabled ? "1" : "0");
					SystemConf::getInstance()->saveSystemConf();
				}
			});

		window->pushGui(s);
	}

void GuiMenu::addESP01Buttons(Window* window, GuiSettings* s)
	{
		// BTN map
		s->mapXcallback([this] {
			hacksSend("stop");
		});


		std::string stopTitle = _U("\uE12E");// X - hand, _U("\uF256");
								stopTitle+= "  ";
								stopTitle+= _("STOP");

		s->addButton(stopTitle, _("stop"), [this] {
			hacksSend("stop");
		});

		s->mapYcallback([this] {
			std::string port = Settings::getInstance()->getString("pe_hack.uart_port");
			appLauncher("ttyprint.sh " + port, false);
		});

		std::string uartTitle = _U("\uE12f"); // Y - console, _U("\uF120");
								uartTitle+= "  ";
								uartTitle+= _("CONSOLE");
		s->addButton(uartTitle, _("console"), [this] {
			std::string port = Settings::getInstance()->getString("pe_hack.uart_port");
			appLauncher("ttyprint.sh " + port, false);
		});

	}
void GuiMenu::ESP01MenuLoader()
	{
		Window* window = mWindow;
		// prepare port - set baudrate
		std::string port = Settings::getInstance()->getString("pe_hack.uart_port");
		std::string baud = Settings::getInstance()->getString("pe_hack.uart_baud");
		runSystemCommand("hacks.sh " + port + " -b " + baud, "", nullptr);

		mWindow->pushGui(new GuiLoading<std::vector<std::string>>(window, _("Connecting..."),
			[this, window](auto gui)
			{
				mWaitingLoad = true;
				loadNames();
				return hacksGet("knock");
			},
			[this, window](std::vector<std::string> knock)
			{
				mWaitingLoad = false;
				if(knock.size() == 1 && knock.at(0) == "deauther")
					{
						hacksSend("bright " + std::to_string(Settings::getInstance()->getInt("pe_hack.neobright")));
						openESP01Menu();
					}
				else
					{
						window->pushGui(new GuiMsgBox(window, _("FAILED TO CONNECT!"),_("OPEN ANYWAY"),[this]{
							openESP01Menu();
						},_("CANCEL"), nullptr));
					}
			}
		));
	}
void GuiMenu::openESP01Menu()
	{
  	Window* window = mWindow;
		std::string port = Settings::getInstance()->getString("pe_hack.uart_port");
		std::string baud = Settings::getInstance()->getString("pe_hack.uart_baud");
		runSystemCommand("hacks.sh " + port + " -b " + baud, "", nullptr);
		std::vector<std::string> knock = hacksGet("knock");
		bool connected = knock.size() == 1 && knock.at(0) == "deauther";
		//hcitool dev | grep -o "[[:xdigit:]:]\{11,17\}"
		loadNames();
		std::string Title = _U("\uf54c");
								Title += " H4CK TH3 FK1N W0RLD!";
		auto s = new GuiSettings(window, Title);
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		addESP01Buttons(window, s);

			s->addWithDescription("ESP UART CONNECTION", "",
				std::make_shared<TextComponent>(window, connected ? "OK" : "ERROR", font, connected ? 0x00ff00 : 0xff0000),
				nullptr,
				"fa-microchip");

		//s->addGroup(_("SYSTEM"));
			if(names.size() > 0)
				{
					s->addWithDescription(_("SAVED NAMES"), "",
						std::make_shared<TextComponent>(window, std::to_string(names.size()), font, color),
						[this, window]
					{
						if(names.size() > 0)
							{
								if(SystemConf::getInstance()->get("pe_hack.names_cat") == "1")
									{
										openNamesCat();
									}
								else
									{
										openNames();
									}
							}
						else
							{
								window->pushGui(new GuiMsgBox(window, _("EMPTY LIST!"),_("OK"),nullptr));
							}
					}, "fa-heart");
				}
			if(SystemConf::getInstance()->get("pe_hack.scandb") == "1")
				{
					s->addEntry(_("SCAN DATABASE"), true, [this, window] {
						mWindow->pushGui(new GuiLoading<int>(window, _("Loading..."),
							[this, window](auto gui)
							{
								mWaitingLoad = true;
								return loadScanDatabase();
							},
							[this, window](int items)
							{
								mWaitingLoad = false;
								if(items > 0)
								{
									openScanDatabase();
								}
								else
								{
									window->pushGui(new GuiMsgBox(window, _("SCAN DATABASE IS EMPTY!"),_("OK"),nullptr));
								}
							}
						));
					}, "fa-warehouse");
				}

			// power management
			s->addEntry(_("POWER"), true, [this, window]{
				std::string powerTitle = _U("\uF011");
										powerTitle+= " ";
										powerTitle+= _("POWER");
				auto p = new GuiSettings(window, powerTitle);
					p->addEntry(_("REBOOT ESP01"), false, [this] {
						hacksSend("reboot");
					}, "fa-refresh");
					p->addEntry(_("SLEEP ESP01"), false, [this] {
						hacksSend("sleep");
					}, "fa-moon");
					window->pushGui(p);
			}, "fa-power-off");

		s->addGroup(_("SCAN NETWORK"));
			if(SystemConf::getInstance()->get("pe_hack.scandb") == "1")
				{
					auto enableDBSaving = std::make_shared<SwitchComponent>(mWindow);
					enableDBSaving->setState(enableScanDBsaving);
					enableDBSaving->setOnChangedCallback([this, enableDBSaving]{
						enableScanDBsaving = enableDBSaving->getState();
					});
					s->addWithLabel(_("STORE SCAN RESULTS"), enableDBSaving, false, "fa-floppy-disk");
				}
			s->addEntry(_("SCAN ALL"), true, [this, window] {
				if(stalist.size() > 0 && scanlist.size() > 0)
					{
						window->pushGui(new GuiMsgBox(window, _("START NEW SCAN?"),
						_("NO"), [this] {
							openAP_STAmenu(stalist, true);
						}, _("YES"), [this] {
							scanBSSIDS(true);
						}));
					}
				else
					{
						scanBSSIDS(true);
					}
			}, "fa-satellite-dish");
			s->addEntry(_("SCAN AP"), true, [this] {
				scanBSSIDS();
			}, "fa-satellite-dish");

			s->addEntry(_("SCAN STA"), true, [this, window] {
				if(stalist.size() == 0)
					{
						scanSTA();
					}
				else
				{
					window->pushGui(new GuiMsgBox(window, _("START NEW SCAN?"),
					_("NO"), [this] {
						if(SystemConf::getInstance()->get("pe_hack.sta_cat") == "1")
							{
								openAP_STAmenu(stalist);
							}
						else
							{
								openSTAmenu(stalist);
							}
					}, _("YES"), [this] {
						scanSTA();
					}));
				}
			}, "fa-satellite-dish");

			s->addEntry(_("SNIFF WPS-PBC FRAME"), true, [this, window] {
				window->pushGui(new GuiMsgBox(window, _("REALLY START SNIFFING?\n(30sec timeout)"),
				_("YES"), [this] {
					sniffWPS();
				}, _("NO"), nullptr));
			}, "fa-rss");

		s->addGroup(_("WIFI/BLE"));
/*---------------------------------------------------------------------------------------------------------------------------*/
			s->addEntry(_("SEND RANDOM BEACONS"), false, [this] {
					hacksSend("beacon");
				}, "fa-tower-broadcast");
			s->addEntry(_("CLONE APs"), false, [this, window] {
					window->pushGui(new GuiMsgBox(window, _("SEND DEAUTH PACKETS?"),
					_("YES"), [this, window] {
						hacksSend("clonedeauth");
					}, _("NO"), [this, window] {
						hacksSend("clonebeacon");
					}));
				}, "fa-clone");
			s->addEntry(_("DEAUTH ALL APs"), false, [this, window] {
				window->pushGui(new GuiMsgBox(window, _("DEAUTHORIZE ALL APs?"),
					_("YES"), [this, window] {
						hacksSend("killall");
					}, _("NO"),nullptr));
			}, "fa-skull");

			s->addEntry(_("FOLLOW THE WHITE RABBIT"), true, [this] {
				openRabbitTargets();
			}, "fa-rabbit");

			s->addEntry(_("BLE SPAMMER"), true, [this] {
				openBLEspammer();
			}, "fa-bluetooth");

		s->addGroup(_("IR"));
/*---------------------------------------------------------------------------------------------------------------------------*/
			s->addEntry(_("IR POWER-OFF"), false, [this] {
				//set space
				std::string space = Settings::getInstance()->getString("pe_hack.irspace");
				if(space != ""){
					hacksSend("irspace " + space);
				}
				//set invert
				if(SystemConf::getInstance()->get("pe_hack.irinvert") == "1"){
					hacksSend("irinvert");
				}
				// run
				hacksSend("irkillonce");
			}, "fa-bolt");
			s->addEntry(_("IR POWER-OFF (LOOP)"), false, [this] {
				//set space
				std::string space = Settings::getInstance()->getString("pe_hack.irspace");
				if(space != ""){
					hacksSend("irspace " + space);
				}
				//set invert
				if(SystemConf::getInstance()->get("pe_hack.irinvert") == "1"){
					hacksSend("irinvert");
				}
				// run
				hacksSend("irkill");
			}, "fa-bolt");

			s->addEntry(_("SEARCH POWER-CODE"), false, [this] {
				sendIRcode();
			}, "fa-search");
			s->addEntry(_("SEND CUSTOM POWER-CODE"), true, [this] {
				openIRlist();
			}, "fa-list");


		window->pushGui(s);
	}

void GuiMenu::openBLEspammer()
	{
		Window* window = mWindow;
		std::string Title = _U("\uf293");
								Title += " BLE SPAMMER";
		auto s = new GuiSettings(window, Title);
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;
		s->addEntry(_("ShitStorm"), false, [this] {
			appLauncher("blespam.sh shitstorm", false);
		}, "fa-poop");
		s->addEntry(_("AppleJuice"), false, [this] {
			appLauncher("blespam.sh applejuice", false);
		}, "fa-apple");
		s->addEntry(_("Select Device"), true, [this, window] {
			auto s = new GuiSettings(window, "BLE Device list");
			std::vector<std::string> list = ApiSystem::getInstance()->getScriptResults("blespam -p");
			for(auto item : list)
				{
					std::vector<std::string> tokens = Utils::String::split(item, ';');
					if(tokens.size() == 3)
						{
							std::string type = tokens.at(0);
							std::string code = tokens.at(1);
							std::string title = tokens.at(2);
							s->addEntry(title, false, [this, type, code] {
								appLauncher("blespam.sh " + type + " -n " + code, false);
								}, (type == "apple" || type == "airpods") ? "fa-apple" : (type == "samsung" ? "fa-watch" : "fa-bluetooth")
							);
						}
				}
				window->pushGui(s);
		}, "fa-list");
		// apple
		s->addGroup(_("Apple"));
			s->addEntry(_("Apple Services"), false, [this] {
				appLauncher("blespam.sh apple", false);
			}, "fa-apple");
			s->addEntry(_("AppleJuice - static"), false, [this] {
				appLauncher("blespam.sh apple1", false);
			}, "fa-apple");
			s->addEntry(_("Apple AirPods"), false, [this] {
				appLauncher("blespam.sh airpods", false);
			}, "fa-apple");
			s->addEntry(_("Apple AirPods - static"), false, [this] {
				appLauncher("blespam.sh airpods1", false);
			}, "fa-apple");

		// Android
		s->addGroup(_("Android"));
			s->addEntry(_("Samsung Galaxy Watch"), false, [this] {
				appLauncher("blespam.sh samsung", false);
			}, "fa-watch");
			s->addEntry(_("Google connect"), false, [this] {
				appLauncher("blespam.sh google", false);
			}, "fa-android");
		// Microsoft
		s->addGroup(_("Microsoft"));
			s->addEntry(_("SwiftPair - random"), false, [this] {
				appLauncher("blespam.sh swift", false);
			}, "fa-watch");
			s->addEntry(_("SwiftPair - MSG"), false, [this] {
				if (Settings::getInstance()->getBool("UseOSK"))
				{
					mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, "SwiftPair message", "B00B5", [this](const std::string& value) {
						appLauncher("blespam.sh swift -m '" + value + "'", false);
					}, false));
				}
				else
				{
					mWindow->pushGui(new GuiTextEditPopup(mWindow, "SwiftPair message", "B00B5", [this](const std::string& value) {
						appLauncher("blespam.sh swift -m '" + value + "'", false);
					}, false));
				}
			}, "fa-watch");
		window->pushGui(s);
	}

void GuiMenu::openRabbitTargets()
	{
		Window* window = mWindow;
		std::string Title = _U("\uf708");
								Title += " F0LL0W TH3 WH1T3 R4881T";
		auto s = new GuiSettings(window, Title);
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		addESP01Buttons(window, s);

		s->addEntry(_("MANUAL INPUT"), false, [this, window] {
			std::string oskTitle = _U("\uf708");
									oskTitle+= " RABBIT MAC ADDR";
			if (Settings::getInstance()->getBool("UseOSK"))
			{
				mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, oskTitle, "", [this, window](const std::string& value) {
					if(!isMac(value))
						{
							window->pushGui(new GuiMsgBox(window, _("BAD MAC ADDRESS") + "\n" + value, _("OK"),nullptr));
						}
					else
						{
							huntRabbit(value);
						}
				}, false));
			}
			else
			{
				mWindow->pushGui(new GuiTextEditPopup(mWindow, oskTitle, "", [this, window](const std::string& value) {
					if(!isMac(value))
						{
							window->pushGui(new GuiMsgBox(window, _("BAD MAC ADDRESS") + "\n" + value, _("OK"),nullptr));
						}
					else
						{
							huntRabbit(value);
						}
				}, false));
			}
		}, "fa-keyboard");

		if(scanlist.size())
			{
				s->addGroup(_("ACCESSPOINTS LIST") + " (" + std::to_string(scanlist.size()) + ")");
				for (auto ap : scanlist)
					{
						std::string _title			= (ap.ssid.empty() ? ap.bssid : ap.ssid);
						std::string _subtitle 	= (ap.ssid.empty() ? "" : ap.bssid + " / ") + ap.vendor;

						s->addWithDescription(_title, _subtitle,
							std::make_shared<TextComponent>(window, ap.rssi + "dBm", font, color),
							[this, window, ap]
						{
							huntRabbit(ap.bssid, ap.channel);
						}, wifiSignalIcon(ap.rssi));
					}
			}

		if(stalist.size() > 0)
			{
				s->addGroup(_("STATIONS LIST") + " (" + std::to_string(stalist.size()) + ")");
				for (auto sta : stalist)
					{
						sta.name = getName("STA", sta.mac).name;
						std::string _title			= (sta.name.empty() ? sta.mac : sta.name) + " / " + sta.vendor;
						std::string _subtitle 	= (sta.ap.ssid.empty() ? "" : ssidFix(sta.ap.ssid) + " / ") + sta.ap.bssid + " / " + sta.ap.vendor;

						s->addWithDescription(_title, _subtitle,
							std::make_shared<TextComponent>(window, sta.rssi + "dBm", font, color),
							[this, window, sta]
						{
							huntRabbit(sta.mac, sta.ap.channel);
						}, wifiSignalIcon(sta.rssi));
					}
			}


		window->pushGui(s);
	}
void GuiMenu::huntRabbit(std::string mac, std::string channel)
	{
		std::string fixMac = macAddr(mac);
		mWindow->pushGui(new GuiMsgBox(mWindow, _("FOLLOW") + "\n" + fixMac + "\n?",
			_("YES"), [this, fixMac, channel] {
				std::string port = Settings::getInstance()->getString("pe_hack.uart_port");
				appLauncher("mon.sh " + port + " " + fixMac, false);
				/*hacksSend("sniffmac " + fixMac + (channel == "0" ? "" : " " + channel) + " 10000000");
				std::string port = Settings::getInstance()->getString("pe_hack.uart_port");
				appLauncher("ttyprint.sh " + port, false);*/
			},
			/*_("RSSI"), [this, fixMac] {
				hacksSend("rssimon " + fixMac + " 10000000");
				std::string port = Settings::getInstance()->getString("pe_hack.uart_port");
				appLauncher("ttyprint.sh " + port, false);
			},*/
			 _("NO"), nullptr));
	}
// SCAN DATABASE
//std::vector<ScanDB_AP> ScanDB;
//std::vector<ScanDB_STA>
int GuiMenu::loadScanDatabase()
	{
		if(SystemConf::getInstance()->get("pe_hack.scandb") != "1")
			{
				return 0;
			}
		int loaded = 0;
		ScanDB.clear();
		std::vector<std::string> rawap = hacksGet("getdb AP");
		for(auto line : rawap)
			{
				ScanDB_AP ap(line);
				ap.vendor = macVendor(ap.bssid);
				ap.ssid 	= ap.ssid == "*HIDDEN*" ? "" : Utils::String::replace(ap.ssid, "_!SPC!_", " ");
				ap.name  	= getName("AP", ap.bssid).name;
				ScanDB.push_back(ap);
				loaded++;
			}
		std::vector<std::string> rawsta = hacksGet("getdb STA");
		STA_ScanDB.clear();
		for(auto line : rawsta)
			{
				ScanDB_STA _sta(line);
				_sta.vendor = macVendor(_sta.mac);
				_sta.name 	= getName("STA", _sta.mac).name;
				STA_ScanDB.push_back(_sta);
				int n = 0;
				for(auto ap : ScanDB)
					{
						if(ap.bssid == _sta.bssid)
							{
								ScanDB.at(n).sta.push_back(_sta);
								break;
							}
						n++;
					}
				loaded++;
			}
		return loaded;
	}
void GuiMenu::addToScanDatabase(AccessPoint ap, bool reload)
	{
		if(SystemConf::getInstance()->get("pe_hack.scandb") != "1")
			{
				return;
			}
		//echo "  saveAP  <BSSID> <CH> <ENC> <RSSI> <SSID>"
		std::string ssid = Utils::String::replace(ap.ssid, " ", "_!SPC!_");
		hacksSet("saveAP " + ap.bssid + " " + ap.channel + " " + ap.enc + " " + ap.rssi + " " + ssid);
		if(reload)
			{
				loadScanDatabase();
			}
	}
void GuiMenu::addToScanDatabase(WifiStation sta, bool reload)
	{
		if(SystemConf::getInstance()->get("pe_hack.scandb") != "1")
			{
				return;
			}
		//echo "  saveSTA <MAC> <BSSID> <RSSI>"
		hacksSet("saveSTA " + sta.mac + " " + sta.ap.bssid + " " + sta.rssi);
		if(reload)
			{
				loadScanDatabase();
			}
	}


void GuiMenu::openScanDatabase()
	{
		Window* window = mWindow;
		std::string Title  = _U("\uf494");
								Title += " ";
								Title	+= _("SCAN DATABASE");

		auto s = new GuiSettings(window, Title);
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		std::string STAicon = _U("\ue1f0");
		for(auto ap : ScanDB)
			{
				s->addWithDescription(ap.ssid.empty() ? ap.bssid : ssidFix(ap.ssid), ap.bssid + " / " + ap.vendor,
					std::make_shared<TextComponent>(window, ap.sta.size() > 0 ? (STAicon + " " + std::to_string(ap.sta.size())) : "", font, color),
					[this, ap]
				{
					openScanDBItem(ap);
				}, "fa-router");
			}

		window->pushGui(s);
	}

void GuiMenu::openScanDBItem(ScanDB_AP ap)
	{
		Window* window = mWindow;
		std::string Title = _U("\uf8da");
								Title += " ";
								Title	+= _("DB: ");
								Title += ap.bssid;
		auto s = new GuiSettings(window, Title);
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		s->addGroup(_("AP INFO"));
			s->addWithLabel(_("BSSID"), 	std::make_shared<TextComponent>(window, ap.bssid, 	font, color));
			s->addWithLabel(_("VENDOR"), 	std::make_shared<TextComponent>(window, ap.vendor, font, color));
			s->addWithLabel(_("CHANNEL"), 	std::make_shared<TextComponent>(window, ap.channel, font, color));
			s->addWithLabel(_("SSID"), 	std::make_shared<TextComponent>(window, ssidFix(ap.ssid), font, color));
			s->addWithLabel(_("ENCRYPTION"), 	std::make_shared<TextComponent>(window, ap.encryption, font, color));
			s->addWithLabel(_("RSSI"), 	std::make_shared<TextComponent>(window, wifiSignalGlyph(ap.rssi) + " " + ap.rssi + "dBm", font, color));
			s->addWithLabel(_("LAST SEEN"), 	std::make_shared<TextComponent>(window, ap.lastSeenDate + " " + ap.lastSeenTime, font, color));
		s->addGroup(_("MANAGEMENT"));
			s->addEntry(_("AP MENU"), true, [this, ap]() {
				AccessPoint a;
					a.bssid 		= ap.bssid;
					a.rssi 			= ap.rssi;
					a.ssid 			= ap.ssid;
					a.vendor 		= ap.vendor;
					a.channel 	= ap.channel;
					a.enc 			= ap.encryption;
				openDEAUTHMenu(a);
			}, "fa-router");
			s->addEntry(_("REMOVE FROM DATABASE"), false, [this, window, ap]() {
				window->pushGui(new GuiMsgBox(window, _("REMOVE") + "\n" + ap.bssid + "\nFROM SCAN DB?",
					_("YES"), [this, window, ap] {
						hacksSet("remDB AP " + ap.bssid);
						window->pushGui(new GuiMsgBox(window, _("REMOVE ASSOCIATED STATIONS?"),
							_("YES"), [this, window, ap] {
								for(auto sta : ap.sta)
									{
										hacksSet("remDB STA " + sta.mac);
									}
							}, _("NO"), nullptr));
							loadScanDatabase();
					}, _("CANCEL"),nullptr));
				}, "fa-trash");
		if(ap.sta.size() > 0)
			{
				s->addGroup(_("STATIONS") + " (" + std::to_string(ap.sta.size()) + ")");
				for(auto sta : ap.sta)
					{
						s->addWithDescription((sta.name.empty() ? sta.mac : (sta.name + " (" + sta.mac + ")")), sta.vendor,
							std::make_shared<TextComponent>(window, sta.rssi + "dBm", font, color),
							[this, sta]
						{
							openScanDBItem(sta);
						}, wifiSignalIcon(sta.rssi));
					}
			}

		window->pushGui(s);
	}
void GuiMenu::openScanDBItem(ScanDB_STA sta)
	{
		Window* window = mWindow;
		std::string Title = _U("\ue1f0");
								Title += " ";
								Title	+= _("DB: ");
								Title += (sta.name.empty() ? sta.mac : sta.name);
		auto s = new GuiSettings(window, Title);

		addESP01Buttons(window, s);

		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;
		std::string apCH = "0";
		for(auto ap : ScanDB)
			{
				if(ap.bssid == sta.bssid)
					{
						apCH = ap.channel;
						break;
					}
			}

		s->addGroup(_("STA INFO"));
			s->addWithLabel(_("MAC"), 				std::make_shared<TextComponent>(window, sta.mac, font, color));
			s->addWithLabel(_("BSSID"), 			std::make_shared<TextComponent>(window, sta.bssid, 	font, color));
			s->addWithLabel(_("VENDOR"), 			std::make_shared<TextComponent>(window, sta.vendor, font, color));
			s->addWithLabel(_("RSSI"), 				std::make_shared<TextComponent>(window, wifiSignalGlyph(sta.rssi) + " " + sta.rssi + "dBm", font, color));
			s->addWithLabel(_("CHANNEL"), 		std::make_shared<TextComponent>(window, apCH, font, color));
			s->addWithLabel(_("LAST SEEN"), 	std::make_shared<TextComponent>(window, sta.lastSeenDate + " " + sta.lastSeenTime, font, color));
		s->addGroup(_("STATION HACKS"));
				s->addEntry(_("DEAUTH STATION"), false, [this, window, sta, apCH]() {
					std::string msg = _("DEAUTH STATION") +":\n\n" + sta.mac + "\n"+ sta.vendor + "\n";
					window->pushGui(new GuiMsgBox(window, msg,
						_("YES"), [this, sta, apCH] {
							std::string port = Settings::getInstance()->getString("pe_hack.uart_port");
							runSystemCommand("hacks.sh " + port + " deauthsta " + sta.mac + " " + sta.bssid + " " + apCH, "", nullptr);
						}, _("CANCEL"),nullptr));
					}, "fa-chain-broken");
				s->addEntry(_("FOLLOW THE WHITE RABBIT"), false, [this, window, sta, apCH]() {
						huntRabbit(sta.mac, apCH);
					}, "fa-rabbit");
		s->addGroup(_("MANAGEMENT"));
			s->addEntry(_("REMOVE FROM DATABASE"), false, [this, window, sta]() {
				window->pushGui(new GuiMsgBox(window, _("REMOVE") + "\n" + sta.mac + "\nFROM SCAN DB?",
					_("YES"), [this, window, sta] {
						hacksSet("remDB STA " + sta.mac);
						loadScanDatabase();
					}, _("CANCEL"),nullptr));
				}, "fa-trash");

		if(!sta.name.empty())
		{
			s->addEntry(_("REMOVE NAME"), true, [this, window, s, sta]() {
				window->pushGui(new GuiMsgBox(window, _("REMOVE NAME") + "?\n" + sta.name,
					_("YES"), [this, sta, s] {
						remName("STA", sta.mac);
						delete s;
						openScanDBItem(sta);
					}, _("NO"),nullptr));
			}, "fa-trash");
		}
		s->addEntry(sta.name.empty() ? _("ADD NAME") : _("EDIT NAME"), true, [this, s, sta, apCH]() {
			if (Settings::getInstance()->getBool("UseOSK"))
				mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, "NAME " + sta.mac, sta.name, [this, s, sta, apCH](const std::string& value) {
					HackName n;
						n.type = "STA";
						n.id = sta.mac;
						n.name = value;
						n.channel = apCH;
						n.bssid = sta.bssid;
					addName(n);
					delete s;
					openScanDBItem(sta);
				}, false));
			else
				mWindow->pushGui(new GuiTextEditPopup(mWindow, "NAME " + sta.mac, sta.name, [this, s, sta, apCH](const std::string& value) {
					HackName n;
						n.type = "STA";
						n.id = sta.mac;
						n.name = value;
						n.channel = apCH;
						n.bssid = sta.bssid;
					addName(n);
					delete s;
					openScanDBItem(sta);
				}, false));
		}, sta.name.empty() ? "fa-heart" : "fa-pencil");


		window->pushGui(s);
	}





void GuiMenu::openNamesCat()
	{
		Window* window = mWindow;
		auto s = new GuiSettings(window, _("SAVED NAMES") + " (" + std::to_string(names.size()) + ")");
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;
		if(namesCounter.STA > 0)
			{
				s->addWithDescription(_("STATIONS LIST"), "",
					std::make_shared<TextComponent>(window, std::to_string(namesCounter.STA), font, color),
					[this]
				{
					openNames("STA");
				}, "fa-mobile-signal-out");
			}
			if(namesCounter.AP > 0)
				{
					s->addWithDescription(_("ACCESSPOINTS LIST"), "",
						std::make_shared<TextComponent>(window, std::to_string(namesCounter.AP), font, color),
						[this]
					{
						openNames("AP");
					}, "fa-router");
				}
			if(namesCounter.IR > 0)
				{
					s->addWithDescription("IR POWER-CODES", "",
						std::make_shared<TextComponent>(window, std::to_string(namesCounter.IR), font, color),
						[this]
					{
						openNames("IR");
					}, "fa-bolt");
				}

		window->pushGui(s);
	}

void GuiMenu::openNames(std::string category)
	{
		Window* window = mWindow;
		auto s = new GuiSettings(window, _("SAVED NAMES") + " " + category);
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		for(auto name : names)
			{
				if(category == "")
					{
						s->addWithDescription(name.type + ": " + name.name, "",
								std::make_shared<TextComponent>(window, name.id, 	font, color),
								[this, name]
							{
								openName(name);
							});
					}
				else if(name.type == category)
					{
						s->addWithDescription(name.name, "",
								std::make_shared<TextComponent>(window, name.id, 	font, color),
								[this, name]
							{
								openName(name);
							});
					}
			}

		window->pushGui(s);
	}
void GuiMenu::openName(HackName name)
	{
		Window* window = mWindow;
		auto s = new GuiSettings(window, name.type + ": " + name.name);
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		addESP01Buttons(window, s);
		// DATA info
		if(name.type == "IR")
			{
				s->addGroup(_("DATA"));
				s->addWithLabel(_("POWER-CODE"), 	std::make_shared<TextComponent>(window, "#" + name.id, 	font, color));
			}
		else if(name.type == "STA")
			{
				s->addGroup(_("STA INFO"));
				s->addWithLabel(_("MAC"), 	std::make_shared<TextComponent>(window, name.id, 	font, color));
				s->addWithLabel(_("VENDOR"), 	std::make_shared<TextComponent>(window, macVendor(name.id), font, color));
				s->addGroup(_("AP INFO"));
				s->addWithLabel(_("BSSID"), 	std::make_shared<TextComponent>(window, name.bssid, 	font, color));
				s->addWithLabel(_("VENDOR"), 	std::make_shared<TextComponent>(window, macVendor(name.bssid), font, color));
				s->addWithLabel(_("CHANNEL"), 	std::make_shared<TextComponent>(window, name.channel, font, color));
			}
		else if(name.type == "AP")
			{
				s->addGroup(_("AP INFO"));
				s->addWithLabel(_("BSSID"), 	std::make_shared<TextComponent>(window, name.id, 	font, color));
				s->addWithLabel(_("VENDOR"), 	std::make_shared<TextComponent>(window, macVendor(name.id), font, color));
				s->addWithLabel(_("CHANNEL"), 	std::make_shared<TextComponent>(window, name.channel, font, color));
				s->addWithLabel(_("SSID"), 	std::make_shared<TextComponent>(window, name.name, font, color));
				s->addWithLabel(_("PASSWORD"), 	std::make_shared<TextComponent>(window, name.password, font, color));
			}
		// TOOLS/HACKS
		if(name.type == "IR")
			{
				s->addGroup(_("ACTIONS"));
					s->addEntry(_("SEND POWER-CODE"), false, [this, name]() {
						hacksSend("ir " + name.id);
					}, "fa-bolt");
					s->addEntry(_("SEND POWER-CODE") + " LOOP", false, [this, name]() {
						hacksSend("irloop " + name.id);
					}, "fa-bolt");
			}
		else if(name.type == "STA")
			{
				s->addGroup(_("ACTIONS"));
					s->addEntry(_("DEAUTH STATION"), false, [this, window, name]() {
						std::string msg = _("DEAUTH") +"\n";
												msg+= name.name.empty() ? "" : (name.name + "\n");
												msg+= name.id + "\n";
						window->pushGui(new GuiMsgBox(window, msg,
							_("YES"), [this, window, name] {
								std::string port = Settings::getInstance()->getString("pe_hack.uart_port");
								runSystemCommand("hacks.sh " + port + " deauthsta " + name.id + " " + name.bssid + " " + name.channel, "", nullptr);
							}, _("CANCEL"),nullptr));
					}, "fa-chain-broken");
					s->addEntry(_("FOLLOW THE WHITE RABBIT"), true, [this, name]() {
							huntRabbit(name.id);
						},"fa-rabbit");

			}
		else if(name.type == "AP")
			{
				s->addGroup(_("AP HACKS"));
					s->addEntry(_("DEAUTH"), true, [this, window, name]() {
							std::string msg = _("DEAUTH") +"\n";
													msg+= name.name.empty() ? "" : (name.name + "\n");
													msg+= name.id + "\n";
							window->pushGui(new GuiMsgBox(window, msg,
								_("YES"), [this, window, name] {
									hacksSend("deauthap " + name.id);
								}, _("CANCEL"),nullptr));
						},"fa-chain-broken");

					s->addEntry(_("CLONE BSSID, DEAUTH"), true, [this, window, name]() {
							std::string msg = _("CLONE BSSID, DEAUTH") +"\n";
													msg+= name.name.empty() ? "" : (name.name + "\n");
													msg+= name.id + "\n";
							window->pushGui(new GuiMsgBox(window, msg,
								_("YES"), [this, window, name] {
									hacksSend("deauthapclone " + name.id);
								}, _("CANCEL"),nullptr));
						},"fa-skull");
					s->addEntry(_("FAKE AP, DEAUTH"), true, [this, window, name]() {
							std::string msg = _("FAKE AP, DEAUTH") +"\n";
													msg+= name.name.empty() ? "" : (name.name + "\n");
													msg+= name.id + "\n";
							window->pushGui(new GuiMsgBox(window, msg,
								_("YES"), [this, window, name] {
									hacksSend("deauthapcaptive " + name.id);
								}, _("CANCEL"),nullptr));
						},"fa-skull");
					s->addEntry(_("FOLLOW THE WHITE RABBIT"), true, [this, name]() {
							huntRabbit(name.id);
						},"fa-rabbit");

			if(!name.password.empty())
				{
					s->addGroup(_("TOOLS"));
						s->addEntry(_("CONNECT TO NETWORK"), false, [this, window, name]() {
							const std::string baseSSID 	= name.name;
							const std::string baseKEY 	= name.password;
							runSystemCommand("ap.sh stop \"" + baseSSID + "\" \"" + baseKEY + "\"", "", nullptr);
							window->pushGui(new GuiMsgBox(window, _("CONNECTING TO") + "\n" + baseSSID,_("OK"),nullptr));
						}, "fa-wifi");
				}

			}
		// NAME
		s->addGroup(_("OPTIONS"));
		s->addEntry(_("REMOVE NAME"), true, [this, window, s, name]() {
				window->pushGui(new GuiMsgBox(window, _("REMOVE NAME") + "\n" + name.name + "\n?",
					_("YES"), [this, window, s, name] {
						remName(name.type, name.id);
						delete s;
					}, _("CANCEL"),nullptr));
			}, "fa-heart-crack");
		if(name.type != "AP")
			{
				s->addEntry(_("EDIT NAME"), true, [this, window, s, name]() {
						if (Settings::getInstance()->getBool("UseOSK"))
						{
							mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, "EDIT NAME ", name.name, [this, name](const std::string& value) {
								HackName n;
									n.type 	= name.type;
									n.id 		= name.id;
									n.bssid = name.bssid;
									n.channel = name.channel;
									n.name 	= value;

								remName(name.type, name.id);
								addName(n);
							}, false));
						}
						else
						{
							mWindow->pushGui(new GuiTextEditPopup(mWindow, "EDIT NAME ", name.name, [this, name](const std::string& value) {
								HackName n;
									n.type 	= name.type;
									n.id 		= name.id;
									n.bssid = name.bssid;
									n.channel = name.channel;
									n.name 	= value;

								remName(name.type, name.id);
								addName(n);
							}, false));
						}
					},"fa-pencil");
			}

		window->pushGui(s);
	}

void GuiMenu::updateNames()
	{
		Window* window = mWindow;
		for(auto name : names)
			{
				if(name.type == "AP" || name.type == "STA")
					{
						for(auto sta : stalist)
							{
								if(sta.mac == name.id &&
										(
											(!sta.ap.bssid.empty() && sta.ap.bssid != name.bssid) ||
											(!sta.ap.channel.empty() && sta.ap.channel != name.channel)
										)
									)
									{
										window->pushGui(new GuiMsgBox(window,
											_("NAME UPDATE") + "\n" + name.name + "\nBSSID: " + sta.ap.bssid + "\nCH: " + sta.ap.channel,
											_("UPDATE"), [this, name, sta]{
												HackName updatedName = name;
													updatedName.bssid 		= sta.ap.bssid;
													updatedName.channel 	= sta.ap.channel;
												addName(updatedName);
											},
											_("CANCEL"),
											nullptr
										));
									}
							}
						for(auto ap : scanlist)
							{
								if(ap.bssid == name.id &&
									(
										(!ap.ssid.empty() && ap.ssid != name.name) ||
										(!ap.channel.empty() && ap.channel != name.channel)
									)
								)
								{
									window->pushGui(new GuiMsgBox(window,
										_("NAME UPDATE") + "\n" + name.name + "\nSSID: " + ap.ssid + "\nCH: " + ap.channel,
										_("UPDATE"), [this, name, ap]{
											HackName updatedName = name;
												updatedName.name 			= ap.ssid;
												updatedName.channel 	= ap.channel;
											addName(updatedName);
										},
										_("CANCEL"),
										nullptr
									));
								}
							}
					}
			}
		// load updated data
		loadNames();
	}

void GuiMenu::openIRlist()
	{
		Window* window = mWindow;
		/*
		Window* window,
			const std::string title,
			const std::string customButton = "",
			const std::function<void(GuiSettings*)>& func = nullptr,
			bool animate = false, bool small = false
		*/

		auto s = new GuiSettings(window, _("SEND CUSTOM POWER-CODE"));
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		for(int i = 0; i < 280; i++)
			{
				std::string code = std::to_string(i);
				std::string name = getName("IR", code).name;
				s->addWithDescription(code, "",
					std::make_shared<TextComponent>(window, name, font, color),
					[this, window, code, name] {
						hacksSend("ir " + code);
						if(!name.empty())
							{
								window->pushGui(new GuiMsgBox(window, _("SENT CODE #") + code + " (" + name + ")", _("OK"), nullptr));
							}
						else
							{
								window->pushGui(new GuiMsgBox(window, _("SENT CODE #") + code + "\n" + _("ADD NAME") + "?",
								_("NO"), nullptr,
								_("YES"), [this, code] {
									if (Settings::getInstance()->getBool("UseOSK"))
									{
										mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, "IR NAME " + code, "", [this, code](const std::string& value) {
											HackName n;
												n.type = "IR";
												n.id = code;
												n.name = value;
											addName(n);
										}, false));
									}
									else
									{
										mWindow->pushGui(new GuiTextEditPopup(mWindow, "IR NAME " + code, "", [this, code](const std::string& value) {
											HackName n;
												n.type = "IR";
												n.id = code;
												n.name = value;
											addName(n);
										}, false));
									}
								})
								);
							}
					}, "fa-hashtag");
			}
		window->pushGui(s);
	}

void GuiMenu::sendIRcode(int code)
	{
		if(code == -1 || code == 280)
			{
				code = 0;
			}
		std::string strCode = std::to_string(code);
		std::string name = getName("IR", strCode).name;
		hacksSend("ir " + strCode);

		Window* window = mWindow;
		window->pushGui(new GuiMsgBox(window, _("SENT CODE #") + strCode + (name.empty() ? "" : (" (" + name + ")") ) + "\n" + _("SEND NEXT?"),
		_("YES"), [this, code] {
			sendIRcode(code + 1);
		}, _("NO"), nullptr, name.empty() ? _("SAVE") : _("EDIT NAME"), [this,window,strCode, name]{
				if (Settings::getInstance()->getBool("UseOSK"))
				{
					mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, "IR NAME " + strCode, name, [this, strCode](const std::string& value) {
						HackName n;
							n.type 	= "IR";
							n.id 		= strCode;
							n.name 	= value;
						addName(n);
					}, false));
				}
				else
				{
					mWindow->pushGui(new GuiTextEditPopup(mWindow, "IR NAME " + strCode, name, [this, strCode](const std::string& value) {
						HackName n;
							n.type 	= "IR";
							n.id 		= strCode;
							n.name 	= value;
						addName(n);
					}, false));
				}
		}));

	}

void GuiMenu::sniffWPS()
	{
		Window* window = mWindow;
		mWindow->pushGui(new GuiLoading<std::vector<std::string>>(window, _("SEARCHING WPS-PBC..."),
			[this, window](auto gui)
			{
				mWaitingLoad = true;
				return hacksGet("wps");
			},
			[this, window](std::vector<std::string> wps)
			{
				mWaitingLoad = false;
				if(wps.size() > 0)
				{
					openWPSpwned(wps.at(0));
				}
				else
				{
					window->pushGui(new GuiMsgBox(window, _("NO WPS-PBC FRAME FOUND!"),_("OK"),nullptr));
				}
			}
		));
	}
void GuiMenu::openWPSpwned(std::string raw)
	{
		std::vector<std::string> tokens = Utils::String::split(raw, ';');
		//c0:c9:e3:9e:dd:b7;4;31;WRT_AP;PASSWORD
		AccessPoint ap;
		ap.bssid 		= Utils::String::toUpper(tokens.at(0));
		ap.vendor 	= macVendor(ap.bssid);
		ap.ssid 		= tokens.at(3);
		ap.password = tokens.at(4);
		ap.channel 	= tokens.at(1);
		ap.rssi 		=	"-" + tokens.at(2);

		Window* window = mWindow;
		auto s = new GuiSettings(window, _("WPS-PBC: ") + ap.ssid);
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		s->addGroup(_("AP INFO"));
			s->addWithLabel(_("RSSI"), 	std::make_shared<TextComponent>(window, wifiSignalGlyph(ap.rssi) + " " + ap.rssi + "dBm", 	font, color));
			s->addWithLabel(_("BSSID"), 	std::make_shared<TextComponent>(window, ap.bssid, 	font, color));
			s->addWithLabel(_("VENDOR"), 	std::make_shared<TextComponent>(window, ap.vendor, 	font, color));
			s->addWithLabel(_("CHANNEL"), 	std::make_shared<TextComponent>(window, ap.channel, 	font, color));
		s->addGroup(_("AP ACCESS DATA"));
			s->addWithLabel(_("SSID"), 	std::make_shared<TextComponent>(window, ap.ssid, 	font, color));
			s->addWithLabel(_("PASSWORD"), 	std::make_shared<TextComponent>(window, ap.password, 	font, color));
		s->addGroup(_("AP HACKS"));
			s->addEntry(_("DEAUTH"), true, [this, window, ap]() {
					std::string msg = _("DEAUTH") +"\n";
											msg+= ap.ssid.empty() ? "" : (ap.ssid + "\n");
											msg+= ap.bssid + "\n";
					window->pushGui(new GuiMsgBox(window, msg,
						_("YES"), [this, window, ap] {
							hacksSend("deauthap " + ap.bssid);
						}, _("CANCEL"),nullptr));
				},"fa-chain-broken");

			s->addEntry(_("CLONE BSSID, DEAUTH"), true, [this, window, ap]() {
					std::string msg = _("CLONE BSSID, DEAUTH") +"\n";
											msg+= ap.ssid.empty() ? "" : (ap.ssid + "\n");
											msg+= ap.bssid + "\n";
					window->pushGui(new GuiMsgBox(window, msg,
						_("YES"), [this, window, ap] {
							hacksSend("deauthapclone " + ap.bssid);
						}, _("CANCEL"),nullptr));
				},"fa-clone");
			if(!ap.ssid.empty())
			{
				s->addEntry(_("FAKE AP, DEAUTH"), true, [this, window, ap]() {
						std::string msg = _("FAKE AP, DEAUTH") +"\n";
												msg+= ap.ssid.empty() ? "" : (ap.ssid + "\n");
												msg+= ap.bssid + "\n";
						window->pushGui(new GuiMsgBox(window, msg,
							_("YES"), [this, window, ap] {
								hacksSend("deauthapcaptive " + ap.bssid);
							}, _("CANCEL"),nullptr));
					},"fa-skull");
			}
			s->addEntry(_("FOLLOW THE WHITE RABBIT"), true, [this, ap]() {
					huntRabbit(ap.bssid, ap.channel);
				},"fa-rabbit");
		s->addGroup(_("TOOLS"));
			s->addEntry(_("SAVE NETWORK"), false, [this, window, ap]() {
				HackName n;
					n.type = "AP";
					n.id = ap.bssid;
					n.name = ap.ssid;
					n.channel = ap.channel;
					n.password = ap.password;
				addName(n);
				window->pushGui(new GuiMsgBox(window, _("SAVED!"),
					_("ok"),nullptr));
			}, "fa-floppy-disk");
			s->addEntry(_("CONNECT TO NETWORK"), false, [this, window, ap]() {
				const std::string baseSSID 	= ap.ssid;
				const std::string baseKEY 	= ap.password;
				runSystemCommand("ap.sh stop \"" + baseSSID + "\" \"" + baseKEY + "\"", "", nullptr);
				window->pushGui(new GuiMsgBox(window, _("CONNECTING TO") + "\n" + ap.ssid,_("OK"),nullptr));
			}, "fa-wifi");
		window->pushGui(s);
	}

AccessPoint GuiMenu::rawToAP(std::string raw)
	{
		AccessPoint ap(raw);
		ap.vendor = macVendor(ap.bssid);
		ap.enc    = encString(ap.enc);
		return ap;
	}

std::vector<AccessPoint> GuiMenu::AccessPointList(std::vector<std::string> bssids)
	{
		std::vector<AccessPoint> list;
		for(auto bssid : bssids)
			{
				AccessPoint ap = rawToAP(bssid);
				list.push_back(ap);
				addToScanDatabase(ap, false);
			}
		loadScanDatabase();
		return list;
	}
WifiStation GuiMenu::rawToSTA(std::string raw)
{
	WifiStation sta(raw);

	sta.vendor 	= macVendor(sta.mac);
	sta.ap.vendor = macVendor(sta.ap.bssid);
	sta.ap.enc    = encString(sta.ap.enc);

	if(sta.ap.ssid.empty() || sta.ap.rssi.empty() || sta.ap.channel.empty())
		{
			AccessPoint ap = getAP(sta.ap.bssid);

			sta.ap.ssid = sta.ap.ssid.empty() ? ap.ssid : sta.ap.ssid;
			sta.ap.rssi = sta.ap.rssi.empty() ? ap.rssi : sta.ap.rssi;
			sta.ap.channel = sta.ap.channel.empty() ? ap.channel : sta.ap.channel;
		}

	return sta;
}
std::vector<WifiStation> GuiMenu::StationsList(std::vector<std::string> stations)
{
	std::vector<WifiStation> list;
	for(auto sta : stations)
		{
			WifiStation sta_ = rawToSTA(sta);
			list.push_back(sta_);
			addToScanDatabase(sta_, false);
		}
	loadScanDatabase();
	return list;
}

std::vector<AccessPoint> GuiMenu::scanBSSIDSlist()
	{
		//const std::string cmd = SystemConf::getInstance()->get("pe_hack.scanbyesp") == "1" ? "espscan" : "scan";
		scanlist = AccessPointList(hacksGet("espscan"));
		return scanlist;
	}


void GuiMenu::scanBSSIDS(bool all)
	{
		Window* window = mWindow;

		mWindow->pushGui(new GuiLoading<std::vector<AccessPoint>>(window, _("SEARCHING APs..."),
			[this, window](auto gui)
			{
				mWaitingLoad = true;
				scanlist = AccessPointList(hacksGet("espscan"));
				updateNames();
				return scanlist;
			},
			[this, window, all](std::vector<AccessPoint> bssids)
			{
				mWaitingLoad = false;
				if(bssids.size() > 0)
				{
					if(all)
						{
							scanSTA(true);
						}
					else
						{
							openBSSIDSMenu(bssids);
						}
				}
				else
				{
					window->pushGui(new GuiMsgBox(window, _("NO AP FOUND!"),_("OK"),nullptr));
				}
			}
		));
	}


AccessPoint GuiMenu::getAP(std::string bssid)
	{
		std::vector<AccessPoint> bslist = scanlist.size() == 0 ? scanBSSIDSlist() : scanlist;
		AccessPoint ap;
		if(bslist.size() > 0)
			{
				for (auto bs : bslist )
					{
						if(Utils::String::toUpper(bs.bssid) == Utils::String::toUpper(bssid)){
							return bs;
						}
					}
			}
		return ap;
	}

std::string GuiMenu::macVendor(std::string mac)
	{
		std::string _oui = Utils::String::toUpper(Utils::String::replace(mac, ":", "")).substr(0, 6);
		return OUI_VENDOR(_oui);
	}

std::string GuiMenu::encString(std::string id)
	{
		/*
		* 5 : ENC_TYPE_WEP - WEP
		* 2 : ENC_TYPE_TKIP - WPA / PSK
		* 4 : ENC_TYPE_CCMP - WPA2 / PSK
		* 7 : ENC_TYPE_NONE - open network
		* 8 : ENC_TYPE_AUTO - WPA / WPA2 / PSK
		*/
		if(id == "5"){return "WEP";}
		if(id == "2"){return "WPA";}
		if(id == "4"){return "WPA2";}
		if(id == "7"){return "OPEN";}
		if(id == "8"){return "WPA/WPA2";}
		return "?";
	}

void GuiMenu::hacksSend(std::string cmd)
{
	std::string port = Settings::getInstance()->getString("pe_hack.uart_port");
	runSystemCommand("hacks.sh " + port + " espconn " + cmd, "", nullptr);
};
void GuiMenu::hacksSet(std::string cmd)
{
	runSystemCommand("hacks.sh " + cmd, "", nullptr);
};
std::vector<std::string> GuiMenu::hacksGet(std::string cmd)
{
	std::string port = Settings::getInstance()->getString("pe_hack.uart_port");
	const std::string cmds = "hacks.sh " + port + " " + cmd;
	return ApiSystem::getInstance()->getScriptResults(cmds);
};

std::string GuiMenu::getShO(const std::string cmd)
	{
		return getShOutput(cmd);
	}

std::string GuiMenu::hacksGetString(std::string cmd, bool tty)
{
	if(tty)
	{
		std::string port = Settings::getInstance()->getString("pe_hack.uart_port");
		const std::string cmds = "hacks.sh " + port + " " + cmd;
		return getShOutput(cmds);
	}
	else
	{
		const std::string cmds = "hacks.sh " + cmd;
		return getShOutput(cmds);
	}
};

void GuiMenu::loadNames()
	{
		namesCounter.IR 	= 0;
		namesCounter.STA 	= 0;
		namesCounter.AP		= 0;
		names.clear();
		std::vector<std::string> raw = hacksGet("names");
		for(auto line : raw)
			{
				HackName n(line);
				if(n.type == "IR")
					{
						namesCounter.IR++;
					}
				else if(n.type == "STA")
					{
						namesCounter.STA++;
					}
				else if(n.type == "AP")
					{
						namesCounter.AP++;
					}
				names.push_back(n);
			}
	}
	//STA;00:00:00:00:00:00;STANAME;CHANNEL;BSSID
	//AP;00:00:00:00:00:00;SSID;CHANNEL;PASSWORD
	//IR;9;IRNAME
void GuiMenu::addName(HackName n, bool reload)
	{
		// base
		std::string raw = Utils::String::replace(n.name, " ", "_!SPC!_");

		// STA
		if(n.type == "STA")
			{
								raw+= ";" + n.channel;
								raw+= ";" + Utils::String::toUpper(n.bssid);
			}
		// NET
		if(n.type == "AP")
			{
								raw+= ";" + n.channel;
								raw+= ";" + Utils::String::replace(n.password, " ", "_!SPC!_");
			}

		hacksSet("setname '" + n.type + "' '" + n.id + "' '" + raw + "'");
		if(reload)
			{
				loadNames();
			}
	}
void GuiMenu::remName(std::string type, std::string id, bool reload)
	{
		hacksSet("remname '" + type + "' '" + id + "'");
		if(reload)
			{
				loadNames();
			}
	}

HackName GuiMenu::getName(std::string type, std::string id)
	{
		HackName g;
		for(auto n : names)
			{
				if(n.type == type && n.id == id)
					{
						return n;
					}
			}
		return g;
	}


/*
float GuiMenu::rssiToPerc(std::string rssi)
	{
		rssiToPerc(rssi.toInt());
	}
float GuiMenu::rssiToPerc(int rssi)
	{
		if(rssi >= -50)
			{
				return 100;
			}
		else if(rssi <= -99)
			{
				return 0;
			}
		return 2* (rssi + 100)
	}
*/
/*
const std::string cmd = "espscansta " + std::to_string(Settings::getInstance()->getInt("pe_hack.stasniffduration") * 1000);
stalist = StationsList(hacksGet(cmd));
return stalist;
*/
/*
void GuiMenu::addESP01ScanButtons(Window* window, GuiSettings* s, uint8_t type){
	s->mapYcallback([this, s, type] {
		s->close();
		if(type == 0) // all
			{
				scanBSSIDS(true);
			}
		else if(type == 1) // aps
			{
				scanBSSIDS();
			}
		else if(type == 2) // sta
			{
				scanSTA();
			}
	});

	std::string reloadTitle = _U("\uE12f"); // Y - console, _U("\uF120");
							reloadTitle+= "  ";
							reloadTitle+= _("RELOAD");
	s->addButton(reloadTitle, _("reload"), [this, s, type] {
		s->close();
		if(type == 0) // all
			{
				scanBSSIDS(true);
			}
		else if(type == 1) // aps
			{
				scanBSSIDS();
			}
		else if(type == 2) // sta
			{
				scanSTA();
			}
	});
}
*/
void GuiMenu::scanSTA(bool apsta)
	{
		Window* window = mWindow;

		mWindow->pushGui(new GuiLoading<std::vector<std::string>>(window, _("SEARCHING STATIONS..."),
			[this, window](auto gui)
			{
				mWaitingLoad = true;
				const std::string cmd = "espscansta " + std::to_string(Settings::getInstance()->getInt("pe_hack.stasniffduration") * 1000);
				return hacksGet(cmd);
			},
			[this, window, apsta](std::vector<std::string> stations)
			{
				mWaitingLoad = false;
					mWindow->pushGui(new GuiLoading<std::vector<WifiStation>>(window, _("PARSING RESULTS..."),
						[this, window, stations](auto gui)
						{
							mWaitingLoad = true;
							stalist = StationsList(stations);
							updateNames();
							return stalist;
						},
						[this, window, apsta](std::vector<WifiStation> stations)
						{
							mWaitingLoad = false;

							if(stations.size() > 0)
							{
								if(SystemConf::getInstance()->get("pe_hack.sta_cat") == "1" || apsta)
									{
										openAP_STAmenu(stations, apsta);
									}
								else
									{
										openSTAmenu(stations);
									}
							}
							else
							{
								window->pushGui(new GuiMsgBox(window, _("NO STA FOUND!"),_("OK"),nullptr));
							}
						}
					));
			}
		));
	}

void GuiMenu::openSTAmenu(std::vector<WifiStation> stations, std::string bssid, std::string ssid)
	{
		Window* window = mWindow;
		std::string wTitle = _("STATIONS LIST") + " ("+std::to_string(stations.size())+ ")";

		if(bssid != "")
			{
				wTitle = _U("\uf7c0");
				wTitle+= " ";
				wTitle+= ssid.empty() ? bssid : ssid;
			}

		auto s = new GuiSettings(window, wTitle);

		addESP01Buttons(window, s);
		//addESP01ScanButtons(window, s, 2);
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		AccessPoint ap;
		if(!bssid.empty())
			{
				for(auto a : scanlist)
					{
						if(a.bssid == bssid)
							{
								ap = a;
								break;
							}
					}
				// not scanned AP
				if(ap.bssid.empty())
					{
						for(auto sta : stations)
							{
								if(bssid == sta.ap.bssid)
									{
										ap = sta.ap;
										break;
									}
							}
					}
				ap.stations = 0;
				for(auto sta : stations)
					{
						if(sta.ap.bssid == bssid)
							{
								ap.stations++;
							}
					}
			}

		if(bssid != "")
			{
				// AP Info
				s->addGroup(_("AP INFO"));

					if(!ap.rssi.empty()){s->addWithLabel(_("RSSI"), 	std::make_shared<TextComponent>(window, wifiSignalGlyph(ap.rssi) + " " + ap.rssi + "dBm", 	font, color));}
					s->addWithLabel(_("BSSID"), 	std::make_shared<TextComponent>(window, ap.bssid, 	font, color));
					s->addWithLabel(_("VENDOR"), 	std::make_shared<TextComponent>(window, ap.vendor, 	font, color));
					if(!ap.ssid.empty()){s->addWithLabel(_("SSID"), 	std::make_shared<TextComponent>(window, ssidFix(ap.ssid), 	font, color));}
					//if(!ap.channel.empty()){s->addWithLabel(_("CHANNEL"), 	std::make_shared<TextComponent>(window, ap.channel, 	font, color));}
					//if(!ap.enc.empty()){s->addWithLabel(_("ENCRYPTION"), 	std::make_shared<TextComponent>(window, ap.enc, 	font, color));}
					if(ap.stations > 0)
						{
							s->addEntry(_("AP MENU"), true, [this, window, ap]() {
								openDEAUTHMenu(ap);
							}, "fa-router");
						}
					else
						{
							if(!ap.channel.empty()){s->addWithLabel(_("CHANNEL"), 	std::make_shared<TextComponent>(window, ap.channel, 	font, color));}
							if(!ap.enc.empty()){s->addWithLabel(_("ENCRYPTION"), 	std::make_shared<TextComponent>(window, ap.enc, 	font, color));}
							std::string name = getName("AP", ap.bssid).name;
							s->addGroup(_("TOOLS"));
							if(name.empty())
								{
									s->addEntry(_("SAVE NETWORK"), false, [this, window, ap]() {
										HackName n;
											n.type = "AP";
											n.id = ap.bssid;
											n.name = ap.ssid;
											n.channel = ap.channel;
										addName(n);
										window->pushGui(new GuiMsgBox(window, _("SAVED!"),
											_("ok"),nullptr));
									}, "fa-save");
								}
							else
								{
									s->addEntry(_("REMOVE NAME"), false, [this, window, ap]() {
										remName("AP", ap.bssid);
										window->pushGui(new GuiMsgBox(window, _("REMOVE") + "\n" + ap.ssid + "\n?",
											_("YES"),[this, ap]{
												remName("AP", ap.bssid);
											},_("NO"),nullptr));
									}, "fa-trash");
								}

							s->addGroup(_("AP HACKS"));
							// -------------------------------------------------------------------------------------

							// -------------------------------------------------------------------------------------

								s->addEntry(_("DEAUTH"), true, [this, window, ap]() {
										std::string msg = _("DEAUTH") +"\n";
																msg+= ap.ssid.empty() ? "" : (ap.ssid + "\n");
																msg+= ap.bssid + "\n";
										window->pushGui(new GuiMsgBox(window, msg,
											_("YES"), [this, window, ap] {
												hacksSend("deauthap " + ap.bssid);
											}, _("CANCEL"),nullptr));
									},"fa-chain-broken");

								s->addEntry(_("CLONE BSSID, DEAUTH"), true, [this, window, ap]() {
										std::string msg = _("CLONE BSSID, DEAUTH") +"\n";
																msg+= ap.ssid.empty() ? "" : (ap.ssid + "\n");
																msg+= ap.bssid + "\n";
										window->pushGui(new GuiMsgBox(window, msg,
											_("YES"), [this, window, ap] {
												hacksSend("deauthapclone " + ap.bssid);
											}, _("CANCEL"),nullptr));
									},"fa-clone");
								if(!ap.ssid.empty())
								{
									s->addEntry(_("FAKE AP, DEAUTH"), true, [this, window, ap]() {
											std::string msg = _("FAKE AP, DEAUTH") +"\n";
																	msg+= ap.ssid.empty() ? "" : (ap.ssid + "\n");
																	msg+= ap.bssid + "\n";
											window->pushGui(new GuiMsgBox(window, msg,
												_("YES"), [this, window, ap] {
													hacksSend("deauthapcaptive " + ap.bssid);
												}, _("CANCEL"),nullptr));
										},"fa-skull");
								}
								s->addEntry(_("FOLLOW THE WHITE RABBIT"), true, [this, ap]() {
										huntRabbit(ap.bssid, ap.channel);
									},"fa-rabbit");
						}
			}
		bool lessAPinfo = bssid != "";
		if (stations.size() > 0)
			{
				if(bssid != "" && ap.stations > 0)
					{
						s->addGroup(_("STATIONS") + " (" + std::to_string(ap.stations) + ")");
					}

				for (auto sta : stations)
					{
						if(bssid == "" || bssid == sta.ap.bssid)
							{
								sta.name = getName("STA", sta.mac).name;
								std::string _title;
								std::string _subtitle;
								if(bssid == "")
									{
										// Title: MAC/NAME (VENDOR)
										// Subtitle: SSID, BSSID (VENDOR)
										/*_title 		=  (sta.name.empty() ? sta.mac : sta.name) + (sta.ap.ssid.empty() ? "" : ( " " + _U("\uf8da") + " " + sta.ap.ssid));
										_subtitle = sta.ap.bssid + " " + _U("\uf5ab") + " " + sta.vendor;*/

										_title 		= (sta.name.empty() ? sta.mac : sta.name) + " / " + sta.vendor;
										_subtitle = (sta.ap.ssid.empty() ? "" : ssidFix(sta.ap.ssid) + " / ") + sta.ap.bssid + " / " + sta.ap.vendor;
									}
								else
									{
										/*_title 			= (sta.name.empty() ? sta.mac : sta.name);
										_subtitle		= sta.pkts + "pkts " + _U("\uf5ab") + " " + sta.vendor;*/
										_title 		= (sta.name.empty() ? sta.mac : sta.name);
										_subtitle		= sta.pkts + "pkts / " + sta.vendor;
									}

								s->addWithDescription(_title, _subtitle,
									std::make_shared<TextComponent>(window, sta.rssi + "dBm", font, color),
									[this, sta, lessAPinfo]
								{
									openSTADetail(sta, lessAPinfo);
								}, wifiSignalIcon(sta.rssi));
							}
					}
			}
		window->pushGui(s);
	}

std::vector<AccessPoint> GuiMenu::APSTAList(std::vector<WifiStation> stations, bool all)
	{
		// generate AP list
		std::vector<AccessPoint> list;
		if(all)
			{
				list = scanlist;

				for(auto station : stations)
					{
						bool found = false;
						for(auto ap : list)
							{
								if(station.ap.bssid == ap.bssid)
									{
										found = true;
										break;
									}
							}
						if(!found)
							{
								// FakeAP
								AccessPoint AP;
								AP.bssid  = station.ap.bssid;
								AP.vendor = macVendor(station.ap.bssid);
								list.push_back(AP);
							}
					}
			}

		if(!all)
			{
				for(auto station : stations)
					{
						bool found = false;
						for(auto ap : list)
							{
								if(ap.bssid == station.ap.bssid)
									{
										found = true;
										break;
									}
							}
						if(!found)
							{
								list.push_back(station.ap);
							}
					}
			}
		int li = 0;
		for(auto ap : list)
			{
				list.at(li).stations = 0;
				li++;
			}

		// push STAs to APs
		int n = 0;
		for(auto ap : list)
			{
				for(auto station : stations)
					{
						if(station.ap.bssid == ap.bssid)
							{
								list.at(n).stations++;
								list.at(n).pkts += std::stoi(station.pkts);
							}
					}
				n++;
			}

		return list;
	}

void GuiMenu::openAP_STAmenu(std::vector<WifiStation> stations, bool all)
	{
		Window* window = mWindow;
		std::string Title = _U("\uf7c0");
								Title+= " ";
								Title+= _("SCAN LIST");
		auto s = new GuiSettings(window, Title);

		//addESP01ScanButtons(window, s, 0);

		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;
		std::vector<AccessPoint> aps = APSTAList(stations, all);
		for(auto ap : aps)
			{
				std::string _title 			= ap.ssid.empty() ? ap.bssid : ssidFix(ap.ssid);
				std::string _subtitle 	= ap.bssid +  " / " + ap.vendor;

				std::string STAicon = _U("\ue1f0");

				s->addWithDescription(_title, _subtitle,
					std::make_shared<TextComponent>(window, (!ap.rssi.empty() ? (ap.rssi + "dBm ") : "") + (ap.stations > 0 ? (" " + STAicon + " " + std::to_string(ap.stations)) : ""),	font, color),
					[this, stations, ap]
				{
					openSTAmenu(stations, ap.bssid, ap.ssid);
				}, ap.rssi.empty() ? "fa-router" : wifiSignalIcon(ap.rssi));
			}

		window->pushGui(s);
	}

void GuiMenu::openSTADetail(WifiStation sta, bool lessAPinfo)
	{
		sta.name = getName("STA", sta.mac).name;
		Window* window = mWindow;
		std::string windowName = _U("\ue1f0");
								windowName+= " ";
								windowName+= (sta.name.empty() ? sta.mac : sta.name);

		auto s = new GuiSettings(window, windowName);
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		addESP01Buttons(window, s);

		s->addGroup(_("STATION INFO"));
			s->addWithLabel(_("RSSI"), 	std::make_shared<TextComponent>(window, wifiSignalGlyph(sta.rssi) + " " + sta.rssi + "dBm", 	font, color));
			s->addWithLabel(_("MAC"), 	std::make_shared<TextComponent>(window, sta.mac, 	font, color));
			s->addWithLabel(_("VENDOR"), 	std::make_shared<TextComponent>(window, sta.vendor, 	font, color));
			s->addWithLabel(_("PACKETS SNIFFED"), 	std::make_shared<TextComponent>(window, sta.pkts, 	font, color));

			if(!sta.name.empty())
			{
				s->addEntry(_("REMOVE NAME"), false, [this, window, s, sta]() {
					window->pushGui(new GuiMsgBox(window, _("REMOVE NAME") + "?\n" + sta.name,
						_("YES"), [this, sta, s] {
							remName("STA", sta.mac);
							delete s;
							openSTADetail(sta);
						}, _("NO"),nullptr));
				}, "fa-trash");
			}

			s->addEntry(sta.name.empty() ? _("ADD NAME") : _("EDIT NAME"), false, [this, s, sta]() {
				if (Settings::getInstance()->getBool("UseOSK"))
					mWindow->pushGui(new GuiTextEditPopupKeyboard(mWindow, "NAME " + sta.mac, sta.name, [this, s, sta](const std::string& value) {
						HackName n;
							n.type = "STA";
							n.id = sta.mac;
							n.name = value;
							n.channel = sta.ap.channel;
							n.bssid = sta.ap.bssid;
						addName(n);
						delete s;
						openSTADetail(sta);
					}, false));
				else
					mWindow->pushGui(new GuiTextEditPopup(mWindow, "NAME " + sta.mac, sta.name, [this, s, sta](const std::string& value) {
						HackName n;
							n.type = "STA";
							n.id = sta.mac;
							n.name = value;
							n.channel = sta.ap.channel;
							n.bssid = sta.ap.bssid;
						addName(n);
						delete s;
						openSTADetail(sta);
					}, false));
			}, sta.name.empty() ? "fa-heart" : "fa-pencil");
		if(lessAPinfo)
			{
				s->addGroup(_("AP INFO"));
				if(!sta.ap.ssid.empty()){s->addWithLabel(_("SSID"), 	std::make_shared<TextComponent>(window, ssidFix(sta.ap.ssid), 	font, color));}
			}
		else
			{
				s->addGroup(_("AP INFO"));
					if(!sta.ap.rssi.empty()){s->addWithLabel(_("RSSI"), 	std::make_shared<TextComponent>(window, wifiSignalGlyph(sta.ap.rssi) + " " + sta.ap.rssi + "dBm", 	font, color));}
					s->addWithLabel(_("BSSID"), 	std::make_shared<TextComponent>(window, sta.ap.bssid, 	font, color));
					s->addWithLabel(_("VENDOR"), 	std::make_shared<TextComponent>(window, sta.ap.vendor, 	font, color));
					if(!sta.ap.ssid.empty()){s->addWithLabel(_("SSID"), 	std::make_shared<TextComponent>(window, ssidFix(sta.ap.ssid), 	font, color));}
					if(!sta.ap.channel.empty()){s->addWithLabel(_("CHANNEL"), 	std::make_shared<TextComponent>(window, sta.ap.channel, 	font, color));}
					if(!sta.ap.enc.empty()){s->addWithLabel(_("ENCRYPTION"), 	std::make_shared<TextComponent>(window, sta.ap.enc, 	font, color));}
			}

		// -------------------------------------------------------------------------------------

		s->addGroup(_("STATION HACKS"));
			s->addEntry(_("DEAUTH STATION"), false, [this, window, sta]() {
				std::string msg = _("DEAUTH STATION") +":\n\n" + sta.mac + "\n"+ sta.vendor + "\n";
				window->pushGui(new GuiMsgBox(window, msg,
					_("YES"), [this, window, sta] {
						hacksSend("deauthsta " + sta.mac);
					}, _("CANCEL"),nullptr));
				}, "fa-chain-broken");
			s->addEntry(_("FOLLOW THE WHITE RABBIT"), false, [this, window, sta]() {
					huntRabbit(sta.mac, sta.ap.channel);
				}, "fa-rabbit");

		window->pushGui(s);
	}
void GuiMenu::openBSSIDSMenu(std::vector<AccessPoint> bssids)
	{
		Window* window = mWindow;
		auto s = new GuiSettings(window, (_("ACCESSPOINTS LIST") + " ("+std::to_string(bssids.size())+")").c_str());
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		//addESP01ScanButtons(window, s, 0);

		if (bssids.size() > 0)
		{
			for (auto ap : bssids)
			{
				if(!ap.bssid.empty())
					{
						s->addWithDescription(ssidFix(ap.ssid), ap.bssid + " / " + ap.vendor,
							std::make_shared<TextComponent>(window,ap.rssi + "dBm", 	font, color),
							[this, ap]
						{
							openDEAUTHMenu(ap);
						}, wifiSignalIcon(ap.rssi));
					}
			}
		}
		window->pushGui(s);
	}

void GuiMenu::openDEAUTHMenu(AccessPoint ap)
	{
		Window* window = mWindow;
		std::string windowName = _U("\uf8da");
								windowName+= " ";
								windowName+= (ap.ssid.empty() ? ap.bssid : ssidFix(ap.ssid));
		auto s = new GuiSettings(window, windowName);
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;
		addESP01Buttons(window, s);

		//std::string vendor = macVendor(bssid);
		s->addGroup(_("AP INFO"));
		// -------------------------------------------------------------------------------------
			if(!ap.rssi.empty()){s->addWithLabel(_("RSSI"), 	std::make_shared<TextComponent>(window, wifiSignalGlyph(ap.rssi) + " " + ap.rssi + "dBm", 	font, color));}
			s->addWithLabel(_("BSSID"), 	std::make_shared<TextComponent>(window, ap.bssid, 	font, color));
			s->addWithLabel(_("VENDOR"), 	std::make_shared<TextComponent>(window, ap.vendor, 	font, color));
			if(!ap.ssid.empty()){s->addWithLabel(_("SSID"), 	std::make_shared<TextComponent>(window, ssidFix(ap.ssid), 	font, color));}
			if(!ap.channel.empty()){s->addWithLabel(_("CHANNEL"), 	std::make_shared<TextComponent>(window, ap.channel, 	font, color));}
			if(!ap.enc.empty()){s->addWithLabel(_("ENCRYPTION"), 	std::make_shared<TextComponent>(window, ap.enc, 	font, color));}
		// -------------------------------------------------------------------------------------
		std::string name = getName("AP", ap.bssid).name;
		s->addGroup(_("TOOLS"));
		if(name.empty())
			{
				s->addEntry(_("SAVE NETWORK"), false, [this, window, ap]() {
					HackName n;
						n.type = "AP";
						n.id = ap.bssid;
						n.name = ap.ssid;
						n.channel = ap.channel;
					addName(n);
					window->pushGui(new GuiMsgBox(window, _("SAVED!"),
						_("ok"),nullptr));
				}, "fa-save");
			}
		else
			{
				s->addEntry(_("REMOVE NAME"), false, [this, window, ap]() {
					remName("AP", ap.bssid);
					window->pushGui(new GuiMsgBox(window, _("REMOVE") + "\n" + ap.ssid + "\n?",
						_("YES"),[this, ap]{
							remName("AP", ap.bssid);
						},_("NO"),nullptr));
				}, "fa-trash");
			}

		s->addGroup(_("AP HACKS"));
		// -------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------

			s->addEntry(_("DEAUTH"), false, [this, window, ap]() {
					std::string msg = _("DEAUTH") +"\n";
											msg+= ap.ssid.empty() ? "" : (ap.ssid + "\n");
											msg+= ap.bssid + "\n";
					window->pushGui(new GuiMsgBox(window, msg,
						_("YES"), [this, window, ap] {
							hacksSend("deauthap " + ap.bssid);
						}, _("CANCEL"),nullptr));
				},"fa-chain-broken");

			s->addEntry(_("CLONE BSSID, DEAUTH"), false, [this, window, ap]() {
					std::string msg = _("CLONE BSSID, DEAUTH") +"\n";
											msg+= ap.ssid.empty() ? "" : (ap.ssid + "\n");
											msg+= ap.bssid + "\n";
					window->pushGui(new GuiMsgBox(window, msg,
						_("YES"), [this, window, ap] {
							hacksSend("deauthapclone " + ap.bssid);
						}, _("CANCEL"),nullptr));
				},"fa-clone");
			if(!ap.ssid.empty())
			{
				s->addEntry(_("FAKE AP, DEAUTH"), false, [this, window, ap]() {
						std::string msg = _("FAKE AP, DEAUTH") +"\n";
												msg+= ap.ssid.empty() ? "" : (ap.ssid + "\n");
												msg+= ap.bssid + "\n";
						window->pushGui(new GuiMsgBox(window, msg,
							_("YES"), [this, window, ap] {
								hacksSend("deauthapcaptive " + ap.bssid);
							}, _("CANCEL"),nullptr));
					},"fa-skull");
			}
			s->addEntry(_("FOLLOW THE WHITE RABBIT"), true, [this, ap]() {
					huntRabbit(ap.bssid, ap.channel);
				},"fa-rabbit");



		window->pushGui(s);
	}

/**
 * EOF
 * ESP01 Deauther
 */

#ifdef _ENABLEEMUELEC
/* < emuelec */
void GuiMenu::openEmuELECSettings()
{
	auto s = new GuiSettings(mWindow, "CUSTOM SYSTEM SETTINGS");

	Window* window = mWindow;
	std::string a;
#if !defined(_ENABLEGAMEFORCE) && !defined(ODROIDGOA)
	auto emuelec_video_mode = std::make_shared< OptionListComponent<std::string> >(mWindow, "VIDEO MODE", false);
        std::vector<std::string> videomode;
		videomode.push_back("1080p60hz");
		videomode.push_back("1080i60hz");
		videomode.push_back("720p60hz");
		videomode.push_back("720p50hz");
		videomode.push_back("480p60hz");
		videomode.push_back("480cvbs");
		videomode.push_back("576p50hz");
		videomode.push_back("1080p50hz");
		videomode.push_back("1080i50hz");
		videomode.push_back("576cvbs");
		videomode.push_back("Custom");
		videomode.push_back("-- AUTO-DETECTED RESOLUTIONS --");
   for(std::stringstream ss(getShOutput(R"(/usr/bin/emuelec-utils resolutions)")); getline(ss, a, ','); ) {
        videomode.push_back(a);
	}
		for (auto it = videomode.cbegin(); it != videomode.cend(); it++) {
		emuelec_video_mode->add(*it, *it, SystemConf::getInstance()->get("ee_videomode") == *it); }
		s->addWithLabel(_("VIDEO MODE"), emuelec_video_mode);

		s->addSaveFunc([this, emuelec_video_mode, window] {

		//bool v_need_reboot = false;

		if (emuelec_video_mode->changed()) {
			std::string selectedVideoMode = emuelec_video_mode->getSelected();
		if (emuelec_video_mode->getSelected() != "-- AUTO-DETECTED RESOLUTIONS --") {
			if (emuelec_video_mode->getSelected() != "Custom") {
			std::string msg = _("You are about to set EmuELEC resolution to:") +"\n" + selectedVideoMode + "\n";
			msg += _("Do you want to proceed ?");

			window->pushGui(new GuiMsgBox(window, msg,
				_("YES"), [selectedVideoMode] {
					runSystemCommand("echo "+selectedVideoMode+" > /sys/class/display/mode", "", nullptr);
					SystemConf::getInstance()->set("ee_videomode", selectedVideoMode);
					LOG(LogInfo) << "Setting video to " << selectedVideoMode;
					runSystemCommand("/usr/bin/setres.sh", "", nullptr);
					SystemConf::getInstance()->saveSystemConf();
				//	v_need_reboot = true;
				}, _("NO"),nullptr));

		} else {
			if(Utils::FileSystem::exists("/storage/.config/EE_VIDEO_MODE")) {
				runSystemCommand("echo $(cat /storage/.config/EE_VIDEO_MODE) > /sys/class/display/mode", "", nullptr);
				LOG(LogInfo) << "Setting custom video mode from /storage/.config/EE_VIDEO_MODE to " << runSystemCommand("cat /storage/.config/EE_VIDEO_MODE", "", nullptr);
				SystemConf::getInstance()->set("ee_videomode", selectedVideoMode);
				SystemConf::getInstance()->saveSystemConf();
				//v_need_reboot = true;
			} else {
				if(Utils::FileSystem::exists("/flash/EE_VIDEO_MODE")) {
				runSystemCommand("echo $(cat /flash/EE_VIDEO_MODE) > /sys/class/display/mode", "", nullptr);
				LOG(LogInfo) << "Setting custom video mode from /flash/EE_VIDEO_MODE to " << runSystemCommand("cat /flash/EE_VIDEO_MODE", "", nullptr);
				SystemConf::getInstance()->set("ee_videomode", selectedVideoMode);
				SystemConf::getInstance()->saveSystemConf();
				//v_need_reboot = true;
					} else {
					runSystemCommand("echo " + SystemConf::getInstance()->get("ee_videomode")+ " > /sys/class/display/mode", "", nullptr);
					std::string msg = "/storage/.config/EE_VIDEO_MODE or /flash/EE_VIDEO_MODE not found";
					window->pushGui(new GuiMsgBox(window, msg,
				"OK", [selectedVideoMode] {
					LOG(LogInfo) << "EE_VIDEO_MODE was not found! Setting video mode to " + SystemConf::getInstance()->get("ee_videomode");
			}));
					}
				}
			}
		   }
			//if (v_need_reboot)
		 	mWindow->displayNotificationMessage(_U("\uF011  ") + _("A REBOOT OF THE SYSTEM IS REQUIRED TO APPLY THE NEW CONFIGURATION"));
		 }
		});
#endif
#ifdef _ENABLEGAMEFORCE
		auto emuelec_blrgboptions_def = std::make_shared< OptionListComponent<std::string> >(mWindow, "BUTTON LED COLOR", false);
		std::vector<std::string> blrgboptions;
		blrgboptions.push_back("off");
		blrgboptions.push_back("red");
		blrgboptions.push_back("green");
		blrgboptions.push_back("blue");
		blrgboptions.push_back("white");
		blrgboptions.push_back("purple");
		blrgboptions.push_back("yellow");
		blrgboptions.push_back("cyan");

		auto blrgboptionsS = SystemConf::getInstance()->get("bl_rgb");
		if (blrgboptionsS.empty())
		blrgboptionsS = "off";

		for (auto it = blrgboptions.cbegin(); it != blrgboptions.cend(); it++)
		emuelec_blrgboptions_def->add(*it, *it, blrgboptionsS == *it);

		s->addWithLabel(_("BUTTON LED COLOR"), emuelec_blrgboptions_def);
		s->addSaveFunc([emuelec_blrgboptions_def] {
			if (emuelec_blrgboptions_def->changed()) {
				std::string selectedblrgb = emuelec_blrgboptions_def->getSelected();
                runSystemCommand("/usr/bin/odroidgoa_utils.sh bl " +selectedblrgb, "", nullptr);
				SystemConf::getInstance()->set("bl_rgb", selectedblrgb);
                SystemConf::getInstance()->saveSystemConf();
			}
		});

        auto emuelec_powerled_def = std::make_shared< OptionListComponent<std::string> >(mWindow, "STATUS LED", false);
		std::vector<std::string> powerledoptions;
		powerledoptions.push_back("off");
		powerledoptions.push_back("heartbeat");
        powerledoptions.push_back("on");

		auto powerledoptionsS = SystemConf::getInstance()->get("gf_statusled");
		if (powerledoptionsS.empty())
		powerledoptionsS = "heartbeat";

		for (auto it = powerledoptions.cbegin(); it != powerledoptions.cend(); it++)
		emuelec_powerled_def->add(*it, *it, powerledoptionsS == *it);

		s->addWithLabel(_("STATUS LED"), emuelec_powerled_def);
		s->addSaveFunc([emuelec_powerled_def] {
			if (emuelec_powerled_def->changed()) {
				std::string selectedpowerled = emuelec_powerled_def->getSelected();
                runSystemCommand("/usr/bin/odroidgoa_utils.sh pl " +selectedpowerled, "", nullptr);
				SystemConf::getInstance()->set("gf_statusled", selectedpowerled);
                SystemConf::getInstance()->saveSystemConf();
			}
		});
#endif
#if !defined(_ENABLEGAMEFORCE) && !defined(ODROIDGOA)
		auto emuelec_audiodev_def = std::make_shared< OptionListComponent<std::string> >(mWindow, "AUDIO DEVICE", false);
		std::vector<std::string> Audiodevices;
		Audiodevices.push_back("auto");
		Audiodevices.push_back("0,0");
		Audiodevices.push_back("0,1");
		Audiodevices.push_back("1,0");
		Audiodevices.push_back("1,1");

		auto AudiodevicesS = SystemConf::getInstance()->get("ee_audio_device");
		if (AudiodevicesS.empty())
		AudiodevicesS = "auto";

		for (auto it = Audiodevices.cbegin(); it != Audiodevices.cend(); it++)
		emuelec_audiodev_def->add(*it, *it, AudiodevicesS == *it);

		s->addWithDescription(_("AUDIO DEVICE"), _("Changes will need an EmulationStation restart."), emuelec_audiodev_def);

        emuelec_audiodev_def->setSelectedChangedCallback([emuelec_audiodev_def](std::string name) {
            if (SystemConf::getInstance()->set("ee_audio_device", name))
                SystemConf::getInstance()->saveSystemConf();
                std::string selectedaudio = emuelec_audiodev_def->getSelected();
                runSystemCommand("/usr/bin/emuelec-utils setauddev " +selectedaudio, "", nullptr);
            });
#endif
s->addGroup(_("PE MOD SETTINGS"));
	// uboot theme
	// PORT
	auto uboot_theme = std::make_shared< OptionListComponent<std::string> >(mWindow, "U-BOOT THEME", false);
	std::vector<std::string> themes = ApiSystem::getInstance()->getScriptResults("ubt.sh -list");

	auto utheme = Settings::getInstance()->getString("pe_hack.uboot_theme");
	if (utheme.empty())
		utheme = "clear";

	for (auto it = themes.cbegin(); it != themes.cend(); it++)
		uboot_theme->add(_(it->c_str()), *it, utheme == *it);

	s->addWithLabel(_("U-BOOT THEME"), uboot_theme);
	s->addSaveFunc([this, uboot_theme]
	{
		runSystemCommand("ubt.sh -install " + uboot_theme->getSelected(), "", nullptr);
		Settings::getInstance()->setString("pe_hack.uboot_theme", uboot_theme->getSelected());
	});

	// hack the world MENU
	auto hack_enabled = std::make_shared<SwitchComponent>(mWindow);
	//bool basehack_enabled = SystemConf::getInstance()->get("pe_hack.enabled") == "1";
	hack_enabled->setState(SystemConf::getInstance()->get("pe_hack.enabled") == "1");
	s->addWithLabel(_("ESP01 DEAUTHER"), hack_enabled);
	s->addSaveFunc([hack_enabled] {
		if (hack_enabled->changed()) {
			bool enabled = hack_enabled->getState();
			SystemConf::getInstance()->set("pe_hack.enabled", enabled ? "1" : "0");
			SystemConf::getInstance()->saveSystemConf();
		}
	});

	auto scanv4 = std::make_shared<SwitchComponent>(mWindow);
	scanv4->setState(SystemConf::getInstance()->get("pe_scanv4.enabled") == "1");
	s->addWithLabel(_("SCAN ONLY IPv4 SERVICES"), scanv4);
	s->addSaveFunc([scanv4] {
		if (scanv4->changed()) {
			bool enabled = scanv4->getState();
			SystemConf::getInstance()->set("pe_scanv4.enabled", enabled ? "1" : "0");
			SystemConf::getInstance()->saveSystemConf();
		}
	});
	// internal music player - opens audiofiles in FRONTEND
	auto intMusicPlayer = std::make_shared<SwitchComponent>(mWindow);
	//bool baseintMusicPlayer = SystemConf::getInstance()->get("pe_femusic.enabled") == "1";
	intMusicPlayer->setState(SystemConf::getInstance()->get("pe_femusic.enabled") == "1");
	s->addWithLabel(_("ES MUSIC PLAYER"), intMusicPlayer);
	s->addSaveFunc([intMusicPlayer] {
		if (intMusicPlayer->changed()) {
			bool enabled = intMusicPlayer->getState();
			SystemConf::getInstance()->set("pe_femusic.enabled", enabled ? "1" : "0");
			SystemConf::getInstance()->saveSystemConf();
		}
	});
	// internal video player - opens videofiles in FRONTEND
	auto intVideoPlayer = std::make_shared<SwitchComponent>(mWindow);
	//bool baseintVideoPlayer = SystemConf::getInstance()->get("pe_fevideo.enabled") == "1";
	intVideoPlayer->setState(SystemConf::getInstance()->get("pe_fevideo.enabled") == "1");
	s->addWithLabel(_("ES VIDEO PLAYER"), intVideoPlayer);
	s->addSaveFunc([intVideoPlayer] {
		if (intVideoPlayer->changed()) {
			bool enabled = intVideoPlayer->getState();
			SystemConf::getInstance()->set("pe_fevideo.enabled", enabled ? "1" : "0");
			SystemConf::getInstance()->saveSystemConf();
		}
	});


s->addGroup(_("EMUELEC SETTINGS"));
    auto bluetoothd_enabled = std::make_shared<SwitchComponent>(mWindow);
		bool btbaseEnabled = SystemConf::getInstance()->get("ee_bluetooth.enabled") == "1";
		bluetoothd_enabled->setState(btbaseEnabled);
		s->addWithLabel(_("ENABLE BLUETOOTH"), bluetoothd_enabled);
		s->addSaveFunc([bluetoothd_enabled] {
			if (bluetoothd_enabled->changed()) {
			if (bluetoothd_enabled->getState() == false) {
				runSystemCommand("systemctl stop bluetooth", "", nullptr);
				runSystemCommand("rm /storage/.cache/services/bluez.conf", "", nullptr);
			} else {
				runSystemCommand("mkdir -p /storage/.cache/services/", "", nullptr);
				runSystemCommand("touch /storage/.cache/services/bluez.conf", "", nullptr);
				runSystemCommand("systemctl start bluetooth", "", nullptr);
			}
                bool bluetoothenabled = bluetoothd_enabled->getState();
                SystemConf::getInstance()->set("ee_bluetooth.enabled", bluetoothenabled ? "1" : "0");
				SystemConf::getInstance()->saveSystemConf();
			}
		});

    auto sshd_enabled = std::make_shared<SwitchComponent>(mWindow);
		bool baseEnabled = SystemConf::getInstance()->get("ee_ssh.enabled") == "1";
		sshd_enabled->setState(baseEnabled);
		s->addWithLabel(_("ENABLE SSH"), sshd_enabled);
		s->addSaveFunc([sshd_enabled] {
			if (sshd_enabled->changed()) {
			if (sshd_enabled->getState() == false) {
				runSystemCommand("systemctl stop sshd", "", nullptr);
				runSystemCommand("rm /storage/.cache/services/sshd.conf", "", nullptr);
			} else {
				runSystemCommand("mkdir -p /storage/.cache/services/", "", nullptr);
				runSystemCommand("touch /storage/.cache/services/sshd.conf", "", nullptr);
				runSystemCommand("systemctl start sshd", "", nullptr);
			}
                bool sshenabled = sshd_enabled->getState();
                SystemConf::getInstance()->set("ee_ssh.enabled", sshenabled ? "1" : "0");
				SystemConf::getInstance()->saveSystemConf();
			}
		});

		auto emuelec_boot_def = std::make_shared< OptionListComponent<std::string> >(mWindow, "START AT BOOT", false);
		std::vector<std::string> devices;
		devices.push_back("Emulationstation");
		devices.push_back("Retroarch");
		for (auto it = devices.cbegin(); it != devices.cend(); it++)
		emuelec_boot_def->add(*it, *it, SystemConf::getInstance()->get("ee_boot") == *it);
		s->addWithLabel(_("START AT BOOT"), emuelec_boot_def);
		s->addSaveFunc([emuelec_boot_def] {
			if (emuelec_boot_def->changed()) {
				std::string selectedBootMode = emuelec_boot_def->getSelected();
				SystemConf::getInstance()->set("ee_boot", selectedBootMode);
				SystemConf::getInstance()->saveSystemConf();
			}
		});

       auto fps_enabled = std::make_shared<SwitchComponent>(mWindow);
		bool fpsEnabled = SystemConf::getInstance()->get("global.showFPS") == "1";
		fps_enabled->setState(fpsEnabled);
		s->addWithLabel(_("SHOW RETROARCH FPS"), fps_enabled);
		s->addSaveFunc([fps_enabled] {
			bool fpsenabled = fps_enabled->getState();
                SystemConf::getInstance()->set("global.showFPS", fpsenabled ? "1" : "0");
				SystemConf::getInstance()->saveSystemConf();
			});
/*
       auto bezels_enabled = std::make_shared<SwitchComponent>(mWindow);
		bool bezelsEnabled = SystemConf::getInstance()->get("global.bezel") == "1";
		bezels_enabled->setState(bezelsEnabled);
		s->addWithLabel(_("ENABLE RA BEZELS"), bezels_enabled);
		s->addSaveFunc([bezels_enabled] {
			bool bezelsenabled = bezels_enabled->getState();
                SystemConf::getInstance()->set("global.bezel", bezelsenabled ? "1" : "0");
				SystemConf::getInstance()->saveSystemConf();
			});
*/
       auto splash_enabled = std::make_shared<SwitchComponent>(mWindow);
		bool splashEnabled = SystemConf::getInstance()->get("ee_splash.enabled") == "1";
		splash_enabled->setState(splashEnabled);
		s->addWithLabel(_("ENABLE RA SPLASH"), splash_enabled);
		s->addSaveFunc([splash_enabled] {
                bool splashenabled = splash_enabled->getState();
                SystemConf::getInstance()->set("ee_splash.enabled", splashenabled ? "1" : "0");
				SystemConf::getInstance()->saveSystemConf();
			});

	auto enable_bootvideo = std::make_shared<SwitchComponent>(mWindow);
	bool bootEnabled = SystemConf::getInstance()->get("ee_bootvideo.enabled") == "1";
	enable_bootvideo->setState(bootEnabled);
	s->addWithLabel(_("ALWAYS SHOW BOOT VIDEO"), enable_bootvideo);

	s->addSaveFunc([enable_bootvideo, window] {
		bool bootvideoenabled = enable_bootvideo->getState();
		SystemConf::getInstance()->set("ee_bootvideo.enabled", bootvideoenabled ? "1" : "0");
		SystemConf::getInstance()->saveSystemConf();
	});

	auto enable_randombootvideo = std::make_shared<SwitchComponent>(mWindow);
	bool randombootEnabled = SystemConf::getInstance()->get("ee_randombootvideo.enabled") == "1";
	enable_randombootvideo->setState(randombootEnabled);
	s->addWithLabel(_("RANDOMIZE BOOT VIDEO"), enable_randombootvideo);

	s->addSaveFunc([enable_randombootvideo, window] {
		bool randombootvideoenabled = enable_randombootvideo->getState();
		SystemConf::getInstance()->set("ee_randombootvideo.enabled", randombootvideoenabled ? "1" : "0");
        if (randombootvideoenabled)
        SystemConf::getInstance()->set("ee_bootvideo.enabled", "1");
		SystemConf::getInstance()->saveSystemConf();
	});

	s->addInputTextRow(_("DEFAULT YOUTUBE SEARCH WORD"), "youtube.searchword", false);

#ifdef _ENABLEEMUELEC
	auto theme = ThemeData::getMenuTheme();

	ComponentListRow row;

	auto createText = std::make_shared<TextComponent>(window, _("GAMEPAD CONFIG"), theme->Text.font, theme->Text.color);
	row.addElement(createText, true);
	row.makeAcceptInputHandler([window, s, createText]
	{
		GuiMenu::createGamepadConfig(window, s);
	});
	s->addRow(row);
#endif

		auto emuelec_retroarch_menu_def = std::make_shared< OptionListComponent<std::string> >(mWindow, "RETROARCH MENU", false);
		std::vector<std::string> ramenuoptions;
		ramenuoptions.push_back("auto");
		ramenuoptions.push_back("ozone");
		ramenuoptions.push_back("xmb");
		ramenuoptions.push_back("rgui");

		auto ramenuoptionsS = SystemConf::getInstance()->get("global.retroarch.menu_driver");
		if (ramenuoptionsS.empty())
		ramenuoptionsS = "auto";

		for (auto it = ramenuoptions.cbegin(); it != ramenuoptions.cend(); it++)
		emuelec_retroarch_menu_def->add(*it, *it, ramenuoptionsS == *it);

		s->addWithLabel(_("RETROARCH MENU"), emuelec_retroarch_menu_def);
		s->addSaveFunc([emuelec_retroarch_menu_def] {
			if (emuelec_retroarch_menu_def->changed()) {
				std::string selectedretroarch_menu = emuelec_retroarch_menu_def->getSelected();
				SystemConf::getInstance()->set("global.retroarch.menu_driver", selectedretroarch_menu);
				SystemConf::getInstance()->saveSystemConf();
			}
		});

if (UIModeController::getInstance()->isUIModeFull())
	{
        //External Mount Options
        s->addEntry(_("EXTERNAL MOUNT OPTIONS"), true, [this] { openExternalMounts(mWindow, "global"); });

        //Danger zone options
        s->addEntry(_("DANGER ZONE"), true, [this] { openDangerZone(mWindow, "global"); });
    }

    mWindow->pushGui(s);
}

#ifdef _ENABLEEMUELEC

void GuiMenu::createGamepadConfig(Window* window, GuiSettings* systemConfiguration)
{
	GuiSettings* gamepadConfiguration = new GuiSettings(window, _("GAMEPAD CONFIG"));

	// Advmame Gamepad
	auto enable_advmamegp = std::make_shared<SwitchComponent>(window);
	bool advgpEnabled = SystemConf::getInstance()->get("advmame_auto_gamepad") == "1";
	enable_advmamegp->setState(advgpEnabled);
	gamepadConfiguration->addWithLabel(_("AUTO CONFIG ADVANCEMAME GAMEPAD"), enable_advmamegp);

	gamepadConfiguration->addSaveFunc([enable_advmamegp, window] {
		bool advmamegpenabled = enable_advmamegp->getState();
		SystemConf::getInstance()->set("advmame_auto_gamepad", advmamegpenabled ? "1" : "0");
		SystemConf::getInstance()->saveSystemConf();
	});

	// Dolphin Gamepad
	auto enable_dolphingp = std::make_shared<SwitchComponent>(window);
	bool dolphingpEnabled = SystemConf::getInstance()->get("dolphin_auto_gamepad") == "1";
	enable_dolphingp->setState(dolphingpEnabled);
	gamepadConfiguration->addWithLabel(_("AUTO CONFIG DOLPHIN GAMEPAD"), enable_dolphingp);

	gamepadConfiguration->addSaveFunc([enable_dolphingp, window] {
		bool dolphingpenabled = enable_dolphingp->getState();
		SystemConf::getInstance()->set("dolphin_auto_gamepad", dolphingpenabled ? "1" : "0");
		SystemConf::getInstance()->saveSystemConf();
	});

	// Duckstation Gamepad
	auto enable_duckstationgp = std::make_shared<SwitchComponent>(window);
	bool duckstationgpEnabled = SystemConf::getInstance()->get("duckstation_auto_gamepad") == "1";
	enable_duckstationgp->setState(duckstationgpEnabled);
	gamepadConfiguration->addWithLabel(_("AUTO CONFIG DUCKSTATION GAMEPAD"), enable_duckstationgp);

	gamepadConfiguration->addSaveFunc([enable_duckstationgp, window] {
		bool duckstationgpenabled = enable_duckstationgp->getState();
		SystemConf::getInstance()->set("duckstation_auto_gamepad", duckstationgpenabled ? "1" : "0");
		SystemConf::getInstance()->saveSystemConf();
	});

	// Flycast Gamepad
	auto enable_flycastgp = std::make_shared<SwitchComponent>(window);
	bool flycastgpEnabled = SystemConf::getInstance()->get("flycast_auto_gamepad") == "1";
	enable_flycastgp->setState(flycastgpEnabled);
	gamepadConfiguration->addWithLabel(_("AUTO CONFIG FLYCAST GAMEPAD"), enable_flycastgp);

	gamepadConfiguration->addSaveFunc([enable_flycastgp, window] {
		bool flycastgpenabled = enable_flycastgp->getState();
		SystemConf::getInstance()->set("flycast_auto_gamepad", flycastgpenabled ? "1" : "0");
		SystemConf::getInstance()->saveSystemConf();
	});

	// Mupen64Plus Gamepad
	auto enable_mupen64plusgp = std::make_shared<SwitchComponent>(window);
	bool mupen64plusgpEnabled = SystemConf::getInstance()->get("mupen64plus_auto_gamepad") == "1";
	enable_mupen64plusgp->setState(mupen64plusgpEnabled);
	gamepadConfiguration->addWithLabel(_("AUTO CONFIG MUPEN64PLUS GAMEPAD"), enable_mupen64plusgp);

	gamepadConfiguration->addSaveFunc([enable_mupen64plusgp, window] {
		bool mupen64plusgpenabled = enable_mupen64plusgp->getState();
		SystemConf::getInstance()->set("mupen64plus_auto_gamepad", mupen64plusgpenabled ? "1" : "0");
		SystemConf::getInstance()->saveSystemConf();
	});

	// yabasanshiro Gamepad
	auto enable_yabasanshirogp = std::make_shared<SwitchComponent>(window);
	bool yabasanshirogpEnabled = SystemConf::getInstance()->get("yabasanshiro_auto_gamepad") == "1";
	enable_yabasanshirogp->setState(yabasanshirogpEnabled);
	gamepadConfiguration->addWithLabel(_("AUTO CONFIG YABASANSHIRO GAMEPAD"), enable_yabasanshirogp);

	gamepadConfiguration->addSaveFunc([enable_yabasanshirogp, window] {
		bool yabasanshirogpenabled = enable_yabasanshirogp->getState();
		SystemConf::getInstance()->set("yabasanshiro_auto_gamepad", yabasanshirogpenabled ? "1" : "0");
		SystemConf::getInstance()->saveSystemConf();
	});

	// ppssppsdl Gamepad
	auto enable_ppssppsdlgp = std::make_shared<SwitchComponent>(window);
	bool ppssppsdlgpEnabled = SystemConf::getInstance()->get("ppssppsdl_auto_gamepad") == "1";
	enable_ppssppsdlgp->setState(ppssppsdlgpEnabled);
	gamepadConfiguration->addWithLabel(_("AUTO CONFIG PPSSPPSDL GAMEPAD"), enable_ppssppsdlgp);

	gamepadConfiguration->addSaveFunc([enable_ppssppsdlgp, window] {
		bool ppssppsdlgpenabled = enable_ppssppsdlgp->getState();
		SystemConf::getInstance()->set("ppssppsdl_auto_gamepad", ppssppsdlgpenabled ? "1" : "0");
		SystemConf::getInstance()->saveSystemConf();
	});

	window->pushGui(gamepadConfiguration);
}

#endif

void GuiMenu::openExternalMounts(Window* mWindow, std::string configName)
{

	GuiSettings* externalMounts = new GuiSettings(mWindow, _("EXTERNAL MOUNT OPTIONS").c_str());
    std::string a;

		auto emuelec_external_device_def = std::make_shared< OptionListComponent<std::string> >(mWindow, "EXTERNAL DEVICE", false);
		std::vector<std::string> extdevoptions;
		extdevoptions.push_back("auto");
		  for(std::stringstream ss(getShOutput(R"(find /var/media/ -type d -maxdepth 1 -mindepth 1 -name EEROMS -prune -o -exec basename {} \; | sed "s/$/,/g")")); getline(ss, a, ','); ) {
            extdevoptions.push_back(a);
	    }
		// use script to get entries

		auto extdevoptionsS = SystemConf::getInstance()->get("global.externalmount");
		if (extdevoptionsS.empty())
		extdevoptionsS = "auto";

		for (auto it = extdevoptions.cbegin(); it != extdevoptions.cend(); it++)
		emuelec_external_device_def->add(*it, *it, extdevoptionsS == *it);

        externalMounts->addWithDescription(_("EXTERNAL DEVICE"), _("Select the mounted drive to be used for ROMS."), emuelec_external_device_def);

        emuelec_external_device_def->setSelectedChangedCallback([emuelec_external_device_def](std::string name) {
       		if (SystemConf::getInstance()->set("global.externalmount", name)) {
			   if (emuelec_external_device_def->getSelected() != "auto") {
                    std::string path = ("/var/media/" + emuelec_external_device_def->getSelected() + "/roms/emuelecroms").c_str();
                        if (!Utils::FileSystem::exists(path)) {
                            system((std::string("mkdir -p \"/var/media/") + emuelec_external_device_def->getSelected() + std::string("/roms\"")).c_str());
                            system((std::string("touch \"/var/media/") + emuelec_external_device_def->getSelected() + std::string("/roms/emuelecroms\"")).c_str());
                        }
                }
            SystemConf::getInstance()->saveSystemConf();
        }
        });

		auto emuelec_external_device_retry = std::make_shared< OptionListComponent<std::string> >(mWindow, _("RETRY TIMES"), false);
		emuelec_external_device_retry->addRange({ { _("AUTO"), "" },{ "1", "1" },{ "2", "2" },{ "3", "3" },{ "4", "4" },{ "5", "5" },{ "6", "6" },{ "7", "7" },{ "8", "8" },{ "9", "9" },{ "10", "10" },{ "11", "11" },{ "12", "12" },{ "13", "13" },{ "14", "14" },{ "15", "15" },{ "16", "16" },{ "17", "17" },{ "18", "18" },{ "19", "19" },{ "20", "20" },{ "21", "21" },{ "22", "22" },{ "23", "23" },{ "24", "24" },{ "25", "25" },{ "26", "26" },{ "27", "27" },{ "28", "28" },{ "29", "29" },{ "30", "30" } }, SystemConf::getInstance()->get("ee_mount.retry"));
        externalMounts->addWithDescription(_("RETRY TIMES"), _("How many times to retry the mount on boot."), emuelec_external_device_retry);
		emuelec_external_device_retry->setSelectedChangedCallback([emuelec_external_device_retry](std::string name) {
            if (SystemConf::getInstance()->set("ee_mount.retry", name))
                SystemConf::getInstance()->saveSystemConf();
            });

		auto emuelec_external_device_retry_delay = std::make_shared< OptionListComponent<std::string> >(mWindow, _("DELAY BETWEEN TRIES"), false);
		emuelec_external_device_retry_delay->addRange({ { _("AUTO"), "" },{ "1", "1" },{ "2", "2" },{ "3", "3" },{ "4", "4" },{ "5", "5" },{ "6", "6" },{ "7", "7" },{ "8", "8" },{ "9", "9" },{ "10", "10" },{ "11", "11" },{ "12", "12" },{ "13", "13" },{ "14", "14" },{ "15", "15" },{ "16", "16" },{ "17", "17" },{ "18", "18" },{ "19", "19" },{ "20", "20" },{ "21", "21" },{ "22", "22" },{ "23", "23" },{ "24", "24" },{ "25", "25" },{ "26", "26" },{ "27", "27" },{ "28", "28" },{ "29", "29" },{ "30", "30" } }, SystemConf::getInstance()->get("ee_load.delay"));
        externalMounts->addWithDescription(_("DELAY BETWEEN TRIES"), _("How much delay in seconds between each retry."), emuelec_external_device_retry_delay);
		emuelec_external_device_retry_delay->setSelectedChangedCallback([emuelec_external_device_retry_delay](std::string name) {
            if (SystemConf::getInstance()->set("ee_load.delay", name))
                SystemConf::getInstance()->saveSystemConf();
            });

        externalMounts->addEntry(_("FORCE MOUNT NOW"), true, [mWindow] {
            std::string selectedExternalDrive = SystemConf::getInstance()->get("global.externalmount");
            mWindow->pushGui(new GuiMsgBox(mWindow, (_("WARNING THIS WILL RESTART EMULATIONSTATION!\n\nSystem will try to mount the external drive selected ") + "\""+ selectedExternalDrive + "\"" + _(". Make sure you have all the settings saved before running this.\n\nTRY TO MOUNT EXTERNAL AND RESTART?")).c_str(), _("YES"),
				[selectedExternalDrive] {
				SystemConf::getInstance()->saveSystemConf();

                auto mountH = SystemConf::getInstance()->get("ee_mount.handler");
                if (mountH == "eemount" || mountH.empty()) {
                   runSystemCommand("eemount --esrestart " + selectedExternalDrive, "", nullptr);
                } else if (mountH == "mount_romfs.sh") {
                   runSystemCommand("mount_romfs.sh yes " + selectedExternalDrive, "", nullptr);
                } else {
                   runSystemCommand(mountH + selectedExternalDrive, "", nullptr);
                }

                }, _("NO"), nullptr));
		});

mWindow->pushGui(externalMounts);
}

#ifdef _ENABLEEMUELEC
void GuiMenu::addFrameBufferOptions(Window* mWindow, GuiSettings* guiSettings, std::string configName, std::string header)
{
	std::string ee_videomode = SystemConf::getInstance()->get("ee_videomode");
	if (ee_videomode.empty() || ee_videomode == "auto")
		ee_videomode = getShOutput(R"(cat /sys/class/display/mode)");

	if (Utils::FileSystem::exists("/storage/.config/EE_VIDEO_MODE"))
		ee_videomode = getShOutput(R"(cat /storage/.config/EE_VIDEO_MODE)");

	if (configName != "ee_es" && configName != "ee_emu")
		ee_videomode = SystemConf::getInstance()->get(configName+".nativevideo");

	std::string ee_framebuffer = SystemConf::getInstance()->get(configName+".framebuffer."+ee_videomode);
	if (ee_framebuffer.empty()) {
		ee_framebuffer = "auto";
	}

	std::vector<std::string> reslist;
		reslist.push_back("3840 2160");
		reslist.push_back("2560 1440");
		reslist.push_back("1920 1080");
		reslist.push_back("1280 720");
		reslist.push_back("720 480");
		reslist.push_back("768 576");
		reslist.push_back("1680 1050");
		reslist.push_back("1440 900");
		reslist.push_back("1280 1024");
		reslist.push_back("1280 960");
		reslist.push_back("1280 800");
		reslist.push_back("1024 768");
		reslist.push_back("800 600");
		reslist.push_back("640 480");

	int* ee_dimensions = getVideoModeDimensions(ee_videomode, reslist);

	static sScreenDimensions ee_screen;
	ee_screen.width = ee_dimensions[0];
	ee_screen.height = ee_dimensions[1];

	auto emuelec_frame_buffer = std::make_shared< OptionListComponent<std::string> >(mWindow, "VIDEO MODE", false);

	emuelec_frame_buffer->add("auto", "auto", ee_framebuffer == "auto");

	for (auto it = reslist.cbegin(); it != reslist.cend(); it++) {
		std::string lbl = *it;
		lbl = lbl.replace(lbl.find(" "),1,"x");
		emuelec_frame_buffer->add(lbl, *it, ee_framebuffer == *it);
	}
	guiSettings->addWithLabel(header+_(" FRAME BUFFER"), emuelec_frame_buffer);

	auto fbSave = [mWindow, configName, emuelec_frame_buffer, ee_videomode, ee_screen] (std::string selectedFB) {
		if (emuelec_frame_buffer->changed()) {
			if (selectedFB == "auto")
				selectedFB = "";

			std::string cfgName = "framebuffer."+ee_videomode;
			if (!configName.empty())
				cfgName = configName+"."+cfgName;

			SystemConf::getInstance()->set(cfgName, selectedFB);
			mWindow->displayNotificationMessage(_U("\uF011  ") + _("A REBOOT OF THE SYSTEM IS REQUIRED TO APPLY THE NEW CONFIGURATION"));

			cfgName = "framebuffer_border."+ee_videomode;
			if (!configName.empty())
				cfgName = configName+".framebuffer_border."+ee_videomode;

			if (selectedFB == "") {
				SystemConf::getInstance()->set(cfgName, "");
				return;
			}

			std::string result = "0 0 "+
				std::to_string(ee_screen.width-1)+" "+
				std::to_string(ee_screen.height-1);

			SystemConf::getInstance()->set(cfgName, result);
		}
	};

	emuelec_frame_buffer->setSelectedChangedCallback([mWindow, configName, emuelec_frame_buffer, fbSave, ee_videomode, ee_screen](std::string name)
	{
		fbSave(emuelec_frame_buffer->getSelected());
	});

	guiSettings->addEntry(_("ADJUST FRAME BORDERS"), true, [mWindow, configName, ee_videomode, ee_framebuffer, ee_screen] {
		static sScreenBorders ee_borders;
		ee_borders.left = 0.0f;
		ee_borders.right = 0.0f;
		ee_borders.top = 0.0f;
		ee_borders.bottom = 0.0f;

		std::string cfgName = "framebuffer_border."+ee_videomode;
		if (!configName.empty())
			cfgName = configName+"."+cfgName;

		std::string str_ee_offsets = SystemConf::getInstance()->get(cfgName);
		if (!str_ee_offsets.empty()) {
			std::vector<int> savedBorders = int_explode(str_ee_offsets, ' ');
			if (savedBorders.size() == 4) {
				ee_borders.left = (float) savedBorders[0];
				ee_borders.top = (float) savedBorders[1];
				ee_borders.right = (float) savedBorders[2];
				ee_borders.bottom = (float) savedBorders[3];
			}
		}

		GuiSettings* bordersConfig = new GuiSettings(mWindow, _("FRAME BORDERS"));
		if (ee_framebuffer.empty())
			return;

		float width = (float)ee_screen.width;
		float height = (float)ee_screen.height;

		// borders
		std::shared_ptr<SliderComponent> fb_borders[] = {
			std::make_shared<SliderComponent>(mWindow, 0.0f, width, 1.0f, "px"),
			std::make_shared<SliderComponent>(mWindow, 0.0f, height, 1.0f, "px"),
			std::make_shared<SliderComponent>(mWindow, 0.0f, width, 1.0f, "px"),
			std::make_shared<SliderComponent>(mWindow, 0.0f, height, 1.0f, "px")
		};

		fb_borders[0]->setValue(ee_borders.left);
		fb_borders[1]->setValue(ee_borders.top);
		fb_borders[2]->setValue((ee_borders.right > 0.0f) ? width-ee_borders.right-1 : 0.0f);
		fb_borders[3]->setValue((ee_borders.bottom > 0.0f) ? height-ee_borders.bottom-1 : 0.0f);

		fb_borders[0]->setOnValueChanged([fb_borders] (float val) {
			fb_borders[2]->setValue(val);
		});
		fb_borders[1]->setOnValueChanged([fb_borders] (float val) {
			fb_borders[3]->setValue(val);
		});

		bordersConfig->addWithLabel(_("LEFT BORDER"), fb_borders[0]);
		bordersConfig->addWithLabel(_("TOP BORDER"), fb_borders[1]);
		bordersConfig->addWithLabel(_("RIGHT BORDER"), fb_borders[2]);
		bordersConfig->addWithLabel(_("BOTTOM BORDER"), fb_borders[3]);

		bordersConfig->addSaveFunc([mWindow, configName, ee_videomode, ee_screen, fb_borders]()
		{
			int borders[4] = {0,0,0,0};
			borders[0] = (int) fb_borders[0]->getValue();
			borders[1] = (int) fb_borders[1]->getValue();
			borders[2] = (int) fb_borders[2]->getValue();
			borders[3] = (int) fb_borders[3]->getValue();

			std::string result = std::to_string(borders[0])+" "+
				std::to_string(borders[1])+" "+
				std::to_string(ee_screen.width-(borders[2])-1)+" "+
				std::to_string(ee_screen.height-(borders[3])-1);

			std::string cfgName = "framebuffer_border."+ee_videomode;
			SystemConf::getInstance()->set(cfgName, result);
			if (!configName.empty())
				SystemConf::getInstance()->set(configName+"."+cfgName, result);

			runSystemCommand("ee_set_borders "+result, "", nullptr);
		});

		mWindow->pushGui(bordersConfig);
	});

}

#endif

void GuiMenu::openDangerZone(Window* mWindow, std::string configName)
{

	GuiSettings* dangerZone = new GuiSettings(mWindow, _("DANGER ZONE").c_str());

#if defined(_ENABLEGAMEFORCE) || defined(ODROIDGOA)
	// OG OC
	auto emuelec_oga_overclock = std::make_shared<OptionListComponent<std::string>>(mWindow, _("OVERCLOCK"));
    emuelec_oga_overclock->addRange({ { _("Off"), "Off" }, { _("1.4ghz"), "1.4ghz" }, { "1.5ghz", "1.5ghz" } }, SystemConf::getInstance()->get("ee_oga_oc"));
    dangerZone->addWithLabel(_("OVERCLOCK"), emuelec_oga_overclock);
    dangerZone->addSaveFunc([configName, emuelec_oga_overclock, mWindow] {

 auto setOverclock = [emuelec_oga_overclock](const std::string& value)
        {
            LOG(LogInfo) << "Setting OGA_OC to " + value;
            runSystemCommand("/usr/bin/odroidgoa_utils.sh oga_oc " + value, "", nullptr);
            SystemConf::getInstance()->set("ee_oga_oc", value);
            SystemConf::getInstance()->saveSystemConf();
        };

        std::string selectedoc = emuelec_oga_overclock->getSelected();
        if (emuelec_oga_overclock && emuelec_oga_overclock->changed())
        {
            if (selectedoc != "Off")
            {
                std::string msg = _("OGA OC is HIGHLY experimental, you may encounter random lockups or your device might not boot anymore. \n");
                msg += _("In case you cannot boot anymore, create an empty file called \"no_oc.oga\" on the boot (EMUELEC) partition.\n\n");
                msg += _("There is also the posibility of SD card file corruption!!! Only enable OC if you agree to the risks!\n\n");
                msg += _("Do you want to proceed ?");

                mWindow->pushGui(new GuiMsgBox(mWindow, msg, _("YES"), [selectedoc, setOverclock]() { setOverclock(selectedoc); }, _("NO"), nullptr));
            }
            else
                setOverclock(selectedoc);
        }


         });
#endif

#ifdef _ENABLEEMUELEC

		addFrameBufferOptions(mWindow, dangerZone, "ee_es", "ES");
		addFrameBufferOptions(mWindow, dangerZone, "ee_emu", "EMU");
#endif

    dangerZone->addEntry(_("CLOUD BACKUP SETTINGS AND GAME SAVES"), true, [mWindow] {
    mWindow->pushGui(new GuiMsgBox(mWindow, _("WARNING THIS WILL RESTART EMULATIONSTATION!\n\nThis will backup your game saves, savestates and emuelec configs to the cloud service configured on rclone.conf\n\nBACKUP TO CLOUD AND RESTART?"), _("YES"),
				[] {
				runSystemCommand("systemd-run /usr/bin/emuelec-utils ee_cloud_backup backup", "", nullptr);
				}, _("NO"), nullptr));
     });

    dangerZone->addEntry(_("CLOUD RESTORE SETTINGS AND GAME SAVES"), true, [mWindow] {
    mWindow->pushGui(new GuiMsgBox(mWindow, _("WARNING THIS WILL RESTART EMULATIONSTATION!\n\nThis will restore your game saves, savestates and emuelec configs from the cloud service configured on rclone.conf, it will overwrite any existing file!!\n\nRESTORE FROM CLOUD AND RESTART?"), _("YES"),
				[] {
				runSystemCommand("systemd-run /usr/bin/emuelec-utils ee_cloud_backup restore", "", nullptr);
				}, _("NO"), nullptr));
     });

    dangerZone->addEntry(_("LOCAL BACKUP EMUELEC CONFIGS"), true, [mWindow] {
    mWindow->pushGui(new GuiMsgBox(mWindow, _("WARNING THIS WILL RESTART EMULATIONSTATION!\n\nAFTER THE SCRIPT IS DONE REMEMBER TO COPY THE FILE /storage/roms/backup/ee_backup_config.tar.gz TO SOME PLACE!\n\nBACKUP CURRENT CONFIG AND RESTART?"), _("YES"),
				[] {
				runSystemCommand("systemd-run /usr/bin/emuelec-utils ee_backup backup", "", nullptr);
				}, _("NO"), nullptr));
     });

    dangerZone->addEntry(_("RESET EMUELEC SCRIPTS AND BINARIES TO DEFAULT"), true, [mWindow] {
    mWindow->pushGui(new GuiMsgBox(mWindow, _("WARNING: SYSTEM WILL RESET SCRIPTS AND BINARIES !\nUPDATE, DOWNLOADS, THEMES, BLUETOOTH PAIRINGS AND ROMS FOLDER WILL NOT BE AFFECTED.\n\nRESET SCRIPTS AND BINARIES TO DEFAULT AND RESTART?"), _("YES"),
				[] {
				runSystemCommand("systemd-run /usr/bin/emuelec-utils clearconfig EMUS", "", nullptr);
				}, _("NO"), nullptr));
     });

    dangerZone->addEntry(_("RESET RETROARCH CONFIG TO DEFAULT"), true, [mWindow] {
    mWindow->pushGui(new GuiMsgBox(mWindow, _("WARNING: RETROARCH CONFIG WILL RESET TO DEFAULT\n\nPER-CORE CONFIGURATIONS WILL NOT BE AFFECTED BUT NO BACKUP WILL BE CREATED!\n\nRESET RETROARCH CONFIG TO DEFAULT?"), _("YES"),
				[] {
				runSystemCommand("systemd-run /usr/bin/emuelec-utils clearconfig retroarch", "", nullptr);
				}, _("NO"), nullptr));
     });

    dangerZone->addEntry(_("RESET SYSTEM TO DEFAULT CONFIG"), true, [mWindow] {
    mWindow->pushGui(new GuiMsgBox(mWindow, _("WARNING: ALL CONFIGURATIONS WILL BE RESET AND NO BACKUP WILL BE CREATED!\n\nIF YOU WANT TO KEEP YOUR SETTINGS MAKE A BACKUP AND SAVE IT ON AN EXTERNAL DRIVE BEFORE RUNING THIS OPTION!\n\nRESET SYSTEM TO DEFAULT CONFIG AND RESTART?"), _("YES"),
				[] {
				runSystemCommand("systemd-run /usr/bin/emuelec-utils clearconfig ALL", "", nullptr);
				}, _("NO"), nullptr));
     });
    dangerZone->addEntry(_("FORCE UPDATE"), true, [mWindow] {

    				if (ApiSystem::getInstance()->getIpAdress() == "NOT CONNECTED")
					{
						mWindow->pushGui(new GuiMsgBox(mWindow, _("YOU ARE NOT CONNECTED TO A NETWORK"), _("OK"), nullptr));
						return;
					}

    mWindow->pushGui(new GuiMsgBox(mWindow, _("WARNING: A FORCE UPDATE WILL DOWNLOAD WHATEVER VERSION IS AVAILABLE FOR UPDATE REGARDLESS OF VERSION BASED ON THE TYPE YOU HAVE SELECTED IN THE UPDATE & DOWNLOADS (beta or stable)\n\nSYSTEM WILL RESET SCRIPTS AND BINARIES !\nDOWNLOADS, THEMES, BLUETOOTH PAIRINGS AND ROMS FOLDER WILL NOT BE AFFECTED.\n\nCONTINUE WITH FORCE UPDATE?"), _("YES"),
				[] {
				runSystemCommand("systemd-run /usr/bin/updatecheck.sh forceupdate", "", nullptr);
				}, _("NO"), nullptr));
     });

mWindow->pushGui(dangerZone);
}


/*  emuelec >*/
#endif

void GuiMenu::openScraperSettings()
{
	mWindow->pushGui(new GuiScraperStart(mWindow));
}

void GuiMenu::openConfigInput()
{
	Window* window = mWindow;
	window->pushGui(new GuiMsgBox(window, _("ARE YOU SURE YOU WANT TO CONFIGURE THE INPUT?"),
		_("YES"), [window] { window->pushGui(new GuiDetectDevice(window, false, nullptr)); },
		_("NO"), nullptr)
	);
}

void GuiMenu::addVersionInfo()
{
	std::string  buildDate = (Settings::getInstance()->getBool("Debug") ? std::string( "   (" + Utils::String::toUpper(PROGRAM_BUILT_STRING) + ")") : (""));

	auto theme = ThemeData::getMenuTheme();

	mVersion.setFont(theme->Footer.font);
	mVersion.setColor(theme->Footer.color);

	mVersion.setLineSpacing(0);

	std::string label;

	if (!ApiSystem::getInstance()->getVersion().empty())
	{
		if (ApiSystem::getInstance()->getApplicationName() == "BATOCERA")
			label = "BATOCERA.LINUX ES V" + ApiSystem::getInstance()->getVersion() + buildDate;
		else
		{
#ifdef _ENABLEEMUELEC
		label = "H4CK-G0 v" + std::string("1.6") + /*" EMUELEC V" + ApiSystem::getInstance()->getVersion() + buildDate +*/ " IP:" + getShOutput(R"(/usr/bin/emuelec-utils getip)");
#else
			std::string aboutInfo = ApiSystem::getInstance()->getApplicationName() + " V" + ApiSystem::getInstance()->getVersion();
			label = aboutInfo + buildDate;
#endif
		}
	}

	if (!label.empty())
	{
		if (Renderer::isSmallScreen())
		{
			mMenu.setSubTitle(label);
			// mMenu.addButton(_("BACK"), _("go back"), [&] { delete this; });
		}
		else
		{
			mVersion.setText(label);
		}
	}

	mVersion.setHorizontalAlignment(ALIGN_CENTER);
	mVersion.setVerticalAlignment(ALIGN_CENTER);
	addChild(&mVersion);
}

void GuiMenu::openScreensaverOptions()
{
	mWindow->pushGui(new GuiGeneralScreensaverOptions(mWindow));
}

void GuiMenu::openCollectionSystemSettings()
{
	if (ThreadedScraper::isRunning() || ThreadedHasher::isRunning())
	{
		mWindow->pushGui(new GuiMsgBox(mWindow, _("THIS FUNCTION IS DISABLED WHILE THE SCRAPER IS RUNNING")));
		return;
	}

	mWindow->pushGui(new GuiCollectionSystemsOptions(mWindow));
}

void GuiMenu::onSizeChanged()
{
	GuiComponent::onSizeChanged();

	float h = mMenu.getButtonGridHeight();

	mVersion.setSize(mSize.x(), h);
	mVersion.setPosition(0, mSize.y() - h);
}

void GuiMenu::addEntry(std::string name, bool add_arrow, const std::function<void()>& func, const std::string iconName)
{
	auto theme = ThemeData::getMenuTheme();
	std::shared_ptr<Font> font = theme->Text.font;
	unsigned int color = theme->Text.color;

	ComponentListRow row;

	MenuComponent::addMenuIcon(mWindow, row, iconName);

	auto text = std::make_shared<TextComponent>(mWindow, name, font, color);
	row.addElement(text, true);

	if (EsLocale::isRTL())
		text->setHorizontalAlignment(Alignment::ALIGN_RIGHT);

	if (add_arrow)
	{
		std::shared_ptr<ImageComponent> bracket = makeArrow(mWindow);

		if (EsLocale::isRTL())
			bracket->setFlipX(true);

		row.addElement(bracket, false);
	}

	row.makeAcceptInputHandler(func);
	mMenu.addRow(row);
}

bool GuiMenu::input(InputConfig* config, Input input)
{
	if(GuiComponent::input(config, input))
		return true;

	if((config->isMappedTo(BUTTON_BACK, input) || config->isMappedTo("start", input)) && input.value != 0)
	{
		delete this;
		return true;
	}

	return false;
}

std::vector<HelpPrompt> GuiMenu::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts;
	prompts.push_back(HelpPrompt("up/down", _("CHOOSE")));
	prompts.push_back(HelpPrompt(BUTTON_OK, _("SELECT")));
	prompts.push_back(HelpPrompt("start", _("CLOSE"), [&] { delete this; }));
	return prompts;
}

class ExitKidModeMsgBox : public GuiSettings
{
	public: ExitKidModeMsgBox(Window* window, const std::string& title, const std::string& text) : GuiSettings(window, title) { addEntry(text); }

	bool input(InputConfig* config, Input input) override
	{
		if (UIModeController::getInstance()->listen(config, input))
		{
			mWindow->pushGui(new GuiMsgBox(mWindow, _("THE USER INTERFACE MODE IS NOW UNLOCKED"),
				_("OK"), [this]
				{
					Window* window = mWindow;
					while (window->peekGui() && window->peekGui() != ViewController::get())
						delete window->peekGui();
				}));


			return true;
		}

		return GuiComponent::input(config, input);
	}
};

void GuiMenu::exitKidMode()
{
	if (Settings::getInstance()->getString("UIMode") == "Basic")
	{
		Settings::getInstance()->setString("UIMode", "Full");

		Window* window = mWindow;
		while (window->peekGui() && window->peekGui() != ViewController::get())
			delete window->peekGui();
	}
	else
		mWindow->pushGui(new ExitKidModeMsgBox(mWindow, _("UNLOCK USER INTERFACE MODE"), _("ENTER THE CODE NOW TO UNLOCK")));
}

void GuiMenu::openSystemInformations()
{
	mWindow->pushGui(new GuiSystemInformation(mWindow));
}

void GuiMenu::openDeveloperSettings()
{
	Window *window = mWindow;

	auto s = new GuiSettings(mWindow, _("FRONTEND DEVELOPER OPTIONS").c_str());

	s->addGroup(_("VIDEO OPTIONS"));

	// maximum vram
	auto max_vram = std::make_shared<SliderComponent>(mWindow, 40.f, 1000.f, 10.f, "Mb");
	max_vram->setValue((float)(Settings::getInstance()->getInt("MaxVRAM")));
	s->addWithLabel(_("VRAM LIMIT"), max_vram);
	s->addSaveFunc([max_vram] { Settings::getInstance()->setInt("MaxVRAM", (int)round(max_vram->getValue())); });

	s->addSwitch(_("SHOW FRAMERATE"), _("Also turns on the emulator's native FPS counter, if available."), "DrawFramerate", true, nullptr);
	s->addSwitch(_("VSYNC"), "VSync", true, [] { Renderer::setSwapInterval(); });

#ifdef BATOCERA
	// overscan
	auto overscan_enabled = std::make_shared<SwitchComponent>(mWindow);
	overscan_enabled->setState(Settings::getInstance()->getBool("Overscan"));
	s->addWithLabel(_("OVERSCAN"), overscan_enabled);
	s->addSaveFunc([overscan_enabled]
	{
		if (Settings::getInstance()->getBool("Overscan") != overscan_enabled->getState())
		{
			Settings::getInstance()->setBool("Overscan", overscan_enabled->getState());
			ApiSystem::getInstance()->setOverscan(overscan_enabled->getState());
		}
	});
#endif

#ifdef _RPI_
	// Video Player - VideoOmxPlayer
	auto omx_player = std::make_shared<SwitchComponent>(mWindow);
	omx_player->setState(Settings::getInstance()->getBool("VideoOmxPlayer"));
	s->addWithLabel(_("USE OMX PLAYER (HARDWARE ACCELERATED)"), omx_player);
	s->addSaveFunc([omx_player, window]
	{
		// need to reload all views to re-create the right video components
		bool needReload = false;
		if (Settings::getInstance()->getBool("VideoOmxPlayer") != omx_player->getState())
			needReload = true;

		Settings::getInstance()->setBool("VideoOmxPlayer", omx_player->getState());

		if (needReload)
		{
			ViewController::get()->reloadAll(window);
			window->closeSplashScreen();
		}
	});
#endif

	s->addGroup(_("TOOLS"));

#ifndef WIN32
	// GAME AT STARTUP
	if (!SystemConf::getInstance()->get("global.bootgame.path").empty())
	{
		std::string gamelabel = SystemConf::getInstance()->get("global.bootgame.path");
		gamelabel = Utils::FileSystem::getStem(gamelabel) + " [" + Utils::FileSystem::getStem(Utils::FileSystem::getParent(gamelabel)) + "]";

		s->addWithDescription(_("STOP LAUNCHING THIS GAME AT STARTUP"), gamelabel, nullptr, [s]
		{
			SystemConf::getInstance()->set("global.bootgame.path", "");
			SystemConf::getInstance()->set("global.bootgame.cmd", "");

			s->close();
		});
	}
#endif
#ifndef _ENABLEEMUELEC
	// WEB ACCESS
	auto hostName = Utils::String::toLower(ApiSystem::getInstance()->getHostsName());

	auto webAccess = std::make_shared<SwitchComponent>(mWindow);
	webAccess->setState(Settings::getInstance()->getBool("PublicWebAccess"));
	s->addWithDescription(_("ENABLE PUBLIC WEB API ACCESS"), Utils::String::format(_("Allow public web access API using %s").c_str(), std::string("http://" + hostName + ":1234").c_str()), webAccess);
	s->addSaveFunc([webAccess, window, s]
	{
	  if (Settings::getInstance()->setBool("PublicWebAccess", webAccess->getState()))
	  {
		  s->setVariable("reboot", true);
		  s->setVariable("exitreboot", true);
	  }
	});
#endif

	// log level
	auto logLevel = std::make_shared< OptionListComponent<std::string> >(mWindow, _("LOG LEVEL"), false);
	std::vector<std::string> modes;
	modes.push_back("default");
#ifdef _ENABLEEMUELEC
	modes.push_back("minimal");
#else
	modes.push_back("disabled");
#endif
	modes.push_back("warning");
	modes.push_back("error");
	modes.push_back("debug");

	auto level = Settings::getInstance()->getString("LogLevel");
	if (level.empty())
		level = "default";

	for (auto it = modes.cbegin(); it != modes.cend(); it++)
		logLevel->add(_(it->c_str()), *it, level == *it);

	s->addWithLabel(_("LOG LEVEL"), logLevel);
	s->addSaveFunc([this, logLevel]
	{
		if (Settings::getInstance()->setString("LogLevel", logLevel->getSelected() == "default" ? "" : logLevel->getSelected()))
		{
			Log::init();
		}
	});

	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::SUPPORTFILE))
	{
		// support
		s->addEntry(_("CREATE A SUPPORT FILE"), true, [window]
		{
			window->pushGui(new GuiMsgBox(window, _("CREATE A SUPPORT FILE? THIS INCLUDES ALL DATA IN YOUR SYSTEM FOLDER."), _("YES"),
				[window]
				{
					if (ApiSystem::getInstance()->generateSupportFile())
						window->pushGui(new GuiMsgBox(window, _("SUPPORT FILE CREATED IN SAVES FOLDER"), _("OK")));
					else
						window->pushGui(new GuiMsgBox(window, _("SUPPORT FILE CREATION FAILED"), _("OK")));
				},
				_("NO"), nullptr));
		});
	}

	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::DISKFORMAT))
		s->addEntry(_("FORMAT A DISK"), true, [this] { openFormatDriveSettings(); });

	s->addEntry(_("CLEAN GAMELISTS & REMOVE UNUSED MEDIA"), true, [this, s]
	{
		mWindow->pushGui(new GuiMsgBox(mWindow, _("ARE YOU SURE?"), _("YES"), [&]
		{
			int idx = 0;
			for (auto system : SystemData::sSystemVector)
			{
				mWindow->renderSplashScreen(_("Cleaning") + ": " + system->getFullName(), (float)idx / (float)SystemData::sSystemVector.size());
				cleanupGamelist(system);
				idx++;
			}

			mWindow->closeSplashScreen();
		}, _("NO"), nullptr));
	});

	s->addEntry(_("CLEAR CACHES"), true, [this, s]
	{
		ImageIO::clearImageCache();

		auto rootPath = Utils::FileSystem::getGenericPath(Paths::getUserEmulationStationPath());

		Utils::FileSystem::deleteDirectoryFiles(rootPath + "/tmp/");
		Utils::FileSystem::deleteDirectoryFiles(Utils::FileSystem::getTempPath());
		Utils::FileSystem::deleteDirectoryFiles(Utils::FileSystem::getPdfTempPath());

		ViewController::reloadAllGames(mWindow, false);
	});

	s->addEntry(_("BUILD IMAGE CACHE"), true, [this, s]
	{
		unsigned int x;
		unsigned int y;

		int idx = 0;
		for (auto sys : SystemData::sSystemVector)
		{
			if (sys->isCollection())
			{
				idx++;
				continue;
			}

			mWindow->renderSplashScreen(_("Building image cache") + ": " + sys->getFullName(), (float)idx / (float)SystemData::sSystemVector.size());

			for (auto file : sys->getRootFolder()->getFilesRecursive(GAME))
			{
				for (auto mdd : MetaDataList::getMDD())
				{
					if (mdd.id != MetaDataId::Image && mdd.id != MetaDataId::Thumbnail)
						continue;

					auto value = file->getMetadata(mdd.id);
					if (value.empty())
						continue;

					auto ext = Utils::String::toLower(Utils::FileSystem::getExtension(value));
					if (ext == ".jpg" || ext == ".png")
						ImageIO::loadImageSize(value.c_str(), &x, &y);
				}
			}

			idx++;
		}

		mWindow->closeSplashScreen();
	});

	s->addEntry(_("RESET FILE EXTENSIONS"), false, [this, s]
	{
		for (auto system : SystemData::sSystemVector)
			Settings::getInstance()->setString(system->getName() + ".HiddenExt", "");

		Settings::getInstance()->saveFile();
		ViewController::reloadAllGames(mWindow, false);
	});

	s->addEntry(_("REDETECT ALL GAMES' LANG/REGION"), false, [this]
	{
		Window* window = mWindow;
		window->pushGui(new GuiLoading<int>(window, _("PLEASE WAIT"), [](auto gui)
		{
			for (auto system : SystemData::sSystemVector)
			{
				if (system->isCollection() || !system->isGameSystem())
					continue;

				for (auto game : system->getRootFolder()->getFilesRecursive(GAME))
					game->detectLanguageAndRegion(true);
			}

			return 0;
		}));
	});

	s->addEntry(_("FIND ALL GAMES WITH NETPLAY/ACHIEVEMENTS"), false, [this] { ThreadedHasher::start(mWindow, ThreadedHasher::HASH_ALL , true); });

	s->addGroup(_("DATA MANAGEMENT"));

	// ExcludeMultiDiskContent
	auto excludeMultiDiskContent = std::make_shared<SwitchComponent>(mWindow);
	excludeMultiDiskContent->setState(Settings::getInstance()->getBool("RemoveMultiDiskContent"));
	s->addWithLabel(_("IGNORE MULTI-FILE DISK CONTENT (CUE/GDI/CCD/M3U)"), excludeMultiDiskContent);
	s->addSaveFunc([excludeMultiDiskContent] { Settings::getInstance()->setBool("RemoveMultiDiskContent", excludeMultiDiskContent->getState()); });

	// enable filters (ForceDisableFilters)
	auto enable_filter = std::make_shared<SwitchComponent>(mWindow);
	enable_filter->setState(!Settings::getInstance()->getBool("ForceDisableFilters"));
	s->addWithDescription(_("ENABLE GAME FILTERING"), _("Whether to show or hide game filtering related settings in the view options."), enable_filter);
	s->addSaveFunc([this, enable_filter]
	{
		Settings::getInstance()->setBool("ForceDisableFilters", !enable_filter->getState());
	});

	// gamelist saving
	auto save_gamelists = std::make_shared<SwitchComponent>(mWindow);
	save_gamelists->setState(Settings::getInstance()->getBool("SaveGamelistsOnExit"));
	s->addWithLabel(_("SAVE METADATA ON EXIT"), save_gamelists);
	s->addSaveFunc([save_gamelists] { Settings::getInstance()->setBool("SaveGamelistsOnExit", save_gamelists->getState()); });

	// gamelist
	auto parse_gamelists = std::make_shared<SwitchComponent>(mWindow);
	parse_gamelists->setState(Settings::getInstance()->getBool("ParseGamelistOnly"));
	s->addWithDescription(_("PARSE GAMELISTS ONLY"), _("Debug tool: Don't check if the ROMs actually exist. Can cause problems!"), parse_gamelists);
	s->addSaveFunc([parse_gamelists] { Settings::getInstance()->setBool("ParseGamelistOnly", parse_gamelists->getState()); });

	// Local Art
	auto local_art = std::make_shared<SwitchComponent>(mWindow);
	local_art->setState(Settings::getInstance()->getBool("LocalArt"));
	s->addWithDescription(_("SEARCH FOR LOCAL ART"), _("If no image is specified in the gamelist, try to find media with the same filename to use."), local_art);
	s->addSaveFunc([local_art] { Settings::getInstance()->setBool("LocalArt", local_art->getState()); });

	s->addGroup(_("USER INTERFACE"));

	// carousel transition option
	auto move_carousel = std::make_shared<SwitchComponent>(mWindow);
	move_carousel->setState(Settings::getInstance()->getBool("MoveCarousel"));
	s->addWithLabel(_("CAROUSEL TRANSITIONS"), move_carousel);
	s->addSaveFunc([move_carousel] { Settings::getInstance()->setBool("MoveCarousel", move_carousel->getState()); });

	// quick system select (left/right in game list view)
	auto quick_sys_select = std::make_shared<SwitchComponent>(mWindow);
	quick_sys_select->setState(Settings::getInstance()->getBool("QuickSystemSelect"));
	s->addWithLabel(_("QUICK SYSTEM SELECT"), quick_sys_select);
	s->addSaveFunc([quick_sys_select] { Settings::getInstance()->setBool("QuickSystemSelect", quick_sys_select->getState()); });

	// Enable OSK (On-Screen-Keyboard)
	auto osk_enable = std::make_shared<SwitchComponent>(mWindow);
	osk_enable->setState(Settings::getInstance()->getBool("UseOSK"));
	s->addWithLabel(_("ON-SCREEN KEYBOARD"), osk_enable);
	s->addSaveFunc([osk_enable] { Settings::getInstance()->setBool("UseOSK", osk_enable->getState()); });

#if defined(_WIN32) || defined(X86) || defined(X86_64)
	// Hide EmulationStation Window when running a game ( windows only )
	auto hideWindowScreen = std::make_shared<SwitchComponent>(mWindow);
	hideWindowScreen->setState(Settings::getInstance()->getBool("HideWindow"));
	s->addWithLabel(_("HIDE EMULATIONSTATION WHEN RUNNING A GAME"), hideWindowScreen);
	s->addSaveFunc([hideWindowScreen] { Settings::getInstance()->setBool("HideWindow", hideWindowScreen->getState()); });
#endif

#if defined(WIN32) && !defined(_DEBUG)
	// full exit
	auto fullExitMenu = std::make_shared<SwitchComponent>(mWindow);
	fullExitMenu->setState(!Settings::getInstance()->getBool("ShowOnlyExit"));
	s->addWithLabel(_("COMPLETE QUIT MENU"), fullExitMenu);
	s->addSaveFunc([fullExitMenu] { Settings::getInstance()->setBool("ShowOnlyExit", !fullExitMenu->getState()); });
#endif

	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::GAMESETTINGS))
	{
		auto retroarchRgui = std::make_shared< OptionListComponent<std::string> >(mWindow, _("RETROARCH MENU DRIVER"), false);
		retroarchRgui->addRange({ { _("AUTO"), "" },{ "rgui", "rgui" },{ "xmb", "xmb" },{ "ozone", "ozone" },{ "glui", "glui" } }, SystemConf::getInstance()->get("global.retroarch.menu_driver"));
		s->addWithLabel(_("RETROARCH MENU DRIVER"), retroarchRgui);
		s->addSaveFunc([retroarchRgui] { SystemConf::getInstance()->set("global.retroarch.menu_driver", retroarchRgui->getSelected()); });
	}

	auto invertJoy = std::make_shared<SwitchComponent>(mWindow);
	invertJoy->setState(Settings::getInstance()->getBool("InvertButtons"));
	s->addWithDescription(_("SWITCH CONFIRM & CANCEL BUTTONS IN EMULATIONSTATION"), _("Switches the South and East buttons' functionality"), invertJoy);
	s->addSaveFunc([this, s, invertJoy]
	{
		if (Settings::getInstance()->setBool("InvertButtons", invertJoy->getState()))
		{
			InputConfig::AssignActionButtons();
			s->setVariable("reloadAll", true);
		}
	});

	auto invertLongPress = std::make_shared<SwitchComponent>(mWindow);
	invertLongPress->setState(Settings::getInstance()->getBool("GameOptionsAtNorth"));
	s->addWithDescription(_("ACCESS GAME OPTIONS WITH NORTH BUTTON"), _("Switches to short-press North for Savestates & long-press South button for Game Options"), invertLongPress);
	s->addSaveFunc([this, s, invertLongPress]
	{
		if (Settings::getInstance()->setBool("GameOptionsAtNorth", invertLongPress->getState()))
			s->setVariable("reloadAll", true);
	});

	auto firstJoystickOnly = std::make_shared<SwitchComponent>(mWindow);
	firstJoystickOnly->setState(Settings::getInstance()->getBool("FirstJoystickOnly"));
	s->addWithLabel(_("CONTROL EMULATIONSTATION WITH FIRST JOYSTICK ONLY"), firstJoystickOnly);
	s->addSaveFunc([this, firstJoystickOnly] { Settings::getInstance()->setBool("FirstJoystickOnly", firstJoystickOnly->getState()); });

#if !defined(WIN32)
	{
	  auto gun_mt = std::make_shared<SliderComponent>(mWindow, 0.f, 10.f, 0.1f, "%");
	  gun_mt->setValue(Settings::getInstance()->getFloat("GunMoveTolerence"));
	  s->addWithLabel(_("GUN MOVE TOLERENCE"), gun_mt);
	  s->addSaveFunc([gun_mt] {
	    Settings::getInstance()->setFloat("GunMoveTolerence", gun_mt->getValue());
	  });
	}
#endif

#if defined(WIN32)

	auto hidJoysticks = std::make_shared<SwitchComponent>(mWindow);
	hidJoysticks->setState(Settings::getInstance()->getBool("HidJoysticks"));
	s->addWithLabel(_("ENABLE HID JOYSTICK DRIVERS"), hidJoysticks);
	s->addSaveFunc([this, hidJoysticks] { Settings::getInstance()->setBool("HidJoysticks", hidJoysticks->getState()); });

	// Network Indicator
	auto networkIndicator = std::make_shared<SwitchComponent>(mWindow);
	networkIndicator->setState(Settings::getInstance()->getBool("ShowNetworkIndicator"));
	s->addWithLabel(_("SHOW NETWORK INDICATOR"), networkIndicator);
	s->addSaveFunc([networkIndicator] { Settings::getInstance()->setBool("ShowNetworkIndicator", networkIndicator->getState()); });
#endif

	s->addGroup(_("OPTIMIZATIONS"));

	// preload UI
	auto preloadUI = std::make_shared<SwitchComponent>(mWindow);
	preloadUI->setState(Settings::getInstance()->getBool("PreloadUI"));
	s->addWithDescription(_("PRELOAD UI ELEMENTS ON BOOT"), _("Reduces lag when entering gamelists from the system menu, increases boot time"), preloadUI);
	s->addSaveFunc([preloadUI] { Settings::getInstance()->setBool("PreloadUI", preloadUI->getState()); });

	// preload Medias
	auto preloadMedias = std::make_shared<SwitchComponent>(mWindow);
	preloadMedias->setState(Settings::getInstance()->getBool("PreloadMedias"));
	s->addWithDescription(_("PRELOAD METADATA MEDIA ON BOOT"), _("Reduces lag when scrolling through a fully scraped gamelist, increases boot time"), preloadMedias);
	s->addSaveFunc([preloadMedias] { Settings::getInstance()->setBool("PreloadMedias", preloadMedias->getState()); });

	// threaded loading
	auto threadedLoading = std::make_shared<SwitchComponent>(mWindow);
	threadedLoading->setState(Settings::getInstance()->getBool("ThreadedLoading"));
	s->addWithLabel(_("THREADED LOADING"), threadedLoading);
	s->addSaveFunc([threadedLoading] { Settings::getInstance()->setBool("ThreadedLoading", threadedLoading->getState()); });

	// threaded loading
	auto asyncImages = std::make_shared<SwitchComponent>(mWindow);
	asyncImages->setState(Settings::getInstance()->getBool("AsyncImages"));
	s->addWithLabel(_("ASYNC IMAGE LOADING"), asyncImages);
	s->addSaveFunc([asyncImages] { Settings::getInstance()->setBool("AsyncImages", asyncImages->getState()); });

	// optimizeVram
	auto optimizeVram = std::make_shared<SwitchComponent>(mWindow);
	optimizeVram->setState(Settings::getInstance()->getBool("OptimizeVRAM"));
	s->addWithLabel(_("OPTIMIZE IMAGES VRAM USE"), optimizeVram);
	s->addSaveFunc([optimizeVram] { Settings::getInstance()->setBool("OptimizeVRAM", optimizeVram->getState()); });

	// optimizeVideo
	auto optimizeVideo = std::make_shared<SwitchComponent>(mWindow);
	optimizeVideo->setState(Settings::getInstance()->getBool("OptimizeVideo"));
	s->addWithLabel(_("OPTIMIZE VIDEO VRAM USAGE"), optimizeVideo);
	s->addSaveFunc([optimizeVideo] { Settings::getInstance()->setBool("OptimizeVideo", optimizeVideo->getState()); });

	s->onFinalize([s, window]
	{
		if (s->getVariable("reloadAll"))
		{
			ViewController::get()->reloadAll(window);
			window->closeSplashScreen();
		}
	});

	mWindow->pushGui(s);
}

void GuiMenu::openUpdatesSettings()
{
	GuiSettings *updateGui = new GuiSettings(mWindow, _("UPDATES & DOWNLOADS").c_str());

	updateGui->addGroup(_("DOWNLOADS"));

	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::BATOCERASTORE))
	{
		updateGui->addEntry(_("CONTENT DOWNLOADER"), true, [this]
		{
			if (!checkNetwork())
				return;

			mWindow->pushGui(new GuiBatoceraStore(mWindow));
		});
	}

	// Themes installer/browser
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::THEMESDOWNLOADER))
	{
		updateGui->addEntry(_("THEMES"), true, [this]
		{
			if (!checkNetwork())
				return;

			mWindow->pushGui(new GuiThemeInstaller(mWindow));
		});
	}

	// integration with theBezelProject
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::DECORATIONS) && ApiSystem::getInstance()->isScriptingSupported(ApiSystem::THEBEZELPROJECT))
	{
		updateGui->addEntry(_("THE BEZEL PROJECT"), true, [this]
		{
			if (!checkNetwork())
				return;

			mWindow->pushGui(new GuiBezelInstaller(mWindow));
		});
	}

	if (true == false/*ApiSystem::getInstance()->isScriptingSupported(ApiSystem::UPGRADE)*/)
	{
		updateGui->addGroup(_("SOFTWARE UPDATES"));

		// Enable updates
		updateGui->addSwitch(_("CHECK FOR UPDATES"), "updates.enabled", false);

		auto updatesTypeList = std::make_shared<OptionListComponent<std::string> >(mWindow, _("UPDATE TYPE"), false);

		std::string updatesType = SystemConf::getInstance()->get("updates.type");

#if WIN32
		if (updatesType == "unstable")
			updatesTypeList->add("unstable", "unstable", updatesType == "unstable");
		else
#endif
			if (updatesType.empty() || updatesType != "beta")
				updatesType = "stable";

		updatesTypeList->add("stable", "stable", updatesType == "stable");
		updatesTypeList->add("beta", "beta", updatesType == "beta");

		updateGui->addWithLabel(_("UPDATE TYPE"), updatesTypeList);
		updatesTypeList->setSelectedChangedCallback([](std::string name)
		{
			if (SystemConf::getInstance()->set("updates.type", name))
				SystemConf::getInstance()->saveSystemConf();
		});

		// Start update
		updateGui->addEntry(GuiUpdate::state == GuiUpdateState::State::UPDATE_READY ? _("APPLY UPDATE") : _("START UPDATE"), true, [this]
		{
			if (GuiUpdate::state == GuiUpdateState::State::UPDATE_READY)
				quitES(QuitMode::RESTART);
			else if (GuiUpdate::state == GuiUpdateState::State::UPDATER_RUNNING)
				mWindow->pushGui(new GuiMsgBox(mWindow, _("UPDATER IS ALREADY RUNNING")));
			else
			{
				if (!checkNetwork())
					return;

				mWindow->pushGui(new GuiUpdate(mWindow));
			}
		});
	}

	mWindow->pushGui(updateGui);
}

bool GuiMenu::checkNetwork()
{
	if (ApiSystem::getInstance()->getIpAdress() == "NOT CONNECTED")
	{
		mWindow->pushGui(new GuiMsgBox(mWindow, _("YOU ARE NOT CONNECTED TO A NETWORK"), _("OK"), nullptr));
		return false;
	}

	return true;
}

void GuiMenu::openSystemSettings()
{
	Window *window = mWindow;

	auto s = new GuiSettings(mWindow, _("SYSTEM SETTINGS").c_str());
	bool isFullUI = UIModeController::getInstance()->isUIModeFull();

	s->addGroup(_("SYSTEM"));

	// System informations
	s->addEntry(_("INFORMATION"), true, [this] { openSystemInformations(); });

#ifdef _ENABLEEMUELEC
	auto emuelec_timezones = std::make_shared<OptionListComponent<std::string> >(mWindow, _("TIMEZONE"), false);
	std::string currentTimezone = SystemConf::getInstance()->get("system.timezone");
	std::string test_shell = getShOutput(R"(/usr/bin/emuelec-utils test)");
	if (!test_shell.compare("success")) {
		if (currentTimezone.empty())
			currentTimezone = std::string(getShOutput(R"(/usr/bin/emuelec-utils current_timezone)"));
		std::string a;
		for(std::stringstream ss(getShOutput(R"(/usr/bin/emuelec-utils timezones)")); getline(ss, a, ','); ) {
			emuelec_timezones->add(a, a, currentTimezone == a); // emuelec
		}
		s->addWithLabel(_("TIMEZONE"), emuelec_timezones);
		s->addSaveFunc([emuelec_timezones] {
			if (emuelec_timezones->changed()) {
				std::string selectedTimezone = emuelec_timezones->getSelected();
				runSystemCommand("ln -sf /usr/share/zoneinfo/" + selectedTimezone + " $(readlink /etc/localtime)", "", nullptr);
			}
			SystemConf::getInstance()->set("system.timezone", emuelec_timezones->getSelected());
		});
	}

#endif

	// language choice
	auto language_choice = std::make_shared<OptionListComponent<std::string> >(window, _("LANGUAGE"), false);

	std::string language = SystemConf::getInstance()->get("system.language");
	if (language.empty())
		language = "en_US";

	language_choice->add("ARABIC",               "ar_YE", language == "ar_YE");
	language_choice->add("CATALÀ",               "ca_ES", language == "ca_ES");
	language_choice->add("ČEŠTINA",                "cs_CZ", language == "cs_CZ");
	language_choice->add("CYMRAEG",              "cy_GB", language == "cy_GB");
	language_choice->add("DEUTSCH", 	     "de_DE", language == "de_DE");
	language_choice->add("GREEK",                "el_GR", language == "el_GR");
	language_choice->add("ENGLISH (US)", 	     "en_US", language == "en_US" || language == "en");
	language_choice->add("ENGLISH (UK)", 	     "en_GB", language == "en_GB");
	language_choice->add("ESPAÑOL", 	     "es_ES", language == "es_ES" || language == "es");
	language_choice->add("ESPAÑOL MEXICANO",     "es_MX", language == "es_MX");
	language_choice->add("BASQUE",               "eu_ES", language == "eu_ES");
	language_choice->add("SUOMI",                "fi_FI", language == "fi_FI");
	language_choice->add("FRANÇAIS",             "fr_FR", language == "fr_FR" || language == "fr");
	language_choice->add("עברית",                "he_IL", language == "he_IL");
	language_choice->add("HUNGARIAN",            "hu_HU", language == "hu_HU");
	language_choice->add("BAHASA INDONESIA",     "id_ID", language == "id_ID");
	language_choice->add("ITALIANO",             "it_IT", language == "it_IT");
	language_choice->add("JAPANESE", 	     "ja_JP", language == "ja_JP");
	language_choice->add("KOREAN",   	     "ko_KR", language == "ko_KR" || language == "ko");
	language_choice->add("NORWEGIAN BOKMAL",     "nb_NO", language == "nb_NO");
	language_choice->add("DUTCH",                "nl_NL", language == "nl_NL");
	language_choice->add("NORWEGIAN",            "nn_NO", language == "nn_NO");
	language_choice->add("OCCITAN",              "oc_FR", language == "oc_FR");
	language_choice->add("POLISH",               "pl_PL", language == "pl_PL");
	language_choice->add("PORTUGUÊS BRASILEIRO", "pt_BR", language == "pt_BR");
	language_choice->add("PORTUGUÊS PORTUGAL",   "pt_PT", language == "pt_PT");
	language_choice->add("РУССКИЙ",              "ru_RU", language == "ru_RU");
	language_choice->add("SVENSKA", 	     "sv_SE", language == "sv_SE");
	language_choice->add("TÜRKÇE",  	     "tr_TR", language == "tr_TR");
	language_choice->add("Українська",           "uk_UA", language == "uk_UA");
	language_choice->add("简体中文", 	     "zh_CN", language == "zh_CN");
	language_choice->add("正體中文", 	     "zh_TW", language == "zh_TW");

	s->addWithLabel(_("LANGUAGE (REGION)"), language_choice);
	s->addSaveFunc([window, language_choice, language, s]
	{
		if (language_choice->changed() && SystemConf::getInstance()->set("system.language", language_choice->getSelected()))
		{
			FileSorts::reset();
			MetaDataList::initMetadata();

#ifdef HAVE_INTL
			s->setVariable("exitreboot", true);
#endif
			s->setVariable("reloadGuiMenu", true);
		}
	});

#if !defined(_ENABLEEMUELEC)
	// Timezone
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::ScriptId::TIMEZONES))
	{
		VectorEx<std::string> availableTimezones = ApiSystem::getInstance()->getTimezones();
		if (availableTimezones.size() > 0)
		{
			std::string currentTZ = ApiSystem::getInstance()->getCurrentTimezone();
			if (currentTZ.empty() || !availableTimezones.any([currentTZ](const std::string& tz) { return tz == currentTZ; }))
				currentTZ = "Europe/Paris";

			auto tzChoices = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SELECT YOUR TIME ZONE"), false);

			for (auto tz : availableTimezones)
				tzChoices->add(_(Utils::String::toUpper(tz).c_str()), tz, currentTZ == tz);

			s->addWithLabel(_("TIME ZONE"), tzChoices);
			s->addSaveFunc([tzChoices]
			{
				if (SystemConf::getInstance()->set("system.timezone", tzChoices->getSelected()))
					ApiSystem::getInstance()->setTimezone(tzChoices->getSelected());
			});
		}
	}

#endif
	// Clock time format (14:42 or 2:42 pm)
	s->addSwitch(_("SHOW CLOCK IN 12-HOUR FORMAT"), "ClockMode12", true);

	// power saver
	auto power_saver = std::make_shared< OptionListComponent<std::string> >(mWindow, _("POWER SAVING MODE"), false);
	power_saver->addRange({ { _("DISABLED"), "disabled" }, { _("DEFAULT"), "default" }, { _("ENHANCED"), "enhanced" }, { _("INSTANT"), "instant" }, }, Settings::PowerSaverMode());
	s->addWithDescription(_("POWER SAVING MODE"), _("Reduces power consumption when idle (useful for handhelds)."), power_saver);
	s->addSaveFunc([this, power_saver]
	{
		if (Settings::PowerSaverMode() != "instant" && power_saver->getSelected() == "instant")
			Settings::getInstance()->setBool("EnableSounds", false);

		Settings::setPowerSaverMode(power_saver->getSelected());
		PowerSaver::init();
	});

#if defined(_ENABLE_TTS_) || defined(WIN32)
	if (TextToSpeech::getInstance()->isAvailable())
	{
			// tts
		auto tts = std::make_shared<SwitchComponent>(mWindow);
		tts->setState(Settings::getInstance()->getBool("TTS"));
		s->addWithLabel(_("SCREEN READER (TEXT TO SPEECH)"), tts);
		s->addSaveFunc([tts] {
			 if(TextToSpeech::getInstance()->isEnabled() != tts->getState()) {
			   TextToSpeech::getInstance()->enable(tts->getState());
			   Settings::getInstance()->setBool("TTS", tts->getState());
			 }});
	}
#endif

	// UI RESTRICTIONS
	auto UImodeSelection = std::make_shared< OptionListComponent<std::string> >(mWindow, _("USER INTERFACE MODE"), false);
	std::vector<std::string> UImodes = UIModeController::getInstance()->getUIModes();
	for (auto it = UImodes.cbegin(); it != UImodes.cend(); it++)
		UImodeSelection->add(_(it->c_str()), *it, Settings::getInstance()->getString("UIMode") == *it);

	s->addWithDescription(_("USER INTERFACE MODE"), _("Lock down certain config menus for use with guest users/kids."), UImodeSelection);
	s->addSaveFunc([UImodeSelection, window]
	{
		if (UImodeSelection->changed())
		{
			std::string selectedMode = UImodeSelection->getSelected();
			if (selectedMode == "Basic" || selectedMode == "Full")
				Settings::getInstance()->setString("UIMode", selectedMode);
			else
			{
				std::string msg = _("You are changing the user interface to a restricted mode:\nThis will hide most menu options to prevent changes to the system.\nTo unlock and return to the full user interface, enter this code:") + "\n";
				msg += "\"" + UIModeController::getInstance()->getFormattedPassKeyStr() + "\"\n\n";
				msg += _("Do you want to proceed?");
				window->pushGui(new GuiMsgBox(window, msg,
					_("YES"), [selectedMode] {
					LOG(LogDebug) << "Setting user interface mode to " << selectedMode;
					Settings::getInstance()->setString("UIMode", selectedMode);
					Settings::getInstance()->saveFile();
				}, _("NO"), nullptr));
			}
		}
	});

	// KODI SETTINGS
#ifdef _ENABLE_KODI_
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::KODI))
	{
		s->addEntry(_("KODI SETTINGS"), true, [this]
		{
			GuiSettings* kodiGui = new GuiSettings(mWindow, _("KODI SETTINGS").c_str());
			kodiGui->addSwitch(_("ENABLE KODI"), "kodi.enabled", false);
			kodiGui->addSwitch(_("LAUNCH KODI AT BOOT"), "kodi.atstartup", false);
			mWindow->pushGui(kodiGui);
		});
	}
#endif

#ifdef BATOCERA
	s->addGroup(_("HARDWARE"));
#endif

	// brighness
	int brighness;
	if (ApiSystem::getInstance()->getBrightness(brighness))
	{
		auto brightnessComponent = std::make_shared<SliderComponent>(mWindow, 1.f, 100.f, 5.f, "%");
		brightnessComponent->setValue(brighness);
		brightnessComponent->setOnValueChanged([](const float &newVal)
		{
#ifdef _ENABLEEMUELEC
            auto thebright = std::to_string((int)Math::round(newVal));
            runSystemCommand("/usr/bin/odroidgoa_utils.sh bright " + thebright, "", nullptr);
#else
			ApiSystem::getInstance()->setBrightness((int)Math::round(newVal));
#if !WIN32
			SystemConf::getInstance()->set("display.brightness", std::to_string((int)Math::round(newVal)));
#endif
#endif
		});

       s->addSaveFunc([this, brightnessComponent] {
            SystemConf::getInstance()->set("brightness.level", std::to_string((int)Math::round(brightnessComponent->getValue())));
       });

		s->addWithLabel(_("BRIGHTNESS"), brightnessComponent);
	}

#ifdef BATOCERA
	// video device
	std::vector<std::string> availableVideo = ApiSystem::getInstance()->getAvailableVideoOutputDevices();
	if (availableVideo.size())
	{
		auto optionsVideo = std::make_shared<OptionListComponent<std::string> >(mWindow, _("VIDEO OUTPUT"), false);
		std::string currentDevice = SystemConf::getInstance()->get("global.videooutput");
		if (currentDevice.empty()) currentDevice = "auto";

		bool vfound = false;
		for (auto it = availableVideo.begin(); it != availableVideo.end(); it++)
		{
			optionsVideo->add((*it), (*it), currentDevice == (*it));
			if (currentDevice == (*it))
				vfound = true;
		}

		if (!vfound)
			optionsVideo->add(currentDevice, currentDevice, true);

		s->addWithLabel(_("VIDEO OUTPUT"), optionsVideo);
		s->addSaveFunc([this, optionsVideo, currentDevice, s]
		{
			if (optionsVideo->changed())
			{
				SystemConf::getInstance()->set("global.videooutput", optionsVideo->getSelected());
				SystemConf::getInstance()->saveSystemConf();
				s->setVariable("exitreboot", true);
			}
		});
	}
#endif

	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::AUDIODEVICE))
	{
		std::vector<std::string> availableAudio = ApiSystem::getInstance()->getAvailableAudioOutputDevices();
		if (availableAudio.size())
		{
			// audio device
			auto optionsAudio = std::make_shared<OptionListComponent<std::string> >(mWindow, _("AUDIO OUTPUT"), false);

			std::string selectedAudio = ApiSystem::getInstance()->getCurrentAudioOutputDevice();
			if (selectedAudio.empty())
				selectedAudio = "auto";

			bool afound = false;
			for (auto it = availableAudio.begin(); it != availableAudio.end(); it++)
			{
				std::vector<std::string> tokens = Utils::String::split(*it, ' ');

				if (selectedAudio == tokens.at(0))
					afound = true;

				if (tokens.size() >= 2)
				{
					// concatenat the ending words
					std::string vname = "";
					for (unsigned int i = 1; i < tokens.size(); i++)
					{
						if (i > 2) vname += " ";
						vname += tokens.at(i);
					}
					optionsAudio->add(vname, tokens.at(0), selectedAudio == tokens.at(0));
				}
				else
					optionsAudio->add((*it), (*it), selectedAudio == tokens.at(0));
			}

			if (!afound)
				optionsAudio->add(selectedAudio, selectedAudio, true);

			s->addWithLabel(_("AUDIO OUTPUT"), optionsAudio);

			s->addSaveFunc([this, optionsAudio, selectedAudio]
			{
				if (optionsAudio->changed())
				{
					SystemConf::getInstance()->set("audio.device", optionsAudio->getSelected());
					ApiSystem::getInstance()->setAudioOutputDevice(optionsAudio->getSelected());
				}
				SystemConf::getInstance()->saveSystemConf();
			});
		}

		// audio profile
		std::vector<std::string> availableAudioProfiles = ApiSystem::getInstance()->getAvailableAudioOutputProfiles();
		if (availableAudioProfiles.size())
		{
			auto optionsAudioProfile = std::make_shared<OptionListComponent<std::string> >(mWindow, _("AUDIO PROFILE"), false);

			std::string selectedAudioProfile = ApiSystem::getInstance()->getCurrentAudioOutputProfile();
			if (selectedAudioProfile.empty())
				selectedAudioProfile = "auto";

			bool afound = false;
			for (auto it = availableAudioProfiles.begin(); it != availableAudioProfiles.end(); it++)
			{
				std::vector<std::string> tokens = Utils::String::split(*it, ' ');

				if (selectedAudioProfile == tokens.at(0))
					afound = true;

				if (tokens.size() >= 2)
				{
					// concatenat the ending words
					std::string vname = "";
					for (unsigned int i = 1; i < tokens.size(); i++)
					{
						if (i > 2) vname += " ";
						vname += tokens.at(i);
					}
					optionsAudioProfile->add(vname, tokens.at(0), selectedAudioProfile == tokens.at(0));
				}
				else
					optionsAudioProfile->add((*it), (*it), selectedAudioProfile == tokens.at(0));
			}

			if (afound == false)
				optionsAudioProfile->add(selectedAudioProfile, selectedAudioProfile, true);

			s->addWithDescription(_("AUDIO PROFILE"), _("Available options can change depending on current audio output."), optionsAudioProfile);

			s->addSaveFunc([this, optionsAudioProfile, selectedAudioProfile]
			{
				if (optionsAudioProfile->changed()) {
					SystemConf::getInstance()->set("audio.profile", optionsAudioProfile->getSelected());
					ApiSystem::getInstance()->setAudioOutputProfile(optionsAudioProfile->getSelected());
				}
				SystemConf::getInstance()->saveSystemConf();
			});
		}
	}

#ifdef BATOCERA
	// video rotation
	auto optionsRotation = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SCREEN ROTATION"), false);

	std::string selectedRotation = SystemConf::getInstance()->get("display.rotate");
	if (selectedRotation.empty())
		selectedRotation = "auto";

	optionsRotation->add(_("AUTO"),              "auto", selectedRotation == "auto");
	optionsRotation->add(_("0 DEGREES"),        "0", selectedRotation == "0");
	optionsRotation->add(_("90 DEGREES"),       "1", selectedRotation == "1");
	optionsRotation->add(_("180 DEGREES"),    "2", selectedRotation == "2");
	optionsRotation->add(_("270 DEGREES"),        "3", selectedRotation == "3");

	s->addWithLabel(_("SCREEN ROTATION"), optionsRotation);

	s->addSaveFunc([this, optionsRotation, selectedRotation, s]
	{
	  if (optionsRotation->changed())
{
	    SystemConf::getInstance()->set("display.rotate", optionsRotation->getSelected());
	    SystemConf::getInstance()->saveSystemConf();
		s->setVariable("exitreboot", true);
	  }
	});

	// splash
	auto optionsSplash = std::make_shared<OptionListComponent<std::string> >(mWindow, _("BOOT SPLASH"), false);

	std::string enabledSplash = SystemConf::getInstance()->get("splash.screen.enabled");
	std::string soundSplash   = SystemConf::getInstance()->get("splash.screen.sound");

	std::string selectedSplash = "auto";
	if(enabledSplash == "0")      selectedSplash = "nosplash";
	else if(soundSplash   == "0") selectedSplash = "silentsplash";

	optionsSplash->add(_("DEFAULT VIDEO/USER SET SPLASH"),          "auto", selectedSplash == "auto");
	optionsSplash->add(_("SILENT VIDEO/USER SET SPLASH"), "silentsplash", selectedSplash == "silentsplash");
	optionsSplash->add(_("BATOCERA SPLASH IMAGE"),     "nosplash", selectedSplash == "nosplash");

	s->addWithLabel(_("SPLASH SETTING"), optionsSplash);

	s->addSaveFunc([this, optionsSplash, selectedSplash]
	{
	  if (optionsSplash->changed()) {
	    if(optionsSplash->getSelected() == "nosplash") {
	      SystemConf::getInstance()->set("splash.screen.enabled", "0");
	    } else {
	      SystemConf::getInstance()->set("splash.screen.enabled", "1");
	      if(optionsSplash->getSelected() == "silentsplash") {
		SystemConf::getInstance()->set("splash.screen.sound", "0");
	      } else {
		SystemConf::getInstance()->set("splash.screen.sound", "1");
	      }
	    }
	    SystemConf::getInstance()->saveSystemConf();
	  }
	});
#else
	if (!ApiSystem::getInstance()->isScriptingSupported(ApiSystem::GAMESETTINGS))
	{
		// Retroachievements
		if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::RETROACHIVEMENTS))
			s->addEntry(_("RETROACHIEVEMENT SETTINGS"), true, [this] { openRetroachievementsSettings(); });

		if (SystemData::isNetplayActivated() && ApiSystem::getInstance()->isScriptingSupported(ApiSystem::NETPLAY))
			s->addEntry(_("NETPLAY SETTINGS"), true, [this] { openNetplaySettings(); }, "iconNetplay");

		if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::BIOSINFORMATION))
		{
			s->addEntry(_("MISSING BIOS CHECK"), true, [this, s] { openMissingBiosSettings(); });
#ifndef _ENABLEEMUELEC
			s->addSwitch(_("CHECK BIOS FILES BEFORE RUNNING A GAME"), "CheckBiosesAtLaunch", true);
#endif
		}

	}
#endif
	std::shared_ptr<OptionListComponent<std::string>> overclock_choice;

#if ODROIDGOA || GAMEFORCE || RK3326
	// multimedia keys
	auto multimediakeys_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("MULTIMEDIA KEYS"));
	multimediakeys_enabled->add(_("AUTO"), "auto", SystemConf::getInstance()->get("system.multimediakeys.enabled") != "0" && SystemConf::getInstance()->get("system.multimediakeys.enabled") != "1");
	multimediakeys_enabled->add(_("ON"), "1", SystemConf::getInstance()->get("system.multimediakeys.enabled") == "1");
	multimediakeys_enabled->add(_("OFF"), "0", SystemConf::getInstance()->get("system.multimediakeys.enabled") == "0");
	s->addWithLabel(_("MULTIMEDIA KEYS"), multimediakeys_enabled);
	s->addSaveFunc([this, multimediakeys_enabled, s]
	{
		if (multimediakeys_enabled->changed())
		{
			SystemConf::getInstance()->set("system.multimediakeys.enabled", multimediakeys_enabled->getSelected());
			s->setVariable("reboot", true);
		}
	});
#endif

#if GAMEFORCE || RK3326
	auto buttonColor_GameForce = std::make_shared< OptionListComponent<std::string> >(mWindow, _("BUTTON LED COLOR"));
	buttonColor_GameForce->add(_("off"), "off", SystemConf::getInstance()->get("color_rgb") == "off" || SystemConf::getInstance()->get("color_rgb") == "");
	buttonColor_GameForce->add(_("red"), "red", SystemConf::getInstance()->get("color_rgb") == "red");
	buttonColor_GameForce->add(_("green"), "green", SystemConf::getInstance()->get("color_rgb") == "green");
	buttonColor_GameForce->add(_("blue"), "blue", SystemConf::getInstance()->get("color_rgb") == "blue");
	buttonColor_GameForce->add(_("white"), "white", SystemConf::getInstance()->get("color_rgb") == "white");
	buttonColor_GameForce->add(_("purple"), "purple", SystemConf::getInstance()->get("color_rgb") == "purple");
	buttonColor_GameForce->add(_("yellow"), "yellow", SystemConf::getInstance()->get("color_rgb") == "yellow");
	buttonColor_GameForce->add(_("cyan"), "cyan", SystemConf::getInstance()->get("color_rgb") == "cyan");
	s->addWithLabel(_("BUTTON LED COLOR"), buttonColor_GameForce);
	s->addSaveFunc([buttonColor_GameForce]
	{
		if (buttonColor_GameForce->changed()) {
			ApiSystem::getInstance()->setButtonColorGameForce(buttonColor_GameForce->getSelected());
			SystemConf::getInstance()->set("color_rgb", buttonColor_GameForce->getSelected());
		}
	});

	auto powerled_GameForce = std::make_shared< OptionListComponent<std::string> >(mWindow, _("POWER LED COLOR"));
	powerled_GameForce->add(_("heartbeat"), "heartbeat", SystemConf::getInstance()->get("option_powerled") == "heartbeat" || SystemConf::getInstance()->get("option_powerled") == "");
	powerled_GameForce->add(_("off"), "off", SystemConf::getInstance()->get("option_powerled") == "off");
	powerled_GameForce->add(_("on"), "on", SystemConf::getInstance()->get("option_powerled") == "on");
	s->addWithLabel(_("POWER LED COLOR"), powerled_GameForce);
	s->addSaveFunc([powerled_GameForce]
	{
		if (powerled_GameForce->changed()) {
			ApiSystem::getInstance()->setPowerLedGameForce(powerled_GameForce->getSelected());
			SystemConf::getInstance()->set("option_powerled", powerled_GameForce->getSelected());
		}
	});
#endif

	// Overclock choice
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::OVERCLOCK))
	{
		auto overclock_choice = std::make_shared<OptionListComponent<std::string>>(window, _("OVERCLOCK"), false);

		std::string currentOverclock = Settings::getInstance()->getString("Overclock");
		if (currentOverclock == "")
			currentOverclock = "none";

		std::vector<std::string> availableOverclocking = ApiSystem::getInstance()->getAvailableOverclocking();

		// Overclocking device
		bool isOneSet = false;
		for (auto it = availableOverclocking.begin(); it != availableOverclocking.end(); it++)
		{
			std::vector<std::string> tokens = Utils::String::split(*it, ' ');
			if (tokens.size() >= 2)
			{
				// concatenat the ending words
				std::string vname;
				for (unsigned int i = 1; i < tokens.size(); i++)
				{
					if (i > 1) vname += " ";
					vname += tokens.at(i);
				}
				bool isSet = currentOverclock == std::string(tokens.at(0));
				if (isSet)
					isOneSet = true;

				if (vname == "NONE" || vname == "none")
					vname = _("NONE");

				overclock_choice->add(vname, tokens.at(0), isSet);
			}
		}

		if (isOneSet == false)
		{
			if (currentOverclock == "none")
				overclock_choice->add(_("NONE"), currentOverclock, true);
			else
				overclock_choice->add(currentOverclock, currentOverclock, true);
		}

		// overclocking
		s->addWithLabel(_("OVERCLOCK"), overclock_choice);

		s->addSaveFunc([overclock_choice, window, s]
		{
			if (overclock_choice->changed() && Settings::getInstance()->setString("Overclock", overclock_choice->getSelected()))
			{
				ApiSystem::getInstance()->setOverclock(overclock_choice->getSelected());
				s->setVariable("reboot", true);
			}
		});
	}

#ifdef BATOCERA
	s->addGroup(_("STORAGE"));

	// Storage device
	std::vector<std::string> availableStorage = ApiSystem::getInstance()->getAvailableStorageDevices();
	if (availableStorage.size())
	{
		std::string selectedStorage = ApiSystem::getInstance()->getCurrentStorage();

		auto optionsStorage = std::make_shared<OptionListComponent<std::string> >(window, _("STORAGE DEVICE"), false);
		for (auto it = availableStorage.begin(); it != availableStorage.end(); it++)
		{
				if (Utils::String::startsWith(*it, "DEV"))
				{
					std::vector<std::string> tokens = Utils::String::split(*it, ' ');

					if (tokens.size() >= 3) {
						// concatenat the ending words
						std::string vname = "";
						for (unsigned int i = 2; i < tokens.size(); i++) {
							if (i > 2) vname += " ";
							vname += tokens.at(i);
						}
						optionsStorage->add(vname, (*it), selectedStorage == std::string("DEV " + tokens.at(1)));
					}
				} else {
				  std::vector<std::string> tokens = Utils::String::split(*it, ' ');
				  if (tokens.size() == 1) {
					optionsStorage->add((*it), (*it), selectedStorage == (*it));
				  } else {
				    // concatenat the ending words
				    std::string vname = "";
				    for (unsigned int i = 1; i < tokens.size(); i++) {
				      if (i > 1) vname += " ";
				      vname += tokens.at(i);
				    }
				    optionsStorage->add(_(vname.c_str()), tokens.at(0), selectedStorage == tokens.at(0));
				  }
				}
		}

		s->addWithLabel(_("STORAGE DEVICE"), optionsStorage);
		s->addSaveFunc([optionsStorage, selectedStorage, s]
		{
			if (optionsStorage->changed())
			{
				ApiSystem::getInstance()->setStorage(optionsStorage->getSelected());
				s->setVariable("reboot", true);
			}
		});
	}

	// backup
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::BACKUP))
		s->addEntry(_("BACKUP USER DATA"), true, [this] { mWindow->pushGui(new GuiBackupStart(mWindow)); });

	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::INSTALL))
		s->addEntry(_("INSTALL ON A NEW DISK"), true, [this] { mWindow->pushGui(new GuiInstallStart(mWindow)); });

	s->addGroup(_("ADVANCED"));

	// Security
	s->addEntry(_("SECURITY"), true, [this, s]
	{
		GuiSettings *securityGui = new GuiSettings(mWindow, _("SECURITY").c_str());
		auto securityEnabled = std::make_shared<SwitchComponent>(mWindow);
		securityEnabled->setState(SystemConf::getInstance()->get("system.security.enabled") == "1");
		securityGui->addWithDescription(_("ENFORCE SECURITY"), _("Require a password for accessing the network share."), securityEnabled);

		auto rootpassword = std::make_shared<TextComponent>(mWindow, ApiSystem::getInstance()->getRootPassword(), ThemeData::getMenuTheme()->Text.font, ThemeData::getMenuTheme()->Text.color);
		securityGui->addWithLabel(_("ROOT PASSWORD"), rootpassword);

		securityGui->addSaveFunc([this, securityEnabled, s]
		{
			Window* window = this->mWindow;

			if (securityEnabled->changed())
			{
				SystemConf::getInstance()->set("system.security.enabled", securityEnabled->getState() ? "1" : "0");
				SystemConf::getInstance()->saveSystemConf();
				s->setVariable("reboot", true);
			}
		});
		mWindow->pushGui(securityGui);
	});
#else
	if (isFullUI)
		s->addGroup(_("ADVANCED"));
#endif

	// Developer options
	if (isFullUI)
		s->addEntry(_("FRONTEND DEVELOPER OPTIONS"), true, [this] { openDeveloperSettings(); });

	auto pthis = this;
	s->onFinalize([s, pthis, window]
	{
		if (s->getVariable("exitreboot") && Settings::getInstance()->getBool("ExitOnRebootRequired"))
		{
			quitES(QuitMode::QUIT);
			return;
		}

		if (s->getVariable("reboot") || s->getVariable("exitreboot"))
			window->displayNotificationMessage(_U("\uF011  ") + _("REBOOT REQUIRED TO APPLY THE NEW CONFIGURATION"));

		if (s->getVariable("reloadGuiMenu"))
		{
			delete pthis;
			window->pushGui(new GuiMenu(window, false));
		}
	});

	mWindow->pushGui(s);
}

void GuiMenu::openLatencyReductionConfiguration(Window* mWindow, std::string configName)
{
	GuiSettings* guiLatency = new GuiSettings(mWindow, _("LATENCY REDUCTION").c_str());

	// run-ahead
	auto runahead_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("RUN-AHEAD FRAMES"));
	runahead_enabled->addRange({ { _("AUTO"), "" }, { _("NONE"), "0" }, { "1", "1" }, { "2", "2" }, { "3", "3" }, { "4", "4" }, { "5", "5" }, { "6", "6" } }, SystemConf::getInstance()->get(configName + ".runahead"));
	guiLatency->addWithDescription(_("RUN-AHEAD FRAMES"), _("High numbers can result in visible jitter."), runahead_enabled);
	guiLatency->addSaveFunc([configName, runahead_enabled] { SystemConf::getInstance()->set(configName + ".runahead", runahead_enabled->getSelected()); });

	// second instance
	auto secondinstance = std::make_shared<OptionListComponent<std::string>>(mWindow, _("USE SECOND INSTANCE FOR RUN-AHEAD"));
	secondinstance->addRange({ { _("AUTO"), "" }, { _("ON"), "1" }, { _("OFF"), "0" } }, SystemConf::getInstance()->get(configName + ".secondinstance"));
	guiLatency->addWithDescription(_("USE SECOND INSTANCE FOR RUN-AHEAD"), _("Can prevent audio skips on button presses."), secondinstance);
	guiLatency->addSaveFunc([configName, secondinstance] { SystemConf::getInstance()->set(configName + ".secondinstance", secondinstance->getSelected()); });

	// auto frame delay
	auto video_frame_delay_auto = std::make_shared<OptionListComponent<std::string>>(mWindow, _("AUTOMATIC FRAME DELAY"));
	video_frame_delay_auto->addRange({ { _("AUTO"), "" }, { _("ON"), "1" }, { _("OFF"), "0" } }, SystemConf::getInstance()->get(configName + ".video_frame_delay_auto"));
	guiLatency->addWithDescription(_("AUTO FRAME DELAY"), _("Automatically decrease frame delay temporarily to prevent frame drops. Can introduce stuttering."), video_frame_delay_auto);
	guiLatency->addSaveFunc([configName, video_frame_delay_auto] { SystemConf::getInstance()->set(configName + ".video_frame_delay_auto", video_frame_delay_auto->getSelected()); });

	// variable refresh rate (freesync, gsync, etc.)
	auto vrr_runloop_enable = std::make_shared<OptionListComponent<std::string>>(mWindow, _("VARIABLE REFRESH RATE (G-SYNC, FREESYNC)"));
	vrr_runloop_enable->addRange({ { _("AUTO"), "" }, { _("ON"), "1" }, { _("OFF"), "0" } }, SystemConf::getInstance()->get(configName + ".vrr_runloop_enable"));
	guiLatency->addWithDescription(_("VARIABLE REFRESH RATE"), _("Don't deviate from core requested timing. G-Sync, FreeSync, HDMI 2.1 VRR."), vrr_runloop_enable);
	guiLatency->addSaveFunc([configName, vrr_runloop_enable] { SystemConf::getInstance()->set(configName + ".vrr_runloop_enable", vrr_runloop_enable->getSelected()); });

	mWindow->pushGui(guiLatency);
}

void GuiMenu::openRetroachievementsSettings()
{
	mWindow->pushGui(new GuiRetroAchievementsSettings(mWindow));
}

void GuiMenu::openNetplaySettings()
{
	mWindow->pushGui(new GuiNetPlaySettings(mWindow));
}

void GuiMenu::addDecorationSetOptionListComponent(Window* window, GuiSettings* parentWindow, const std::vector<DecorationSetInfo>& sets, const std::string& configName)
{
	auto decorations = std::make_shared<OptionListComponent<std::string> >(window, _("DECORATION SET"), false);
	decorations->setRowTemplate([window, sets](std::string data, ComponentListRow& row) { createDecorationItemTemplate(window, sets, data, row); });

	std::vector<std::string> items = { _("AUTO"), _("NONE") };
	for (auto set : sets)
		items.push_back(set.name);

	std::string bezel = SystemConf::getInstance()->get(configName + ".bezel");

	for (auto item : items)
		decorations->add(item, item, (bezel == item) || (bezel == "none" && item == _("NONE")) || (bezel == "" && item == _("AUTO")));

	if (!decorations->hasSelection())
		decorations->selectFirstItem();

	parentWindow->addWithLabel(_("DECORATION SET"), decorations);
	parentWindow->addSaveFunc([decorations, configName]
	{
		SystemConf::getInstance()->set(configName + ".bezel", decorations->getSelected() == _("NONE") ? "none" : decorations->getSelected() == _("AUTO") ? "" : decorations->getSelected());
	});
}



void GuiMenu::addFeatureItem(Window* window, GuiSettings* settings, const CustomFeature& feat, const std::string& configName, const std::string& system, const std::string& emulator, const std::string& core)
{
	if (feat.preset == "hidden")
		return;

	std::string storageName = configName + "." + feat.value;

	if (configName == "global" && Utils::String::startsWith(feat.value, configName + "."))
		storageName = feat.value;

	if (feat.preset == "input")
	{
		settings->addInputTextRow(pgettext("game_options", feat.name.c_str()), storageName, false);
		return;
	}

	if (feat.preset == "password")
	{
		settings->addInputTextRow(pgettext("game_options", feat.name.c_str()), storageName, true);
		return;
	}

	if (feat.preset == "image")
	{
		settings->addFileBrowser(pgettext("game_options", feat.name.c_str()), storageName, GuiFileBrowser::IMAGES);
		return;
	}

	if (feat.preset == "video")
	{
		settings->addFileBrowser(pgettext("game_options", feat.name.c_str()), storageName, GuiFileBrowser::VIDEO);
		return;
	}

	if (feat.preset == "folder")
	{
		settings->addFileBrowser(pgettext("game_options", feat.name.c_str()), storageName, GuiFileBrowser::DIRECTORY);
		return;
	}

	if (feat.preset == "document")
	{
		settings->addFileBrowser(pgettext("game_options", feat.name.c_str()), storageName, GuiFileBrowser::MANUALS);
		return;
	}

	std::string storedValue = SystemConf::getInstance()->get(storageName);

	std::string inheritedValue;
	if (!Utils::String::startsWith(storageName, "global."))
	{
		std::string systemSetting = storageName;

		bool querySystemSetting = false;

		// Look if we are using a "per-game" setting, then compute the system setting name
		auto gameInfoStart = storageName.find("[\"");
		if (gameInfoStart != std::string::npos)
		{
			auto gameInfoEnd = storageName.find("\"]");
			if (gameInfoEnd != std::string::npos)
			{
				systemSetting = storageName.substr(0, gameInfoStart) + storageName.substr(gameInfoEnd + 2);
				querySystemSetting = true;
			}
		}

		// First find the global option
		auto dotPos = systemSetting.find(".");
		if (dotPos != std::string::npos)
		{
			std::string globalSetting = "global." + systemSetting.substr(dotPos + 1);
			std::string globalStoredValue = SystemConf::getInstance()->get(globalSetting);
			if (!globalStoredValue.empty() && globalStoredValue != "auto" && globalStoredValue != storedValue)
				inheritedValue = globalStoredValue;
		}

		// Then take the system option
		if (querySystemSetting)
		{
			std::string systemStoredValue = SystemConf::getInstance()->get(systemSetting);
			if (!systemStoredValue.empty() && systemStoredValue != "auto" && systemStoredValue != storedValue)
				inheritedValue = systemStoredValue;
		}
	}

	if (feat.preset == "switch")
	{
		auto switchComponent = std::make_shared<SwitchComponent>(window);
		switchComponent->setState(storedValue == "1");

		if (!feat.description.empty())
			settings->addWithDescription(pgettext("game_options", feat.name.c_str()), pgettext("game_options", feat.description.c_str()), switchComponent);
		else
			settings->addWithLabel(pgettext("game_options", feat.name.c_str()), switchComponent);

		settings->addSaveFunc([storageName, switchComponent] { SystemConf::getInstance()->set(storageName, switchComponent->getState() ? "1" : ""); });
		return;
	}

	if (feat.preset == "switchon")
	{
		auto switchComponent = std::make_shared<SwitchComponent>(window);
		switchComponent->setState(storedValue != "0");

		if (!feat.description.empty())
			settings->addWithDescription(pgettext("game_options", feat.name.c_str()), pgettext("game_options", feat.description.c_str()), switchComponent);
		else
			settings->addWithLabel(pgettext("game_options", feat.name.c_str()), switchComponent);

		settings->addSaveFunc([storageName, switchComponent] { SystemConf::getInstance()->set(storageName, switchComponent->getState() ? "" : "0"); });
		return;
	}

	if (feat.preset == "switchoff")
	{
		auto switchComponent = std::make_shared<SwitchComponent>(window);
		switchComponent->setState(storedValue != "1");

		if (!feat.description.empty())
			settings->addWithDescription(pgettext("game_options", feat.name.c_str()), pgettext("game_options", feat.description.c_str()), switchComponent);
		else
			settings->addWithLabel(pgettext("game_options", feat.name.c_str()), switchComponent);

		settings->addSaveFunc([storageName, switchComponent] { SystemConf::getInstance()->set(storageName, switchComponent->getState() ? "" : "1"); });
		return;
	}

	auto item = std::make_shared<OptionListComponent<std::string>>(window, pgettext("game_options", feat.name.c_str()));

	if (feat.preset == "shaders" || feat.preset == "shaderset")
	{
		item->add(_("AUTO"), "auto", storedValue.empty() || storedValue == "auto");

		auto shaders = ApiSystem::getInstance()->getShaderList(configName != "global" ? system : "", configName != "global" ? emulator : "", configName != "global" ? core : "");
		if (shaders.size() > 0)
		{
			item->add(_("NONE"), "none", storedValue == "none");

			for (auto shader : shaders)
			  item->add(pgettext("game_options", Utils::String::toUpper(shader).c_str()), shader, storedValue == shader);
		}
	}
	else if (feat.preset == "decorations" || feat.preset == "bezel")
	{
		item->add(_("AUTO"), "auto", storedValue.empty() || storedValue == "auto");

		auto sets = GuiMenu::getDecorationsSets(ViewController::get()->getState().getSystem());
		if (sets.size() > 0)
		{
			item->setRowTemplate([window, sets](std::string data, ComponentListRow& row) { createDecorationItemTemplate(window, sets, data, row); });
			item->add(_("NONE"), "none", storedValue == "none");

			for (auto set : sets)
				item->add(set.name, set.name, storedValue == set.name);
		}
	}
	else if (feat.preset == "videomodes" || feat.preset == "videomode")
	{
		item->add(_("AUTO"), "auto", storedValue.empty() || storedValue == "auto");

		auto modes = ApiSystem::getInstance()->getVideoModes();
		for (auto videoMode : modes)
		{
			std::vector<std::string> tokens = Utils::String::split(videoMode, ':');
			if (tokens.size() == 0)
				continue;

			std::string vname;
			for (unsigned int i = 1; i < tokens.size(); i++)
			{
				if (i > 1)
					vname += ":";

				vname += tokens.at(i);
			}

			item->add(_(vname.c_str()), tokens.at(0), storedValue == tokens.at(0));
		}
	}
	else
	{
		item->add(_("AUTO"), "", storedValue.empty() || storedValue == "auto");

		for (auto fval : feat.choices)
			item->add(pgettext("game_options", fval.name.c_str()), fval.value, storedValue == fval.value);
	}

	if (!item->hasSelection())
		item->selectFirstItem();

	std::string desc = pgettext("game_options", feat.description.c_str());

	if (!inheritedValue.empty())
	{
		auto displayName = item->getItemDisplayName(inheritedValue);
		if (!displayName.empty())
		{
			if (desc.empty())
				desc = _("Current setting") + " : " + displayName;
			else
				desc = desc + "\r\n" + _("Current setting") + " : " + displayName;
		}
	}

	if (!desc.empty())
		settings->addWithDescription(pgettext("game_options", feat.name.c_str()), desc, item);
	else
		settings->addWithLabel(pgettext("game_options", feat.name.c_str()), item);

	settings->addSaveFunc([item, storageName] { SystemConf::getInstance()->set(storageName, item->getSelected()); });
}

static bool hasGlobalFeature(const std::string& name)
{
	return CustomFeatures::GlobalFeatures.hasGlobalFeature(name);
}

void GuiMenu::addFeatures(const VectorEx<CustomFeature>& features, Window* window, GuiSettings* settings, const std::string& configName, const std::string& system, const std::string& emulator, const std::string& core, const std::string& defaultGroupName, bool addDefaultGroupOnlyIfNotFirst)
{
	bool firstGroup = true;

	auto groups = features.groupBy([](auto x) { return x.group; });
	for (auto group : groups)
	{
		settings->removeLastRowIfGroup();

		if (!group.first.empty())
			settings->addGroup(pgettext("game_options", group.first.c_str()));
		else if (!defaultGroupName.empty())
		{
			if (!addDefaultGroupOnlyIfNotFirst || !firstGroup)
				settings->addGroup(defaultGroupName); // _("DEFAULT GLOBAL SETTINGS")
		}

		firstGroup = false;

		std::set<std::string> processed;

		for (auto feat : group.second)
		{
			if (feat.submenu.empty())
			{
				addFeatureItem(window, settings, feat, configName, system, emulator, core);
				continue;
			}

			if (processed.find(feat.submenu) != processed.cend())
				continue;

			processed.insert(feat.submenu);

			auto items = features.where([feat](auto x) { return x.preset != "hidden" && x.submenu == feat.submenu; });
			if (items.size() > 0)
			{
				settings->addEntry(pgettext("game_options", feat.submenu.c_str()), true, [window, configName, feat, items, system, emulator, core]
				{
					GuiSettings* groupSettings = new GuiSettings(window, pgettext("game_options", feat.submenu.c_str()));

					for (auto feat : items)
						addFeatureItem(window, groupSettings, feat, configName, system, emulator, core);

					window->pushGui(groupSettings);
				});
			}
		}
	}
}

void GuiMenu::openGamesSettings()
{
	Window* window = mWindow;

	auto s = new GuiSettings(mWindow, _("GAME SETTINGS").c_str());

	s->addGroup(_("TOOLS"));

	// Game List Update
	s->addEntry(_("UPDATE GAMELISTS"), false, [this, window] { updateGameLists(window); });

	if (SystemConf::getInstance()->getBool("global.retroachievements") && !Settings::getInstance()->getBool("RetroachievementsMenuitem") && SystemConf::getInstance()->get("global.retroachievements.username") != "")
	{
		s->addEntry(_("RETROACHIEVEMENTS").c_str(), true, [this]
		{
			if (!checkNetwork())
				return;

			GuiRetroAchievements::show(mWindow);
		});
	}


	s->addGroup(_("DEFAULT GLOBAL SETTINGS"));

	// Screen ratio choice
	if (!hasGlobalFeature("ratio"))
	{
		auto ratio_choice = createRatioOptionList(mWindow, "global");
		s->addWithDescription(_("GAME ASPECT RATIO"), _("Force the game to render in this aspect ratio."), ratio_choice);
		s->addSaveFunc([ratio_choice] { SystemConf::getInstance()->set("global.ratio", ratio_choice->getSelected()); });
	}
#ifndef _ENABLEEMUELEC
	// video resolution mode
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::RESOLUTION) && !hasGlobalFeature("videomode"))
	{
		auto videoModeOptionList = createVideoResolutionModeOptionList(mWindow, "global");
		s->addWithDescription(_("VIDEO MODE"), _("Sets the display's resolution. Does not affect the rendering resolution."), videoModeOptionList);
		s->addSaveFunc([this, videoModeOptionList] { SystemConf::getInstance()->set("global.videomode", videoModeOptionList->getSelected()); });
	}
#endif

	// smoothing
	if (!hasGlobalFeature("smooth"))
	{
		auto smoothing_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("SMOOTH GAMES (BILINEAR FILTERING)"));
		smoothing_enabled->addRange({ { _("AUTO"), "auto" },{ _("ON") , "1" },{ _("OFF") , "0" } }, SystemConf::getInstance()->get("global.smooth"));
		s->addWithLabel(_("SMOOTH GAMES (BILINEAR FILTERING)"), smoothing_enabled);
		s->addSaveFunc([smoothing_enabled] { SystemConf::getInstance()->set("global.smooth", smoothing_enabled->getSelected()); });
	}
#ifdef _ENABLEEMUELEC
	// bezel
	auto bezel_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("ENABLE RA BEZELS"));
	bezel_enabled->add(_("AUTO"), "auto", SystemConf::getInstance()->get("global.bezel") != "0" && SystemConf::getInstance()->get("global.bezel") != "1");
	bezel_enabled->add(_("ON"), "1", SystemConf::getInstance()->get("global.bezel") == "1");
	bezel_enabled->add(_("OFF"), "0", SystemConf::getInstance()->get("global.bezel") == "0");
	s->addWithLabel(_("ENABLE RA BEZELS"), bezel_enabled);
    s->addSaveFunc([bezel_enabled] { SystemConf::getInstance()->set("global.bezel", bezel_enabled->getSelected()); });

	//maxperf
	auto maxperf_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("ENABLE MAX PERFORMANCE"));
	maxperf_enabled->add(_("ON"), "1", SystemConf::getInstance()->get("global.maxperf") == "1" || SystemConf::getInstance()->get("global.maxperf") != "0");
	maxperf_enabled->add(_("OFF"), "0", SystemConf::getInstance()->get("global.maxperf") == "0");
	s->addWithLabel(_("ENABLE MAX PERFORMANCE"), maxperf_enabled);
    s->addSaveFunc([maxperf_enabled] { SystemConf::getInstance()->set("global.maxperf", maxperf_enabled->getSelected()); });
#endif

	// rewind
	if (!hasGlobalFeature("rewind"))
	{
		auto rewind_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("REWIND"));
		rewind_enabled->addRange({ { _("AUTO"), "auto" },{ _("ON") , "1" },{ _("OFF") , "0" } }, SystemConf::getInstance()->get("global.rewind"));
		s->addWithDescription(_("REWIND"), _("Store past states to rewind to in realtime, if the core supports it."), rewind_enabled);
		s->addSaveFunc([rewind_enabled] { SystemConf::getInstance()->set("global.rewind", rewind_enabled->getSelected()); });
	}

	// Integer scale
	if (!hasGlobalFeature("integerscale"))
	{
		auto integerscale_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("INTEGER SCALING (PIXEL PERFECT)"));
		integerscale_enabled->addRange({ { _("AUTO"), "auto" },{ _("ON") , "1" },{ _("OFF") , "0" } }, SystemConf::getInstance()->get("global.integerscale"));
		s->addWithLabel(_("INTEGER SCALING (PIXEL PERFECT)"), integerscale_enabled);
		s->addSaveFunc([integerscale_enabled] { SystemConf::getInstance()->set("global.integerscale", integerscale_enabled->getSelected()); });
	}

	// Integer scale overscale
	auto integerscaleoverscale_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("INTEGER SCALING (OVERSCALE)"));
	integerscaleoverscale_enabled->addRange({ { _("AUTO"), "auto" },{ _("ON") , "1" },{ _("OFF") , "0" } }, SystemConf::getInstance()->get("global.integerscaleoverscale"));
	s->addWithLabel(_("INTEGER SCALING (OVERSCALE)"), integerscaleoverscale_enabled);
	s->addSaveFunc([integerscaleoverscale_enabled] { SystemConf::getInstance()->set("global.integerscaleoverscale", integerscaleoverscale_enabled->getSelected()); });

	// Shaders preset
#ifndef _ENABLEEMUELEC
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::SHADERS) && !hasGlobalFeature("shaderset"))
	{
		auto installedShaders = ApiSystem::getInstance()->getShaderList("", "", "");
		if (installedShaders.size() > 0)
		{
#endif
			std::string currentShader = SystemConf::getInstance()->get("global.shaderset");

			auto shaders_choices = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SHADER SET"), false);
			shaders_choices->add(_("AUTO"), "auto", currentShader.empty() || currentShader == "auto");
			shaders_choices->add(_("NONE"), "none", currentShader == "none");

#ifdef _ENABLEEMUELEC
	std::string a;
	for(std::stringstream ss(getShOutput(R"(/usr/bin/emuelec-utils getshaders)")); getline(ss, a, ','); )
		shaders_choices->add(a, a, currentShader == a); // emuelec
#else
			for (auto shader : installedShaders)
				shaders_choices->add(_(Utils::String::toUpper(shader).c_str()), shader, currentShader == shader);

			if (!shaders_choices->hasSelection())
				shaders_choices->selectFirstItem();

#endif
			s->addWithLabel(_("SHADER SET"), shaders_choices);
			s->addSaveFunc([shaders_choices] { SystemConf::getInstance()->set("global.shaderset", shaders_choices->getSelected()); });
#ifndef _ENABLEEMUELEC
        }
	}
#endif

#ifdef _ENABLEEMUELEC
#if defined(ODROIDGOA) || defined(_ENABLEGAMEFORCE)
	// RGA SCALING
	auto rgascale_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("RGA SCALING"));
	rgascale_enabled->addRange({ { _("AUTO"), "auto" },{ _("ON") , "1" },{ _("OFF") , "0" } }, SystemConf::getInstance()->get("global.rgascale"));
	s->addWithLabel(_("RGA SCALING"), rgascale_enabled);
	s->addSaveFunc([rgascale_enabled] { SystemConf::getInstance()->set("global.rgascale", rgascale_enabled->getSelected()); });
#endif
#endif
#ifndef _ENABLEEMUELEC
	// decorations
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::DECORATIONS) && !hasGlobalFeature("bezel"))
	{
		auto sets = GuiMenu::getDecorationsSets(ViewController::get()->getState().getSystem());
		if (sets.size() > 0)
		{
#ifdef BATOCERA
			s->addEntry(_("DECORATIONS"), true, [this, sets]
			{
				GuiSettings *decorations_window = new GuiSettings(mWindow, _("DECORATIONS").c_str());

				addDecorationSetOptionListComponent(mWindow, decorations_window, sets);

				// stretch bezels
				auto bezel_stretch_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("STRETCH BEZELS (4K & ULTRAWIDE)"));
				bezel_stretch_enabled->add(_("AUTO"), "auto", SystemConf::getInstance()->get("global.bezel_stretch") != "0" && SystemConf::getInstance()->get("global.bezel_stretch") != "1");
				bezel_stretch_enabled->add(_("ON"), "1", SystemConf::getInstance()->get("global.bezel_stretch") == "1");
				bezel_stretch_enabled->add(_("OFF"), "0", SystemConf::getInstance()->get("global.bezel_stretch") == "0");
				decorations_window->addWithLabel(_("STRETCH BEZELS (4K & ULTRAWIDE)"), bezel_stretch_enabled);
				decorations_window->addSaveFunc([bezel_stretch_enabled] {
						if (bezel_stretch_enabled->changed()) {
						SystemConf::getInstance()->set("global.bezel_stretch", bezel_stretch_enabled->getSelected());
						SystemConf::getInstance()->saveSystemConf();
						}
						});

				// tattoo and controller overlays
				auto bezel_tattoo = std::make_shared<OptionListComponent<std::string>>(mWindow, _("SHOW TATTOO OVER BEZEL"));
				bezel_tattoo->add(_("AUTO"), "auto", SystemConf::getInstance()->get("global.bezel.tattoo") != "0"
						&& SystemConf::getInstance()->get("global.bezel.tattoo") != "system"
						&& SystemConf::getInstance()->get("global.bezel.tattoo") != "custom");
				bezel_tattoo->add(_("NO"), "0", SystemConf::getInstance()->get("global.bezel.tattoo") == "0");
				bezel_tattoo->add(_("SYSTEM CONTROLLERS"), "system", SystemConf::getInstance()->get("global.bezel.tattoo") == "system");
				bezel_tattoo->add(_("CUSTOM IMAGE"), "custom", SystemConf::getInstance()->get("global.bezel.tattoo") == "custom");
				decorations_window->addWithDescription(_("SHOW TATTOO OVER BEZEL"), _("Show an image overlaid on top of the bezel."), bezel_tattoo);
				decorations_window->addSaveFunc([bezel_tattoo] {
						if (bezel_tattoo->changed()) {
						SystemConf::getInstance()->set("global.bezel.tattoo", bezel_tattoo->getSelected());
						SystemConf::getInstance()->saveSystemConf();
						}
						});

				auto bezel_tattoo_corner = std::make_shared<OptionListComponent<std::string>>(mWindow, _("TATTOO CORNER"));
				bezel_tattoo_corner->add(_("AUTO"), "auto", SystemConf::getInstance()->get("global.bezel.tattoo_corner") != "NW"
						&& SystemConf::getInstance()->get("global.bezel.tattoo_corner") != "NE"
						&& SystemConf::getInstance()->get("global.bezel.tattoo_corner") != "SE"
						&& SystemConf::getInstance()->get("global.bezel.tattoo_corner") != "SW");
				bezel_tattoo_corner->add(_("NORTH WEST"), "NW", SystemConf::getInstance()->get("global.bezel.tattoo_corner") == "NW");
				bezel_tattoo_corner->add(_("NORTH EAST"), "NE", SystemConf::getInstance()->get("global.bezel.tattoo_corner") == "NE");
				bezel_tattoo_corner->add(_("SOUTH EAST"), "SE", SystemConf::getInstance()->get("global.bezel.tattoo_corner") == "SE");
				bezel_tattoo_corner->add(_("SOUTH WEST"), "SW", SystemConf::getInstance()->get("global.bezel.tattoo_corner") == "SW");
				decorations_window->addWithLabel(_("TATTOO CORNER"), bezel_tattoo_corner);
				decorations_window->addSaveFunc([bezel_tattoo_corner] {
						if (bezel_tattoo_corner->changed()) {
						SystemConf::getInstance()->set("global.bezel.tattoo_corner", bezel_tattoo_corner->getSelected());
						SystemConf::getInstance()->saveSystemConf();
						}
						});
				decorations_window->addInputTextRow(_("CUSTOM .PNG IMAGE PATH"), "global.bezel.tattoo_file", false);

				auto bezel_resize_tattoo = std::make_shared<SwitchComponent>(mWindow);
				bezel_resize_tattoo->setState(SystemConf::getInstance()->getBool("global.bezel.resize_tattoo"));
				decorations_window->addWithDescription(_("RESIZE TATTOO"), _("Shrink/expand tattoo to fit within the bezel's border."), bezel_resize_tattoo);
				decorations_window->addSaveFunc([bezel_resize_tattoo]
				{
					if (SystemConf::getInstance()->getBool("global.bezel.resize_tattoo") != bezel_resize_tattoo->getState()) {
						SystemConf::getInstance()->setBool("global.bezel.resize_tattoo", bezel_resize_tattoo->getState());
					}
				});

				mWindow->pushGui(decorations_window);
			});
#else
			addDecorationSetOptionListComponent(mWindow, s, sets);
#endif
		}
	}
#endif
	// latency reduction
	if (!hasGlobalFeature("runahead"))
		s->addEntry(_("LATENCY REDUCTION"), true, [this] { openLatencyReductionConfiguration(mWindow, "global"); });

	//AI-enabled translations
	if (!hasGlobalFeature("ai_service_enabled"))
	{
		s->addEntry(_("AI GAME TRANSLATION"), true, [this]
		{
			GuiSettings *ai_service = new GuiSettings(mWindow, _("AI GAME TRANSLATION").c_str());

			// AI service enabled?
			auto ai_service_enabled = std::make_shared<SwitchComponent>(mWindow);
			ai_service_enabled->setState(
				SystemConf::getInstance()->get("global.ai_service_enabled") == "1");
			ai_service->addWithLabel(_("ENABLE AI TRANSLATION SERVICE"), ai_service_enabled);

			// Target language - order is: popular languages in the Batocera community first
			// then alphabetical order of the 2-char lang code (because the strings are localized)
			auto lang_choices = std::make_shared<OptionListComponent<std::string> >(mWindow,
				_("TARGET LANGUAGE"), false);
			std::string currentLang = SystemConf::getInstance()->get("global.ai_target_lang");
			if (currentLang.empty())
				currentLang = std::string("En");
			lang_choices->add("ENGLISH", "En", currentLang == "En");
			lang_choices->add("FRANÇAIS", "Fr", currentLang == "Fr");
			lang_choices->add("PORTUGUÊS", "Pt", currentLang == "Pt");
			lang_choices->add("DEUTSCH", "De", currentLang == "De");
			lang_choices->add("GREEK", "El", currentLang == "El");
			lang_choices->add("ESPAÑOL", "Es", currentLang == "Es");
			lang_choices->add("CZECH", "Cs", currentLang == "Cs");
			lang_choices->add("DANISH", "Da", currentLang == "Da");
			lang_choices->add("CROATIAN", "Hr", currentLang == "Hr");
			lang_choices->add("HUNGARIAN", "Hu", currentLang == "Hu");
			lang_choices->add("ITALIANO", "It", currentLang == "It");
			lang_choices->add("JAPANESE", "Ja", currentLang == "Ja");
			lang_choices->add("KOREAN", "Ko", currentLang == "Ko");
			lang_choices->add("DUTCH", "Nl", currentLang == "Nl");
			lang_choices->add("NORWEGIAN", "Nn", currentLang == "Nn");
			lang_choices->add("POLISH", "Po", currentLang == "Po");
			lang_choices->add("ROMANIAN", "Ro", currentLang == "Ro");
			lang_choices->add("РУССКИЙ", "Ru", currentLang == "Ru");
			lang_choices->add("SVENSKA", "Sv", currentLang == "Sv");
			lang_choices->add("TÜRKÇE", "Tr", currentLang == "Tr");
			lang_choices->add("简体中文", "Zh", currentLang == "Zh");
			ai_service->addWithLabel(_("TARGET LANGUAGE"), lang_choices);

			// Service  URL
			ai_service->addInputTextRow(_("AI TRANSLATION SERVICE URL"), "global.ai_service_url", false);

			// Pause game for translation?
			auto ai_service_pause = std::make_shared<SwitchComponent>(mWindow);
			ai_service_pause->setState(
				SystemConf::getInstance()->get("global.ai_service_pause") == "1");
			ai_service->addWithLabel(_("PAUSE ON TRANSLATED SCREEN"), ai_service_pause);

			ai_service->addSaveFunc([ai_service_enabled, lang_choices, ai_service_pause] {
				if (ai_service_enabled->changed())
					SystemConf::getInstance()->set("global.ai_service_enabled",
						ai_service_enabled->getState() ? "1" : "0");
				if (lang_choices->changed())
					SystemConf::getInstance()->set("global.ai_target_lang",
						lang_choices->getSelected());
				if (ai_service_pause->changed())
					SystemConf::getInstance()->set("global.ai_service_pause",
						ai_service_pause->getState() ? "1" : "0");
				SystemConf::getInstance()->saveSystemConf();
			});

			mWindow->pushGui(ai_service);
		});
	}

	// Load global custom features
	addFeatures(CustomFeatures::GlobalFeatures, window, s, "global", "", "", "", _("DEFAULT GLOBAL SETTINGS"));

	if (!hasGlobalFeature("disableautocontrollers") && SystemData::sSystemVector.any([](auto sys) { return !sys->getCompatibleCoreNames(EmulatorFeatures::autocontrollers).empty(); }))
	{
		auto autoControllers = std::make_shared<SwitchComponent>(mWindow);
		autoControllers->setState(SystemConf::getInstance()->get("global.disableautocontrollers") != "1");
		s->addWithLabel(_("AUTOCONFIGURE CONTROLLERS"), autoControllers);
		s->addSaveFunc([autoControllers] { SystemConf::getInstance()->set("global.disableautocontrollers", autoControllers->getState() ? "" : "1"); });
	}

	// Custom config for systems
	s->addGroup(_("SAVESTATES"));

	// AUTO SAVE/LOAD
	auto autosave_enabled = std::make_shared<SwitchComponent>(mWindow);
	autosave_enabled->setState(SystemConf::getInstance()->get("global.autosave") == "1");
	s->addWithDescription(_("AUTO SAVE/LOAD"), _("Load latest savestate on game launch and savestate when exiting game."), autosave_enabled);
	s->addSaveFunc([autosave_enabled] { SystemConf::getInstance()->set("global.autosave", autosave_enabled->getState() ? "1" : ""); });

	// INCREMENTAL SAVESTATES
	auto incrementalSaveStates = std::make_shared<OptionListComponent<std::string>>(mWindow, _("INCREMENTAL SAVESTATES"));
	incrementalSaveStates->addRange({
		{ _("INCREMENT PER SAVE"), _("Never overwrite old savestates, always make new ones."), "" }, // Don't use 1 -> 1 is YES, auto too
		{ _("INCREMENT SLOT"), _("Increment slot on a new game."), "0" },
		{ _("DO NOT INCREMENT"), _("Use current slot on a new game."), "2" } },
		SystemConf::getInstance()->get("global.incrementalsavestates"));

	s->addWithLabel(_("INCREMENTAL SAVESTATES"), incrementalSaveStates);
	s->addSaveFunc([incrementalSaveStates] { SystemConf::getInstance()->set("global.incrementalsavestates", incrementalSaveStates->getSelected()); });

	// SHOW SAVE STATES
	auto showSaveStates = std::make_shared<OptionListComponent<std::string>>(mWindow, _("SHOW SAVESTATE MANAGER"));
	showSaveStates->addRange({ { _("NO"), "auto" },{ _("ALWAYS") , "1" },{ _("IF NOT EMPTY") , "2" } }, SystemConf::getInstance()->get("global.savestates"));
	s->addWithDescription(_("SHOW SAVESTATE MANAGER"), _("Display savestate manager before launching a game."), showSaveStates);
	s->addSaveFunc([showSaveStates] { SystemConf::getInstance()->set("global.savestates", showSaveStates->getSelected()); });


	s->addGroup(_("SYSTEM SETTINGS"));

	// Custom config for systems
	s->addEntry(_("PER SYSTEM ADVANCED CONFIGURATION"), true, [this, s, window]
	{
		s->save();
		GuiSettings* configuration = new GuiSettings(window, _("PER SYSTEM ADVANCED CONFIGURATION").c_str());

		// For each activated system
		std::vector<SystemData *> systems = SystemData::sSystemVector;
		for (auto system : systems)
		{
			if (system->isCollection() || !system->isGameSystem())
				continue;

			if (system->hasPlatformId(PlatformIds::PLATFORM_IGNORE))
				continue;

			if (!system->hasFeatures() && !system->hasEmulatorSelection())
				continue;

			configuration->addEntry(system->getFullName(), true, [this, system, window] {
				popSystemConfigurationGui(window, system);
			});
		}

		window->pushGui(configuration);
	});

	// Retroachievements
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::RETROACHIVEMENTS))
		s->addEntry(_("RETROACHIEVEMENT SETTINGS"), true, [this] { openRetroachievementsSettings(); });

	// Netplay
	if (SystemData::isNetplayActivated() && ApiSystem::getInstance()->isScriptingSupported(ApiSystem::NETPLAY))
		s->addEntry(_("NETPLAY SETTINGS"), true, [this] { openNetplaySettings(); }, "iconNetplay");

	// Missing Bios
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::BIOSINFORMATION))
	{
		s->addEntry(_("MISSING BIOS CHECK"), true, [this, s] { openMissingBiosSettings(); });

#ifndef _ENABLEEMUELEC
		auto checkBiosesAtLaunch = std::make_shared<SwitchComponent>(mWindow);
		checkBiosesAtLaunch->setState(Settings::getInstance()->getBool("CheckBiosesAtLaunch"));
		s->addWithLabel(_("CHECK BIOS FILES BEFORE RUNNING A GAME"), checkBiosesAtLaunch);
		s->addSaveFunc([checkBiosesAtLaunch] { Settings::getInstance()->setBool("CheckBiosesAtLaunch", checkBiosesAtLaunch->getState()); });
#endif
	}
	mWindow->pushGui(s);
}

void GuiMenu::openMissingBiosSettings()
{
	GuiBios::show(mWindow);
}

void GuiMenu::updateGameLists(Window* window, bool confirm)
{
	if (ThreadedScraper::isRunning())
	{
		window->pushGui(new GuiMsgBox(window, _("SCRAPER IS RUNNING. DO YOU WANT TO STOP IT?"),
			_("YES"), [] { ThreadedScraper::stop(); },
			_("NO"), nullptr));

		return;
	}

	if (ThreadedHasher::isRunning())
	{
		window->pushGui(new GuiMsgBox(window, _("GAME HASHING IS RUNNING. DO YOU WANT TO STOP IT?"),
			_("YES"), [] { ThreadedHasher::stop(); },
			_("NO"), nullptr));

		return;
	}

	if (!confirm)
	{
		ViewController::reloadAllGames(window, true, true);
		return;
	}

	window->pushGui(new GuiMsgBox(window, _("REALLY UPDATE GAMELISTS?"), _("YES"), [window]
		{
		ViewController::reloadAllGames(window, true, true);
		},
		_("NO"), nullptr));
}

void GuiMenu::openSystemEmulatorSettings(SystemData* system)
{
	auto theme = ThemeData::getMenuTheme();

	GuiSettings* s = new GuiSettings(mWindow, system->getFullName().c_str());

	auto emul_choice = std::make_shared<OptionListComponent<std::string>>(mWindow, _("Emulator"), false);
	auto core_choice = std::make_shared<OptionListComponent<std::string>>(mWindow, _("Core"), false);

	std::string currentEmul = system->getEmulator(false);
	std::string defaultEmul = system->getDefaultEmulator();

	emul_choice->add(_("AUTO"), "", false);

	bool found = false;
	for (auto emul : system->getEmulators())
	{
		if (emul.name == currentEmul)
			found = true;

		emul_choice->add(emul.name, emul.name, emul.name == currentEmul);
	}

	if (!found)
		emul_choice->selectFirstItem();

	ComponentListRow row;
	row.addElement(std::make_shared<TextComponent>(mWindow, Utils::String::toUpper(_("Emulator")), theme->Text.font, theme->Text.color), true);
	row.addElement(emul_choice, false);

	s->addRow(row);

	emul_choice->setSelectedChangedCallback([this, system, core_choice](std::string emulatorName)
	{
		std::string currentCore = system->getCore(false);
		std::string defaultCore = system->getDefaultCore(emulatorName);

		core_choice->clear();
		core_choice->add(_("AUTO"), "", false);

		bool found = false;

		for (auto& emulator : system->getEmulators())
		{
			if (emulatorName != emulator.name)
				continue;

			for (auto core : emulator.cores)
			{
				core_choice->add(core.name, core.name, currentCore == core.name);
				if (currentCore == core.name)
					found = true;
			}
		}

		if (!found)
			core_choice->selectFirstItem();
		else
			core_choice->invalidate();
	});

	row.elements.clear();
	row.addElement(std::make_shared<TextComponent>(mWindow, Utils::String::toUpper(_("Core")), theme->Text.font, theme->Text.color), true);
	row.addElement(core_choice, false);
	s->addRow(row);

	// force change event to load core list
	emul_choice->invalidate();


	s->addSaveFunc([system, emul_choice, core_choice]
	{
		Settings::getInstance()->setString(system->getName() + ".emulator", emul_choice->getSelected());
		Settings::getInstance()->setString(system->getName() + ".core", core_choice->getSelected());
	});

	mWindow->pushGui(s);
}

void GuiMenu::openEmulatorSettings()
{
	GuiSettings* configuration = new GuiSettings(mWindow, _("EMULATOR SETTINGS").c_str());

	Window* window = mWindow;

	// For each activated system
	for (auto system : SystemData::sSystemVector)
	{
		if (system->isCollection())
			continue;

		if (system->getEmulators().size() == 0)
			continue;

		if (system->getEmulators().size() == 1 && system->getEmulators().cbegin()->cores.size() <= 1)
			continue;

		configuration->addEntry(system->getFullName(), true, [this, system] { openSystemEmulatorSettings(system); });
	}

	window->pushGui(configuration);
}

void GuiMenu::openControllersSettings(int autoSel)
{
	GuiSettings* s = new GuiSettings(mWindow, controllers_settings_label.c_str());

	Window* window = mWindow;

	s->addGroup(_("SETTINGS"));
	auto rumble = std::make_shared<SwitchComponent>(window);
	rumble->setState(getShOutput("rumble.sh") == "on");
	s->addWithLabel(_("ENABLE RUMBLE"), rumble, false, "fa-drum");
	s->addSaveFunc([rumble] {
		if (rumble->changed()) {
			bool enabled = rumble->getState();
			const std::string cmd = std::string("rumble.sh ") + (enabled ? std::string("on") : std::string("off"));
			runSystemCommand(cmd, "", nullptr);
		}
	});
	// CONTROLLER CONFIGURATION
	s->addEntry(_("CONTROLLER MAPPING"), false, [window, this, s]
	{
		window->pushGui(new GuiMsgBox(window,
			_("YOU ARE GOING TO MAP A CONTROLLER. MAP BASED ON THE BUTTON'S POSITION, "
				"NOT ITS PHYSICAL LABEL. IF YOU DO NOT HAVE A SPECIAL BUTTON FOR HOTKEY, "
				"USE THE SELECT BUTTON. SKIP ALL BUTTONS/STICKS YOU DO NOT HAVE BY "
				"HOLDING ANY BUTTON. PRESS THE SOUTH BUTTON TO CONFIRM WHEN DONE."),
			_("OK"), [window, this, s] { window->pushGui(new GuiDetectDevice(window, false, [this, s] { s->setSave(false); delete s; this->openControllersSettings(); })); },
			_("CANCEL"), nullptr,
			GuiMsgBoxIcon::ICON_INFORMATION));
	});

	bool sindenguns_menu = false;
	bool wiiguns_menu = false;

	for (auto gun : InputManager::getInstance()->getGuns())
	{
		sindenguns_menu |= gun->needBorders();
		wiiguns_menu |= gun->name() == "wiigun calibrated";
	}

	for (auto joy : InputManager::getInstance()->getInputConfigs())
		wiiguns_menu |= joy->getDeviceName() == "Nintendo Wii Remote";

	if (sindenguns_menu)
		s->addEntry(_("SINDEN GUN SETTINGS"), true, [this] { openControllersSpecificSettings_sindengun(); });

	if (wiiguns_menu)
		s->addEntry(_("WIIMOTE GUN SETTINGS"), true, [this] { openControllersSpecificSettings_wiigun(); });

	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::BLUETOOTH))
	{
		s->addGroup(_("BLUETOOTH"));

		// PAIR A BLUETOOTH CONTROLLER
		s->addEntry(_("PAIR BLUETOOTH PADS AUTOMATICALLY"), false, [window] { ThreadedBluetooth::start(window); });

#if defined(BATOCERA) || defined(WIN32)
		// PAIR A BLUETOOTH CONTROLLER OR BT AUDIO DEVICE
		s->addEntry(_("PAIR A BLUETOOTH DEVICE MANUALLY"), false, [window, this, s]
		{
			if (ThreadedBluetooth::isRunning())
				window->pushGui(new GuiMsgBox(window, _("BLUETOOTH SCAN IS ALREADY RUNNING.")));
			else
				window->pushGui(new GuiBluetoothPair(window));
		});
#endif
		// FORGET BLUETOOTH CONTROLLERS OR BT AUDIO DEVICES
		s->addEntry(_("FORGET A BLUETOOTH DEVICE"), false, [window, this, s] { window->pushGui(new GuiBluetoothForget(window)); });
	}

	s->addGroup(_("DISPLAY OPTIONS"));

	// CONTROLLER ACTIVITY
	auto activity = std::make_shared<SwitchComponent>(mWindow);
	activity->setState(Settings::getInstance()->getBool("ShowControllerActivity"));
	s->addWithLabel(_("SHOW CONTROLLER ACTIVITY"), activity, autoSel == 1);
	activity->setOnChangedCallback([this, s, activity]
	{
		if (Settings::getInstance()->setBool("ShowControllerActivity", activity->getState()))
		{
			delete s;
			openControllersSettings(1);
		}
	});

	if (Settings::getInstance()->getBool("ShowControllerActivity"))
		s->addSwitch(_("SHOW CONTROLLER BATTERY LEVEL"), "ShowControllerBattery", true);


	s->addGroup(_("PLAYER ASSIGNMENTS"));

	ComponentListRow row;

	// Here we go; for each player
	std::list<int> alreadyTaken = std::list<int>();

	// clear the current loaded inputs
	clearLoadedInput();

	std::vector<std::shared_ptr<OptionListComponent<StrInputConfig *>>> options;

	auto configList = InputManager::getInstance()->getInputConfigs();

	for (int player = 0; player < MAX_PLAYERS; player++)
	{
		std::string confName = Utils::String::format("INPUT P%iNAME", player + 1);
		std::string confGuid = Utils::String::format("INPUT P%iGUID", player + 1);
		std::string confPath = Utils::String::format("INPUT P%iPATH", player + 1);

		std::string label = Utils::String::format(_("P%i'S CONTROLLER").c_str(), player + 1);

		auto inputOptionList = std::make_shared<OptionListComponent<StrInputConfig *> >(mWindow, label, false);
		inputOptionList->add(_("default"), nullptr, false);
		options.push_back(inputOptionList);

		// Checking if a setting has been saved, else setting to default
		std::string configuratedName = Settings::getInstance()->getString(confName);
		std::string configuratedGuid = Settings::getInstance()->getString(confGuid);
		std::string configuratedPath = Settings::getInstance()->getString(confPath);

		bool found = false;

		// For each available and configured input
		for (auto config : configList)
		{
#if WIN32
			std::string displayName = config->getDeviceName();
#else
			std::string displayName = "#" + std::to_string(config->getDeviceIndex()) + " " + config->getDeviceName();
#endif

			bool foundFromConfig = !configuratedPath.empty() ? config->getSortDevicePath() == configuratedPath : configuratedName == config->getDeviceName() && configuratedGuid == config->getDeviceGUIDString();

			int deviceID = config->getDeviceId();

			// Si la manette est configurée, qu'elle correspond a la configuration, et qu'elle n'est pas
			// deja selectionnée on l'ajoute en séléctionnée
			StrInputConfig* newInputConfig = new StrInputConfig(config->getDeviceName(), config->getDeviceGUIDString(), config->getSortDevicePath());
			mLoadedInput.push_back(newInputConfig);

			if (foundFromConfig && std::find(alreadyTaken.begin(), alreadyTaken.end(), deviceID) == alreadyTaken.end() && !found)
			{
				found = true;
				alreadyTaken.push_back(deviceID);

				LOG(LogWarning) << "adding entry for player" << player << " (selected): " << config->getDeviceName() << "  " << config->getDeviceGUIDString() << "  " << config->getDevicePath();

#if WIN32
				inputOptionList->addEx(displayName, config->getDevicePath(), newInputConfig, true, false, false);
#else
				inputOptionList->add(displayName, newInputConfig, true);
#endif
			}
			else
			{
				LOG(LogInfo) << "adding entry for player" << player << " (not selected): " << config->getDeviceName() << "  " << config->getDeviceGUIDString() << "  " << config->getDevicePath();
#if WIN32
				inputOptionList->addEx(displayName, config->getDevicePath(), newInputConfig, false, false, false);
#else
				inputOptionList->add(displayName, newInputConfig, false);
#endif
			}
		}

		if (!inputOptionList->hasSelection())
			inputOptionList->selectFirstItem();

		// Populate controllers list
		s->addWithLabel(label, inputOptionList);
	}

	s->addSaveFunc([this, options, window]
	{
		bool changed = false;

		for (int player = 0; player < MAX_PLAYERS; player++)
		{
			std::string confName = Utils::String::format("INPUT P%iNAME", player + 1);
			std::string confGuid = Utils::String::format("INPUT P%iGUID", player + 1);
			std::string confPath = Utils::String::format("INPUT P%iPATH", player + 1);

			auto input = options.at(player);

			StrInputConfig* selected = input->getSelected();
			if (selected == nullptr)
			{
				changed |= Settings::getInstance()->setString(confName, "DEFAULT");
				changed |= Settings::getInstance()->setString(confGuid, "");
				changed |= Settings::getInstance()->setString(confPath, "");
			}
			else if (input->changed())
			{
				LOG(LogInfo) << "Found the selected controller : " << input->getSelectedName() << ", " << selected->deviceGUIDString << ", " << selected->devicePath;

				changed |= Settings::getInstance()->setString(confName, selected->deviceName);
				changed |= Settings::getInstance()->setString(confGuid, selected->deviceGUIDString);
				changed |= Settings::getInstance()->setString(confPath, selected->devicePath);
			}
		}

		if (changed)
			Settings::getInstance()->saveFile();

		// this is dependant of this configuration, thus update it
		InputManager::getInstance()->computeLastKnownPlayersDeviceIndexes();
	});

	window->pushGui(s);
}

void GuiMenu::openControllersSpecificSettings_sindengun()
{
	GuiSettings* s = new GuiSettings(mWindow, controllers_settings_label.c_str());

	std::string selectedSet = SystemConf::getInstance()->get("controllers.guns.borderssize");
	auto border_set = std::make_shared<OptionListComponent<std::string> >(mWindow, _("BORDER SIZE"), false);
	border_set->add(_("AUTO"),   "",       ""       == selectedSet);
	border_set->add(_("THIN"),   "THIN",   "THIN"   == selectedSet);
	border_set->add(_("MEDIUM"), "MEDIUM", "MEDIUM" == selectedSet);
	border_set->add(_("BIG"),    "BIG",    "BIG"    == selectedSet);

	s->addOptionList(_("BORDER SIZE"), { { _("AUTO"), "auto" },{ _("THIN") , "thin" },{ _("MEDIUM"), "medium" },{ _("BIG"), "big" } }, "controllers.guns.borderssize", false);

	std::string selectedBordersMode = SystemConf::getInstance()->get("controllers.guns.bordersmode");
	auto bordersmode_set = std::make_shared<OptionListComponent<std::string> >(mWindow, _("BORDER MODE"), false);
	border_set->add(_("AUTO"),   "",       ""       == selectedSet);
	border_set->add(_("NORMAL"),   "NORMAL",   "NORMAL"   == selectedSet);
	border_set->add(_("IN GAME ONLY"), "INGAMEONLY", "INGAMEONLY" == selectedSet);
	border_set->add(_("HIDDEN"),    "HIDDEN",    "HIDDEN"    == selectedSet);

	s->addOptionList(_("BORDER MODE"), { { _("AUTO"), "auto" },{ _("NORMAL") , "normal" },{ _("IN GAME ONLY"), "gameonly" },{ _("HIDDEN"), "hidden" } }, "controllers.guns.bordersmode", false);

	std::string baseMode = SystemConf::getInstance()->get("controllers.guns.recoil");
	auto sindenmode_choices = std::make_shared<OptionListComponent<std::string> >(mWindow, _("RECOIL"), false);
	sindenmode_choices->add(_("AUTO"), "auto", baseMode.empty() || baseMode == "auto");
	sindenmode_choices->add(_("DISABLED"), "disabled", baseMode == "disabled");
	sindenmode_choices->add(_("GUN"), "gun", baseMode == "gun");
	sindenmode_choices->add(_("MACHINE GUN"), "machinegun", baseMode == "machinegun");
	sindenmode_choices->add(_("QUIET GUN"), "gun-quiet", baseMode == "gun-quiet");
	sindenmode_choices->add(_("QUIET MACHINE GUN"), "machinegun-quiet", baseMode == "machinegun-quiet");
	s->addWithLabel(_("RECOIL"), sindenmode_choices);
	s->addSaveFunc([sindenmode_choices] {
	  if(sindenmode_choices->getSelected() != SystemConf::getInstance()->get("controllers.guns.recoil")) {
	    SystemConf::getInstance()->set("controllers.guns.recoil", sindenmode_choices->getSelected());
	    SystemConf::getInstance()->saveSystemConf();
	    ApiSystem::getInstance()->replugControllers_sindenguns();
	  }
	});

	mWindow->pushGui(s);
}

void GuiMenu::openControllersSpecificSettings_wiigun()
{
	GuiSettings* s = new GuiSettings(mWindow, controllers_settings_label.c_str());

	std::string baseMode = SystemConf::getInstance()->get("controllers.wiimote.mode");
	auto wiimode_choices = std::make_shared<OptionListComponent<std::string> >(mWindow, _("MODE"), false);
	wiimode_choices->add(_("AUTO"), "auto", baseMode.empty() || baseMode == "auto");
	wiimode_choices->add(_("GUN"), "gun", baseMode == "gun");
	wiimode_choices->add(_("JOYSTICK"), "joystick", baseMode == "joystick");
	s->addWithLabel(_("MODE"), wiimode_choices);
	s->addSaveFunc([wiimode_choices] {
	  if(wiimode_choices->getSelected() != SystemConf::getInstance()->get("controllers.wiimote.mode")) {
	    SystemConf::getInstance()->set("controllers.wiimote.mode", wiimode_choices->getSelected());
	    SystemConf::getInstance()->saveSystemConf();
	    ApiSystem::getInstance()->replugControllers_wiimotes();
	  }
	});
	mWindow->pushGui(s);
}

struct ThemeConfigOption
{
	std::string defaultSettingName;
	std::string subset;
	std::shared_ptr<OptionListComponent<std::string>> component;
};

void GuiMenu::openThemeConfiguration(Window* mWindow, GuiComponent* s, std::shared_ptr<OptionListComponent<std::string>> theme_set, const std::string systemTheme)
{
	if (theme_set != nullptr && Settings::getInstance()->getString("ThemeSet") != theme_set->getSelected())
	{
		mWindow->pushGui(new GuiMsgBox(mWindow, _("YOU MUST APPLY THE THEME BEFORE EDITING ITS CONFIGURATION"), _("OK")));
		return;
	}

	Window* window = mWindow;

	auto system = ViewController::get()->getState().getSystem();
	auto theme = system->getTheme();

	auto themeconfig = new GuiSettings(mWindow, (systemTheme.empty() ? _("THEME CONFIGURATION") : _("VIEW CUSTOMIZATION")).c_str());

	auto themeSubSets = theme->getSubSets();

	std::string viewName;
	bool showGridFeatures = true;
	if (!systemTheme.empty())
	{
		auto glv = ViewController::get()->getGameListView(system);
		viewName = glv->getName();
		std::string baseType = theme->getCustomViewBaseType(viewName);

		showGridFeatures = (viewName == "grid" || baseType == "grid");
	}

	// gamelist_style
	std::shared_ptr<OptionListComponent<std::string>> gamelist_style = nullptr;

	if (systemTheme.empty() || showGridFeatures && system != NULL && theme->hasView("grid"))
		themeconfig->addGroup(_("GAMELIST STYLE"));

	if (systemTheme.empty())
	{
		gamelist_style = std::make_shared< OptionListComponent<std::string> >(mWindow, _("GAMELIST VIEW STYLE"), false);

		std::vector<std::pair<std::string, std::string>> styles;
		styles.push_back(std::pair<std::string, std::string>("automatic", _("automatic")));

		bool showViewStyle = true;

		if (system != NULL)
		{
			auto mViews = theme->getViewsOfTheme();

			showViewStyle = mViews.size() > 1;

			for (auto it = mViews.cbegin(); it != mViews.cend(); ++it)
			{
				if (it->first == "basic" || it->first == "detailed" || it->first == "grid" || it->first == "video" || it->first == "gamecarousel")
					styles.push_back(std::pair<std::string, std::string>(it->first, _(it->first.c_str())));
				else
					styles.push_back(*it);
			}
		}
		else
		{
			styles.push_back(std::pair<std::string, std::string>("basic", _("basic")));
			styles.push_back(std::pair<std::string, std::string>("detailed", _("detailed")));
		}

		auto viewPreference = systemTheme.empty() ? Settings::getInstance()->getString("GamelistViewStyle") : system->getSystemViewMode();
		if (!theme->hasView(viewPreference))
			viewPreference = "automatic";

		for (auto it = styles.cbegin(); it != styles.cend(); it++)
			gamelist_style->add(it->second, it->first, viewPreference == it->first);

		if (!gamelist_style->hasSelection())
			gamelist_style->selectFirstItem();

		if (showViewStyle)
			themeconfig->addWithLabel(_("GAMELIST VIEW STYLE"), gamelist_style);
	}

	// Default grid size
	std::shared_ptr<OptionListComponent<std::string>> mGridSize = nullptr;
	if (showGridFeatures && system != NULL && theme->hasView("grid"))
	{
		Vector2f gridOverride =
			systemTheme.empty() ? Vector2f::parseString(Settings::getInstance()->getString("DefaultGridSize")) :
			system->getGridSizeOverride();

		auto ovv = std::to_string((int)gridOverride.x()) + "x" + std::to_string((int)gridOverride.y());

		mGridSize = std::make_shared<OptionListComponent<std::string>>(mWindow, _("DEFAULT GRID SIZE"), false);

		bool found = false;
		for (auto it = GuiGamelistOptions::gridSizes.cbegin(); it != GuiGamelistOptions::gridSizes.cend(); it++)
		{
			bool sel = (gridOverride == Vector2f(0, 0) && *it == "automatic") || ovv == *it;
			if (sel)
				found = true;

			mGridSize->add(_(it->c_str()), *it, sel);
		}

		if (!found)
			mGridSize->selectFirstItem();

		themeconfig->addWithLabel(_("DEFAULT GRID SIZE"), mGridSize);
	}



	std::map<std::string, ThemeConfigOption> options;

	auto subsetNames = theme->getSubSetNames(viewName);

	// push appliesTo at end of list
	std::stable_sort(subsetNames.begin(), subsetNames.end(), [themeSubSets](const std::string& a, const std::string& b) -> bool
	{
		auto sa = ThemeData::getSubSet(themeSubSets, a);
		auto sb = ThemeData::getSubSet(themeSubSets, b);

		bool aHasApplies = sa.size() > 0 && !sa.cbegin()->appliesTo.empty();
		bool bHasApplies = sb.size() > 0 && !sb.cbegin()->appliesTo.empty();

		return aHasApplies < bHasApplies;
	});

	bool hasThemeOptionGroup = false;
	bool hasApplyToGroup = false;
	for (std::string subset : subsetNames) // theme->getSubSetNames(viewName)
	{
		std::string settingName = "subset." + subset;
		std::string perSystemSettingName = systemTheme.empty() ? "" : "subset." + systemTheme + "." + subset;

		if (subset == "colorset") settingName = "ThemeColorSet";
		else if (subset == "iconset") settingName = "ThemeIconSet";
		else if (subset == "menu") settingName = "ThemeMenu";
		else if (subset == "systemview") settingName = "ThemeSystemView";
		else if (subset == "gamelistview") settingName = "ThemeGamelistView";
		else if (subset == "region") settingName = "ThemeRegionName";

		auto themeColorSets = ThemeData::getSubSet(themeSubSets, subset);

		if (themeColorSets.size() > 0)
		{
			auto selectedColorSet = themeColorSets.end();
			auto selectedName = !perSystemSettingName.empty() ? Settings::getInstance()->getString(perSystemSettingName) : Settings::getInstance()->getString(settingName);

			if (!perSystemSettingName.empty() && selectedName.empty())
				selectedName = Settings::getInstance()->getString(settingName);

			for (auto it = themeColorSets.begin(); it != themeColorSets.end() && selectedColorSet == themeColorSets.end(); it++)
				if (it->name == selectedName)
					selectedColorSet = it;

			std::string displayName;
			if (!themeColorSets.empty())
				displayName = themeColorSets.cbegin()->subSetDisplayName;

			std::shared_ptr<OptionListComponent<std::string>> item = std::make_shared<OptionListComponent<std::string> >(mWindow, displayName.empty() ? _(("THEME " + Utils::String::toUpper(subset)).c_str()) : displayName, false);
			item->setTag(!perSystemSettingName.empty() ? perSystemSettingName : settingName);

			std::string defaultName;
			for (auto it = themeColorSets.begin(); it != themeColorSets.end(); it++)
			{
				std::string displayName = it->displayName;

				if (!systemTheme.empty())
				{
					std::string defaultValue = Settings::getInstance()->getString(settingName);
					if (defaultValue.empty())
						defaultValue = system->getTheme()->getDefaultSubSetValue(subset);

					if (it->name == defaultValue)
					{
						defaultName = Utils::String::toUpper(displayName);
						// displayName = displayName + " (" + _("DEFAULT") + ")";
					}
				}

				item->add(displayName, it->name, it == selectedColorSet);
			}

			if (selectedColorSet == themeColorSets.end())
				item->selectFirstItem();

			if (!themeColorSets.empty())
			{
				if (!displayName.empty())
				{
					bool hasApplyToSubset = themeColorSets.cbegin()->appliesTo.size() > 0;

					std::string prefix;

					if (systemTheme.empty())
					{
						for (auto subsetName : themeColorSets.cbegin()->appliesTo)
						{
							std::string pfx = theme->getViewDisplayName(subsetName);
							if (!pfx.empty())
							{
								if (prefix.empty())
									prefix = pfx;
								else
									prefix = prefix + ", " + pfx;
							}
						}

						prefix = Utils::String::toUpper(prefix);
					}

					if (hasApplyToSubset && !hasApplyToGroup)
					{
						hasApplyToGroup = true;
						themeconfig->addGroup(_("GAMELIST THEME OPTIONS"));
					}
					else if (!hasApplyToSubset && !hasThemeOptionGroup)
					{
						hasThemeOptionGroup = true;
						themeconfig->addGroup(_("THEME OPTIONS"));
					}

					if (displayName == "-" && item->size() <= 1)
					{
						ComponentListRow row;
						row.selectable = false;

						auto font = ThemeData::getMenuTheme()->TextSmall.font;
						auto text = std::make_shared<TextComponent>(mWindow, "", font, 0);
						text->setLineSpacing(1.0f);
						row.addElement(text, true);

						themeconfig->addRow(row);
					}
					else if (!prefix.empty())
						themeconfig->addWithDescription(displayName, prefix, item);
					else if (!defaultName.empty())
						themeconfig->addWithDescription(displayName, _("DEFAULT VALUE") + " : " + defaultName, item);
					else
						themeconfig->addWithLabel(displayName + prefix, item);
				}
				else
				{
					if (!hasThemeOptionGroup)
					{
						hasThemeOptionGroup = true;
						themeconfig->addGroup(_("THEME OPTIONS"));
					}

					themeconfig->addWithLabel(_(("THEME " + Utils::String::toUpper(subset)).c_str()), item);
				}
			}

			ThemeConfigOption opt;
			opt.component = item;
			opt.subset = subset;
			opt.defaultSettingName = settingName;
			options[!perSystemSettingName.empty() ? perSystemSettingName : settingName] = opt;
		}
		else
		{
			ThemeConfigOption opt;
			opt.component = nullptr;
			options[!perSystemSettingName.empty() ? perSystemSettingName : settingName] = opt;
		}
	}


	if (!systemTheme.empty())
	{
		themeconfig->addGroup(_("GAMELIST OPTIONS"));

		// Show favorites first in gamelists
		auto fav = Settings::getInstance()->getString(system->getName() + ".FavoritesFirst");
		auto favoritesFirst = std::make_shared<OptionListComponent<std::string>>(mWindow, _("SHOW FAVORITES ON TOP"), false);
		std::string defFav = Settings::getInstance()->getBool("FavoritesFirst") ? _("YES") : _("NO");
		favoritesFirst->add(_("AUTO"), "", fav == "" || fav == "auto");
		favoritesFirst->add(_("YES"), "1", fav == "1");
		favoritesFirst->add(_("NO"), "0", fav == "0");
		themeconfig->addWithDescription(_("SHOW FAVORITES ON TOP"), _("DEFAULT VALUE") + " : " + defFav, favoritesFirst);
		themeconfig->addSaveFunc([themeconfig, favoritesFirst, system]
		{
			if (Settings::getInstance()->setString(system->getName() + ".FavoritesFirst", favoritesFirst->getSelected()))
				themeconfig->setVariable("reloadAll", true);
		});

		// Show favorites first in gamelists
		auto defHid = Settings::ShowHiddenFiles() ? _("YES") : _("NO");
		auto curhid = Settings::getInstance()->getString(system->getName() + ".ShowHiddenFiles");
		auto hiddenFiles = std::make_shared<OptionListComponent<std::string>>(mWindow, _("SHOW HIDDEN FILES"), false);
		hiddenFiles->add(_("AUTO"), "", curhid == "" || curhid == "auto");
		hiddenFiles->add(_("YES"), "1", curhid == "1");
		hiddenFiles->add(_("NO"), "0", curhid == "0");
		themeconfig->addWithDescription(_("SHOW HIDDEN FILES"), _("DEFAULT VALUE") + " : " + defHid, hiddenFiles);
		themeconfig->addSaveFunc([themeconfig, hiddenFiles, system]
		{
			if (Settings::getInstance()->setString(system->getName() + ".ShowHiddenFiles", hiddenFiles->getSelected()))
				themeconfig->setVariable("reloadAll", true);
		});

		// Folder View Mode
		auto folderView = Settings::getInstance()->getString("FolderViewMode");
		auto defFol = folderView.empty() ? "" : Utils::String::toUpper(_(folderView.c_str()));
		auto curFol = Settings::getInstance()->getString(system->getName() + ".FolderViewMode");

		auto foldersBehavior = std::make_shared<OptionListComponent<std::string>>(mWindow, _("SHOW FOLDERS"), false);
		foldersBehavior->add(_("AUTO"), "", curFol == "" || curFol == "auto"); //  + " (" + defFol + ")"
		foldersBehavior->add(_("always"), "always", curFol == "always");
		foldersBehavior->add(_("never"), "never", curFol == "never");
		foldersBehavior->add(_("having multiple games"), "having multiple games", curFol == "having multiple games");

		themeconfig->addWithDescription(_("SHOW FOLDERS"), _("DEFAULT VALUE") + " : " + defFol, foldersBehavior);
		themeconfig->addSaveFunc([themeconfig, foldersBehavior, system]
		{
			if (Settings::getInstance()->setString(system->getName() + ".FolderViewMode", foldersBehavior->getSelected()))
				themeconfig->setVariable("reloadAll", true);
		});

		// Show parent folder in gamelists
		auto defPf = Settings::getInstance()->getBool("ShowParentFolder") ? _("YES") : _("NO");
		auto curPf = Settings::getInstance()->getString(system->getName() + ".ShowParentFolder");
		auto parentFolder = std::make_shared<OptionListComponent<std::string>>(mWindow, _("SHOW '..' PARENT FOLDER"), false);
		parentFolder->add(_("AUTO"), "", curPf == "" || curPf == "auto");
		parentFolder->add(_("YES"), "1", curPf == "1");
		parentFolder->add(_("NO"), "0", curPf == "0");
		themeconfig->addWithDescription(_("SHOW '..' PARENT FOLDER"), _("DEFAULT VALUE") + " : " + defPf, parentFolder);
		themeconfig->addSaveFunc([themeconfig, parentFolder, system]
		{
			if (Settings::getInstance()->setString(system->getName() + ".ShowParentFolder", parentFolder->getSelected()))
				themeconfig->setVariable("reloadAll", true);
		});

		// Show flags

		auto defSF = Settings::getInstance()->getString("ShowFlags");
		if (defSF == "1")
			defSF = _("BEFORE NAME");
		else if (defSF == "2")
			defSF = _("AFTER NAME");
		else
			defSF = _("NO");

		auto curSF = Settings::getInstance()->getString(system->getName() + ".ShowFlags");
		auto showRegionFlags = std::make_shared<OptionListComponent<std::string>>(mWindow, _("SHOW REGION FLAG"), false);

		showRegionFlags->addRange({
			{ _("AUTO"), "auto" },
			{ _("NO"), "0" },
			{ _("BEFORE NAME") , "1" },
			{ _("AFTER NAME"), "2" } },
			curSF);

		themeconfig->addWithDescription(_("SHOW REGION FLAG"), _("DEFAULT VALUE") + " : " + defSF, showRegionFlags);
		themeconfig->addSaveFunc([themeconfig, showRegionFlags, system]
		{
			if (Settings::getInstance()->setString(system->getName() + ".ShowFlags", showRegionFlags->getSelected()))
				themeconfig->setVariable("reloadAll", true);
		});

		// Show SaveStates
		auto defSS = Settings::getInstance()->getBool("ShowSaveStates") ? _("YES") : _("NO");
		auto curSS = Settings::getInstance()->getString(system->getName() + ".ShowSaveStates");
		auto showSaveStates = std::make_shared<OptionListComponent<std::string>>(mWindow, _("SHOW SAVESTATE ICON"), false);
		showSaveStates->add(_("AUTO"), "", curSS == "" || curSS == "auto");
		showSaveStates->add(_("YES"), "1", curSS == "1");
		showSaveStates->add(_("NO"), "0", curSS == "0");
		themeconfig->addWithDescription(_("SHOW SAVESTATE ICON"), _("DEFAULT VALUE") + " : " + defSS, showSaveStates);
		themeconfig->addSaveFunc([themeconfig, showSaveStates, system]
		{
			if (Settings::getInstance()->setString(system->getName() + ".ShowSaveStates", showSaveStates->getSelected()))
				themeconfig->setVariable("reloadAll", true);
		});

		// Show Manual
		auto defMM = Settings::getInstance()->getBool("ShowManualIcon") ? _("YES") : _("NO");
		auto curMM = Settings::getInstance()->getString(system->getName() + ".ShowManualIcon");
		auto showManual = std::make_shared<OptionListComponent<std::string>>(mWindow, _("SHOW MANUAL ICON"), false);
		showManual->add(_("AUTO"), "", curMM == "" || curMM == "auto");
		showManual->add(_("YES"), "1", curMM == "1");
		showManual->add(_("NO"), "0", curMM == "0");
		themeconfig->addWithDescription(_("SHOW MANUAL ICON"), _("DEFAULT VALUE") + " : " + defMM, showManual);
		themeconfig->addSaveFunc([themeconfig, showManual, system]
		{
			if (Settings::getInstance()->setString(system->getName() + ".ShowManualIcon", showManual->getSelected()))
				themeconfig->setVariable("reloadAll", true);
		});

		// Show filenames
		auto defFn = Settings::getInstance()->getBool("ShowFilenames") ? _("YES") : _("NO");
		auto curFn = Settings::getInstance()->getString(system->getName() + ".ShowFilenames");

		auto showFilenames = std::make_shared<OptionListComponent<std::string>>(mWindow, _("SHOW FILENAMES INSTEAD"), false);
		showFilenames->add(_("AUTO"), "", curFn == "");
		showFilenames->add(_("YES"), "1", curFn == "1");
		showFilenames->add(_("NO"), "0", curFn == "0");
		themeconfig->addWithDescription(_("SHOW FILENAMES INSTEAD"), _("DEFAULT VALUE") + " : " + defFn, showFilenames);
		themeconfig->addSaveFunc([themeconfig, showFilenames, system]
		{
			if (Settings::getInstance()->setString(system->getName() + ".ShowFilenames", showFilenames->getSelected()))
			{
				SystemData::resetSettings();
				FileData::resetSettings();

		//		themeconfig->setVariable("reloadCollections", true);
				themeconfig->setVariable("reloadAll", true);
			}
		});


		// File extensions
		if (!system->isCollection() && system->isGameSystem())
		{
			auto hiddenExts = Utils::String::split(Settings::getInstance()->getString(system->getName() + ".HiddenExt"), ';');

			auto hiddenCtrl = std::make_shared<OptionListComponent<std::string>>(mWindow, _("FILE EXTENSIONS"), true);

			for (auto ext : system->getExtensions())
			{
				std::string extid = Utils::String::toLower(Utils::String::replace(ext, ".", ""));
				hiddenCtrl->add(ext, extid, std::find(hiddenExts.cbegin(), hiddenExts.cend(), extid) == hiddenExts.cend());
			}

			themeconfig->addWithLabel(_("FILE EXTENSIONS"), hiddenCtrl);
			themeconfig->addSaveFunc([themeconfig, system, hiddenCtrl]
			{
				std::string hiddenSystems;

				std::vector<std::string> sel = hiddenCtrl->getSelectedObjects();

				for (auto ext : system->getExtensions())
				{
					std::string extid = Utils::String::toLower(Utils::String::replace(ext, ".", ""));
					if (std::find(sel.cbegin(), sel.cend(), extid) == sel.cend())
					{
						if (hiddenSystems.empty())
							hiddenSystems = extid;
						else
							hiddenSystems = hiddenSystems + ";" + extid;
					}
				}

				if (Settings::getInstance()->setString(system->getName() + ".HiddenExt", hiddenSystems))
				{
					Settings::getInstance()->saveFile();

					themeconfig->setVariable("reloadAll", true);
					themeconfig->setVariable("forceReloadGames", true);
				}
			});
		}
	}

	if (systemTheme.empty())
	{
		themeconfig->addGroup(_("TOOLS"));

		themeconfig->addEntry(_("RESET CUSTOMIZATIONS"), false, [s, themeconfig, window]
		{
			themeconfig->setVariable("resetTheme", true);
			themeconfig->setVariable("reloadAll", true);
			themeconfig->close();
		});
	}

	//  theme_colorset, theme_iconset, theme_menu, theme_systemview, theme_gamelistview, theme_region,
	themeconfig->addSaveFunc([systemTheme, system, themeconfig, options, gamelist_style, mGridSize, window]
	{
		bool reloadAll = false;

		for (auto option : options)
		{
			ThemeConfigOption& opt = option.second;

			std::string value;

			if (opt.component != nullptr)
			{
				value = opt.component->getSelected();

				if (!systemTheme.empty() && !value.empty())
				{
					std::string defaultValue = Settings::getInstance()->getString(opt.defaultSettingName);
					if (defaultValue.empty())
						defaultValue = system->getTheme()->getDefaultSubSetValue(opt.subset);

					if (value == defaultValue)
						value = "";
				}
				else if (systemTheme.empty() && value == system->getTheme()->getDefaultSubSetValue(opt.subset))
					value = "";
			}

			if (value != Settings::getInstance()->getString(option.first))
				reloadAll |= Settings::getInstance()->setString(option.first, value);
		}

		Vector2f gridSizeOverride(0, 0);

		if (mGridSize != nullptr)
		{
			std::string str = mGridSize->getSelected();
			std::string value = "";

			size_t divider = str.find('x');
			if (divider != std::string::npos)
			{
				std::string first = str.substr(0, divider);
				std::string second = str.substr(divider + 1, std::string::npos);

				gridSizeOverride = Vector2f((float)atof(first.c_str()), (float)atof(second.c_str()));
				value = Utils::String::replace(Utils::String::replace(gridSizeOverride.toString(), ".000000", ""), "0 0", "");
			}

			if (systemTheme.empty())
				reloadAll |= Settings::getInstance()->setString("DefaultGridSize", value);
		}
		else if (systemTheme.empty())
			reloadAll |= Settings::getInstance()->setString("DefaultGridSize", "");

		if (systemTheme.empty())
			reloadAll |= Settings::getInstance()->setString("GamelistViewStyle", gamelist_style == nullptr ? "" : gamelist_style->getSelected());
		else
		{
			std::string viewMode = gamelist_style == nullptr ? system->getSystemViewMode() : gamelist_style->getSelected();
			reloadAll |= system->setSystemViewMode(viewMode, gridSizeOverride);
		}

		if (themeconfig->getVariable("resetTheme"))
		{
			Settings::getInstance()->setString("GamelistViewStyle", "");
			Settings::getInstance()->setString("DefaultGridSize", "");
			Settings::getInstance()->setString("ThemeRegionName", "");
			Settings::getInstance()->setString("ThemeColorSet", "");
			Settings::getInstance()->setString("ThemeIconSet", "");
			Settings::getInstance()->setString("ThemeMenu", "");
			Settings::getInstance()->setString("ThemeSystemView", "");
			Settings::getInstance()->setString("ThemeGamelistView", "");
			Settings::getInstance()->setString("GamelistViewStyle", "");
			Settings::getInstance()->setString("DefaultGridSize", "");

			for (auto sm : Settings::getInstance()->getStringMap())
				if (Utils::String::startsWith(sm.first, "subset."))
					Settings::getInstance()->setString(sm.first, "");

			for (auto system : SystemData::sSystemVector)
			{
				system->setSystemViewMode("automatic", Vector2f(0, 0));

				Settings::getInstance()->setString(system->getName() + ".FavoritesFirst", "");
				Settings::getInstance()->setString(system->getName() + ".ShowHiddenFiles", "");
				Settings::getInstance()->setString(system->getName() + ".FolderViewMode", "");
				Settings::getInstance()->setString(system->getName() + ".ShowFilenames", "");
				Settings::getInstance()->setString(system->getName() + ".ShowParentFolder", "");
			}

			Settings::getInstance()->saveFile();
			std::string path = Paths::getUserEmulationStationPath() + "/themesettings/" + Settings::getInstance()->getString("ThemeSet") + ".cfg";
			if (Utils::FileSystem::exists(path))
				Utils::FileSystem::removeFile(path);
		}

		if (reloadAll || themeconfig->getVariable("reloadAll"))
		{
			if (themeconfig->getVariable("forceReloadGames"))
			{
				ViewController::reloadAllGames(window, false);
			}
			else if (systemTheme.empty())
			{
				CollectionSystemManager::get()->updateSystemsList();
				ViewController::get()->reloadAll(window);
				window->closeSplashScreen();
			}
			else
			{
				system->loadTheme();
				system->resetFilters();

				ViewController::get()->reloadSystemListViewTheme(system);
				ViewController::get()->reloadGameListView(system);
			}
		}
	});

	mWindow->pushGui(themeconfig);
}

void GuiMenu::openUISettings()
{
	auto pthis = this;
	Window* window = mWindow;

	auto s = new GuiSettings(mWindow, _("USER INTERFACE SETTINGS").c_str());

	// theme set
	auto theme = ThemeData::getMenuTheme();
	auto themeSets = ThemeData::getThemeSets();
	auto system = ViewController::get()->getState().getSystem();

	s->addGroup(_("APPEARANCE"));

	if (system != nullptr && !themeSets.empty())
	{
		auto selectedSet = themeSets.find(Settings::getInstance()->getString("ThemeSet"));
		if (selectedSet == themeSets.end())
			selectedSet = themeSets.begin();

		auto theme_set = std::make_shared<OptionListComponent<std::string> >(mWindow, _("THEME SET"), false);

		std::vector<std::string> themeList;
		for (auto it = themeSets.begin(); it != themeSets.end(); it++)
			themeList.push_back(it->first);

		std::sort(themeList.begin(), themeList.end(), [](const std::string& a, const std::string& b) -> bool { return Utils::String::toLower(a).compare(Utils::String::toLower(b)) < 0; });

		for (auto themeName : themeList)
			theme_set->add(themeName, themeName, themeName == selectedSet->first);

		//for (auto it = themeSets.begin(); it != themeSets.end(); it++)
		//	theme_set->add(it->first, it->first, it == selectedSet);

		s->addWithLabel(_("THEME SET"), theme_set);
		s->addSaveFunc([s, theme_set, pthis, window, system]
		{
			std::string oldTheme = Settings::getInstance()->getString("ThemeSet");
			if (oldTheme != theme_set->getSelected())
			{
				saveSubsetSettings();

				Settings::getInstance()->setString("ThemeSet", theme_set->getSelected());

				// theme changed without setting options, forcing options to avoid crash/blank theme
				Settings::getInstance()->setString("ThemeRegionName", "");
				Settings::getInstance()->setString("ThemeColorSet", "");
				Settings::getInstance()->setString("ThemeIconSet", "");
				Settings::getInstance()->setString("ThemeMenu", "");
				Settings::getInstance()->setString("ThemeSystemView", "");
				Settings::getInstance()->setString("ThemeGamelistView", "");
				Settings::getInstance()->setString("GamelistViewStyle", "");
				Settings::getInstance()->setString("DefaultGridSize", "");

				for(auto sm : Settings::getInstance()->getStringMap())
					if (Utils::String::startsWith(sm.first, "subset."))
						Settings::getInstance()->setString(sm.first, "");

				for (auto sysIt = SystemData::sSystemVector.cbegin(); sysIt != SystemData::sSystemVector.cend(); sysIt++)
					(*sysIt)->setSystemViewMode("automatic", Vector2f(0,0));

				loadSubsetSettings(theme_set->getSelected());

				s->setVariable("reloadCollections", true);
				s->setVariable("reloadAll", true);
				s->setVariable("reloadGuiMenu", true);

				Scripting::fireEvent("theme-changed", theme_set->getSelected(), oldTheme);
			}
		});

		bool showThemeConfiguration = system->getTheme()->hasSubsets() || system->getTheme()->hasView("grid");
		if (showThemeConfiguration)
		{
			s->addSubMenu(_("THEME CONFIGURATION"), [this, s, theme_set]() { openThemeConfiguration(mWindow, s, theme_set); });
		}
		else // GameList view style only, acts like Retropie for simple themes
		{
			auto gamelist_style = std::make_shared< OptionListComponent<std::string> >(mWindow, _("GAMELIST VIEW STYLE"), false);
			std::vector<std::pair<std::string, std::string>> styles;
			styles.push_back(std::pair<std::string, std::string>("automatic", _("automatic")));

			auto system = ViewController::get()->getState().getSystem();
			if (system != NULL)
			{
				auto mViews = system->getTheme()->getViewsOfTheme();
				for (auto it = mViews.cbegin(); it != mViews.cend(); ++it)
					styles.push_back(*it);
			}
			else
			{
				styles.push_back(std::pair<std::string, std::string>("basic", _("basic")));
				styles.push_back(std::pair<std::string, std::string>("detailed", _("detailed")));
				styles.push_back(std::pair<std::string, std::string>("video", _("video")));
				styles.push_back(std::pair<std::string, std::string>("grid", _("grid")));
			}

			auto viewPreference = Settings::getInstance()->getString("GamelistViewStyle");
			if (!system->getTheme()->hasView(viewPreference))
				viewPreference = "automatic";

			for (auto it = styles.cbegin(); it != styles.cend(); it++)
				gamelist_style->add(it->second, it->first, viewPreference == it->first);

			s->addWithLabel(_("GAMELIST VIEW STYLE"), gamelist_style);
			s->addSaveFunc([s, gamelist_style, window] {
				if (Settings::getInstance()->setString("GamelistViewStyle", gamelist_style->getSelected()))
				{
					s->setVariable("reloadAll", true);
					s->setVariable("reloadGuiMenu", true);
				}
			});
		}
	}

	s->addGroup(_("DISPLAY OPTIONS"));
	auto backButton = std::make_shared<SwitchComponent>(mWindow);
	backButton->setState(SystemConf::getInstance()->get("pe_backbtn.disabled") == "1");
	s->addWithLabel(_("DISABLE BACK BUTTON IN MENU"), backButton);
	s->addSaveFunc([backButton] {
		if (backButton->changed()) {
			bool enabled = backButton->getState();
			SystemConf::getInstance()->set("pe_backbtn.disabled", enabled ? "1" : "0");
			SystemConf::getInstance()->saveSystemConf();
		}
	});

	s->addEntry(_("SCREENSAVER SETTINGS"), true, std::bind(&GuiMenu::openScreensaverOptions, this));
	s->addOptionList(_("LIST TRANSITION"), { { _("auto"), "auto" },{ _("fade") , "fade" },{ _("slide"), "slide" },{ _("fade & slide"), "fade & slide" },{ _("instant"), "instant" } }, "TransitionStyle", true);
	s->addOptionList(_("GAME LAUNCH TRANSITION"), { { _("auto"), "auto" },{ _("fade") , "fade" },{ _("slide"), "slide" },{ _("instant"), "instant" } }, "GameTransitionStyle", true);
	s->addSwitch(_("SHOW CLOCK"), "DrawClock", true);
	s->addSwitch(_("ON-SCREEN HELP"), "ShowHelpPrompts", true, [s] { s->setVariable("reloadAll", true); });

	if (queryBatteryInformation().hasBattery)
		s->addOptionList(_("SHOW BATTERY STATUS"), { { _("NO"), "" },{ _("ICON"), "icon" },{ _("ICON AND TEXT"), "text" } }, "ShowBattery", true);

	s->addGroup(_("GAMELIST OPTIONS"));
	s->addSwitch(_("SHOW FAVORITES ON TOP"), "FavoritesFirst", true, [s] { s->setVariable("reloadAll", true); });
	s->addSwitch(_("SHOW HIDDEN FILES"), "ShowHiddenFiles", true, [s] { s->setVariable("reloadAll", true); });
	s->addOptionList(_("SHOW FOLDERS"), { { _("always"), "always" },{ _("never") , "never" },{ _("having multiple games"), "having multiple games" } }, "FolderViewMode", true, [s] { s->setVariable("reloadAll", true); });
	s->addSwitch(_("SHOW '..' PARENT FOLDER"), "ShowParentFolder", true, [s] { s->setVariable("reloadAll", true); });
	s->addOptionList(_("SHOW REGION FLAG"), { { _("NO"), "auto" },{ _("BEFORE NAME") , "1" },{ _("AFTER NAME"), "2" } }, "ShowFlags", true, [s] { s->setVariable("reloadAll", true); });
	s->addSwitch(_("SHOW SAVESTATE ICON"), "ShowSaveStates", true, [s] { s->setVariable("reloadAll", true); });
	s->addSwitch(_("SHOW MANUAL ICON"), "ShowManualIcon", true, [s] { s->setVariable("reloadAll", true); });
	s->addSwitch(_("SHOW FILENAMES INSTEAD"), "ShowFilenames", true, [s]
		{
			SystemData::resetSettings();
			FileData::resetSettings();

			s->setVariable("reloadCollections", true);
			s->setVariable("reloadAll", true);
		});
	s->addSwitch(_("IGNORE LEADING ARTICLES WHEN SORTING"), _("Ignore 'The' and 'A' if at the start."), "IgnoreLeadingArticles", true, [s] { s->setVariable("reloadAll", true); });

	s->onFinalize([s, pthis, window]
	{
		if (s->getVariable("reloadCollections"))
			CollectionSystemManager::get()->updateSystemsList();

		if (s->getVariable("reloadAll"))
		{
			ViewController::get()->reloadAll(window);
			window->closeSplashScreen();
		}

		if (s->getVariable("reloadGuiMenu"))
		{
			delete pthis;
			window->pushGui(new GuiMenu(window));
		}
	});

	mWindow->pushGui(s);
}

void GuiMenu::openSoundSettings()
{
	auto s = new GuiSettings(mWindow, _("SOUND SETTINGS").c_str());

	if (VolumeControl::getInstance()->isAvailable())
	{
		s->addGroup(_("VOLUME"));

		// volume
		auto volume = std::make_shared<SliderComponent>(mWindow, 0.f, 100.f, 1.f, "%");
		volume->setValue((float)VolumeControl::getInstance()->getVolume());
		volume->setOnValueChanged([](const float &newVal) { VolumeControl::getInstance()->setVolume((int)Math::round(newVal)); });
		s->addWithLabel(_("SYSTEM VOLUME"), volume);
		s->addSaveFunc([this, volume]
		{
			VolumeControl::getInstance()->setVolume((int)Math::round(volume->getValue()));
#if !WIN32
			SystemConf::getInstance()->set("audio.volume", std::to_string((int)round(volume->getValue())));
#endif
		});


		// Music Volume
		auto musicVolume = std::make_shared<SliderComponent>(mWindow, 0.f, 100.f, 1.f, "%");
		musicVolume->setValue(Settings::getInstance()->getInt("MusicVolume"));
		musicVolume->setOnValueChanged([](const float &newVal) { Settings::getInstance()->setInt("MusicVolume", (int)round(newVal)); });
		s->addWithLabel(_("MUSIC VOLUME"), musicVolume);

		s->addSwitch(_("SHOW OVERLAY WHEN VOLUME CHANGES"), "VolumePopup", true);
	}

	s->addGroup(_("MUSIC"));

	s->addSwitch(_("FRONTEND MUSIC"), "audio.bgmusic", true, []
	{
		if (Settings::getInstance()->getBool("audio.bgmusic"))
			AudioManager::getInstance()->playRandomMusic();
		else
			AudioManager::getInstance()->stopMusic();
	});

	s->addSwitch(_("DISPLAY SONG TITLES"), "audio.display_titles", true);

	// how long to display the song titles?
	auto titles_time = std::make_shared<SliderComponent>(mWindow, 2.f, 120.f, 2.f, "s");
	titles_time->setValue(Settings::getInstance()->getInt("audio.display_titles_time"));
	s->addWithLabel(_("SONG TITLE DISPLAY DURATION"), titles_time);
	s->addSaveFunc([titles_time] {
		Settings::getInstance()->setInt("audio.display_titles_time", (int)Math::round(titles_time->getValue()));
	});

	s->addSwitch(_("ONLY PLAY SYSTEM-SPECIFIC MUSIC FOLDER"), "audio.persystem", true, [] { AudioManager::getInstance()->changePlaylist(ViewController::get()->getState().getSystem()->getTheme(), true); } );
	s->addSwitch(_("PLAY SYSTEM-SPECIFIC MUSIC"), "audio.thememusics", true, [] { AudioManager::getInstance()->changePlaylist(ViewController::get()->getState().getSystem()->getTheme(), true); });
	s->addSwitch(_("LOWER MUSIC WHEN PLAYING VIDEO"), "VideoLowersMusic", true);

	s->addGroup(_("SOUNDS"));

	s->addSwitch(_("ENABLE NAVIGATION SOUNDS"), "EnableSounds", true, []
	{
		if (Settings::getInstance()->getBool("EnableSounds") && PowerSaver::getMode() == PowerSaver::INSTANT)
		{
			Settings::getInstance()->setPowerSaverMode("default");
			PowerSaver::init();
		}
	});

	s->addSwitch(_("ENABLE VIDEO PREVIEW AUDIO"), "VideoAudio", true);

	mWindow->pushGui(s);
}

void GuiMenu::openWifiSettings(Window* win, std::string title, std::string data, const std::function<void(std::string)>& onsave)
{
	win->pushGui(new GuiWifi(win, title, data, onsave));
}

void GuiMenu::loadChromecast(Window* mWindow, std::string file)
	{
		Window* window = mWindow;
		LOG(LogInfo) << "Chromecast:" << file;
		mWindow->pushGui(new GuiLoading<std::vector<AVAHIserviceDetail>>(mWindow, _("Loading..."),
			[window, file](auto gui)
			{
				//mWaitingLoad = true;
				return getAvahiService("_googlecast._tcp");
			},
			[window, file](std::vector<AVAHIserviceDetail> casts)
			{
				//mWaitingLoad = false;
				if(casts.size() == 0)
					{
						window->pushGui(new GuiMsgBox(window, _("NO CHROMECAST DEVICES FOUND"),_("OK"),nullptr));
					}
				else
					{
						loadChromecastDevices(window, casts, file);
					}
			}
		));
	}
void GuiMenu::loadChromecastDevices(Window* mWindow, std::vector<AVAHIserviceDetail> casts, std::string file)
	{
		Window* window = mWindow;

		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;
		std::string basename;
		if(!file.empty())
			{
				std::vector<std::string> bstr = Utils::String::split(file, '/');
				basename = bstr[bstr.size() - 1];
			}

		auto s = new GuiSettings(window, _("CHROMECAST") + (!basename.empty() ? (": "+basename) : ""));


		for(auto dev : casts)
			{
				Chromecast device(dev);

				s->addWithDescription(device.name , device.player,
					std::make_shared<TextComponent>(window, device.oname, font, color),
					[window, device, file]
				{
					LOG(LogInfo) << "Chromecast device:" << device.name;
					loadChromecastDevice(window, device, file);
				}, "fa-chromecast");
			}

		window->pushGui(s);
	}

void GuiMenu::ChromecastControl(std::string id, std::string action, std::string file)
	{
		LOG(LogInfo) << "Chromecast action:" << action;

		if(action == "load")
			{
				//curl -H "content-Type: application/json" http://192.168.1.105:8009/apps/YouTube -X POST -d 'v=Nz8BXRGRXc0'
				if(AudioManager::getInstance()->ChromecastData().castID != id)
					{
						runSystemCommand("killall go-chromecast &", "", nullptr);
					}

				AudioManager::getInstance()->setChromecast(true, file, id);
			}
		else if(action == "stop")
			{
				AudioManager::getInstance()->clearChromecast();
			}
	  else if(action == "pause")
			{
				AudioManager::getInstance()->setChromecastPaused(true);
			}
		else if(action == "unpause")
			{
				AudioManager::getInstance()->setChromecastPaused(false);
			}

		if(action == "YouTube")
			{
				AudioManager::getInstance()->setChromecast(true, file, id);
				runSystemCommand("go-chromecast -u " + id + " load-app YouTube \"v="+file+"\" &", "", nullptr);
			}
		else
			{
				runSystemCommand("go-chromecast -u " + id + " " + action + (file.empty() ? " &" : " '" + file + "' &"), "", nullptr);
			}

	}


void GuiMenu::loadChromecastDevice(Window* mWindow, Chromecast device, std::string file)
	{
		Window* window = mWindow;
		std::string basename;
		if(!file.empty())
			{
				std::vector<std::string> bstr = Utils::String::split(file, '/');
				basename = bstr[bstr.size() - 1];
			}

		auto s = new GuiSettings(window, device.name + (!file.empty() ? ": "+basename : ""));
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;


		if(!file.empty())
			{
				if(file != AudioManager::getInstance()->ChromecastData().filename)
					{
						std::vector<std::string> bstr = Utils::String::split(file, '/');
						if(bstr.size() == 2 && bstr.at(0) == "YouTube")
							{
								ChromecastControl(device.id, "YouTube", bstr.at(1));
							}
						else
							{
								ChromecastControl(device.id, "load", file);
							}
					}

				std::string basename = file;
				std::vector<std::string> bstr = Utils::String::split(file, '/');
				basename = bstr[bstr.size() - 1];
				s->addEntry("LOAD " + basename, false, [device, file] {
					ChromecastControl(device.id, "load", file);
				});
			}

			s->addEntry("STOP", false, [s, window, device] {
				ChromecastControl(device.id, "stop");
			});

			s->addEntry("PAUSE / RESUME", false, [window, device] {
				ChromecastControl(device.id, AudioManager::getInstance()->ChromecastData().paused ? "unpause" : "pause");
			});

			auto volumeSlider = std::make_shared<SliderComponent>(mWindow, 0.f, 100.f, 1.f, "%");
			float currVolume = std::stof(getShOutput("go-chromecast -u " + device.id + " volume"));
			volumeSlider->setValue(currVolume * 100);
			volumeSlider->setOnValueChanged([device](const float &newVal) {
				runSystemCommand("go-chromecast -u " + device.id + " volume " + std::to_string(newVal / 100), "", nullptr);
			});
			s->addWithLabel(_("VOLUME"), volumeSlider);


		window->pushGui(s);
	}

void GuiMenu::pingIP(std::string ip)
	{
		const std::string cmd = "ping " + ip + " -c 1";
		msgExec(cmd);
	}
void GuiMenu::msgExec(const std::string cmd){
	Window* window = mWindow;
	window->pushGui(new GuiLoading<std::string>(window, _("Loading..."),
		[this, window, cmd](auto gui)
		{
			mWaitingLoad = true;
			std::vector<std::string> results = ApiSystem::getInstance()->getScriptResults(cmd);
			std::string result = "";
			for (auto line : results)
				{
					result = result + line + "\n";
				}
			return result;
		},
		[this, window](std::string result)
		{
			mWaitingLoad = false;
			window->pushGui(new GuiMsgBox(window, result,_("OK"),nullptr));
		}
	));
}

std::string GuiMenu::apInlineInfo(std::string cmd)
	{
		//std::vector<std::string> result = ApiSystem::getInstance()->getScriptResults("ap.sh " + cmd);
		return getShOutput("ap.sh " + cmd);
	}

DHCPClient GuiMenu::rawToDHCPCli(std::string raw)
	{
		DHCPClient cli(raw);
		cli.vendor = macVendor(cli.mac);
		return cli;
	}
std::vector<DHCPClient> GuiMenu::DHCPClientList(std::vector<std::string> clients)
	{
		std::vector<DHCPClient> list;
		for(auto cli : clients)
			{
				DHCPClient _cli(cli);
				_cli.vendor = macVendor(_cli.mac);
				list.push_back(_cli);
			}
		return list;
	}

void GuiMenu::openDHCPclient(DHCPClient lease)
	{
		Window* window = mWindow;
		auto s = new GuiSettings(window, lease.ip);
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		s->addGroup(_("INFO"));
			s->addWithLabel(_("IP"), 				std::make_shared<TextComponent>(mWindow, lease.ip, font, color));
			s->addWithLabel(_("HOSTNAME"), 	std::make_shared<TextComponent>(mWindow, lease.hostname, font, color));
			s->addWithLabel(_("MAC"),				std::make_shared<TextComponent>(mWindow, lease.mac, font, color));
			s->addWithLabel(_("VENDOR"), 		std::make_shared<TextComponent>(mWindow, lease.vendor, font, color));
			s->addWithLabel(_("LEASETIME"), std::make_shared<TextComponent>(mWindow, lease.leasetime, font, color));

		s->addGroup(_("TOOLS"));
			s->addEntry(_("PING"), true, [this, window, lease] {
				pingIP(lease.ip);
			}, "fa-toolbox");

			s->addEntry(_("DEAUTHENTICATE CLIENT"), false, [this, window, s, lease] {
				window->pushGui(new GuiMsgBox(window, _("DEAUTHENTICATE CLIENT") + "?",
					_("YES"), [this, s, lease] {
						runSystemCommand("ap.sh deauth " + lease.mac, "", nullptr);
						delete s;
					},_("NO"), nullptr));
				}, "fa-link-slash");

		window->pushGui(s);
	}

void GuiMenu::openAPleases()
	{
		Window *window = mWindow;
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		auto s = new GuiSettings(window, _("DHCP LEASES").c_str());

		const std::string cmd = "ap.sh leases";
		std::vector<DHCPClient> leases = DHCPClientList(ApiSystem::getInstance()->getScriptResults(cmd));

		if(leases.size() > 0)
		{
			for (auto lease : leases)
				{

					std::string title = lease.ip + " " + lease.hostname;

					s->addWithDescription(title, lease.vendor,
						std::make_shared<TextComponent>(window,lease.mac, font, color),
						[this, lease]
					{
						openDHCPclient(lease);
					}, "fa-server");
				}
		}

		window->pushGui(s);

	}
void GuiMenu::openARPrecord(ARPcli cli)
	{
		Window *window = mWindow;
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		auto s = new GuiSettings(mWindow, cli.ip.c_str());
		s->addGroup(_("INFO"));
			s->addWithLabel(_("IP"), std::make_shared<TextComponent>(mWindow, cli.ip, font, color));
			s->addWithLabel(_("MAC"), std::make_shared<TextComponent>(mWindow, cli.mac, font, color));
			s->addWithLabel(_("VENDOR"), std::make_shared<TextComponent>(mWindow, cli.vendor, font, color));
			s->addGroup(_("TOOLS"));
				s->addEntry("PING", true, [this, window, cli] {
					pingIP(cli.ip);
				}, "fa-toolbox");
				s->addEntry("HOSTNAME", true, [this, window, cli] {
					std::string cmd = "avahi-resolve -a " + cli.ip + " | awk '{print $2}'";
					msgExec(cmd);
				}, "fa-toolbox");
				s->addEntry("NMAP", true, [this, window, cli] {
					std::string cmd = "nmap " + cli.ip;
					msgExec(cmd);
				}, "fa-toolbox");

		mWindow->pushGui(s);
	}

std::vector<AVAHIserviceDetail> GuiMenu::getAvahiService(std::string service)
		{

			std::string cmd = "avahi-browse -d local ";
									cmd+= service;
									cmd+= " -t -r -p -l | grep -a '=;'";
			if(SystemConf::getInstance()->get("pe_scanv4.enabled") == "1")
				{
					cmd+= " | grep -a 'IPv4'";
				}
			std::vector<std::string> rawServices = ApiSystem::getInstance()->getScriptResults(cmd);
			std::vector<AVAHIserviceDetail> list;
			for(auto s : rawServices)
				{
					AVAHIserviceDetail service(s);
					list.push_back(service);
				}
			return list;
		}

std::vector<AVAHIservice> GuiMenu::getAvahiServices()
		{
			bool v4 = SystemConf::getInstance()->get("pe_scanv4.enabled") == "1";

			std::string cmd = "avahi-browse -a -t -p -l";
			if(SystemConf::getInstance()->get("pe_scanv4.enabled") == "1")
				{
					cmd+= " | grep -a 'IPv4'";
				}
			std::vector<std::string> rawServices = ApiSystem::getInstance()->getScriptResults(cmd);
			std::vector<AVAHIservice> list;

			for(auto s : rawServices)
				{
					AVAHIservice service(s);
					list.push_back(service);
				}
			return list;
		}

std::vector<ARPcli> GuiMenu::getARPclients()
	{
		std::vector<std::string> rawClients = ApiSystem::getInstance()->getScriptResults("arp-scan -l -x -q -F '${mac};${ip}' | uniq");
		std::vector<ARPcli> list;
		for(auto _cli : rawClients)
			{
				ARPcli cli(_cli);
				cli.vendor = macVendor(cli.mac);
				list.push_back(cli);
			}
		return list;
	}

void GuiMenu::openARPlist(std::vector<ARPcli> list)
{
	Window *window = mWindow;
	auto theme = ThemeData::getMenuTheme();
	std::shared_ptr<Font> font = theme->Text.font;
	unsigned int color = theme->Text.color;

	auto s = new GuiSettings(mWindow, (_("ARP LIST") + " ("+std::to_string(list.size())+")").c_str());

	for(auto cli : list)
		{
			s->addWithDescription(cli.mac, cli.vendor,
				std::make_shared<TextComponent>(window, cli.ip, font, color),
				[this, window, cli]
			{
				openARPrecord(cli);
			});
		}

	mWindow->pushGui(s);
}

void GuiMenu::openAvahiList(std::vector<AVAHIservice> list)
	{
		Window *window = mWindow;
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		auto s = new GuiSettings(mWindow, (_("AVAHI SERVICES LIST") + " ("+std::to_string(list.size())+")").c_str());

		for(auto service : list)
			{
				s->addWithDescription(service.serviceID, service.service,
					std::make_shared<TextComponent>(window, service.ipv, font, color),
					[this, window, service]
				{
						window->pushGui(new GuiLoading<std::vector<AVAHIserviceDetail>>(window, _("Loading..."),
							[this, window, service](auto gui)
							{
								mWaitingLoad = true;
								return getAvahiService(service.serviceID);
							},
							[this, window](std::vector<AVAHIserviceDetail> list)
							{
								mWaitingLoad = false;
								if(list.size() > 0)
								{
									openAvahiDetailList(list);
								}
								else
								{
									window->pushGui(new GuiMsgBox(window, _("EMPTY LIST!"),_("OK"),nullptr));
								}
							}
						));
				});
			}

		mWindow->pushGui(s);
	}

void GuiMenu::openAvahiDetailList(std::vector<AVAHIserviceDetail> list)
	{
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;
		Window *window = mWindow;

		auto s = new GuiSettings(mWindow, ( list.at(0).serviceID + " ("+std::to_string(list.size())+")").c_str());

		for(auto service : list)
			{
				s->addWithDescription("(" + service.ipv + ") " + service.ip, service.hostname + ": " + service.service,
					std::make_shared<TextComponent>(window, service.port, font, color),
					[this, window, service]
					{
						openAvahiDetail(service);
					});
			}

		mWindow->pushGui(s);
	}

void GuiMenu::openAvahiDetail(AVAHIserviceDetail service)
	{
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;
		Window *window = mWindow;

		auto s = new GuiSettings(mWindow, service.service.c_str());
		s->addGroup(_("INFORMATION"));
			s->addWithLabel(_("HOSTNAME"), std::make_shared<TextComponent>(window, service.hostname, font, color));
			s->addWithLabel(_("IP, PORT"), std::make_shared<TextComponent>(window, service.ip + ": " + service.port, font, color));
			s->addWithLabel(_("ID"), std::make_shared<TextComponent>(window, service.serviceID, font, color));
		if(service.details.size() > 0)
			{
				s->addGroup(_("SERVICE DETAILS"));
				for(auto detail : service.details)
					{
						if(detail.value.empty())
							{
								s->addEntry(detail.key, false, nullptr);
							}
						else
							{
								s->addWithLabel(detail.key, std::make_shared<TextComponent>(window, detail.value, font, color));
							}
					}
			}
		mWindow->pushGui(s);
	}

void GuiMenu::openNetworkTools()
	{
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;

		Window *window = mWindow;

		if (ApiSystem::getInstance()->getIpAdress() == "NOT CONNECTED")
			{
				mWindow->pushGui(new GuiMsgBox(mWindow, _("YOU ARE NOT CONNECTED TO A NETWORK"), _("OK"), nullptr));
				return;
			}

		//std::string gameApMode = apInlineInfo("gameapmode");

		auto s = new GuiSettings(mWindow, _("NETWORK TOOLS").c_str());
		//ffmpeg -f fbdev -re -i /dev/fb0 -framerate 20 -listen 1 -f mp4 -preset veryfast -b:v 320k -maxrate 320k -bufsize 1280k -movflags frag_keyframe+empty_moov -vf "transpose=1,mpdecimate,setpts=N/FRAME_RATE/TB" http://127.0.0.1:8080

		s->addEntry(_("CHROMECAST").c_str(), true, [this] { loadChromecast(mWindow); }, "fa-chromecast");

		s->addEntry(_("ARP-SCAN"), true, [this, window]() {
			window->pushGui(new GuiLoading<std::vector<ARPcli>>(window, _("Loading..."),
				[this, window](auto gui)
				{
					mWaitingLoad = true;
					return getARPclients();
				},
				[this, window](std::vector<ARPcli> list)
				{
					mWaitingLoad = false;
					if(list.size() > 0)
					{
						openARPlist(list);
					}
					else
					{
						window->pushGui(new GuiMsgBox(window, _("EMPTY LIST!"),_("OK"),nullptr));
					}
				}
			));
		});

		s->addEntry(_("AVAHI-BROWSE"), true, [this, window]() {
			window->pushGui(new GuiLoading<std::vector<AVAHIservice>>(window, _("Loading..."),
				[this, window](auto gui)
				{
					mWaitingLoad = true;
					return getAvahiServices();
				},
				[this, window](std::vector<AVAHIservice> list)
				{
					mWaitingLoad = false;
					if(list.size() > 0)
					{
						openAvahiList(list);
					}
					else
					{
						window->pushGui(new GuiMsgBox(window, _("EMPTY LIST!"),_("OK"),nullptr));
					}
				}
			));
		});

			s->addEntry(_("NSLOOKUP"), true, [this, window]() {
				if (Settings::getInstance()->getBool("UseOSK"))
					mWindow->pushGui(new GuiTextEditPopupKeyboard(window, "ENTER LOOKUP ADDRESS", "", [this](const std::string& value) { const std::string cmd = "nslookup " + value; msgExec(cmd); }, false));
				else
					mWindow->pushGui(new GuiTextEditPopup(window, "ENTER LOOKUP ADDRESS", "", [this](const std::string& value) { const std::string cmd = "nslookup " + value; msgExec(cmd); }, false));
			});

			s->addEntry(_("TRACEROUTE"), true, [this, window]() {
				if (Settings::getInstance()->getBool("UseOSK"))
					mWindow->pushGui(new GuiTextEditPopupKeyboard(window, "ENTER ADDRESS", "8.8.8.8", [this](const std::string& value) { traceroute(value); }, false));
				else
					mWindow->pushGui(new GuiTextEditPopup(window, "ENTER ADDRESS", "8.8.8.8", [this](const std::string& value) { traceroute(value); }, false));
			});

		mWindow->pushGui(s);
	}

void GuiMenu::traceroute(std::string addr)
	{
		Window* window = mWindow;
		mWindow->pushGui(new GuiLoading<std::vector<TraceRouteHop>>(window, _("TRACING") + " " + addr,
			[this, window, addr](auto gui)
			{
				mWaitingLoad = true;
				//std::string cmd = "traceroute -w 1 -q 1 " + addr + " | awk '{print $1\";\"$2\";\"$3\";\"$4}'";
				Traceroute hops(ApiSystem::getInstance()->getScriptResults("traceroute -w 1 -q 1 " + addr + " | awk '{print $1\";\"$2\";\"$3\";\"$4}'"));
				return hops.hops;
			},
			[this, window, addr](std::vector<TraceRouteHop> hops)
			{
				mWaitingLoad = false;
				if(hops.size() > 0)
				{
					openTraceroute(addr, hops);
				}
				else
				{
					window->pushGui(new GuiMsgBox(window, _("EMPTY LIST!"),_("OK"),nullptr));
				}
			}
		));
	}
void GuiMenu::openTraceroute(std::string addr, std::vector<TraceRouteHop> hops)
	{
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;
		Window *window = mWindow;
		auto s = new GuiSettings(mWindow, (_("TRACEROUTE RESULT") + " " + addr + " (" + std::to_string(hops.size()) + "hops)").c_str());
		for(auto hop : hops)
			{
				if(hop.isValid)
					{
						s->addWithDescription(hop.hop + ": " + hop.ip + (hop.host != hop.ip ? " (" + hop.host + ")" : ""), "",
							std::make_shared<TextComponent>(window, hop.timeout ? "TIMEOUT!" : hop.dur + "ms", font, color),
							[this, hop]
						{
								pingIP(hop.ip);
						});
					}
			}
		mWindow->pushGui(s);
	}

// YOUTUBE
void GuiMenu::YouTube()
	{
		YouTubeLoad();

		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;
		Window *window = mWindow;
		std::string Title = _U("\uf16a");
								Title+= " YouTube";
		auto s = new GuiSettings(mWindow, Title);
		/*s->addEntry(_("SEARCH"), true, [this](){
			YouTubeSearchMenu();
		}, "fa-search");*/

		std::string sTitle = _U("\uE12f"); //Y... lupa _U("\uF002");
								sTitle+= "  ";
								sTitle+= _("SEARCH");
		std::string shTitle = _U("\uE12E"); //X... lupa _U("\uF002");
								shTitle+= "  ";
								shTitle+= _("SEARCH HISTORY");
		if (ApiSystem::getInstance()->getIpAdress() == "NOT CONNECTED")
			{
				std::string sTitle = _U("\uE12f"); //Y... lupa _U("\uF002");
										sTitle+= "  ";
										sTitle+= _("NETWORK SETTINGS");
				s->addButton(sTitle, _("network settings"), [this] {
					openNetworkSettings();
				});

				s->mapYcallback([this] {
					openNetworkSettings();
				});
			}
		else
			{
				s->addButton(sTitle, _("search"), [this] {
					YouTubeSearchKeyboard();
				});

				s->addButton(shTitle, _("search history"), [this] {
					YouTubeSearchMenu();
				});

				s->mapYcallback([this] {
					YouTubeSearchKeyboard();
				});
				s->mapXcallback([this] {
					YouTubeSearchMenu();
				});
			}

		s->addGroup(_("RECENTLY PLAYED"));
		for(auto link : YouTubeLastPlayed)
			{
				YTResultRow(window, s, link);
			}

		window->pushGui(s);
	}
void GuiMenu::YouTubeLoad()
	{
		YouTubeSearchHistory.clear();
			for(auto shistory : ApiSystem::getInstance()->getScriptResults("youtube.sh getShistory"))
				{
					YouTubeSearchHistory.push_back(shistory);
				}

		YouTubeLastPlayed.clear();
			for(auto phistory : ApiSystem::getInstance()->getScriptResults("youtube.sh getPhistory"))
				{
					std::string json = "";
					int t = 0;
					for(auto token : Utils::String::split(phistory, ';'))
						{
							if(t > 0)
								{
									json += token;
								}
							t++;
						}
					YouTubeLastPlayed.push_back(YoutubeLink(json));
				}
	}
void GuiMenu::YouTubeSearchKeyboard(){
	Window *window = mWindow;
	std::string sTitle = _U("\uF002");
							sTitle+= " ";
							sTitle+= _("SEARCH");
	if (Settings::getInstance()->getBool("UseOSK"))
		mWindow->pushGui(new GuiTextEditPopupKeyboard(window, sTitle, "", [this](const std::string& value) { YTJsonSearch(value); }, false));
	else
		mWindow->pushGui(new GuiTextEditPopup(window, sTitle, "", [this](const std::string& value) { YTJsonSearch(value); }, false));
}

void GuiMenu::YouTubeSearchMenu()
	{
		Window *window = mWindow;
		std::string Title = _U("\uf16a");
								Title+= " ";
								Title+= _("SEARCH HISTORY");
		auto s = new GuiSettings(mWindow, Title);

		std::string qTitle = _U("\uE12f"); //Y... lupa _U("\uF002");
								qTitle+= "  ";
								qTitle+= _("NEW SEARCH");

		s->addButton(qTitle, _("search"), [this] {
			YouTubeSearchKeyboard();
		});

		s->mapYcallback([this] {
			YouTubeSearchKeyboard();
		});

		/*s->addEntry(_("NEW SEARCH"), true, [this, window, Title]() {
			if (Settings::getInstance()->getBool("UseOSK"))
				mWindow->pushGui(new GuiTextEditPopupKeyboard(window, Title + " search", "", [this](const std::string& value) { YTJsonSearch(value); }, false));
			else
				mWindow->pushGui(new GuiTextEditPopup(window, Title + " search", "", [this](const std::string& value) { YTJsonSearch(value); }, false));
		}, "fa-search");*/

		for(auto item : YouTubeSearchHistory)
			{
				s->addEntry(item, false, [this, window, item]() {
					YTJsonSearch(item);
				});
			}

		window->pushGui(s);
	}

void GuiMenu::YTJsonSearch(std::string q, int maxResults)
	{
		Window* window = mWindow;
		//std::vector<std::string> r = ApiSystem::getInstance()->getScriptResults("youtube.sh shistory \"" + q + "\"");

		mWindow->pushGui(new GuiLoading<std::vector<std::string>>(window, _("SEARCHING...") + "\n" + q,
			[this, window, q, maxResults](auto gui)
			{
				mWaitingLoad = true;
				return ApiSystem::getInstance()->getScriptResults("youtube.sh search \"" + q + "\"");
			},
			[this, window, q](std::vector<std::string> links)
			{
				mWaitingLoad = false;
				if(links.size() == 0)
					{
						window->pushGui(new GuiMsgBox(window, q + "\n" + _("NOT FOUND"),_("OK"),nullptr));
					}
				else
					{
						std::vector<YoutubeLink> Links;
						for(auto json : links)
							{
								YoutubeLink yt_item(json);
								if(!yt_item.link.empty())
									{
										Links.push_back(yt_item);
									}
							}
						YouTubeLoad();
						YTResults(Links, q);
					}
			}
		));
	}

void GuiMenu::YTResultRow(Window* window, GuiSettings* s, YoutubeLink link)
	{
		float w = !link.thumbnails.size() ? -1 : link.thumbnails.at(0).width;
		float h = !link.thumbnails.size() ? -1 : link.thumbnails.at(0).height;

		float minifier = (w > h) ? 96 / w : 96 / h;

		Vector2f maxSize(w * minifier, h * minifier);

		auto icon = std::make_shared<WebImageComponent>(window, 600); // image expire after 10 minutes
		icon->setImage(!link.thumbnails.size() ? "" : link.thumbnails.at(0).url, false, maxSize);
		icon->setMaxSize(maxSize);
		//icon->setSize(maxSize);
		icon->setMinSize(maxSize);
		icon->setUpdateColors(false);
		//icon->setPadding(4);
		s->addWithDescription(
				link.title,
				link.duration_string,
				icon,
				[this, window, link]
					{
						mWindow->pushGui(new GuiLoading<std::string>(window, _("Running..."),
						 [this, window, link](auto gui)
						 {
							 mWaitingLoad = true;
							 return getShOutput("sleep 0.5; echo 1");
						 },
						 [this, window, link](std::string l)
						 {
							 mWaitingLoad = false;
							 appLauncher("youtube.sh play " + link.link);
							 std::vector<std::string> r = ApiSystem::getInstance()->getScriptResults("youtube.sh phistory \"" + link.id + "\" \"" + Utils::String::replace(link.json, "\"", "\\\"") + "\"");
							 YouTubeLoad();
						 }));
							 	//YTResult(link);
					}
		);
	}
void GuiMenu::YTResults(std::vector<YoutubeLink> links, std::string search)
	{
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;
		Window *window = mWindow;

		std::string Title = _U("\uf16a");
								Title+= " YouTube";

		auto s = new GuiSettings(mWindow, Title);
		if(!search.empty())
			{
				s->setSubTitle(search);
			}
		for(auto link : links)
			{
				YTResultRow(window, s, link);
			}
		mWindow->pushGui(s);
	}

void GuiMenu::YTResult(YoutubeLink link)
	{
		auto theme = ThemeData::getMenuTheme();
		std::shared_ptr<Font> font = theme->Text.font;
		unsigned int color = theme->Text.color;
		Window *window = mWindow;
		auto s = new GuiSettings(mWindow, link.title);
			s->addWithDescription(_("PLAY"), "",
				nullptr,
				[this, window, link]
			{
				mWindow->pushGui(new GuiLoading<std::string>(window, _("Running..."),
				 [this, window, link](auto gui)
				 {
					 mWaitingLoad = true;
					 return getShOutput("sleep 0.5; echo 1");
				 },
				 [this, window, link](std::string l)
				 {
					 mWaitingLoad = false;
					 appLauncher("youtube.sh play " + link.link);
					 std::vector<std::string> r = ApiSystem::getInstance()->getScriptResults("youtube.sh phistory \"" + link.id + "\" \"" + Utils::String::replace(link.json, "\"", "\\\"") + "\"");
					 YouTubeLoad();
				 }));
			}, "fa-play", true);

			/*s->addEntry(_("CAST"), false, [this, window, link]{
				loadChromecast(window, "YouTube/"+link.id);
			}, "fa-chromecast");*/

			s->addEntry(_("DOWNLOAD"), false, [this, window, link]{
				// download menu
			}, "fa-download");

		window->pushGui(s);
	}
// EOF YOUTUBE


void GuiMenu::openNetworkSettings(bool selectWifiEnable)
{
	bool baseWifiEnabled = SystemConf::getInstance()->getBool("wifi.enabled");

	auto theme = ThemeData::getMenuTheme();
	std::shared_ptr<Font> font = theme->Text.font;
	unsigned int color = theme->Text.color;

	Window *window = mWindow;
	std::string gameApMode = apInlineInfo("gameapmode");

	auto s = new GuiSettings(mWindow, _("NETWORK SETTINGS").c_str());

	s->addEntry(_("RECONNECT TO SAVED NETWORK"), false, [s, this, window]() {
			std::string msg = _("RECONNECT TO SAVED NETWORK") + "?";
			window->pushGui(new GuiMsgBox(window, msg,
				 _("YES"),[s, this]{
					 	const std::string baseSSID = SystemConf::getInstance()->get("wifi.ssid");
 						const std::string baseKEY = SystemConf::getInstance()->get("wifi.key");

					runSystemCommand("ap.sh stop \"" + baseSSID + "\" \"" + baseKEY + "\"", "", nullptr);
					delete s;
					openNetworkSettings();
				 },
				 _("NO"), nullptr));
		}, "fa-link");

	s->addGroup(_("INFORMATION"));


	// Wifi enable
	auto enable_wifi = std::make_shared<SwitchComponent>(mWindow);
	enable_wifi->setState(baseWifiEnabled);
	s->addWithLabel(_("ENABLE WIFI"), enable_wifi, selectWifiEnable);

#if !WIN32
	// Hostname
	s->addInputTextRow(_("HOSTNAME"), "system.hostname", false);
#endif

	auto ip = std::make_shared<TextComponent>(mWindow, ApiSystem::getInstance()->getIpAdress(), font, color);
	s->addWithLabel(_("IP ADDRESS"), ip);


	// Network Indicator
	auto networkIndicator = std::make_shared<SwitchComponent>(mWindow);
	networkIndicator->setState(Settings::getInstance()->getBool("ShowNetworkIndicator"));
	s->addWithLabel(_("SHOW NETWORK INDICATOR"), networkIndicator);
	s->addSaveFunc([networkIndicator] { Settings::getInstance()->setBool("ShowNetworkIndicator", networkIndicator->getState()); });

	const std::string wlMode = apInlineInfo("mode");
	s->addWithLabel(_("MODE"), std::make_shared<TextComponent>(mWindow, wlMode, font, color));

	//auto status = std::make_shared<TextComponent>(mWindow, ApiSystem::getInstance()->ping() ? _("CONNECTED") : _("NOT CONNECTED"), font, color);
	//s->addWithLabel(_("INTERNET STATUS"), status);

	bool wlModeAP = wlMode == "AP";


	if(gameApMode == "off") // kdyz je gameApMode AP stejne vraci off?!?!
	{
		s->addGroup(_("GAME AP"));

		s->addEntry(_("CREATE GAME AP"), false, [s, this, window]() {
				std::string msg = _("REALLY START AP MODE?");
				window->pushGui(new GuiMsgBox(window, msg,
					 _("YES"),[s, this]{
					 	runSystemCommand("ap.sh startgameap", "", nullptr);
					 	delete s;
						openNetworkSettings();
					 },
					 _("NO"), nullptr));
			}, "fa-router");

			s->addEntry(_("CONNECT TO GAME AP"), false, [this]() {
					searchGameAP();
			}, "fa-link");
	}

	if(gameApMode == "host")
	{
		s->addGroup(_("GAME AP"));

		s->addEntry(_("DISABLE GAME AP"), false, [s, this, window]() {
				std::string msg = _("REALLY STOP AP MODE?");
				window->pushGui(new GuiMsgBox(window, msg,
					 _("YES"),[s, this]{
					 	runSystemCommand("ap.sh stop", "", nullptr);
					 	delete s;
						openNetworkSettings();
					 },
					 _("NO"), nullptr));
			}, "fa-circle-stop");
	}

	if(gameApMode == "cli")
	{
		s->addGroup(_("GAME AP"));

		s->addWithLabel(_("CONNECTED AS CLIENT"), std::make_shared<TextComponent>(mWindow, apInlineInfo("ssid"), font, color));
	}


	if(gameApMode != "cli")
		{
			s->addGroup(_("SETTINGS"));
		}

	// window, title, settingstring,
	const std::string baseSSID = SystemConf::getInstance()->get("wifi.ssid");
	const std::string baseKEY = SystemConf::getInstance()->get("wifi.key");

	if (baseWifiEnabled && !wlModeAP && gameApMode != "cli")
	{
		s->addInputTextRow(_("WIFI SSID"), "wifi.ssid", false, false, &openWifiSettings);
		s->addInputTextRow(_("WIFI KEY"), "wifi.key", true);
	}
	else if(wlModeAP)
	{
		auto apSSID = std::make_shared<TextComponent>(mWindow, apInlineInfo("ssid"), font, color);
		auto apPWD = std::make_shared<TextComponent>(mWindow, apInlineInfo("passwd"), font, color);
		s->addWithLabel(_("AP SSID"), apSSID);
		s->addWithLabel(_("AP KEY"), apPWD);

		s->addEntry(_("DHCP LEASES") + " (" +apInlineInfo("clients") + ")", true, [this, window]() {
			openAPleases();
		});
	}

	s->addSaveFunc([baseWifiEnabled, baseSSID, baseKEY, enable_wifi, window]
	{
		bool wifienabled = enable_wifi->getState();

		SystemConf::getInstance()->setBool("wifi.enabled", wifienabled);

		if (wifienabled)
		{
			std::string newSSID = SystemConf::getInstance()->get("wifi.ssid");
			std::string newKey = SystemConf::getInstance()->get("wifi.key");

			if (baseSSID != newSSID || baseKEY != newKey || !baseWifiEnabled)
			{
				if (ApiSystem::getInstance()->enableWifi(newSSID, newKey))
					window->pushGui(new GuiMsgBox(window, _("WIFI ENABLED")));
				else
					window->pushGui(new GuiMsgBox(window, _("WIFI CONFIGURATION ERROR")));
			}
		}
		else if (baseWifiEnabled)
			ApiSystem::getInstance()->disableWifi();
	});


	enable_wifi->setOnChangedCallback([this, s, baseWifiEnabled, enable_wifi]()
	{
		bool wifienabled = enable_wifi->getState();
		if (baseWifiEnabled != wifienabled)
		{
			SystemConf::getInstance()->setBool("wifi.enabled", wifienabled);

			if (wifienabled)
				ApiSystem::getInstance()->enableWifi(SystemConf::getInstance()->get("wifi.ssid"), SystemConf::getInstance()->get("wifi.key"));
			else
				ApiSystem::getInstance()->disableWifi();

			delete s;
			openNetworkSettings(true);
		}
	});

	mWindow->pushGui(s);
}

void GuiMenu::openQuitMenu()
{
  GuiMenu::openQuitMenu_static(mWindow);
}

void GuiMenu::openQuitMenu_static(Window *window, bool quickAccessMenu, bool animate)
{
#ifdef WIN32
	if (!quickAccessMenu && Settings::getInstance()->getBool("ShowOnlyExit"))
	{
		quitES(QuitMode::QUIT);
		return;
	}
#endif

	auto s = new GuiSettings(window, (quickAccessMenu ? _("QUICK ACCESS") : _("QUIT")).c_str());
	s->setCloseButton("select");

	if (quickAccessMenu)
	{
		s->addGroup(_("QUICK ACCESS"));

		// Don't like one of the songs? Press next
		if (AudioManager::getInstance()->isSongPlaying())
		{
			auto sname = AudioManager::getInstance()->getSongName();
			if (!sname.empty())
			{
				s->addWithDescription(_("SKIP TO THE NEXT SONG"), _("NOW PLAYING") + ": " + sname, nullptr, [s, window]
				{
					Window* w = window;
					AudioManager::getInstance()->playRandomMusic(false);
					delete s;
					openQuitMenu_static(w, true, false);
				}, "fa-volume-high");
			}
		}

		s->addEntry(_("LAUNCH SCREENSAVER"), false, [s, window]
		{
			Window* w = window;
			window->postToUiThread([w]()
			{
				w->startScreenSaver();
				w->renderScreenSaver();
			});
			delete s;

		}, "fa-photo-film", true);

		if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::ScriptId::PDFEXTRACTION) && Utils::FileSystem::exists(Paths::getUserManualPath()))
		{
			s->addEntry(_("VIEW USER MANUAL"), false, [s, window]
			{
				GuiImageViewer::showPdf(window, Paths::getUserManualPath());
				delete s;
			}, "iconManual");
		}

		auto rumble = std::make_shared<SwitchComponent>(window);
		rumble->setState(getShOutput("rumble.sh") == "on");
		s->addWithLabel(_("ENABLE RUMBLE"), rumble, false, "fa-drum");
		s->addSaveFunc([rumble] {
			if (rumble->changed()) {
				bool enabled = rumble->getState();
				const std::string cmd = std::string("rumble.sh ") + (enabled ? std::string("on") : std::string("off"));
				runSystemCommand(cmd, "", nullptr);
			}
		});
	}

#ifdef _ENABLEEMUELEC
	s->addEntry(_("RESTART EMULATIONSTATION"), false, [window] {
		window->pushGui(new GuiMsgBox(window, _("REALLY RESTART EMULATIONSTATION?"), _("YES"),
			[] {
    		   /*runSystemCommand("systemctl restart emustation.service", "", nullptr);*/
    		   Scripting::fireEvent("quit", "restart");
			   quitES(QuitMode::QUIT);
		}, _("NO"), nullptr));
	}, "fa-rotate");


	bool isFullUI = UIModeController::getInstance()->isUIModeFull();
	if (isFullUI)
	{
		/*s->addEntry(_("START RETROARCH"), false, [window] {
			window->pushGui(new GuiMsgBox(window, _("REALLY START RETROARCH?"), _("YES"),
				[] {
				remove("/var/lock/start.games");
				runSystemCommand("touch /var/lock/start.retro", "", nullptr);
				runSystemCommand("systemctl start retroarch.service", "", nullptr);
				Scripting::fireEvent("quit", "retroarch");
				quitES(QuitMode::QUIT);
			}, _("NO"), nullptr));
		}, "fa-rocket");

		s->addEntry(_("REBOOT FROM NAND"), false, [window] {
			window->pushGui(new GuiMsgBox(window, _("REALLY REBOOT FROM NAND?"), _("YES"),
				[] {
				Scripting::fireEvent("quit", "nand");
				runSystemCommand("rebootfromnand", "", nullptr);
				runSystemCommand("sync", "", nullptr);
				runSystemCommand("systemctl reboot", "", nullptr);
				quitES(QuitMode::QUIT);
			}, _("NO"), nullptr));
		}, "fa-microchip");*/
	}
#endif

	if (quickAccessMenu)
		s->addGroup(_("QUIT"));

	s->addEntry(_("RESTART SYSTEM"), false, [window] {
		window->pushGui(new GuiMsgBox(window, _("REALLY RESTART?"),
			_("YES"), [] { quitES(QuitMode::REBOOT); },
			_("NO"), nullptr));
	}, "fa-rotate");

	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::SUSPEND))
	{
		s->addEntry(_("SUSPEND SYSTEM"), false, [window, s] {
			window->pushGui(new GuiMsgBox(window, _("REALLY SUSPEND ?"),
				_("YES"), [s] { s->close(); ApiSystem::getInstance()->suspend(); },
				_("NO"), nullptr));
		}, "iconFastShutdown");
	}

	s->addEntry(_("SHUTDOWN SYSTEM"), false, [window] {
		window->pushGui(new GuiMsgBox(window, _("REALLY SHUTDOWN?"),
			_("YES"), [] { quitES(QuitMode::SHUTDOWN); },
			_("NO"), nullptr));
	}, "fa-power-off");

#ifndef _ENABLEEMUELEC
	s->addWithDescription(_("FAST SHUTDOWN SYSTEM"), _("Shutdown without saving metadata."), nullptr, [window] {
		window->pushGui(new GuiMsgBox(window, _("REALLY SHUTDOWN WITHOUT SAVING METADATA?"),
			_("YES"), [] { quitES(QuitMode::FAST_SHUTDOWN); },
			_("NO"), nullptr));
	}, "iconFastShutdown");
#endif

#ifdef WIN32
	if (Settings::getInstance()->getBool("ShowExit"))
	{
		s->addEntry(_("QUIT EMULATIONSTATION"), false, [window] {
			window->pushGui(new GuiMsgBox(window, _("REALLY QUIT?"),
				_("YES"), [] { quitES(QuitMode::QUIT); },
				_("NO"), nullptr));
		}, "iconQuit");
	}
#endif

	if (quickAccessMenu && animate)
		s->getMenu().animateTo(Vector2f((Renderer::getScreenWidth() - s->getMenu().getSize().x()) / 2, (Renderer::getScreenHeight() - s->getMenu().getSize().y()) / 2));
	else if (quickAccessMenu)
		s->getMenu().setPosition((Renderer::getScreenWidth() - s->getMenu().getSize().x()) / 2, (Renderer::getScreenHeight() - s->getMenu().getSize().y()) / 2);

	window->pushGui(s);
}

void GuiMenu::createDecorationItemTemplate(Window* window, std::vector<DecorationSetInfo> sets, std::string data, ComponentListRow& row)
{
	Vector2f maxSize(Renderer::getScreenWidth() * 0.14, Renderer::getScreenHeight() * 0.14);

	int IMGPADDING = Renderer::getScreenHeight()*0.01f;

	auto theme = ThemeData::getMenuTheme();
	std::shared_ptr<Font> font = theme->Text.font;
	unsigned int color = theme->Text.color;

	// spacer between icon and text
	auto spacer = std::make_shared<GuiComponent>(window);
	spacer->setSize(IMGPADDING, 0);
	row.addElement(spacer, false);

	std::string label = data;
	if (data.empty())
		label = _("AUTO");
	else if (data == "none")
		label = _("NONE");
	else
		label = Utils::String::toUpper(Utils::String::replace(data, "_", " "));

	row.addElement(std::make_shared<TextComponent>(window, label, font, color, ALIGN_LEFT), true);

	std::string imageUrl;

	for (auto set : sets)
		if (set.name == data)
			imageUrl = set.imageUrl;

	// image
	if (!imageUrl.empty())
	{
		auto icon = std::make_shared<ImageComponent>(window);
		icon->setImage(imageUrl, false, maxSize);
		icon->setMaxSize(maxSize);
		icon->setColorShift(theme->Text.color);
		icon->setPadding(IMGPADDING);
		row.addElement(icon, false);
	}
}

void GuiMenu::popSystemConfigurationGui(Window* mWindow, SystemData* systemData)
{
	popSpecificConfigurationGui(mWindow,
		systemData->getFullName(),
		systemData->getName(),
		systemData,
		nullptr);
}

void GuiMenu::popGameConfigurationGui(Window* mWindow, FileData* fileData)
{
	popSpecificConfigurationGui(mWindow,
		fileData->getName(),
		fileData->getConfigurationName(),
		fileData->getSourceFileData()->getSystem(),
		fileData);
}

// TODO

#ifdef _ENABLEEMUELEC

std::shared_ptr<OptionListComponent<std::string>> GuiMenu::btn_choice = nullptr;
std::shared_ptr<OptionListComponent<std::string>> GuiMenu::del_choice = nullptr;
std::shared_ptr<OptionListComponent<std::string>> GuiMenu::edit_choice = nullptr;

std::shared_ptr<OptionListComponent<std::string>> GuiMenu::createJoyBtnRemapOptionList(Window *window, std::string prefixName, int btnIndex)
{
	auto btn_choice = std::make_shared< OptionListComponent<std::string> >(window, _("BUTTON REMAP CONFIG"), false);

	std::string joy_btns = SystemConf::getInstance()->get(prefixName + ".joy_btns");

	if (joy_btns.empty()) {
		btn_choice->add("NONE", "-1", true);
		return btn_choice;
	}

	std::vector<std::string> arr_joy_btn(explode(joy_btns));

	int index = 0;
	btn_choice->add("NONE", "-1", false);
	for (auto it = arr_joy_btn.cbegin(); it != arr_joy_btn.cend(); it++) {
		btn_choice->add(*it, std::to_string(index), btnIndex == index);
		index++;
	}
	return btn_choice;
}

void GuiMenu::editJoyBtnRemapOptionList(Window *window, GuiSettings *systemConfiguration, std::string prefixName)
{
	const std::function<void()> editFunc([window, systemConfiguration, del_choice, edit_choice, prefixName] {
		int editIndex = edit_choice->getSelectedIndex();
		if (editIndex <= 0)
			return;

		std::string btnIndexes = SystemConf::getInstance()->get(prefixName + ".joy_btn_indexes");
		int btnIndex = int_explode(btnIndexes)[editIndex-1];

		GuiSettings* systemConfiguration = new GuiSettings(window, _("EDIT REMAP"));
		GuiMenu::createBtnJoyCfgRemap(window, systemConfiguration, prefixName, edit_choice->getSelectedName(), btnIndex, editIndex);

		edit_choice->selectFirstItem();
		del_choice->selectFirstItem();

		window->pushGui(systemConfiguration);
	});

	edit_choice->setSelectedChangedCallback([window, systemConfiguration, editFunc, edit_choice, del_choice, prefixName](std::string s) {
		long unsigned int m1 = (long unsigned int) &(*window->peekGui());
		long unsigned int m2 = (long unsigned int) &(*systemConfiguration);
		if (m1 == m2)
			return;

		editFunc();
	});

	systemConfiguration->addSaveFunc([window, systemConfiguration, editFunc, edit_choice, del_choice, prefixName] {
		editFunc();
	});
}

void GuiMenu::createBtnJoyCfgRemap(Window *window, GuiSettings *systemConfiguration,
	std::string prefixName, std::string remapName, int btnIndex, int editIndex)
{
	std::vector<std::shared_ptr<OptionListComponent<std::string>>> remap_choice;

	std::string btnNames = SystemConf::getInstance()->get(prefixName + ".joy_btns");
	int btnCount = static_cast<int>(std::count(btnNames.begin(), btnNames.end(), ',')+1);

	std::string remapNames = SystemConf::getInstance()->get(prefixName + ".joy_btn_names");
	int remapCount = static_cast<int>(std::count(remapNames.begin(), remapNames.end(), ',')+1);

	std::vector<int> iOrders;
	if (btnIndex > -1)
	{
		std::string sOrder = SystemConf::getInstance()->get(prefixName + ".joy_btn_order" + std::to_string(btnIndex));
		iOrders = int_explode(sOrder, ' ');
	}

	for (int index=0; index < btnCount; ++index)
	{
		auto remap = createJoyBtnRemapOptionList(window, prefixName, (btnIndex > -1) ? iOrders[index] : index);
		remap_choice.push_back(remap);
		systemConfiguration->addWithLabel(_("JOY BUTTON ")+std::to_string(index), remap);
	}

	// Loops through remaps assigns Event to make sure no remap duplicates exist.
	for(auto it = remap_choice.cbegin(); it != remap_choice.cend(); ++it) {
		auto self = (*it);
		self->setSelectedChangedCallback([self, window, systemConfiguration, remap_choice] (std::string choice) {
			long unsigned int m1 = (long unsigned int) &(*window->peekGui());
			long unsigned int m2 = (long unsigned int) &(*systemConfiguration);
			if (m1 == m2)
				return;

			std::string choice2;
			if (choice == "-1")
				return;
			for(auto it = remap_choice.cbegin(); it != remap_choice.cend(); ++it) {
				auto remap = *it;
				choice2 = remap->getSelected();
				if (choice2 == "-1")
					continue;
				if (self != remap && choice == choice2) {
					remap->selectFirstItem();
					continue;
				}
			}
		});
	}


	systemConfiguration->addSaveFunc([window, systemConfiguration, remap_choice, del_choice, btn_choice, remapCount, prefixName, remapName, remapNames, btnCount, btnIndex, editIndex] {
		// Hack to avoid over-writing defaults.
		if (btnIndex != -1 && editIndex > 0 && editIndex <= 2)
		{
			window->pushGui(new GuiMsgBox(window,  _("CANNOT SAVE DEFAULT BUTTON MAPS."),
				_("OK")));
			edit_choice->selectFirstItem();
			return;
		}

		int err = 0;
		if (btnCount == 0)
			err = 1;

		// Loops through remap values and makes sure no conflicts or empty fields.
		[&] {
			for(auto it = remap_choice.cbegin(); it != remap_choice.cend(); ++it) {
				auto remap = *it;
				std::string choice = remap->getSelected();
				if (choice == "-1") {
					err=1;
					break;
				}
				for(auto it2 = remap_choice.cbegin(); it2 != remap_choice.cend(); ++it2) {
					auto remap2 = *it2;
					std::string choice2 = remap2->getSelected();
					if (choice2 == "-1") {
						err=1;
						return;
					}
					if (remap != remap2 && choice == choice2) {
						err=1;
						return;
					}
				}
			}
		}();

		if (err > 0)
		{
			window->pushGui(new GuiMsgBox(window, _("ERROR - Remap is not configured properly, All buttons must be assigned and no duplicates."),
			_("OK")));
			return;
		}

		const std::function<void()> addRemaps([remap_choice, btnIndex, prefixName] {
			std::string sRemap = "";
			for(auto it = remap_choice.cbegin(); it != remap_choice.cend(); ++it) {
				if (it != remap_choice.cbegin())
					sRemap += " ";
				sRemap += (*it)->getSelected();
			}
			SystemConf::getInstance()->set(prefixName + ".joy_btn_order" + std::to_string(btnIndex), sRemap);
			SystemConf::getInstance()->saveSystemConf();
		});

		if (btnIndex == -1)
		{
			window->pushGui(new GuiMsgBox(window, _("ARE YOU SURE YOU WANT TO CREATE THE REMAP?"),
				_("YES"), [addRemaps, remap_choice, del_choice, btn_choice, remapCount, prefixName, remapName, remapNames, btnCount]
			{
				int btnIndex = remapCount+1;

				std::string names = remapNames+","+remapName;
				SystemConf::getInstance()->set(prefixName + ".joy_btn_names", names);

				std::string sRemap = "";
				for(int i=0; i < btnCount; ++i) {
					if (i > 0)
						sRemap += " ";
					sRemap += remap_choice[i]->getSelected();
				}
				std::string sNewIndex = std::to_string(btnIndex);
				SystemConf::getInstance()->set(prefixName + ".joy_btn_order" + sNewIndex, sRemap);

				std::string indexes = SystemConf::getInstance()->get(prefixName + ".joy_btn_indexes");
				SystemConf::getInstance()->set(prefixName + ".joy_btn_indexes", indexes+","+sNewIndex);

				addRemaps();

				GuiMenu::addJoyBtnEntry(remapName, sNewIndex);
			}, _("NO"), nullptr));
		}
		else {
			addRemaps();
		}
	});
}

void GuiMenu::createBtnJoyCfgName(Window *window, GuiSettings *systemConfiguration,
	std::string prefixName)
{
	InputConfig* inputCfg = nullptr;
	if (InputManager::getInstance()->getNumJoysticks() > 0) {
		auto configList = InputManager::getInstance()->getInputConfigs();
		inputCfg = configList[0];
	}

	if (inputCfg == nullptr)
		return;

	auto theme = ThemeData::getMenuTheme();

	ComponentListRow row;

	auto createText = std::make_shared<TextComponent>(window, _("CREATE BUTTON REMAP"), theme->Text.font, theme->Text.color);
	row.addElement(createText, true);

	auto updateVal = [window, prefixName](const std::string& newVal)
	{
		if (newVal.empty()) return;
		if(newVal.find(',') != std::string::npos) {
			window->pushGui(new GuiMsgBox(window, _("YOU CANNOT HAVE COMMAS IN REMAP NAME"), _("OK"), nullptr));
		  return;
		}

		std::string remapNames = SystemConf::getInstance()->get(prefixName + ".joy_btn_names");
		std::vector<std::string> arr_btn_names(explode(remapNames));
		std::string newName = toupper(newVal);
		for (int i=0; i < arr_btn_names.size(); ++i) {
			std::string tName = toupper(arr_btn_names[i]);
			if (newName == tName) {
				window->pushGui(new GuiMsgBox(window, _("REMAP NAME MUST BE UNIQUE"), _("OK"), nullptr));
				return;
			}
		}

		GuiSettings* systemConfiguration = new GuiSettings(window, "CREATE REMAP");
		window->pushGui(new GuiMsgBox(window, _("All buttons must be assigned and no duplicates."), _("OK"),
		[window, systemConfiguration, prefixName, newVal] {
			GuiMenu::createBtnJoyCfgRemap(window, systemConfiguration, prefixName, newVal);
			window->pushGui(systemConfiguration);
		}));
	};

	row.makeAcceptInputHandler([window, createText, updateVal]
	{
		if (Settings::getInstance()->getBool("UseOSK"))
			window->pushGui(new GuiTextEditPopupKeyboard(window, _("REMAP NAME"), "", updateVal, false));
		else
			window->pushGui(new GuiTextEditPopup(window, _("REMAP NAME"), "", updateVal, false));
	});

	systemConfiguration->addRow(row);
}

void GuiMenu::removeJoyBtnEntry(int index) {
	del_choice->removeIndex(index);
	edit_choice->removeIndex(index);
	btn_choice->removeIndex(index);
}

void GuiMenu::addJoyBtnEntry(std::string name, std::string val) {
	del_choice->add(name, val, false);
	edit_choice->add(name, val, false);
	btn_choice->add(name, val, false);
}

void GuiMenu::deleteBtnJoyCfg(Window *window, GuiSettings *systemConfiguration,
	 std::string prefixName)
{
	const std::function<void()> saveFunc([window, btn_choice, del_choice, edit_choice, prefixName] {
		int delIndex = del_choice->getSelectedIndex();
		int remapIndex = delIndex-1;

		// Protect default maps (mk and sf).
		if (delIndex <= 2)
		{
			window->pushGui(new GuiMsgBox(window,  _("CANNOT DELETE DEFAULT BUTTON MAPS."),
				_("OK"), nullptr));
			del_choice->selectFirstItem();
			return;
		}

		// Delete does not remove the existing button maps so any game/emulator references will still work.
		std::string remapNames = SystemConf::getInstance()->get(prefixName + ".joy_btn_names");
		std::vector<std::string> remap_names(explode(remapNames));

		std::string indexes = SystemConf::getInstance()->get(prefixName + ".joy_btn_indexes");
		std::vector<int> iIndexes(int_explode(indexes));

		std::string sRemapNames = "";
		int i;
		for(i=0; i < remap_names.size(); ++i)
		{
			if (i == remapIndex)
				continue;
			if (i > 0)
				sRemapNames += ",";
			sRemapNames += remap_names[i];
		}
		SystemConf::getInstance()->set(prefixName + ".joy_btn_names", sRemapNames);

		std::string sIndexes = "";
		for(i=0; i < iIndexes.size(); ++i)
		{
			if (i == remapIndex)
				continue;
			if (i > 0)
				sIndexes += ",";
			sIndexes += std::to_string(iIndexes[i]);
		}
		SystemConf::getInstance()->set(prefixName + ".joy_btn_indexes", sIndexes);
		SystemConf::getInstance()->saveSystemConf();

		int btnIndex = btn_choice->getSelectedIndex();
		GuiMenu::removeJoyBtnEntry(delIndex);

		if (btnIndex == delIndex)
			btnIndex = 0;
		if (btnIndex >= del_choice->size() && del_choice->size() > 0)
			btnIndex--;

		del_choice->selectFirstItem();
		edit_choice->selectFirstItem();
		btn_choice->selectIndex(btnIndex);
	});

	del_choice->setSelectedChangedCallback(
		[window, systemConfiguration, saveFunc, btn_choice, del_choice, edit_choice, prefixName] (std::string s) {
		long unsigned int m1 = (long unsigned int) &(*window->peekGui());
		long unsigned int m2 = (long unsigned int) &(*systemConfiguration);
		if (m1 == m2)
			return;

		int delIndex = del_choice->getSelectedIndex();
		if (delIndex <= 0)
			return;

		window->pushGui(new GuiMsgBox(window,  _("ARE YOU SURE YOU WANT TO DELETE THE REMAP?"),
			_("YES"), saveFunc, _("NO"), nullptr));
	});

	systemConfiguration->addSaveFunc([window, saveFunc, btn_choice, del_choice, edit_choice, prefixName] {
		int delIndex = del_choice->getSelectedIndex();
		if (delIndex <= 0)
			return;

		window->pushGui(new GuiMsgBox(window,  _("ARE YOU SURE YOU WANT TO DELETE THE REMAP?"),
			_("YES"), saveFunc, _("NO"), nullptr));
	});
}

std::shared_ptr<OptionListComponent<std::string>> GuiMenu::createJoyBtnOptionList(Window *window, std::string prefixName,
	std::string title, int selectId)
{
	auto btn_cfg = std::make_shared< OptionListComponent<std::string> >(window, title, false);

	std::string btnNames = SystemConf::getInstance()->get(prefixName + ".joy_btn_names");
	std::vector<std::string> btn_names(explode(btnNames));

	std::string strIndexes = SystemConf::getInstance()->get(prefixName + ".joy_btn_indexes");
	std::vector<int> indexes(int_explode(strIndexes));

	if (prefixName == "auto" || prefixName.empty() || btn_names.size() == 0) {
		btn_cfg->add(_("NONE"), "-1", true);
		return btn_cfg;
	}

	if (selectId >= btn_names.size())
		selectId = -1;

	int i = 0;
	btn_cfg->add(_("NONE"), "-1", selectId == -1);
	for (auto it = btn_names.cbegin(); it != btn_names.cend(); it++) {
		btn_cfg->add(*it, std::to_string(indexes[i]), selectId == i);
		i++;
	}
	return btn_cfg;
}

#endif

void GuiMenu::popSpecificConfigurationGui(Window* mWindow, std::string title, std::string configName, SystemData *systemData, FileData* fileData, bool selectCoreLine)
{
	// The system configuration
	GuiSettings* systemConfiguration = new GuiSettings(mWindow, title.c_str());

	if (fileData != nullptr)
		systemConfiguration->setSubTitle(systemData->getFullName());

	std::string currentEmulator = fileData != nullptr ? fileData->getEmulator(false) : systemData->getEmulator(false);
	std::string currentCore = fileData != nullptr ? fileData->getCore(false) : systemData->getCore(false);

	if (systemData->hasEmulatorSelection())
	{
		auto emulChoice = std::make_shared<OptionListComponent<std::string>>(mWindow, _("Emulator"), false);
		emulChoice->add(_("AUTO"), "", false);
		for (auto& emul : systemData->getEmulators())
		{
			if (emul.cores.size() == 0)
				emulChoice->add(emul.name, emul.name, emul.name == currentEmulator);
			else
			{
				for (auto& core : emul.cores)
				{
					bool selected = (emul.name == currentEmulator && core.name == currentCore);

					if (emul.name == core.name)
						emulChoice->add(emul.name, emul.name + "/" + core.name, selected);
					else
						emulChoice->add(emul.name + ": " + Utils::String::replace(core.name, "_", " "), emul.name + "/" + core.name, selected);
				}
			}
		}

		if (!emulChoice->hasSelection())
			emulChoice->selectFirstItem();

		emulChoice->setSelectedChangedCallback([mWindow, title, systemConfiguration, systemData, fileData, configName, emulChoice](std::string s)
		{
			std::string newEmul;
			std::string newCore;

			auto values = Utils::String::split(emulChoice->getSelected(), '/');
			if (values.size() > 0)
				newEmul = values[0];

			if (values.size() > 1)
				newCore = values[1];

			if (fileData != nullptr)
			{
				fileData->setEmulator(newEmul);
				fileData->setCore(newCore);
			}
			else
			{
				SystemConf::getInstance()->set(configName + ".emulator", newEmul);
				SystemConf::getInstance()->set(configName + ".core", newCore);
			}

			popSpecificConfigurationGui(mWindow, title, configName, systemData, fileData);
			delete systemConfiguration;

		});

		systemConfiguration->addWithLabel(_("Emulator"), emulChoice);
	}

	auto customFeatures = systemData->getCustomFeatures(currentEmulator, currentCore);

#ifdef _ENABLEEMUELEC

	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::nativevideo))
	{
		auto videoNativeResolutionMode_choice = createNativeVideoResolutionModeOptionList(mWindow, configName);
		systemConfiguration->addWithLabel(_("NATIVE VIDEO"), videoNativeResolutionMode_choice);

		const std::function<void()> video_changed([mWindow, configName, videoNativeResolutionMode_choice] {

			std::string def_video;
			std::string video_choice = videoNativeResolutionMode_choice->getSelected();
			bool safe_video = false;

			if (video_choice == "auto")
				safe_video = true;
			else {
				for(std::stringstream ss(getShOutput(R"(/usr/bin/emuelec-utils resolutions)")); getline(ss, def_video, ','); ) {
					if (video_choice == def_video) {
						safe_video = true;
						break;
					}
				}
			}

			const std::function<void()> saveFunc([configName, videoNativeResolutionMode_choice] {
				SystemConf::getInstance()->set(configName + ".nativevideo", videoNativeResolutionMode_choice->getSelected());
				SystemConf::getInstance()->saveSystemConf();
			});

			const std::function<void()> abortFunc([configName, videoNativeResolutionMode_choice] {
				videoNativeResolutionMode_choice->selectFirstItem();
				SystemConf::getInstance()->set(configName + ".nativevideo", "");
				SystemConf::getInstance()->saveSystemConf();
			});

			if (!safe_video) {
				mWindow->pushGui(new GuiMsgBox(mWindow,  video_choice + _(" UNSAFE RESOLUTION DETECTED, CONTINUE?"),
					_("YES"), saveFunc, _("NO"), abortFunc));
			}
			else {
				saveFunc();
			}
		});

		systemConfiguration->addSaveFunc([mWindow, video_changed] {
			video_changed();
		});
	}

	std::string tEmulator = fileData != nullptr ? fileData->getEmulator(true) : systemData->getEmulator(true);
	if (tEmulator == "auto")
		tEmulator = systemData->getEmulator(true);
	if (!tEmulator.empty() && systemData->isFeatureSupported(tEmulator, currentCore, EmulatorFeatures::joybtnremap))
	{
		[&] {
			std::string prefixName = tEmulator;
			if (SystemConf::getInstance()->get(prefixName + ".joy_btns").empty() ||
					SystemConf::getInstance()->get(prefixName + ".joy_btn_names").empty() ||
					SystemConf::getInstance()->get(prefixName + ".joy_btn_indexes").empty())
			{
				SystemConf::getInstance()->set(prefixName + ".joy_btns", "input a button,input b button,input x button,input y button,input r button,input l button,input r2 button,input l2 button");
				SystemConf::getInstance()->set(prefixName + ".joy_btn_indexes", "1,2" );
        SystemConf::getInstance()->set(prefixName + ".joy_btn_names", "mk,sf" );
        SystemConf::getInstance()->set(prefixName + ".joy_btn_order0", "0 1 2 3 4 5 6 7" );
        SystemConf::getInstance()->set(prefixName + ".joy_btn_order1", "3 4 2 1 0 5 6 7" );
        SystemConf::getInstance()->set(prefixName + ".joy_btn_order2", "3 2 5 1 0 4 6 7" );
				SystemConf::getInstance()->saveSystemConf();
			}

			int btnCfgIndex = atoi(SystemConf::getInstance()->get(configName + ".joy_btn_cfg").c_str());
			std::vector<int> remapIndexes = int_explode( SystemConf::getInstance()->get(prefixName + ".joy_btn_indexes"));
			int btnId = -1;
			for (int i = 0; i < remapIndexes.size(); ++i) {
				if (btnCfgIndex == remapIndexes[i])
				{
					btnId = i;
					break;
				}
			}

			btn_choice = createJoyBtnOptionList(mWindow, prefixName, _("BUTTON REMAP"), btnId);
			edit_choice = createJoyBtnOptionList(mWindow, prefixName, _("EDIT REMAP"));
			del_choice = createJoyBtnOptionList(mWindow, prefixName, _("DELETE REMAP"));

			systemConfiguration->addWithLabel(_("BUTTON REMAP"), btn_choice);
			systemConfiguration->addWithLabel(_("EDIT REMAP"), edit_choice);
			systemConfiguration->addWithLabel(_("DELETE REMAP"), del_choice);

			systemConfiguration->addSaveFunc([btn_choice, configName, prefixName] {
				int index = atoi(btn_choice->getSelected().c_str());
				if (index == -1)
				{
					SystemConf::getInstance()->set(configName + ".joy_btn_cfg", "");
					SystemConf::getInstance()->saveSystemConf();
					return;
				}
				if (index > 0)
				{
					std::string btnIndexes = SystemConf::getInstance()->get(prefixName + ".joy_btn_indexes");

					SystemConf::getInstance()->set(configName + ".joy_btn_cfg", std::to_string(index));
					SystemConf::getInstance()->saveSystemConf();
				}
			});
			GuiMenu::editJoyBtnRemapOptionList(mWindow, systemConfiguration, prefixName);
			GuiMenu::createBtnJoyCfgName(mWindow, systemConfiguration, prefixName);
			GuiMenu::deleteBtnJoyCfg(mWindow, systemConfiguration, prefixName);
		}();
	}

	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::hlebios))
	{
		systemConfiguration->addSwitch(_("Use HLE BIOS"), configName + ".hlebios", false);
	}

// Frame Buffer Video Mode.

	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::nativevideo))
	{
		std::string ee_nativevideomode = SystemConf::getInstance()->get(configName + ".nativevideo");
		std::string fb_conf = configName+"."+ee_nativevideomode;
		std::string ee_framebuffer = SystemConf::getInstance()->get(fb_conf+".ee_framebuffer");
		if (ee_framebuffer.empty()) {
			ee_framebuffer = "auto";
		}

		std::vector<std::string> reslist;
			reslist.push_back("3840 2160");
			reslist.push_back("2560 1440");
			reslist.push_back("1920 1080");
			reslist.push_back("1280 720");
			reslist.push_back("720 480");
			reslist.push_back("768 576");
			reslist.push_back("1680 1050");
			reslist.push_back("1440 900");
			reslist.push_back("1280 1024");
			reslist.push_back("1280 960");
			reslist.push_back("1280 800");
			reslist.push_back("1024 768");
			reslist.push_back("800 600");
			reslist.push_back("640 480");
			reslist.push_back("320 240");

		int* ee_dimensions = getVideoModeDimensions(ee_nativevideomode, reslist);

		static sScreenDimensions ee_screen;
		ee_screen.width = ee_dimensions[0];
		ee_screen.height = ee_dimensions[1];

		auto emuelec_frame_buffer = std::make_shared< OptionListComponent<std::string> >(mWindow, "VIDEO MODE", false);

		emuelec_frame_buffer->add("auto", "auto", ee_framebuffer == "auto");
		for (auto it = reslist.cbegin(); it != reslist.cend(); it++) {
			std::string lbl = *it;
			lbl = lbl.replace(lbl.find(" "),1,"x");
			emuelec_frame_buffer->add(lbl, *it, ee_framebuffer == *it);
		}
		systemConfiguration->addWithLabel(_("FRAME BUFFER"), emuelec_frame_buffer);

		auto fbSave = [emuelec_frame_buffer, fb_conf, ee_screen] (std::string selectedFB) {
			if (emuelec_frame_buffer->changed()) {
				if (selectedFB == "auto")
					selectedFB = "";

				SystemConf::getInstance()->set(fb_conf+".ee_framebuffer", selectedFB);

				if (selectedFB == "") {
					SystemConf::getInstance()->set(fb_conf+".ee_offsets", "");
					return;
				}

				std::string result = "0 0 "+
					std::to_string(ee_screen.width-1)+" "+
					std::to_string(ee_screen.height-1);

				SystemConf::getInstance()->set(fb_conf+".ee_offsets", result);
			}
		};

		emuelec_frame_buffer->setSelectedChangedCallback([emuelec_frame_buffer, fbSave, fb_conf, ee_screen](std::string name)
		{
			fbSave(emuelec_frame_buffer->getSelected());
		});

		systemConfiguration->addSaveFunc([emuelec_frame_buffer, fbSave, fb_conf, ee_screen] {
			fbSave(emuelec_frame_buffer->getSelected());
		});
	}

#endif

	// Screen ratio choice
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::ratio))
	{
		auto ratio_choice = createRatioOptionList(mWindow, configName);
		systemConfiguration->addWithDescription(_("GAME ASPECT RATIO"), _("Force the game to render in this aspect ratio."), ratio_choice);
		systemConfiguration->addSaveFunc([configName, ratio_choice] { SystemConf::getInstance()->set(configName + ".ratio", ratio_choice->getSelected()); });
	}

	// video resolution mode
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::videomode))
	{
		auto videoResolutionMode_choice = createVideoResolutionModeOptionList(mWindow, configName);
		systemConfiguration->addWithDescription(_("VIDEO MODE"), _("Sets the display's resolution. Does not affect the rendering resolution."), videoResolutionMode_choice);
		systemConfiguration->addSaveFunc([configName, videoResolutionMode_choice] { SystemConf::getInstance()->set(configName + ".videomode", videoResolutionMode_choice->getSelected()); });
	}

	// smoothing
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::smooth))
	{
		auto smoothing_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("SMOOTH GAMES (BILINEAR FILTERING)"));
		smoothing_enabled->addRange({ { _("AUTO"), "auto" },{ _("ON") , "1" },{ _("OFF"), "0" } }, SystemConf::getInstance()->get(configName + ".smooth"));
		systemConfiguration->addWithLabel(_("SMOOTH GAMES (BILINEAR FILTERING)"), smoothing_enabled);
		systemConfiguration->addSaveFunc([configName, smoothing_enabled] { SystemConf::getInstance()->set(configName + ".smooth", smoothing_enabled->getSelected()); });
	}

	// Rewind
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::rewind))
	{
		auto rewind_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("REWIND"));
		rewind_enabled->addRange({ { _("AUTO"), "auto" }, { _("ON") , "1" }, { _("OFF"), "0" } }, SystemConf::getInstance()->get(configName + ".rewind"));
		systemConfiguration->addWithLabel(_("REWIND"), rewind_enabled);
		systemConfiguration->addSaveFunc([configName, rewind_enabled] { SystemConf::getInstance()->set(configName + ".rewind", rewind_enabled->getSelected()); });
	}

	// AUTO SAVE/LOAD
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::autosave) && !customFeatures.hasFeature("autosave"))
	{
		auto autosave_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("AUTO SAVE/LOAD ON GAME LAUNCH"));
		autosave_enabled->addRange({ { _("AUTO"), "auto" }, { _("ON") , "1" }, { _("OFF"), "0" } }, SystemConf::getInstance()->get(configName + ".autosave"));
		systemConfiguration->addWithDescription(_("AUTO SAVE/LOAD ON GAME LAUNCH"), _("Load latest save state on game launch and save state when exiting game."), autosave_enabled);
		systemConfiguration->addSaveFunc([configName, autosave_enabled] { SystemConf::getInstance()->set(configName + ".autosave", autosave_enabled->getSelected()); });
	}
#ifdef _ENABLEEMUELEC
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::cloudsave))
	{
		auto enable_cloudsave = std::make_shared<SwitchComponent>(mWindow);
		bool cloudSaveEnabled = SystemConf::getInstance()->get(configName + ".cloudsave") == "1";
		enable_cloudsave->setState(cloudSaveEnabled);
		systemConfiguration->addWithLabel(_("ENABLE CLOUD SAVE"), enable_cloudsave);

		systemConfiguration->addSaveFunc([enable_cloudsave, mWindow, configName] {
			bool cloudSaveEnabled = enable_cloudsave->getState();
			SystemConf::getInstance()->set(configName + ".cloudsave", cloudSaveEnabled ? "1" : "0");
			SystemConf::getInstance()->saveSystemConf();
		});
	}
#endif

#ifdef _ENABLEEMUELEC
	// Shaders preset
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::SHADERS) &&
		systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::shaders))
	{
        std::string a;
		auto shaders_choices = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SHADERS SET"),false);
		std::string currentShader = SystemConf::getInstance()->get(configName + ".shaderset");
		if (currentShader.empty()) {
			currentShader = std::string("auto");
		}

		shaders_choices->add(_("AUTO"), "auto", currentShader == "auto");
		shaders_choices->add(_("NONE"), "none", currentShader == "none");
		for(std::stringstream ss(getShOutput(R"(/usr/bin/emuelec-utils getshaders)")); getline(ss, a, ','); )
		shaders_choices->add(a, a, currentShader == a); // emuelec
		systemConfiguration->addWithLabel(_("SHADERS SET"), shaders_choices);
		systemConfiguration->addSaveFunc([shaders_choices, configName] { SystemConf::getInstance()->set(configName + ".shaderset", shaders_choices->getSelected()); });
	}

#if defined(ODROIDGOA) || defined(_ENABLEGAMEFORCE)
	// RGA SCALING
		auto rgascale_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("RGA SCALING"));
		rgascale_enabled->add(_("AUTO"), "auto", SystemConf::getInstance()->get(configName + ".rgascale") != "0" && SystemConf::getInstance()->get(configName + ".rgascale") != "1");
		rgascale_enabled->add(_("ON"), "1", SystemConf::getInstance()->get(configName + ".rgascale") == "1");
		rgascale_enabled->add(_("OFF"), "0", SystemConf::getInstance()->get(configName + ".rgascale") == "0");
		systemConfiguration->addWithLabel(_("RGA SCALING"), rgascale_enabled);
		systemConfiguration->addSaveFunc([configName, rgascale_enabled] { SystemConf::getInstance()->set(configName + ".rgascale", rgascale_enabled->getSelected()); });
#endif

	// Vertical Game
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::vertical))
	{
		auto vertical_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("ENABLE VERTICAL"));
		vertical_enabled->add(_("OFF"), "auto", SystemConf::getInstance()->get(configName + ".vertical") != "1");
		vertical_enabled->add(_("ON"), "1", SystemConf::getInstance()->get(configName + ".vertical") == "1");
		systemConfiguration->addWithLabel(_("ENABLE VERTICAL"), vertical_enabled);
		systemConfiguration->addSaveFunc([configName, vertical_enabled] { SystemConf::getInstance()->set(configName + ".vertical", vertical_enabled->getSelected()); });

        auto vert_aspect_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("VERTICAL ASPECT RATIO"));
		vert_aspect_enabled->addRange({ { _("16:9") , "1" }, { _("3:2") , "7" }, { _("21:9"), "4" }, { _("4:3") , "0" } }, SystemConf::getInstance()->get(configName + ".vert_aspect"));
		systemConfiguration->addWithLabel(_("VERTICAL ASPECT RATIO"), vert_aspect_enabled);
		systemConfiguration->addSaveFunc([configName, vert_aspect_enabled] { SystemConf::getInstance()->set(configName + ".vert_aspect", vert_aspect_enabled->getSelected()); });
	}
#else

	// Shaders preset
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::SHADERS) &&
		systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::shaders))
	{
		auto installedShaders = ApiSystem::getInstance()->getShaderList(systemData->getName(), currentEmulator, currentCore);
		if (installedShaders.size() > 0)
		{
			std::string currentShader = SystemConf::getInstance()->get(configName + ".shaderset");

			auto shaders_choices = std::make_shared<OptionListComponent<std::string> >(mWindow, _("SHADER SET"), false);
			shaders_choices->add(_("AUTO"), "auto", currentShader.empty() || currentShader == "auto");
			shaders_choices->add(_("NONE"), "none", currentShader == "none");

			for (auto shader : installedShaders)
				shaders_choices->add(_(Utils::String::toUpper(shader).c_str()), shader, currentShader == shader);

			if (!shaders_choices->hasSelection())
				shaders_choices->selectFirstItem();

			systemConfiguration->addWithLabel(_("SHADER SET"), shaders_choices);
			systemConfiguration->addSaveFunc([configName, shaders_choices] { SystemConf::getInstance()->set(configName + ".shaderset", shaders_choices->getSelected()); });
		}
	}
#endif
	// Integer scale
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::pixel_perfect))
	{
		auto integerscale_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("INTEGER SCALING (PIXEL PERFECT)"));
		integerscale_enabled->addRange({ { _("AUTO"), "auto" },{ _("ON") , "1" },{ _("OFF"), "0" } }, SystemConf::getInstance()->get(configName + ".integerscale"));
		systemConfiguration->addWithLabel(_("INTEGER SCALING (PIXEL PERFECT)"), integerscale_enabled);
		systemConfiguration->addSaveFunc([integerscale_enabled, configName] { SystemConf::getInstance()->set(configName + ".integerscale", integerscale_enabled->getSelected()); });

        auto integerscaleoverscale_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("INTEGER SCALING (OVERSCALE)"));
		integerscaleoverscale_enabled->addRange({ { _("AUTO"), "auto" },{ _("ON") , "1" },{ _("OFF"), "0" } }, SystemConf::getInstance()->get(configName + ".integerscaleoverscale"));
		systemConfiguration->addWithLabel(_("INTEGER SCALING (OVERSCALE)"), integerscaleoverscale_enabled);
		systemConfiguration->addSaveFunc([integerscaleoverscale_enabled, configName] { SystemConf::getInstance()->set(configName + ".integerscaleoverscale", integerscaleoverscale_enabled->getSelected()); });
	}
#ifdef _ENABLEEMUELEC
	// bezel
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::decoration))
	{
		auto bezel_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("BEZEL"));
		bezel_enabled->add(_("AUTO"), "auto", SystemConf::getInstance()->get(configName + ".bezel") != "0" && SystemConf::getInstance()->get(configName + ".bezel") != "1");
		bezel_enabled->add(_("YES"), "1", SystemConf::getInstance()->get(configName + ".bezel") == "1");
		bezel_enabled->add(_("NO"), "0", SystemConf::getInstance()->get(configName + ".bezel") == "0");
		systemConfiguration->addWithLabel(_("BEZEL"), bezel_enabled);
		systemConfiguration->addSaveFunc([bezel_enabled, configName] { SystemConf::getInstance()->set(configName + ".bezel", bezel_enabled->getSelected()); });
	}

	// maxperf
		auto maxperf_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("ENABLE MAX PERFORMANCE"));
		maxperf_enabled->add(_("AUTO"), "auto", SystemConf::getInstance()->get(configName + ".maxperf") != "0" && SystemConf::getInstance()->get(configName + ".maxperf") != "1");
		maxperf_enabled->add(_("YES"), "1", SystemConf::getInstance()->get(configName + ".maxperf") == "1");
		maxperf_enabled->add(_("NO"), "0", SystemConf::getInstance()->get(configName + ".maxperf") == "0");
		systemConfiguration->addWithLabel(_("ENABLE MAX PERFORMANCE"), maxperf_enabled);
		systemConfiguration->addSaveFunc([maxperf_enabled, configName] { SystemConf::getInstance()->set(configName + ".maxperf", maxperf_enabled->getSelected()); });
#else
	// decorations
	if (ApiSystem::getInstance()->isScriptingSupported(ApiSystem::DECORATIONS) && systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::decoration))
	{
		auto sets = GuiMenu::getDecorationsSets(systemData);
		if (sets.size() > 0)
		{
#ifdef BATOCERA
			systemConfiguration->addEntry(_("DECORATIONS"), true, [mWindow, configName, systemData, sets]
			{
				GuiSettings* decorations_window = new GuiSettings(mWindow, _("DECORATIONS").c_str());

				addDecorationSetOptionListComponent(mWindow, decorations_window, sets, configName);

				// stretch bezels
				auto bezel_stretch_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("STRETCH BEZELS (4K & ULTRAWIDE)"));
				bezel_stretch_enabled->add(_("AUTO"), "auto", SystemConf::getInstance()->get(configName + ".bezel_stretch") != "0" && SystemConf::getInstance()->get(configName + ".bezel_stretch") != "1");
				bezel_stretch_enabled->add(_("ON"), "1", SystemConf::getInstance()->get(configName + ".bezel_stretch") == "1");
				bezel_stretch_enabled->add(_("OFF"), "0", SystemConf::getInstance()->get(configName + ".bezel_stretch") == "0");
				decorations_window->addWithLabel(_("STRETCH BEZELS (4K & ULTRAWIDE)"), bezel_stretch_enabled);
				decorations_window->addSaveFunc([bezel_stretch_enabled, configName] {
					if (bezel_stretch_enabled->changed()) {
						SystemConf::getInstance()->set(configName + ".bezel_stretch", bezel_stretch_enabled->getSelected());
						SystemConf::getInstance()->saveSystemConf();
					}
				});

				// tattoo and controller overlays
				auto bezel_tattoo = std::make_shared<OptionListComponent<std::string>>(mWindow, _("SHOW CONTROLLER OVERLAYS"));
				bezel_tattoo->add(_("AUTO"), "auto", SystemConf::getInstance()->get(configName + ".bezel.tattoo") != "0"
					&& SystemConf::getInstance()->get(configName + ".bezel.tattoo") != "system"
					&& SystemConf::getInstance()->get(configName + ".bezel.tattoo") != "custom");
				bezel_tattoo->add(_("NO"), "0", SystemConf::getInstance()->get(configName + ".bezel.tattoo") == "0");
				bezel_tattoo->add(_("SYSTEM CONTROLLERS"), "system", SystemConf::getInstance()->get(configName + ".bezel.tattoo") == "system");
				bezel_tattoo->add(_("CUSTOM .PNG IMAGE"), "custom", SystemConf::getInstance()->get(configName + ".bezel.tattoo") == "custom");
				decorations_window->addWithLabel(_("SHOW CONTROLLER OVERLAYS"), bezel_tattoo);
				decorations_window->addSaveFunc([bezel_tattoo, configName] {
					if (bezel_tattoo->changed()) {
						SystemConf::getInstance()->set(configName + ".bezel.tattoo", bezel_tattoo->getSelected());
						SystemConf::getInstance()->saveSystemConf();
					}
				});

				auto bezel_tattoo_corner = std::make_shared<OptionListComponent<std::string>>(mWindow, _("OVERLAY CORNER"));
				bezel_tattoo_corner->add(_("AUTO"), "auto", SystemConf::getInstance()->get(configName + ".bezel.tattoo_corner") != "NW"
					&& SystemConf::getInstance()->get(configName + ".bezel.tattoo_corner") != "NE"
					&& SystemConf::getInstance()->get(configName + ".bezel.tattoo_corner") != "SE"
					&& SystemConf::getInstance()->get(configName + ".bezel.tattoo_corner") != "SW");
				bezel_tattoo_corner->add(_("NORTH WEST"), "NW", SystemConf::getInstance()->get(configName + ".bezel.tattoo_corner") == "NW");
				bezel_tattoo_corner->add(_("NORTH EAST"), "NE", SystemConf::getInstance()->get(configName + ".bezel.tattoo_corner") == "NE");
				bezel_tattoo_corner->add(_("SOUTH EAST"), "SE", SystemConf::getInstance()->get(configName + ".bezel.tattoo_corner") == "SE");
				bezel_tattoo_corner->add(_("SOUTH WEST"), "SW", SystemConf::getInstance()->get(configName + ".bezel.tattoo_corner") == "SW");
				decorations_window->addWithLabel(_("OVERLAY CORNER"), bezel_tattoo_corner);
				decorations_window->addSaveFunc([bezel_tattoo_corner, configName] {
					if (bezel_tattoo_corner->changed()) {
						SystemConf::getInstance()->set(configName + ".bezel.tattoo_corner", bezel_tattoo_corner->getSelected());
						SystemConf::getInstance()->saveSystemConf();
					}
				});

				std::string tatpath = configName + ".bezel.tattoo_file";
				const char *bezelpath = const_cast<char*>(tatpath.data());
				decorations_window->addInputTextRow(_("CUSTOM .PNG IMAGE PATH"), bezelpath, false);

				mWindow->pushGui(decorations_window);
			});
#else
			addDecorationSetOptionListComponent(mWindow, systemConfiguration, sets, configName);
#endif
		}
	}
#endif
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::latency_reduction))
		systemConfiguration->addEntry(_("LATENCY REDUCTION"), true, [mWindow, configName] { openLatencyReductionConfiguration(mWindow, configName); });

	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::colorization))
	{
		// gameboy colorize
		auto colorizations_choices = std::make_shared<OptionListComponent<std::string> >(mWindow, _("COLORIZATION"), false);
		std::string currentColorization = SystemConf::getInstance()->get(configName + "-renderer.colorization");
		if (currentColorization.empty())
			currentColorization = std::string("auto");

		colorizations_choices->add(_("AUTO"), "auto", currentColorization == "auto");
		colorizations_choices->add(_("NONE"), "none", currentColorization == "none");
#ifdef _ENABLEEMUELEC
        colorizations_choices->add(_("GBC"), "GBC", currentColorization == "GBC");
		colorizations_choices->add(_("SGB"), "SGB", currentColorization == "SGB");
#endif
		colorizations_choices->add(_("Best Guess"), "Best Guess", currentColorization == "Best Guess");

		const char* all_gambate_gc_colors_modes[] = { "GB - DMG",
								 "GB - Light",
								 "GB - Pocket",
								 "GBC - Blue",
								 "GBC - Brown",
								 "GBC - Dark Blue",
								 "GBC - Dark Brown",
								 "GBC - Dark Green",
								 "GBC - Grayscale",
								 "GBC - Green",
								 "GBC - Inverted",
								 "GBC - Orange",
								 "GBC - Pastel Mix",
								 "GBC - Red",
								 "GBC - Yellow",
								 "SGB - 1A",
								 "SGB - 1B",
								 "SGB - 1C",
								 "SGB - 1D",
								 "SGB - 1E",
								 "SGB - 1F",
								 "SGB - 1G",
								 "SGB - 1H",
								 "SGB - 2A",
								 "SGB - 2B",
								 "SGB - 2C",
								 "SGB - 2D",
								 "SGB - 2E",
								 "SGB - 2F",
								 "SGB - 2G",
								 "SGB - 2H",
								 "SGB - 3A",
								 "SGB - 3B",
								 "SGB - 3C",
								 "SGB - 3D",
								 "SGB - 3E",
								 "SGB - 3F",
								 "SGB - 3G",
								 "SGB - 3H",
								 "SGB - 4A",
								 "SGB - 4B",
								 "SGB - 4C",
								 "SGB - 4D",
								 "SGB - 4E",
								 "SGB - 4F",
								 "SGB - 4G",
								 "SGB - 4H",
								 "Special 1",
								 "Special 2",
								 "Special 3",
								 "TWB01 - 756 Production",
								 "TWB02 - AKB48 Pink",
								 "TWB03 - Angry Volcano",
								 "TWB04 - Anime Expo",
								 "TWB05 - Aqours Blue",
								 "TWB06 - Aquatic Iro",
								 "TWB07 - Bandai Namco",
								 "TWB08 - Blossom Pink",
								 "TWB09 - Bubbles Blue",
								 "TWB10 - Builder Yellow",
								 "TWB11 - Buttercup Green",
								 "TWB12 - Camouflage",
								 "TWB13 - Cardcaptor Pink",
								 "TWB14 - Christmas",
								 "TWB15 - Crunchyroll Orange",
								 "TWB16 - Digivice",
								 "TWB17 - Do The Dew",
								 "TWB18 - Eevee Brown",
								 "TWB19 - Fruity Orange",
								 "TWB20 - Game.com",
								 "TWB21 - Game Grump Orange",
								 "TWB22 - GameKing",
								 "TWB23 - Game Master",
								 "TWB24 - Ghostly Aoi",
								 "TWB25 - Golden Wild",
								 "TWB26 - Green Banana",
								 "TWB27 - Greenscale",
								 "TWB28 - Halloween",
								 "TWB29 - Hero Yellow",
								 "TWB30 - Hokage Orange",
								 "TWB31 - Labo Fawn",
								 "TWB32 - Legendary Super Saiyan",
								 "TWB33 - Lemon Lime Green",
								 "TWB34 - Lime Midori",
								 "TWB35 - Mania Plus Green",
								 "TWB36 - Microvision",
								 "TWB37 - Million Live Gold",
								 "TWB38 - Miraitowa Blue",
								 "TWB39 - NASCAR",
								 "TWB40 - Neo Geo Pocket",
								 "TWB41 - Neon Blue",
								 "TWB42 - Neon Green",
								 "TWB43 - Neon Pink",
								 "TWB44 - Neon Red",
								 "TWB45 - Neon Yellow",
								 "TWB46 - Nick Orange",
								 "TWB47 - Nijigasaki Orange",
								 "TWB48 - Odyssey Gold",
								 "TWB49 - Patrick Star Pink",
								 "TWB50 - Pikachu Yellow",
								 "TWB51 - Pocket Tales",
								 "TWB52 - Pokemon mini",
								 "TWB53 - Pretty Guardian Gold",
								 "TWB54 - S.E.E.S. Blue",
								 "TWB55 - Saint Snow Red",
								 "TWB56 - Scooby-Doo Mystery",
								 "TWB57 - Shiny Sky Blue",
								 "TWB58 - Sidem Green",
								 "TWB59 - Slime Blue",
								 "TWB60 - Spongebob Yellow",
								 "TWB61 - Stone Orange",
								 "TWB62 - Straw Hat Red",
								 "TWB63 - Superball Ivory",
								 "TWB64 - Super Saiyan Blue",
								 "TWB65 - Super Saiyan Rose",
								 "TWB66 - Supervision",
								 "TWB67 - Survey Corps Brown",
								 "TWB68 - Tea Midori",
								 "TWB69 - TI-83",
								 "TWB70 - Tokyo Midtown",
								 "TWB71 - Travel Wood",
								 "TWB72 - Virtual Boy",
								 "TWB73 - VMU",
								 "TWB74 - Wisteria Murasaki",
								 "TWB75 - WonderSwan",
								 "TWB76 - Yellow Banana" };

		int n_all_gambate_gc_colors_modes = 126;
		for (int i = 0; i < n_all_gambate_gc_colors_modes; i++)
			colorizations_choices->add(all_gambate_gc_colors_modes[i], all_gambate_gc_colors_modes[i], currentColorization == std::string(all_gambate_gc_colors_modes[i]));
#ifdef _ENABLEEMUELEC
        if (CustomFeatures::FeaturesLoaded || (!CustomFeatures::FeaturesLoaded && (systemData->getName() == "gb" || systemData->getName() == "gbc" || systemData->getName() == "gb2players" || systemData->getName() == "gbc2players" || systemData->getName() == "gbh" || systemData->getName() == "gbch"))) // only for gb, gbc and gb2players gbh gbch
#else
		if (CustomFeatures::FeaturesLoaded || (!CustomFeatures::FeaturesLoaded && (systemData->getName() == "gb" || systemData->getName() == "gbc" || systemData->getName() == "gb2players" || systemData->getName() == "gbc2players")))  // only for gb, gbc and gb2players
#endif
		{
			systemConfiguration->addWithLabel(_("COLORIZATION"), colorizations_choices);
			systemConfiguration->addSaveFunc([colorizations_choices, configName] { SystemConf::getInstance()->set(configName + "-renderer.colorization", colorizations_choices->getSelected()); });
		}
	}
#ifndef _ENABLEEMUELEC
	// ps2 full boot
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::fullboot))
	{
		if (CustomFeatures::FeaturesLoaded || (!CustomFeatures::FeaturesLoaded && systemData->getName() == "ps2")) // only for ps2
		{
			auto fullboot_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("FULL BOOT"));
			fullboot_enabled->addRange({ { _("AUTO"), "auto" },{ _("ON") , "1" },{ _("OFF"), "0" } }, SystemConf::getInstance()->get(configName + ".fullboot"));
			systemConfiguration->addWithLabel(_("FULL BOOT"), fullboot_enabled);
			systemConfiguration->addSaveFunc([fullboot_enabled, configName] { SystemConf::getInstance()->set(configName + ".fullboot", fullboot_enabled->getSelected()); });
		}
	}

	// wii emulated wiimotes
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::emulated_wiimotes))
	{
		if (CustomFeatures::FeaturesLoaded || (!CustomFeatures::FeaturesLoaded && systemData->getName() == "wii"))  // only for wii
		{
			auto emulatedwiimotes_enabled = std::make_shared<OptionListComponent<std::string>>(mWindow, _("EMULATED WIIMOTES"));
			emulatedwiimotes_enabled->addRange({ { _("AUTO"), "auto" },{ _("ON") , "1" },{ _("OFF"), "0" } }, SystemConf::getInstance()->get(configName + ".emulatedwiimotes"));
			systemConfiguration->addWithLabel(_("EMULATED WIIMOTES"), emulatedwiimotes_enabled);
			systemConfiguration->addSaveFunc([emulatedwiimotes_enabled, configName] { SystemConf::getInstance()->set(configName + ".emulatedwiimotes", emulatedwiimotes_enabled->getSelected()); });
		}
	}

	// citra change screen layout
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::screen_layout))
	{
		if (CustomFeatures::FeaturesLoaded || (!CustomFeatures::FeaturesLoaded && systemData->getName() == "3ds"))  // only for 3ds
		{
			auto changescreen_layout = std::make_shared<OptionListComponent<std::string>>(mWindow, _("CHANGE SCREEN LAYOUT"));
			changescreen_layout->addRange({ { _("AUTO"), "auto" },{ _("LARGE SCREEN") , "2" },{ _("SIDE BY SIDE"), "3" } }, SystemConf::getInstance()->get(configName + ".layout_option"));
			systemConfiguration->addWithLabel(_("CHANGE SCREEN LAYOUT"), changescreen_layout);
			systemConfiguration->addSaveFunc([changescreen_layout, configName] { SystemConf::getInstance()->set(configName + ".layout_option", changescreen_layout->getSelected()); });
		}
	}

	// psp internal resolution
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::internal_resolution))
	{
		std::string curResol = SystemConf::getInstance()->get(configName + ".internalresolution");

		auto internalresolution = std::make_shared<OptionListComponent<std::string>>(mWindow, _("INTERNAL RESOLUTION"));
		internalresolution->add(_("AUTO"), "auto", curResol.empty() || curResol == "auto");
		internalresolution->add("1:1", "0", curResol == "0");
		internalresolution->add("x1", "1", curResol == "1");
		internalresolution->add("x2", "2", curResol == "2");
		internalresolution->add("x3", "3", curResol == "3");
		internalresolution->add("x4", "4", curResol == "4");
		internalresolution->add("x5", "5", curResol == "5");
		internalresolution->add("x8", "8", curResol == "8");
		internalresolution->add("x10", "10", curResol == "10");

		if (!internalresolution->hasSelection())
			internalresolution->selectFirstItem();

		if (CustomFeatures::FeaturesLoaded || (!CustomFeatures::FeaturesLoaded && (systemData->getName() == "psp" || systemData->getName() == "wii" || systemData->getName() == "gamecube"))) // only for psp, wii, gamecube
		{
			systemConfiguration->addWithLabel(_("INTERNAL RESOLUTION"), internalresolution);
			systemConfiguration->addSaveFunc([internalresolution, configName] { SystemConf::getInstance()->set(configName + ".internalresolution", internalresolution->getSelected()); });
		}
	}
#endif
	// Load per-game / per-emulator / per-system custom features
	addFeatures(customFeatures, mWindow, systemConfiguration, configName, systemData->getName(), currentEmulator.empty() ? systemData->getEmulator(true) : currentEmulator, currentCore.empty() ? systemData->getCore(true) : currentCore, _("SETTINGS"), true);

	// automatic controller configuration
	if (systemData->isFeatureSupported(currentEmulator, currentCore, EmulatorFeatures::autocontrollers))
	{
		auto autoControllers = std::make_shared<OptionListComponent<std::string>>(mWindow, _("AUTOCONFIGURE CONTROLLERS"));
		autoControllers->addRange({ { _("AUTO"), "" },{ _("ON"), "0" },{ _("OFF"), "1" } }, SystemConf::getInstance()->get(configName + ".disableautocontrollers"));
		systemConfiguration->addWithLabel(_("AUTOCONFIGURE CONTROLLERS"), autoControllers);
		systemConfiguration->addSaveFunc([configName, autoControllers] { SystemConf::getInstance()->set(configName + ".disableautocontrollers", autoControllers->getSelected()); });
	}

	if (fileData == nullptr && ApiSystem::getInstance()->isScriptingSupported(ApiSystem::ScriptId::EVMAPY) && systemData->isCurrentFeatureSupported(EmulatorFeatures::Features::padTokeyboard))
	{
		if (systemData->hasKeyboardMapping())
			systemConfiguration->addEntry(_("EDIT PADTOKEY PROFILE"), true, [mWindow, systemData] { editKeyboardMappings(mWindow, systemData, true); });
		else
			systemConfiguration->addEntry(_("CREATE PADTOKEY PROFILE"), true, [mWindow, systemData] { editKeyboardMappings(mWindow, systemData, true); });
	}

#ifndef WIN32
	// Set as boot game
	if (fileData != nullptr)
	{
		std::string gamePath = fileData->getFullPath();

		auto bootgame = std::make_shared<SwitchComponent>(mWindow);
		bootgame->setState(SystemConf::getInstance()->get("global.bootgame.path") == gamePath);
		systemConfiguration->addWithLabel(_("LAUNCH THIS GAME AT STARTUP"), bootgame);
		systemConfiguration->addSaveFunc([bootgame, fileData, gamePath]
		{
			if (bootgame->changed())
			{
				SystemConf::getInstance()->set("global.bootgame.path", bootgame->getState() ? gamePath : "");
				SystemConf::getInstance()->set("global.bootgame.cmd", bootgame->getState() ? fileData->getlaunchCommand(false) : "");
			}
		});
	}
#endif

	mWindow->pushGui(systemConfiguration);
}

std::shared_ptr<OptionListComponent<std::string>> GuiMenu::createRatioOptionList(Window *window, std::string configname)
{
	auto ratio_choice = std::make_shared<OptionListComponent<std::string> >(window, _("GAME ASPECT RATIO"), false);
	std::string currentRatio = SystemConf::getInstance()->get(configname + ".ratio");
	if (currentRatio.empty())
		currentRatio = std::string("auto");

	std::map<std::string, std::string> *ratioMap = LibretroRatio::getInstance()->getRatio();
	for (auto ratio = ratioMap->begin(); ratio != ratioMap->end(); ratio++)
		ratio_choice->add(_(ratio->first.c_str()), ratio->second, currentRatio == ratio->second);

	if (!ratio_choice->hasSelection())
		ratio_choice->selectFirstItem();

	return ratio_choice;
}

#ifdef _ENABLEEMUELEC

int getResWidth (std::string res)
{
	std::string tmp = "";
	std::size_t pos = res.find("x");

	if (pos != std::string::npos) {
		tmp = res.substr(0, pos);
		return atoi( tmp.c_str() );
	}
	pos = res.find("p");
	if (pos != std::string::npos) {
		tmp = res.substr(0, pos);
		int resv = atoi(tmp.c_str());
		return std::ceil(( (float)16 / 9 * resv));
	}
	pos = res.find("i");
	if (pos != std::string::npos) {
		tmp = res.substr(0, pos);
		int resv = atoi(tmp.c_str());
		return std::ceil(( (float)16 / 9 * resv));
	}
	return 0;
}

int getHzFromRes(std::string res)
{
	int tmp = atoi(res.substr(res.length()-4, 2).c_str());
	if (tmp > 0) return tmp;
	return 60;
}

bool sortResolutions (std::string a, std::string b) {
	int ia = getResWidth(a);
	int ib = getResWidth(b);

	if (ia == ib) return (getHzFromRes(a) < getHzFromRes(b));

	return (ia < ib);
}

std::shared_ptr<OptionListComponent<std::string>> GuiMenu::createNativeVideoResolutionModeOptionList(Window *window, std::string configname)
{
	auto emuelec_video_mode = std::make_shared< OptionListComponent<std::string> >(window, "NATIVE VIDEO", false);
	std::vector<std::string> videomode;
	videomode.push_back("640x480p60hz");
	videomode.push_back("480p60hz");
	videomode.push_back("576p50hz");
	videomode.push_back("576p60hz");
	videomode.push_back("720p50hz");
	videomode.push_back("720p60hz");
	videomode.push_back("1080i50hz");
	videomode.push_back("1080p50hz");
	videomode.push_back("1080i60hz");
	videomode.push_back("1080p60hz");

	std::string def_video;
	for(std::stringstream ss(getShOutput(R"(/usr/bin/emuelec-utils resolutions)")); getline(ss, def_video, ','); ) {
		if (!std::count(videomode.begin(), videomode.end(), def_video)) {
			 videomode.push_back(def_video);
		}
	}

	std::sort(videomode.begin(), videomode.end(), sortResolutions);

	std::string index = SystemConf::getInstance()->get(configname + ".nativevideo");
	if (index.empty())
		index = "auto";

	emuelec_video_mode->add(_("AUTO"), "auto", index == "auto");
	for (auto it = videomode.cbegin(); it != videomode.cend(); it++) {
		emuelec_video_mode->add(*it, *it, index == *it);
	}

	return emuelec_video_mode;
}

#endif


std::shared_ptr<OptionListComponent<std::string>> GuiMenu::createVideoResolutionModeOptionList(Window *window, std::string configname)
{
	auto videoResolutionMode_choice = std::make_shared<OptionListComponent<std::string> >(window, _("VIDEO MODE"), false);

	std::string currentVideoMode = SystemConf::getInstance()->get(configname + ".videomode");
	if (currentVideoMode.empty())
		currentVideoMode = std::string("auto");

	std::vector<std::string> videoResolutionModeMap = ApiSystem::getInstance()->getVideoModes();
	videoResolutionMode_choice->add(_("AUTO"), "auto", currentVideoMode == "auto");
	for (auto videoMode = videoResolutionModeMap.begin(); videoMode != videoResolutionModeMap.end(); videoMode++)
	{
		std::vector<std::string> tokens = Utils::String::split(*videoMode, ':');

		// concatenat the ending words
		std::string vname;
		for (unsigned int i = 1; i < tokens.size(); i++)
		{
			if (i > 1)
				vname += ":";

			vname += tokens.at(i);
		}

		videoResolutionMode_choice->add(_(vname.c_str()), tokens.at(0), currentVideoMode == tokens.at(0));
	}

	if (!videoResolutionMode_choice->hasSelection())
		videoResolutionMode_choice->selectFirstItem();

	return videoResolutionMode_choice;
}

void GuiMenu::clearLoadedInput() {
  for(int i=0; i<mLoadedInput.size(); i++) {
    delete mLoadedInput[i];
  }
  mLoadedInput.clear();
}

GuiMenu::~GuiMenu() {
  clearLoadedInput();
}

std::vector<DecorationSetInfo> GuiMenu::getDecorationsSets(SystemData* system)
{
	std::vector<DecorationSetInfo> sets;
	if (system == nullptr)
		return sets;

	static const size_t pathCount = 3;

	std::vector<std::string> paths =
	{
#if WIN32
		Paths::getUserEmulationStationPath() + "/decorations", // for win32 testings
#endif
		Paths::getUserDecorationsPath(),
		Paths::getDecorationsPath()
	};

	Utils::FileSystem::stringList dirContent;
	std::string folder;

	for (auto path : paths)
	{
		if (!Utils::FileSystem::isDirectory(path))
			continue;

		dirContent = Utils::FileSystem::getDirContent(path);
		for (Utils::FileSystem::stringList::const_iterator it = dirContent.cbegin(); it != dirContent.cend(); ++it)
		{
			if (Utils::FileSystem::isDirectory(*it))
			{
				folder = *it;

				DecorationSetInfo info;
				info.name = folder.substr(path.size() + 1);
				info.path = folder;

				if (system != nullptr && Utils::String::startsWith(info.name, "default"))
				{
					std::string systemImg = path + "/"+ info.name +"/systems/" + system->getName() + ".png";
					if (Utils::FileSystem::exists(systemImg))
						info.imageUrl = systemImg;
				}

				if (info.imageUrl.empty())
				{
					std::string img = folder + "/default.png";
					if (Utils::FileSystem::exists(img))
						info.imageUrl = img;
				}

				sets.push_back(info);
			}
		}
	}

	struct { bool operator()(DecorationSetInfo& a, DecorationSetInfo& b) const { return a.name < b.name; } } compareByName;
	struct { bool operator()(DecorationSetInfo& a, DecorationSetInfo& b) const { return a.name == b.name; } } nameEquals;

	// sort and remove duplicates
	std::sort(sets.begin(), sets.end(), compareByName);
	sets.erase(std::unique(sets.begin(), sets.end(), nameEquals), sets.end());

	return sets;
}


void GuiMenu::openFormatDriveSettings()
{
	Window *window = mWindow;

	auto s = new GuiSettings(mWindow, _("FORMAT DEVICE").c_str());

	// Drive
	auto optionsStorage = std::make_shared<OptionListComponent<std::string> >(window, _("DEVICE TO FORMAT"), false);

	std::vector<std::string> disks = ApiSystem::getInstance()->getFormatDiskList();
	if (disks.size() == 0)
		optionsStorage->add(_("NONE"), "", false);
	else
	{
		for (auto disk : disks)
		{
			auto idx = disk.find(" ");
			if (idx != std::string::npos)
				optionsStorage->add(disk.substr(idx + 1), disk.substr(0, idx), false);
		}
	}

	optionsStorage->selectFirstItem();
	s->addWithLabel(_("DEVICE TO FORMAT"), optionsStorage);

	// File system
	auto fileSystem = std::make_shared<OptionListComponent<std::string> >(window, _("FILE SYSTEM"), false);

	std::vector<std::string> fileSystems = ApiSystem::getInstance()->getFormatFileSystems();
	if (fileSystems.size() == 0)
		fileSystem->add(_("NONE"), "", false);
	else
	{
		for (auto fs : fileSystems)
			fileSystem->add(fs, fs, false);
	}

	fileSystem->selectFirstItem();
	s->addWithLabel(_("FILE SYSTEM"), fileSystem);

	s->addEntry(_("FORMAT NOW"), false, [s, optionsStorage, fileSystem, window]
		{
			std::string disk = optionsStorage->getSelected();
			std::string fs = fileSystem->getSelected();

			if (disk.empty() || fs.empty())
			{
				window->pushGui(new GuiMsgBox(window, _("SELECTED OPTIONS ARE INVALID")));
				return;
			}

			window->pushGui(new GuiMsgBox(window, _("ARE YOU SURE YOU WANT TO FORMAT THIS DRIVE?"), _("YES"), [s, window, disk, fs]
			{
				ThreadedFormatter::start(window, disk, fs);
				s->close();
			}, _("NO"), nullptr));

		});

	mWindow->pushGui(s);
}



void GuiMenu::saveSubsetSettings()
{
	auto currentSystem = ViewController::get()->getState().getSystem();
	if (currentSystem == nullptr || currentSystem->getTheme() == nullptr)
		return;

	std::string fileData;

	auto subsets = currentSystem->getTheme()->getSubSetNames();
	for (auto subset : subsets)
	{
		std::string name = subset;
		std::string value;

		if (name == "colorset")
			value = Settings::getInstance()->getString("ThemeColorSet");
		else if (name == "iconset")
			value = Settings::getInstance()->getString("ThemeIconSet");
		else if (name == "menu")
			value = Settings::getInstance()->getString("ThemeMenu");
		else if (name == "systemview")
			value = Settings::getInstance()->getString("ThemeSystemView");
		else if (name == "gamelistview")
			value = Settings::getInstance()->getString("ThemeGamelistView");
		else if (name == "region")
			value = Settings::getInstance()->getString("ThemeRegionName");
		else
		{
			value = Settings::getInstance()->getString("subset." + name);
			name = "subset." + name;
		}

		if (!value.empty())
			fileData += name + "=" + value + "\r";

		for (auto system : SystemData::sSystemVector)
		{
			value = Settings::getInstance()->getString("subset." + system->getThemeFolder() + "." + subset);
			if (!value.empty())
				fileData += "subset." + system->getThemeFolder() + "." + subset + "=" + value + "\r";
		}
	}

	if (!Settings::getInstance()->getString("GamelistViewStyle").empty() && Settings::getInstance()->getString("GamelistViewStyle") != "automatic")
		fileData += "GamelistViewStyle=" + Settings::getInstance()->getString("GamelistViewStyle") + "\r";

	if (!Settings::getInstance()->getString("DefaultGridSize").empty())
		fileData += "DefaultGridSize=" + Settings::getInstance()->getString("DefaultGridSize") + "\r";

	for (auto system : SystemData::sSystemVector)
	{
		auto defaultView = Settings::getInstance()->getString(system->getName() + ".defaultView");
		if (!defaultView.empty())
			fileData += system->getName() + ".defaultView=" + defaultView + "\r";

		auto gridSizeOverride = Settings::getInstance()->getString(system->getName() + ".gridSize");
		if (!gridSizeOverride.empty())
			fileData += system->getName() + ".gridSize=" + gridSizeOverride + "\r";
	}

	std::string path = Paths::getUserEmulationStationPath() + "/themesettings";
	if (!Utils::FileSystem::exists(path))
		Utils::FileSystem::createDirectory(path);

	std::string themeSet = Settings::getInstance()->getString("ThemeSet");
	std::string fileName = path + "/" + themeSet + ".cfg";

	if (fileData.empty())
	{
		if (Utils::FileSystem::exists(fileName))
			Utils::FileSystem::removeFile(fileName);
	}
	else
		Utils::FileSystem::writeAllText(fileName, fileData);

}

void GuiMenu::loadSubsetSettings(const std::string themeName)
{
	std::string path = Paths::getUserEmulationStationPath() + "/themesettings";
	if (!Utils::FileSystem::exists(path))
		Utils::FileSystem::createDirectory(path);

	std::string fileName = path + "/" + themeName + ".cfg";
	if (!Utils::FileSystem::exists(fileName))
		return;

	std::string line;
	std::ifstream systemConf(fileName);
	if (systemConf && systemConf.is_open())
	{
		while (std::getline(systemConf, line, '\r'))
		{
			int idx = line.find("=");
			if (idx == std::string::npos || line.find("#") == 0 || line.find(";") == 0)
				continue;

			std::string name = line.substr(0, idx);
			std::string value = line.substr(idx + 1);
			if (!name.empty() && !value.empty())
			{
				if (name == "colorset")
					Settings::getInstance()->setString("ThemeColorSet", value);
				else if (name == "iconset")
					Settings::getInstance()->setString("ThemeIconSet", value);
				else if (name == "menu")
					Settings::getInstance()->setString("ThemeMenu", value);
				else if (name == "systemview")
					Settings::getInstance()->setString("ThemeSystemView", value);
				else if (name == "gamelistview")
					Settings::getInstance()->setString("ThemeGamelistView", value);
				else if (name == "region")
					Settings::getInstance()->setString("ThemeRegionName", value);
				else if (name == "GamelistViewStyle")
					Settings::getInstance()->setString("GamelistViewStyle", value);
				else if (name == "DefaultGridSize")
					Settings::getInstance()->setString("DefaultGridSize", value);
				else if (name.find(".defaultView") != std::string::npos)
					Settings::getInstance()->setString(name, value);
				else if (name.find(".gridSize") != std::string::npos)
					Settings::getInstance()->setString(name, value);
				else if (Utils::String::startsWith(name, "subset."))
					Settings::getInstance()->setString(name, value);
			}
		}
		systemConf.close();

		for (auto system : SystemData::sSystemVector)
		{
			auto defaultView = Settings::getInstance()->getString(system->getName() + ".defaultView");
			auto gridSizeOverride = Vector2f::parseString(Settings::getInstance()->getString(system->getName() + ".gridSize"));
			system->setSystemViewMode(defaultView, gridSizeOverride, false);
		}
	}
	else
		LOG(LogError) << "Unable to open " << fileName;
}

void GuiMenu::editKeyboardMappings(Window *window, IKeyboardMapContainer* mapping, bool editable)
{
	window->pushGui(new GuiKeyMappingEditor(window, mapping, editable));
}

bool GuiMenu::hitTest(int x, int y, Transform4x4f& parentTransform, std::vector<GuiComponent*>* pResult)
{
	if (pResult) pResult->push_back(this); // Always return this as it's a fake fullscreen, so we always have click events
	GuiComponent::hitTest(x, y, parentTransform, pResult);
	return true;
}

bool GuiMenu::onMouseClick(int button, bool pressed, int x, int y)
{
	if (pressed && button == 1 && !mMenu.isMouseOver())
	{
		delete this;
		return true;
	}

	return (button == 1);
}
