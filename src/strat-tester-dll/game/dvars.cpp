#include <std_include.hpp>

#define GET_DVAR(ptr) ((game::dvar_t*)((*(INT64*)(((uintptr_t)GetModuleHandle(NULL) + ptr)))))

namespace dvars
{
	game::dvar_t* ui_error_report = GET_DVAR(0x168EEEA8); //updated
	game::dvar_t* ui_error_report_delay = GET_DVAR(0x168EEEB0); //updated
	game::dvar_t* ui_error_callstack_ship = GET_DVAR(0x168EDCA0); //updated

	game::dvar_t* sv_mapname = GET_DVAR(0x177C57C8); //updated
	game::dvar_t* ui_mapname = GET_DVAR(0x179DFAC0); //updated
	game::dvar_t* ui_gametype = GET_DVAR(0x179DFAD0); //updated

	game::dvar_t* com_maxclients = GET_DVAR(0x168ED850); //updated
}