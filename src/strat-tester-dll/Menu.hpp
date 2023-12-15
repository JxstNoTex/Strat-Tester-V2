#pragma once
#include <std_include.hpp>

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
);



namespace StratTester
{
	void Update(int host, bool open);
	class Menu
	{
	public:
		Menu();
		~Menu();

		void draw();
		
	private:
		std::vector<std::string> m_perks;

	};
}