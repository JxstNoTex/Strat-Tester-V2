#include <std_include.hpp>
#include "includes.h"
#include "Menu.hpp"
#include <Windows.h>
#include <main.h>
#include "legacy_injection/builtins.h"
#include "utils/memory.hpp"

bool _openMenu = 0;

std::vector<std::string> split(std::string s, std::string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

std::map<std::string, int> getVKCodeMap()
	{
    // Add all VK key codes here
    std::map<std::string, int> vkMap = {
	{"MOUSE1", 0x01},
	{"MOUSE2", 0x01},
	{"0",0x30},
    {"1",0x31},
    {"2",0x32},
    {"3",0x33},
    {"4",0x34},
    {"5",0x35},
    {"6",0x36},
    {"7",0x37},
    {"8",0x38},
    {"9",0x39},
    {"A",0x41},
    {"B",0x42},
    {"C",0x43},
    {"D",0x44},
    {"E",0x45},
    {"F",0x46},
    {"G",0x47},
    {"H",0x48},
    {"I",0x49},
    {"J",0x4A},
    {"K",0x4B},
    {"L",0x4C},
    {"M",0x4D},
    {"N",0x4E},
    {"O",0x4F},
    {"P",0x50},
    {"Q",0x51},
    {"R",0x52},
    {"S",0x53},
    {"T",0x54},
    {"U",0x55},
    {"V",0x56},
    {"W",0x57},
    {"X",0x58},
    {"Y",0x59},
    {"Z",0x5A},
	{"ESCAPE",0x1B}
    };
    return vkMap;
}

int getVKCode(const std::string& key) {
    static std::map<std::string, int> vkMap = getVKCodeMap();
    auto it = vkMap.find(key);
    if (it != vkMap.end()) {
        return it->second;
    }
    return -1; // Return -1 if the key is not found
}

std::map<std::string, int> parseFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
	std::map<std::string, int> mapping;

	if(file.is_open())
	{
		printf("File found\n");
			std::string bind, key, action;
			size_t pos = 0;
		while (std::getline(file, line))
		{
			if (pos == 0 || pos == 1)
			{
				pos++;
				
			}
			else if (line == "unbindallaxis")
			{
				return mapping;
			}
			else
			{
				pos++;
				std::vector<std::string> s_input = split(line, " ");
				int vkCode = getVKCode(s_input[1]);
                if (vkCode != -1) {
                    mapping[s_input[2]] = vkCode;
                }
				s_input.clear();
			}
		}
		return mapping;
	}
	else
	{
		printf("File not found\n");
		return mapping;
	
	}
}

namespace StratTester
{
	struct weaponData
	{
		std::string categorie;
		std::string weaponId;
		std::string weaponName;
	};
	struct perkData
	{
		std::string perkId;
		std::string perkName;
	};
	std::vector<weaponData> weaponDataList;
	std::vector<perkData> perkDataList;
	void Update(int host, bool open)
	{
		_openMenu = open;
		
	}

	void insertWeaponData(char* categorie, char* weaponId, char* weaponName)
	{
		//check for duplicates
		if (weaponDataList.size() > 0)
		{
			for (int i = 0; i < weaponDataList.size(); i++)
			{
				if (weaponDataList[i].weaponId.c_str() == weaponId)
				{
					return;
				}
			}
		}
		weaponDataList.push_back({ categorie, weaponId, weaponName });
	}
	
	void insertPerkData(char* perkId, char* perkName)
	{
		if (perkDataList.size() > 0)
		{
			for (int i = 0; i < perkDataList.size(); i++)
			{
				if (perkDataList[i].perkId.c_str() == perkId)
				{
					return;
				}
			}
		}
		perkDataList.push_back({ perkId, perkName });
	}
	std::map<std::string, int> keybinds;
	std::once_flag f1;
	Menu::Menu()
	{

	}

	Menu::~Menu()
	{

	}

	std::vector<int> v_page = {0x00};
	void Menu::draw()
	{

		int category;
		std::vector<std::string> Categories = { "Assault Rifles", "Submachine Guns", "Shotguns", "Light Machine Guns", "Sniper Rifles", "Pistols", "Launchers", "Extras" };
		
	#define Switchmenu(index) v_page.push_back(index);
	#define Call(x) GSCBuiltins::pushUpdate((char*)x);
		if (GetAsyncKeyState(0x38/* key 8*/) & 0x8000)
		{
			_openMenu = 1;
		}

		

		if (_openMenu == 1)
		{
			std::call_once(f1, []()
			{
				keybinds = parseFile(".\\players\\bindings_0.cfg");

			});

			ImGuiIO& io = ImGui::GetIO();
			io.KeyMap[ImGuiKey_Space] = keybinds["\"+activate\""];
			
			if (GetAsyncKeyState(keybinds["\"togglemenu\""]) < 0)
			{
				_openMenu = 0;
			}
			if (GetAsyncKeyState(keybinds["\"+melee\""]) < 0)
			{
				if (v_page.back() != 0x0)
				{
					
					Call("[Abort]");
					v_page.pop_back();
					Sleep(200);
				}
				
			}
			 

			switch (v_page.back())
			{
			case 0x0:
				ImGui::Begin("Strat Tester V2", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				if (ImGui::Button("Basic Options", ImVec2(185, 20)))
				{
					//create a new menu page
					
					Switchmenu(1)
				}
				if (ImGui::Button("Weapon Options", ImVec2(185, 20)))
				{
					Switchmenu(2)
				}
				if (ImGui::Button("Perk Options", ImVec2(185, 20)))
				{
					Call("[Perk]");
					Switchmenu(3)
				}
				if (ImGui::Button("Points", ImVec2(185, 20)))
				{
					Switchmenu(4)
				}
				if (ImGui::Button("Round Options", ImVec2(185, 20)))
				{
					Switchmenu(5)
				}
				if (ImGui::Button("Drop Options", ImVec2(185, 20)))
				{
					Switchmenu(6)
				}
				/*Dev Note:
				* Function Add
				* Set the Text to the map name where the player is currently on
				*/
				if (ImGui::Button("Map Options", ImVec2(185, 20))) 
				{
					Switchmenu(7)
				}
				if (ImGui::Button("Presets *WIP*", ImVec2(185, 20)))
				{
					Switchmenu(8)
				}
				if (ImGui::Button("Debug Options", ImVec2(185, 20)))
				{
					Switchmenu(9)
				}

				ImGui::End();
				break;
			case 0x1:
				ImGui::Begin("Basic Options", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				if (ImGui::Button("God Mode", ImVec2(185, 20)))
				{
					Call("[Basic]->[GodMode]")
				}
				if (ImGui::Button("Unlimited Ammo", ImVec2(185, 20)))
				{
					Call("[Basic]->[uAmmo]")
				}
				if (ImGui::Button("No Target", ImVec2(185, 20)))
				{
					Call("[Basic]->[NoTarget]")
				}

				ImGui::End();
				break;
			case 0x2:
				ImGui::Begin("Weapon Options", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				if (ImGui::Button("Give Weapon", ImVec2(185, 20)))
				{
					Switchmenu(0x10)
				}
				if (ImGui::Button("Give Upgraded Weapon", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("AAT Menu", ImVec2(185, 20)))
				{

				}

				ImGui::End();
				break;
			case 0x3:
				ImGui::Begin("Perk Options", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				if (ImGui::Button("Give All Perks", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("Retain Perks", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("Remove All Perks", ImVec2(185, 20)))
				{

				}
				for (int i = 0; i < perkDataList.size(); i++)
				{
					ImGui::PushID(i);
					if (ImGui::Button(perkDataList[i].perkName.c_str(), ImVec2(185, 20)))
					{
						Call(perkDataList[i].perkId.c_str());
					}
					ImGui::PopID();
				}


				ImGui::End();
				break;
			case 0x4:
				ImGui::Begin("Points", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				if (ImGui::Button("Give +1000", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("Give +5000", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("Give +10000", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("Give +100000", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("Give +1000000", ImVec2(185, 20)))
				{

				}

				if (ImGui::Button("Give -1000", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("Give -5000", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("Give -10000", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("Give -100000", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("Give -1000000", ImVec2(185, 20)))
				{

				}
				ImGui::End();
				break;
			case 0x09:
				ImGui::Begin("Debugging Options", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				if (ImGui::Button("print weapon data to file", ImVec2(185, 20)))
				{
					//log gsc_WeaponData to file
					std::ofstream myfile;
					myfile.open(".\\mods\\strat_tester_2.0\\zone\\weaponData.txt");
					for (int i = 0; i < weaponDataList.size(); i++)
					{
						myfile << "Weapon Name: " << weaponDataList[i].weaponName << " Weapon ID: " << weaponDataList[i].weaponId << " Weapon Categorie: " << weaponDataList[i].categorie << "\n";
					}
					myfile.close();
				}


				ImGui::End();
				break;
			case 0x10:
				ImGui::Begin("Give Weapon", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				//create a button for each weapon category
				
				for (int i = 0; i < Categories.size(); i++)
				{
					ImGui::PushID(i);
					
					if (ImGui::Button(Categories[i].c_str(), ImVec2(185, 20))) //
					{
						Call("[Weapon]->[GiveWeapon]");
						Switchmenu(i + 0x11)
					}
					ImGui::PopID();
				}
				ImGui::End();
				break;
			case 0x11:
				category = 0x11 - 0x11;
				ImGui::Begin(Categories[category].c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				for (int j = 0; j < weaponDataList.size(); j++)
				{
					if (weaponDataList[j].categorie == Categories[category])
					{
						ImGui::PushID(j);
						if (ImGui::Button(weaponDataList[j].weaponName.c_str(), ImVec2(185, 20)))
						{
							//give weapon
							
							Call(weaponDataList[j].weaponId.c_str());
						}
						ImGui::PopID();
					}
				}
				ImGui::End();
				break;
			case 0x12:
				category = 0x12 - 0x11;
				ImGui::Begin(Categories[category].c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				for (int j = 0; j < weaponDataList.size(); j++)
				{
					if (weaponDataList[j].categorie == Categories[category])
					{
						ImGui::PushID(j);
						if (ImGui::Button(weaponDataList[j].weaponName.c_str(), ImVec2(185, 20)))
						{
							Call(weaponDataList[j].weaponId.c_str());
						}
						ImGui::PopID();
					}
				}
				ImGui::End();
				break;
			case 0x13:
				category = 0x13 - 0x11;
				ImGui::Begin(Categories[category].c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				for (int j = 0; j < weaponDataList.size(); j++)
				{
					if (weaponDataList[j].categorie == Categories[category])
					{
						ImGui::PushID(j);
						if (ImGui::Button(weaponDataList[j].weaponName.c_str(), ImVec2(185, 20)))
						{
							Call(weaponDataList[j].weaponId.c_str());
						}
						ImGui::PopID();
					}
				}
				ImGui::End();
				break;
			case 0x14:
				category = 0x14 - 0x11;
				ImGui::Begin(Categories[category].c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				for (int j = 0; j < weaponDataList.size(); j++)
				{
					if (weaponDataList[j].categorie == Categories[category])
					{
						ImGui::PushID(j);
						if (ImGui::Button(weaponDataList[j].weaponName.c_str(), ImVec2(185, 20)))
						{
							Call(weaponDataList[j].weaponId.c_str());
						}
						ImGui::PopID();
					}
				}
				ImGui::End();
				break;
			case 0x15:
				category = 0x15 - 0x11;
				ImGui::Begin(Categories[category].c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				for (int j = 0; j < weaponDataList.size(); j++)
				{
					if (weaponDataList[j].categorie == Categories[category])
					{
						ImGui::PushID(j);
						if (ImGui::Button(weaponDataList[j].weaponName.c_str(), ImVec2(185, 20)))
						{
							Call(weaponDataList[j].weaponId.c_str());
						}
						ImGui::PopID();
					}
				}
				ImGui::End();
				break;
			case 0x16:
				category = 0x16 - 0x11;
				ImGui::Begin(Categories[category].c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				for (int j = 0; j < weaponDataList.size(); j++)
				{
					if (weaponDataList[j].categorie == Categories[category])
					{
						ImGui::PushID(j);
						if (ImGui::Button(weaponDataList[j].weaponName.c_str(), ImVec2(185, 20)))
						{
							Call(weaponDataList[j].weaponId.c_str());
						}
						ImGui::PopID();
					}
				}
				ImGui::End();
				break;
			case 0x17:
				 category = 0x17 - 0x11;
				 ImGui::Begin(Categories[category].c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				 ImGui::SetWindowFocus();
				 for (int j = 0; j < weaponDataList.size(); j++)
				 {
					 if (weaponDataList[j].categorie == Categories[category])
					 {
						 ImGui::PushID(j);
						 if (ImGui::Button(weaponDataList[j].weaponName.c_str(), ImVec2(185, 20)))
						 {
							 //give weapon
							 Call(weaponDataList[j].weaponId.c_str());
						 }
						 ImGui::PopID();
					 }
				 }
				 ImGui::End();
				break;
			case 0x18:
				category = 0x18 - 0x11;
				ImGui::Begin(Categories[category].c_str(), 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
				ImGui::SetWindowFocus();
				for (int j = 0; j < weaponDataList.size(); j++)
				{
					if (weaponDataList[j].categorie == Categories[category])
					{
						ImGui::PushID(j);
						if (ImGui::Button(weaponDataList[j].weaponName.c_str(), ImVec2(185, 20)))
						{
							//give weapon
							Call(weaponDataList[j].weaponId.c_str());
						}
						ImGui::PopID();
					}
				}
				ImGui::End();
				break;
			default:
				break;
			}
			

			
		}


	}


}