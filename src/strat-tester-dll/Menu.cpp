#include <std_include.hpp>
#include "includes.h"
#include "Menu.h"
#include <Windows.h>
#include <main.h>

bool _openMenu = 0;
int page = 0x0;

namespace StratTester
{
	Menu::Menu()
	{
		/*Dev Note:
		*Sanity Check
		*check for map name if map is zm_frontend return
		*/
		/*Dev Note
		*Function Add
		*get perks from gsc callback on entry
		*/
	}

	Menu::~Menu()
	{

	}

	void Menu::draw()
	{
		if (GetAsyncKeyState(0x38/* key 8*/) & 0x8000)
		{
			_openMenu = 1;
		}
		if (GetAsyncKeyState(0x39/* key 9*/) & 0x8000)
		{
			_openMenu = 0;
		}

		if (_openMenu == 1)
		{
			

			switch (page)
			{
			case 0x0:
				ImGui::Begin("Strat Tester V2", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

				if (ImGui::Button("Basic Options", ImVec2(185, 20)))
				{
					page = 0x1;
				}
				if (ImGui::Button("Weapon Options", ImVec2(185, 20)))
				{
					page = 0x2;
				}
				if (ImGui::Button("Perk Options", ImVec2(185, 20)))
				{
					page = 0x3;
				}
				if (ImGui::Button("Points", ImVec2(185, 20)))
				{
					page = 0x4;
				}
				if (ImGui::Button("Round Options", ImVec2(185, 20)))
				{
					page = 0x5;
				}
				if (ImGui::Button("Drop Options", ImVec2(185, 20)))
				{
					page = 0x6;
				}
				/*Dev Note:
				* Function Add
				* Set the Text to the map name where the player is currently on
				*/
				if (ImGui::Button("Map Options", ImVec2(185, 20))) 
				{
					page = 0x7;
				}
				if (ImGui::Button("Presets *WIP*", ImVec2(185, 20)))
				{
					page = 0x8;
				}
				if (ImGui::Button("Debug Options", ImVec2(185, 20)))
				{
					page = 0x9;
				}

				ImGui::End();
				break;
			case 0x1:
				ImGui::Begin("Basic Options", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

				if (ImGui::Button("God Mode", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("Unlimited Ammo", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("No Target", ImVec2(185, 20)))
				{

				}

				ImGui::End();
				break;
			case 0x2:
				ImGui::Begin("Weapon Options", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

				if (ImGui::Button("Give Weapon", ImVec2(185, 20)))
				{

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

				if (ImGui::Button("Give All Perks", ImVec2(185, 20)))
				{

				}
				if (ImGui::Button("Retain Perks", ImVec2(185, 20)))
				{

				}
				for (auto perk : m_perks)
				{
					if(ImGui::Button(perk, ImVec2(185,20)))
					{

					}
				}

				ImGui::End();
				break;
			case 0x4:
				ImGui::Begin("Points", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

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
			default:
				break;
			}
			if (ImGui::Button("Test", ImVec2(185, 20)))
			{

			}

			
		}


	}
}