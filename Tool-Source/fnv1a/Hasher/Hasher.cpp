// Hasher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

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
	{"Z",0x5A}
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


inline uint32_t fnv1a(const char* key) {

	const char* data = key;
	uint32_t hash = 0x4B9ACE2F;
	while (*data)
	{
		hash ^= tolower(*data);
		hash *= 0x1000193;
		data++;
	}
	hash *= 0x1000193;
	return hash;
}



std::map<std::string, int> parseFile(const std::string& filename) {
	std::ifstream file(filename);
	std::string line;
	std::map<std::string, int> mapping;

	if (file.is_open())
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
				printf("%s, %s, %s\n", s_input[0].c_str(), s_input[1].c_str(),s_input[2].c_str());
				s_input.clear();
				// Now bind, key, and action hold the three parts of the line
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

int main()
{
	std::map<std::string, int> keybinds;
	const char* h = "insertData";

	//keybinds = parseFile("C:\\Users\\Tex\\source\\repos\\Strat-Tester-V2\\build\\x64\\Debug\\bindings_0.cfg");
    std::cout << h << " " << std::hex << fnv1a(h) << std::endl;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
