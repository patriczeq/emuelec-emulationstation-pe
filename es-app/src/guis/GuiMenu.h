#pragma once
#ifndef ES_APP_GUIS_GUI_MENU_H
#define ES_APP_GUIS_GUI_MENU_H

#include "components/MenuComponent.h"
#include "components/BusyComponent.h"
#include "GuiComponent.h"
#include "guis/GuiSettings.h"
#include "components/OptionListComponent.h"
#include <SystemData.h>
#include "KeyboardMapping.h"
#include "utils/VectorEx.h"


class StrInputConfig
{
public:
	StrInputConfig(const std::string& ideviceName, const std::string& ideviceGUIDString, const std::string& idevicePath)
	{
		deviceName = ideviceName;
		deviceGUIDString = ideviceGUIDString;
		devicePath = idevicePath;
	}

	std::string deviceName;
	std::string deviceGUIDString;
	std::string devicePath;
};

struct DecorationSetInfo
{
	DecorationSetInfo() {}
	DecorationSetInfo(const std::string _name, const std::string _path, const std::string _imageUrl)
	{
		name = _name; path = _path; imageUrl = _imageUrl;
	}

	std::string name;
	std::string path;
	std::string imageUrl;
};

#ifdef _ENABLEEMUELEC
struct sScreenDimensions
{
	int width;
	int height;
};

struct sScreenBorders
{
	float left;
	float right;
	float top;
	float bottom;
};

#endif

struct MPserver {
	MPserver() {}
	MPserver(std::string raw) {
		//=;wlan0;IPv4;oga-mp-broadcast;_oga-mp._udp;local;OGAred.local;192.168.1.111;1234;"psx|Crash Bandicoot"
		std::vector<std::string> tokens = Utils::String::split(raw, ';');
		if(tokens.size() < 8){
			return;
		}
		ip 				= tokens.at(7);
		hostname 	= tokens.at(6);
		if(tokens.size() == 10)
			{
				std::string _platformName = Utils::String::replace(tokens.at(9), "\"", "");
				std::vector<std::string> ptokens = Utils::String::split(_platformName, '|');
				if(ptokens.size() == 2)
					{
						platform = ptokens.at(0);
						gamename = ptokens.at(1);
					}
			}
	}

	std::string ip;
	std::string hostname;
	std::string platform;
	std::string gamename;
};

struct AccessPoint {
	AccessPoint() {}
	AccessPoint(std::string raw) {
		std::vector<std::string> tokens = Utils::String::split(raw, ';');
		//c0:c9:e3:9e:dd:b7;-43;WRT_AP
		if(tokens.size() == 3)
			{
				bssid 	= Utils::String::toUpper(tokens.at(0));
				if(tokens.at(1) != "-200"){
					rssi 	= tokens.at(1);
				}
				if(tokens.at(2) != "*"){
					ssid 	= Utils::String::trim(tokens.at(2));
				}
			}
		//c0:c9:e3:9e:dd:b7;-43;4;WRT_AP
		else if(tokens.size() == 4)
			{
				bssid 	= Utils::String::toUpper(tokens.at(0));
				if(tokens.at(1) != "-200"){
					rssi 	= tokens.at(1);
				}
				if(tokens.at(2) != "0"){
					channel = tokens.at(2);
				}
				if(tokens.at(3) != "*"){
					ssid 	= tokens.at(3);
				}
			}
	}
	std::string bssid;
	std::string rssi;
	std::string ssid;
	std::string vendor;
	std::string channel;
};


struct WifiStation {
	WifiStation() {}
	WifiStation(std::string raw) {
		std::vector<std::string> tokens = Utils::String::split(raw, ';');
			if(tokens.size() >= 4) // simplePrint
			{
				if(tokens.size() == 4) // simplePrint
				{
					//ac:67:84:2c:9e:92; 34:60:f9:e2:07:52; -68; 129
					// mac; bssid; rssi; packets
					mac 			= Utils::String::toUpper(tokens.at(0));
					pkts			= tokens.at(3);
					rssi			= tokens.at(2);
					// name
					// vendor
					ap.bssid	= Utils::String::toUpper(tokens.at(1));
					/*ap.ssid		= "UpdateESP!";
					ap.rssi 	= "UpdateESP!";
					ap.channel= "";*/
					// vendor
				}

				if(tokens.size() == 7) // simplePrint
				{
					//cc:db:a7:a2:0f:c4;-23;7; c0:c9:e3:9e:dd:b7;-38;4;WRT_AP
					// mac, rssi, packets, bssid, aprssi, channel, ssid
					mac 			= Utils::String::toUpper(tokens.at(0));
					pkts 			= tokens.at(2);
					rssi 			= tokens.at(1);
					// name
					// vendor
					ap.bssid  = Utils::String::toUpper(tokens.at(3));
					if(tokens.at(6) != "*"){
						ap.ssid 	= tokens.at(6);
					}
					if(tokens.at(4) != "-200"){
						ap.rssi 	= tokens.at(4);
					}
					if(tokens.at(5) != "0"){
						ap.channel 	= tokens.at(5);
					}
					// vendor
				}
			}
	}

	std::string mac;
	std::string pkts;
	std::string rssi;
	std::string vendor;
	std::string name;
	AccessPoint ap;
};

struct ARPcli {
	ARPcli(){}
	ARPcli(std::string raw)
		{
			std::vector<std::string> tokens = Utils::String::split(raw, ';');
			if(tokens.size() >= 2)
				{
					mac = Utils::String::toUpper(tokens.at(0));
					ip 	= tokens.at(1);
				}
		}
	std::string mac;
	std::string ip;
	std::string hostname;
	std::string vendor;
};

struct DHCPClient {
	DHCPClient(){}
	DHCPClient(std::string raw){
		std::vector<std::string> tokens = Utils::String::split(raw, ' ');
		if(tokens.size() >= 4)
			{
				mac = Utils::String::toUpper(tokens.at(1));
				ip = tokens.at(2);
				hostname 	= tokens.at(3);
				leasetime 	= tokens.at(0);
			}
	}

	std::string mac;
	std::string ip;
	std::string hostname;
	std::string vendor;
	std::string leasetime;
};


struct AVAHIservice {
	AVAHIservice(){}
	AVAHIservice(std::string raw){
		std::vector<std::string> tokens = Utils::String::split(raw, ';');
		ipv 			= tokens.at(2);
		service 	= tokens.at(3);
		serviceID = tokens.at(4);
		domain 		= tokens.at(5);

		service = Utils::String::replace(service, "\\032", " ");
		service = Utils::String::replace(service, "\\033", "!");
		service = Utils::String::replace(service, "\\034", "\"");
		service = Utils::String::replace(service, "\\035", "#");
		service = Utils::String::replace(service, "\\036", "$");
		service = Utils::String::replace(service, "\\037", "%");
		service = Utils::String::replace(service, "\\038", "&");
		service = Utils::String::replace(service, "\\039", "'");
		service = Utils::String::replace(service, "\\040", "(");
		service = Utils::String::replace(service, "\\041", ")");
		service = Utils::String::replace(service, "\\042", "*");
		service = Utils::String::replace(service, "\\043", "+");
		service = Utils::String::replace(service, "\\044", ",");
		service = Utils::String::replace(service, "\\045", "-");
		service = Utils::String::replace(service, "\\046", ".");
		service = Utils::String::replace(service, "\\047", "/");
		service = Utils::String::replace(service, "\\058", ":");
		service = Utils::String::replace(service, "\\059", ";");
		service = Utils::String::replace(service, "\\060", "<");
		service = Utils::String::replace(service, "\\061", "=");
		service = Utils::String::replace(service, "\\062", ">");
		service = Utils::String::replace(service, "\\063", "?");
		service = Utils::String::replace(service, "\\064", "@");
		service = Utils::String::replace(service, "\\091", "[");
		service = Utils::String::replace(service, "\\092", "\\");
		service = Utils::String::replace(service, "\\093", "]");
		service = Utils::String::replace(service, "\\094", "^");
		service = Utils::String::replace(service, "\\095", "_");
		service = Utils::String::replace(service, "\\096", "`");
		service = Utils::String::replace(service, "\\123", "{");
		service = Utils::String::replace(service, "\\124", "|");
		service = Utils::String::replace(service, "\\125", "}");
		service = Utils::String::replace(service, "\\126", "~");
		service = Utils::String::replace(service, "\\194\\128", " ");
		service = Utils::String::replace(service, "\\194\\129", " ");
		service = Utils::String::replace(service, "\\194\\130", " ");
		service = Utils::String::replace(service, "\\194\\131", " ");
		service = Utils::String::replace(service, "\\194\\132", " ");
		service = Utils::String::replace(service, "\\194\\133", " ");
		service = Utils::String::replace(service, "\\194\\134", " ");
		service = Utils::String::replace(service, "\\194\\135", " ");
		service = Utils::String::replace(service, "\\194\\136", " ");
		service = Utils::String::replace(service, "\\194\\137", " ");
		service = Utils::String::replace(service, "\\194\\138", " ");
		service = Utils::String::replace(service, "\\194\\139", " ");
		service = Utils::String::replace(service, "\\194\\140", " ");
		service = Utils::String::replace(service, "\\194\\141", " ");
		service = Utils::String::replace(service, "\\194\\142", " ");
		service = Utils::String::replace(service, "\\194\\143", " ");
		service = Utils::String::replace(service, "\\194\\144", " ");
		service = Utils::String::replace(service, "\\194\\145", " ");
		service = Utils::String::replace(service, "\\194\\146", " ");
		service = Utils::String::replace(service, "\\194\\147", " ");
		service = Utils::String::replace(service, "\\194\\148", " ");
		service = Utils::String::replace(service, "\\194\\149", " ");
		service = Utils::String::replace(service, "\\194\\150", " ");
		service = Utils::String::replace(service, "\\194\\151", " ");
		service = Utils::String::replace(service, "\\194\\152", " ");
		service = Utils::String::replace(service, "\\194\\153", " ");
		service = Utils::String::replace(service, "\\194\\154", " ");
		service = Utils::String::replace(service, "\\194\\155", " ");
		service = Utils::String::replace(service, "\\194\\156", " ");
		service = Utils::String::replace(service, "\\194\\157", " ");
		service = Utils::String::replace(service, "\\194\\158", " ");
		service = Utils::String::replace(service, "\\194\\159", " ");
		service = Utils::String::replace(service, "\\194\\160", " ");
	}
	std::string ipv;
	std::string service;
	std::string serviceID;
	std::string domain;
};

struct AVAHIServiceDetails {
	AVAHIServiceDetails(){}
	AVAHIServiceDetails(std::string k, std::string v)
		{
			key = k;
			value = v;
		}
	std::string key;
	std::string value;
};

/*
=;wlan0;	IPv4;
					google-nest-hub-be389c40cbbba1ee717820a6b45c1b0b;
					_googlecast._tcp;
					local;
					fuchsia-ac67-842d-398d.local;
					192.168.1.105;
					8009;
					"rs=" "nf=1" "bs=FA8FCA90B8AA" "st=0" "ca=231941" "fn=Kuchyně Display" "ic=/setup/icon.png" "md=Google Nest Hub" "ve=05" "rm=" "cd=453032A8D16F03D52948EC835528CE4F" "id=be389c40cbbba1ee717820a6b45c1b0b"
*/
struct AVAHIserviceDetail {
	AVAHIserviceDetail(){}
	AVAHIserviceDetail(std::string raw)
		{
			std::vector<std::string> tokens = Utils::String::split(raw, ';');
			ipv 			= tokens.at(2);
			service 	= tokens.at(3);
			serviceID = tokens.at(4);
			domain 		= tokens.at(5);
			hostname	= tokens.at(6);
			ip				= tokens.at(7);
			port 			= tokens.at(8);

			service = Utils::String::replace(service, "\\032", " ");
			service = Utils::String::replace(service, "\\033", "!");
			service = Utils::String::replace(service, "\\034", "\"");
			service = Utils::String::replace(service, "\\035", "#");
			service = Utils::String::replace(service, "\\036", "$");
			service = Utils::String::replace(service, "\\037", "%");
			service = Utils::String::replace(service, "\\038", "&");
			service = Utils::String::replace(service, "\\039", "'");
			service = Utils::String::replace(service, "\\040", "(");
			service = Utils::String::replace(service, "\\041", ")");
			service = Utils::String::replace(service, "\\042", "*");
			service = Utils::String::replace(service, "\\043", "+");
			service = Utils::String::replace(service, "\\044", ",");
			service = Utils::String::replace(service, "\\045", "-");
			service = Utils::String::replace(service, "\\046", ".");
			service = Utils::String::replace(service, "\\047", "/");
			service = Utils::String::replace(service, "\\058", ":");
			service = Utils::String::replace(service, "\\059", ";");
			service = Utils::String::replace(service, "\\060", "<");
			service = Utils::String::replace(service, "\\061", "=");
			service = Utils::String::replace(service, "\\062", ">");
			service = Utils::String::replace(service, "\\063", "?");
			service = Utils::String::replace(service, "\\064", "@");
			service = Utils::String::replace(service, "\\091", "[");
			service = Utils::String::replace(service, "\\092", "\\");
			service = Utils::String::replace(service, "\\093", "]");
			service = Utils::String::replace(service, "\\094", "^");
			service = Utils::String::replace(service, "\\095", "_");
			service = Utils::String::replace(service, "\\096", "`");
			service = Utils::String::replace(service, "\\123", "{");
			service = Utils::String::replace(service, "\\124", "|");
			service = Utils::String::replace(service, "\\125", "}");
			service = Utils::String::replace(service, "\\126", "~");
			service = Utils::String::replace(service, "\\194\\128", " ");
			service = Utils::String::replace(service, "\\194\\129", " ");
			service = Utils::String::replace(service, "\\194\\130", " ");
			service = Utils::String::replace(service, "\\194\\131", " ");
			service = Utils::String::replace(service, "\\194\\132", " ");
			service = Utils::String::replace(service, "\\194\\133", " ");
			service = Utils::String::replace(service, "\\194\\134", " ");
			service = Utils::String::replace(service, "\\194\\135", " ");
			service = Utils::String::replace(service, "\\194\\136", " ");
			service = Utils::String::replace(service, "\\194\\137", " ");
			service = Utils::String::replace(service, "\\194\\138", " ");
			service = Utils::String::replace(service, "\\194\\139", " ");
			service = Utils::String::replace(service, "\\194\\140", " ");
			service = Utils::String::replace(service, "\\194\\141", " ");
			service = Utils::String::replace(service, "\\194\\142", " ");
			service = Utils::String::replace(service, "\\194\\143", " ");
			service = Utils::String::replace(service, "\\194\\144", " ");
			service = Utils::String::replace(service, "\\194\\145", " ");
			service = Utils::String::replace(service, "\\194\\146", " ");
			service = Utils::String::replace(service, "\\194\\147", " ");
			service = Utils::String::replace(service, "\\194\\148", " ");
			service = Utils::String::replace(service, "\\194\\149", " ");
			service = Utils::String::replace(service, "\\194\\150", " ");
			service = Utils::String::replace(service, "\\194\\151", " ");
			service = Utils::String::replace(service, "\\194\\152", " ");
			service = Utils::String::replace(service, "\\194\\153", " ");
			service = Utils::String::replace(service, "\\194\\154", " ");
			service = Utils::String::replace(service, "\\194\\155", " ");
			service = Utils::String::replace(service, "\\194\\156", " ");
			service = Utils::String::replace(service, "\\194\\157", " ");
			service = Utils::String::replace(service, "\\194\\158", " ");
			service = Utils::String::replace(service, "\\194\\159", " ");
			service = Utils::String::replace(service, "\\194\\160", " ");

			// parse service details
			std::vector<std::string> dTokens = Utils::String::split(Utils::String::replace(Utils::String::replace(tokens.at(9), ";", "."), "\" \"", "\";\""), ';');
			for(auto item : dTokens)
				{
					std::vector<std::string> dItem = Utils::String::split(Utils::String::replace(item, "\"", ""), '=');
					if(dItem.size() == 2)
						{
							AVAHIServiceDetails sItem(dItem.at(0), dItem.at(1));
							details.push_back(sItem);
						}
					else if(dItem.size() == 1)
						{
							AVAHIServiceDetails sItem(dItem.at(0), "");
							details.push_back(sItem);
						}
				}
		}
	std::string ipv;
	std::string service;
	std::string serviceID;
	std::string domain;
	std::string hostname;
	std::string ip;
	std::string port;
	std::vector<AVAHIServiceDetails> details;
};

struct TraceRouteHop {
	TraceRouteHop(){};
	TraceRouteHop(std::string raw)
		{
			std::vector<std::string> tokens = Utils::String::split(raw, ';');
			if(tokens.size() == 4 && tokens.at(0) != "traceroute")
				{
					isValid = true;
					//11;142.251.224.127;(142.251.224.127);3.123
					//4;*;;
					hop 	= tokens.at(0);
					if(tokens.at(1) == "*")
						{
							timeout = true;
						}
					else
						{
							host 	= tokens.at(1);
							ip	 	= tokens.at(2);
							dur 	= tokens.at(3);
							ip		= Utils::String::replace(ip, "(", "");
							ip		= Utils::String::replace(ip, ")", "");
						}
				}
			else
				{
					isValid = false;
				}
		}
	bool isValid;
	bool timeout;
	std::string hop;
	std::string host;
	std::string ip;
	std::string dur;
};

struct Traceroute {
	Traceroute(){};
	Traceroute(std::vector<std::string> raw)
		{
			for(auto h : raw)
				{
					TraceRouteHop hop(h);
					if(hop.isValid)
						{
							hops.push_back(hop);
						}
				}
		}
	std::vector<TraceRouteHop> hops;
};

struct Chromecast {
	Chromecast(){};
	Chromecast(AVAHIserviceDetail service){
		ip = service.ip;
		for(auto d : service.details)
			{
				if(d.key == "rs"){player 	= d.value;}
				if(d.key == "fn"){name 		= d.value;}
				if(d.key == "md"){oname 	= d.value;}
				if(d.key == "id"){id 	= d.value;}
			}
	};

	std::string player;
	std::string name;
	std::string oname;
	std::string ip;
	std::string id;
};


class GuiMenu : public GuiComponent
{
public:
	GuiMenu(Window* window, bool animate = true);
	~GuiMenu();

	bool input(InputConfig* config, Input input) override;
	void onSizeChanged() override;
	std::vector<HelpPrompt> getHelpPrompts() override;
	static void openQuitMenu_static(Window *window, bool quickAccessMenu = false, bool animate = true);

	static void popSystemConfigurationGui(Window* mWindow, SystemData *systemData);
	static void popGameConfigurationGui(Window* mWindow, FileData* fileData);

	static void openThemeConfiguration(Window* mWindow, GuiComponent* s, std::shared_ptr<OptionListComponent<std::string>> theme_set, const std::string systemTheme = "");

	static void updateGameLists(Window* window, bool confirm = true);
	static void editKeyboardMappings(Window *window, IKeyboardMapContainer* mapping, bool editable);

	static void ChromecastControl(std::string id, std::string action, std::string file = "");
	static void loadChromecast(Window* mWindow, std::string file = "");
	static void loadChromecastDevices(Window* mWindow, std::vector<AVAHIserviceDetail> casts, std::string file = "");
	static void loadChromecastDevice(Window* mWindow, Chromecast device, std::string file = "");
	static std::vector<AVAHIserviceDetail> getAvahiService(std::string service);
private:
	void addEntry(std::string name, bool add_arrow, const std::function<void()>& func, const std::string iconName = "");

	void addVersionInfo();
	void openCollectionSystemSettings();
	void openConfigInput();
	void openScraperSettings();
	void openScreensaverOptions();
	void openSoundSettings();
	void openUISettings();
	void openUpdatesSettings();

	void openAllSettings();
	void openAppsMenu();

	std::string getShO(const std::string cmd);

	void openNetworkTools();
	std::vector<ARPcli> getARPclients();
	void openARPlist(std::vector<ARPcli> list);
	void openARPrecord(ARPcli cli);

	std::vector<AVAHIservice> getAvahiServices();
	void openAvahiList(std::vector<AVAHIservice> list);


	void openAvahiDetailList(std::vector<AVAHIserviceDetail> list);
	void openAvahiDetail(AVAHIserviceDetail service);

	/*net tools*/
	void pingIP(std::string ip);
	void msgExec(const std::string cmd);

	void traceroute(std::string addr);
	void openTraceroute(std::string addr, std::vector<TraceRouteHop> hops);

	std::vector<AccessPoint> scanlist;
	std::vector<WifiStation> stalist;
	std::vector<AccessPoint> scanBSSIDSlist();
	std::vector<WifiStation> scanSTAlist();
	AccessPoint getAP(std::string bssid);
	WifiStation getSTA(std::string mac);
	AccessPoint rawToAP(std::string raw);
	std::vector<AccessPoint> AccessPointList(std::vector<std::string> bssids);
	WifiStation rawToSTA(std::string raw);
	std::vector<WifiStation> StationsList(std::vector<std::string> stations);

	void appLauncher(std::string cmd);
	// Deauther
	void hacksSend(std::string cmd);
	void hacksSet(std::string cmd);
	std::vector<std::string> hacksGet(std::string cmd);
	std::string hacksGetString(std::string cmd, bool tty = true);
	std::string macVendor(std::string mac);
	// macnames
	std::string macName(std::string mac);
	void setMacName(std::string mac, std::string name);
	void remMacName(std::string mac);


	void openESP01Menu();
	void openESP01Settings();

	void scanBSSIDS();
	void openBSSIDSMenu(std::vector<AccessPoint> bssids);
	void openDEAUTHMenu(AccessPoint ap);


	void scanSTA();
	void openSTAmenu(std::vector<WifiStation> stations);
	void openSTADetail(WifiStation sta);

	void openIRlist();


	// playertoo client
	void scanMPServers();
	void openMPServers(std::vector<AVAHIserviceDetail> servers);

	// wifi ap helper
	std::string apInlineInfo(std::string cmd);
	DHCPClient rawToDHCPCli(std::string raw);
	std::vector<DHCPClient> DHCPClientList(std::vector<std::string> clients);
	void openAPleases();
	void openDHCPclient(DHCPClient lease);
	// AP conn
	void searchGameAP();
	// music player
	void openMusicPlayer();


#ifdef _ENABLEEMUELEC

	void openEmuELECSettings(); /* < emuelec */
    static void openDangerZone(Window* mWindow, std::string configName);
    static void createGamepadConfig(Window* window, GuiSettings* systemConfiguration);
    static void openExternalMounts(Window* mWindow, std::string configName);
		static void addFrameBufferOptions(Window* mWindow, GuiSettings* guiSettings, std::string configName, std::string header);
#endif

	void openSystemSettings();
	void openGamesSettings();
	void openControllersSettings(int autoSel = 0);
	void openControllersSpecificSettings_sindengun();
	void openControllersSpecificSettings_wiigun();
	void openNetworkSettings(bool selectWifiEnable = false);
	void openQuitMenu();
	void openSystemInformations();
	void openDeveloperSettings();
	void openNetplaySettings();
	void openRetroachievementsSettings();
	void openMissingBiosSettings();
	void openFormatDriveSettings();
	void exitKidMode();

	// windows
	void openEmulatorSettings();
	void openSystemEmulatorSettings(SystemData* system);

	static void openWifiSettings(Window* win, std::string title, std::string data, const std::function<void(std::string)>& onsave);

	MenuComponent mMenu;
	bool		mWaitingLoad;
	TextComponent mVersion;

	static std::shared_ptr<OptionListComponent<std::string>> createRatioOptionList(Window *window, std::string configname);
#ifdef _ENABLEEMUELEC
  static std::shared_ptr<OptionListComponent<std::string>> createNativeVideoResolutionModeOptionList(Window *window, std::string configname);

  static std::shared_ptr<OptionListComponent<std::string>> createJoyBtnOptionList(Window *window, std::string prefixName, std::string title, int selectId = -1);
  static std::shared_ptr<OptionListComponent<std::string>> createJoyBtnRemapOptionList(Window *window, std::string prefixName, int btnIndex = -1);

  static void createBtnJoyCfgName(Window *window, GuiSettings *systemConfiguration, std::string prefixName);
  static void createBtnJoyCfgRemap(Window *window, GuiSettings *systemConfiguration, std::string prefixName, std::string remapName, int btnIndex = -1, int editIndex = 0);

  static void deleteBtnJoyCfg(Window *window, GuiSettings *systemConfiguration, std::string prefixName);
  static void editJoyBtnRemapOptionList(Window *window, GuiSettings *systemConfiguration, std::string prefixName);
  static void removeJoyBtnEntry(int index);
  static void addJoyBtnEntry(std::string name, std::string val);
#endif
	static std::shared_ptr<OptionListComponent<std::string>> createVideoResolutionModeOptionList(Window *window, std::string configname);
	static void popSpecificConfigurationGui(Window* mWindow, std::string title, std::string configName, SystemData *systemData, FileData* fileData, bool selectCoreLine = false);

	static void openLatencyReductionConfiguration(Window* mWindow, std::string configName);

	std::vector<StrInputConfig*> mLoadedInput; // used to keep information about loaded devices in case there are unpluged between device window load and save
	void clearLoadedInput();

	static void addDecorationSetOptionListComponent(Window* window, GuiSettings* parentWindow, const std::vector<DecorationSetInfo>& sets, const std::string& configName = "global");
	static void createDecorationItemTemplate(Window* window, std::vector<DecorationSetInfo> sets, std::string data, ComponentListRow& row);

	static void addFeatureItem(Window* window, GuiSettings* settings, const CustomFeature& feat, const std::string& configName, const std::string& system, const std::string& emulator, const std::string& core);
	static void addFeatures(const VectorEx<CustomFeature>& features, Window* window, GuiSettings* settings, const std::string& configName, const std::string& system, const std::string& emulator, const std::string& core, const std::string& defaultGroupName = "", bool addDefaultGroupOnlyIfNotFirst = false);

	bool checkNetwork();

	static void saveSubsetSettings();
	static void loadSubsetSettings(const std::string themeName);

	inline void addWithDescription(const std::string& label, const std::string& description, const std::shared_ptr<GuiComponent>& comp, bool setCursorHere = false)
	{
		mMenu.addWithDescription(label, description, comp, nullptr, "", setCursorHere, true);
	}

	inline void addWithDescription(const std::string& label, const std::string& description, const std::shared_ptr<GuiComponent>& comp, const std::function<void()>& func, const std::string iconName = "", bool setCursorHere = false, /*bool invert_when_selected = true,*/ bool multiLine = false)
	{
		mMenu.addWithDescription(label, description, comp, func, iconName, setCursorHere, multiLine);
	}

public:
	static std::vector<DecorationSetInfo> getDecorationsSets(SystemData* system = nullptr);
	virtual bool hitTest(int x, int y, Transform4x4f& parentTransform, std::vector<GuiComponent*>* pResult = nullptr) override;
	virtual bool onMouseClick(int button, bool pressed, int x, int y);
#ifdef _ENABLEEMUELEC
  static std::shared_ptr<OptionListComponent<std::string>> btn_choice;
  static std::shared_ptr<OptionListComponent<std::string>> del_choice;
  static std::shared_ptr<OptionListComponent<std::string>> edit_choice;
#endif

};

#endif // ES_APP_GUIS_GUI_MENU_H
