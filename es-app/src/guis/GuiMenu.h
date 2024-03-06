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
#include <rapidjson/rapidjson.h>
#include <rapidjson/pointer.h>

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
		//36:60:f9:e2:07:52;-52;4;8;*HIDDEN*

		if(tokens.size() == 5)
			{
				bssid 	= Utils::String::toUpper(tokens.at(0));
				if(tokens.at(1) != "-200"){
					rssi 	= tokens.at(1);
				}
				if(tokens.at(2) != "0"){
					channel = tokens.at(2);
				}
				if(tokens.at(3) != "-1"){
					enc 	= tokens.at(3);
				}
				if(tokens.at(4) != "*"){
					ssid 	= tokens.at(4);
				}
				pkts = 0;
			}
	}
	std::string bssid;
	std::string rssi;
	std::string ssid;
	std::string vendor;
	std::string channel;
	std::string enc;
	std::string password;
	int stations;
	int pkts;
};

struct WifiStation {
	WifiStation() {}
	WifiStation(std::string raw) {
		std::vector<std::string> tokens = Utils::String::split(raw, ';');
			//18:fe:34:34:5a:da;-40;2;34:60:f9:e2:07:52;-54;4;4;WRT_AP
			if(tokens.size() == 8) // simplePrint
			{
				mac 			= Utils::String::toUpper(tokens.at(0));
				rssi 			= tokens.at(1);
				pkts 			= tokens.at(2);
				ap.bssid  = Utils::String::toUpper(tokens.at(3));
				if(tokens.at(4) != "-200"){
					ap.rssi 	= tokens.at(4);
				}
				if(tokens.at(5) != "0"){
					ap.channel 	= tokens.at(5);
				}
				if(tokens.at(6) != "-1"){
					ap.enc 	= tokens.at(6);
				}
				if(tokens.at(7) != "*"){
					ap.ssid 	= tokens.at(7);
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

struct HackName {
	//STA;00:00:00:00:00:00;STANAME;CHANNEL;BSSID
	//NET;00:00:00:00:00:00;SSID;CHANNEL;PASSWORD
	//IR;9;IRNAME
	HackName(){}
	HackName(std::string raw){
		std::vector<std::string> tokens = Utils::String::split(raw, ';');
			type 	= tokens.at(0);
			id 		= tokens.at(1);
			name 	= Utils::String::replace(tokens.at(2), "_!SPC!_", " ");
			if(type == "AP" && tokens.size() == 5)
				{
					channel 	= tokens.at(3);
					password 	= Utils::String::replace(tokens.at(4), "_!SPC!_", " ");
				}
			if(type == "STA" && tokens.size() == 5)
				{
					channel = tokens.at(3);
					bssid 	= tokens.at(4);
				}
	}
	std::string type;
	std::string id;
	std::string name;
	std::string password;
	std::string bssid;
	std::string channel;
};

struct HackNameCounter {
	int IR;
	int STA;
	int AP;
};

struct SysProccess {
	std::string PID;
	std::string CPU;
	std::string MEM;
	std::string PROC;
	std::string CMD;
};
struct YoutubeThumbnail {
	YoutubeThumbnail(){}
	YoutubeThumbnail(std::string _url, int _w, int _h)
		{
			url = _url;
			width = _w;
			height = _h;
		}
	std::string url;
	int width;
	int height;
};
struct YoutubeLink {
	YoutubeLink(){}
	YoutubeLink(std::string j)
		{
			json = j;
			rapidjson::Document doc;
			doc.Parse(j.c_str());
			if (!doc.HasParseError())
			{
					link 							= doc.HasMember("url") && doc["url"].IsString()	? doc["url"].GetString() : "";
					id 							= doc.HasMember("id") && doc["id"].IsString()	? doc["id"].GetString() : "";
					title 						= doc.HasMember("title") && doc["title"].IsString()	? doc["title"].GetString() : "";
					duration_string 	= doc.HasMember("duration_string") && doc["duration_string"].IsString()		? doc["duration_string"].GetString() : "";
					description 			= doc.HasMember("description") && doc["description"].IsString()		? doc["description"].GetString() : "";
					uploader 					= doc.HasMember("uploader") 	&& doc["uploader"].IsString()	? doc["uploader"].GetString() : "";
					view_count 				= doc.HasMember("view_count") && doc["uploader"].IsNumber()		? doc["view_count"].GetInt() : -1;
					if(doc.HasMember("thumbnails"))
						{
							for (auto& item : doc["thumbnails"].GetArray())
								{
									thumbnails.push_back(
										YoutubeThumbnail(
											item["url"].GetString(),
											item["width"].GetInt(),
											item["height"].GetInt()
										)
									);
								}
						}
			}
		}
	std::string link;
	std::string title;
	std::string duration_string;
	std::string description;
	std::string uploader;
	std::string id;
	int view_count;
	std::string json;
	std::vector<YoutubeThumbnail> thumbnails;
};

struct ScanDB_STA {
	ScanDB_STA(){}
	ScanDB_STA(std::string raw)
		{
			std::vector<std::string> tokens = Utils::String::split(raw, ';');
			mac 			= tokens.at(0);
			bssid 		= tokens.at(1);
			rssi 			= tokens.at(2);
			lastSeenDate = tokens.at(3);
			lastSeenTime = tokens.at(4);
		}
	std::string mac;
	std::string bssid;
	std::string rssi;
	std::string vendor;
	std::string name;
	std::string lastSeenDate;
	std::string lastSeenTime;
};

struct ScanDB_AP {
	ScanDB_AP(){}
	ScanDB_AP(std::string raw)
		{
			std::vector<std::string> tokens = Utils::String::split(raw, ';');
			bssid 			= tokens.at(0);
			channel 		= tokens.at(1);
			encryption 	= tokens.at(2);
			rssi 				= tokens.at(3);
			ssid 				= tokens.at(4);
			lastSeenDate = tokens.at(5);
			lastSeenTime = tokens.at(6);
		}
	std::string bssid;
	std::string vendor;
	std::string channel;
	std::string rssi;
	std::string encryption;
	std::string ssid;
	std::string name;
	std::string lastSeenDate;
	std::string lastSeenTime;
	std::vector<ScanDB_STA> sta;
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
	void addButton(const std::string& label, const std::string& helpText, const std::function<void()>& callback)
		{
			mMenu.addButton(label, helpText, callback);
		}
	void mapXcallback(const std::function<void()>& callback)
		{
			mMenu.mapXcallback(callback);
		}

	void mapYcallback(const std::function<void()>& callback)
		{
			mMenu.mapYcallback(callback);
		}
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
	AccessPoint getAP(std::string bssid);
	WifiStation getSTA(std::string mac);
	AccessPoint rawToAP(std::string raw);
	std::vector<AccessPoint> AccessPointList(std::vector<std::string> bssids);
	WifiStation rawToSTA(std::string raw);
	std::vector<WifiStation> StationsList(std::vector<std::string> stations);

	// Names
	void loadNames();
	//STA;00:00:00:00:00:00;STANAME;CHANNEL;BSSID
	//NET;00:00:00:00:00:00;SSID;CHANNEL;PASSWORD
	//IR;9;IRNAME
	void addName(HackName n, bool reload = true);
	void remName(std::string type, std::string id, bool reload = true);
	HackName getName(std::string type, std::string id);
	std::vector<HackName> names;
	HackNameCounter namesCounter;
	void openNamesCat();
	void openNames(std::string category = "");
	void openName(HackName name);
	// names UPDATEr
	void updateNames();

	// SCAN DATABASE
	std::vector<ScanDB_STA> STA_ScanDB;
	std::vector<ScanDB_AP> ScanDB;
	bool enableScanDBsaving = true;
	int loadScanDatabase();
	void addToScanDatabase(AccessPoint ap, bool reload = true);
	void addToScanDatabase(WifiStation sta, bool reload = true);
	void openScanDatabase();

	void openScanDBItem(ScanDB_STA sta);
	void openScanDBItem(ScanDB_AP ap);

	// WPS
	void sniffWPS();
	void openWPSpwned(std::string raw);

	void appLauncher(std::string cmd, bool gpkill = false);
	// Deauther
	void hacksSend(std::string cmd);
	void hacksSet(std::string cmd);
	std::vector<std::string> hacksGet(std::string cmd);
	std::string hacksGetString(std::string cmd, bool tty = true);
	std::string macVendor(std::string mac);
	std::string encString(std::string id);

	std::string ssidFix(std::string ssid)
		{
			std::string output;
			if(ssid == "*HIDDEN*")
				{
					output = _U("\uf070");
				}
			else
				{
					output = ssid;
				}
			return output;
		}
	std::string wifiSignalIcon(std::string in)
		{
			return wifiSignalIcon(stoi(in));
		}
	std::string wifiSignalIcon(int in)
		{
			if(in >= -55)
				{
					return "fa-signal-bars";
				}
			else if(in >= -66)
				{
					return "fa-signal-bars-good";
				}
			else if(in >= -77)
				{
					return "fa-signal-bars-fair";
				}
			else if(in >= -88)
				{
					return "fa-signal-bars-weak";
				}
			return "fa-signal-bars-slash";
		}

	std::string wifiSignalGlyph(std::string in)
		{
			return wifiSignalGlyph(stoi(in));
		}
	std::string wifiSignalGlyph(int in)
		{
			/*

    -55 or higher: 4 bars
    -56 to -66: 3 bars
    -67 to -77: 2 bars
    -78 to -88: 1 bar
    -89 or lower: 0 bars

			*/
			if(in >= -55)
				{
					return _U("\uf690");
				}
			else if(in >= -66)
				{
					return _U("\uf693");
				}
			else if(in >= -77)
				{
					return _U("\uf692");
				}
			else if(in >= -88)
				{
					return _U("\uf691");
				}
			return _U("\uf694");
		}
	void openESP01Menu();
	void addESP01Buttons(Window* window, GuiSettings* s);
	//void addESP01ScanButtons(Window* window, GuiSettings* s, uint8_t type = 0);
	void openESP01Settings();

	void scanBSSIDS(bool all = false);
	void openBSSIDSMenu(std::vector<AccessPoint> bssids);
	void openDEAUTHMenu(AccessPoint ap);


	void scanSTA(bool apsta = false);
	void openSTAmenu(std::vector<WifiStation> stations, std::string bssid = "", std::string ssid = "");
	void openAP_STAmenu(std::vector<WifiStation> stations, bool all = false);
	std::vector<AccessPoint> APSTAList(std::vector<WifiStation> stations, bool all = false);
	void openSTADetail(WifiStation sta, bool lessAPinfo = false);

	void openRabbitTargets();


	void openIRlist();
	void sendIRcode(int code = -1);


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

	// PE System Info
	void openSysInfo();

	void openProccesses(std::vector<SysProccess> p);

	//void YTSearch(std::string q);
	void YouTube();
	void YouTubeLoad();
	std::vector<std::string> YouTubeSearchHistory;
	std::vector<YoutubeLink> YouTubeLastPlayed;
	void YouTubeSearchMenu();
	void YouTubeSearchKeyboard();
	void YTJsonSearch(std::string q, int maxResults = 20);
	void YTResultRow(Window* window, GuiSettings* s, YoutubeLink link);
	void YTResults(std::vector<YoutubeLink> links, std::string search = "");
	void YTResult(YoutubeLink link);




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
