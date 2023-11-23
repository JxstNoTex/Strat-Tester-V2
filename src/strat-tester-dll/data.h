#pragma once

namespace StratTester
{
    struct MenuCallback
    {
        int header; /*0x4*/
        bool active; /*0x1*/
        int callback; /*0x4*/

        long long magic;
        /*Format:
        *   0000:0:000:00000000
        *   header disc:
                    0xC8 = trigger response;
                    0x32 = error;
                    0x19 = Heartbeat;
            callback format:
                    0x0a = single Trigger
                    0x14 = multi Trigger
                    0x28 = No Return
            magic:
                8 bit magic number
                header + active + callback << 0x5


        */
    };
    MenuCallback* p_menu;

    /*Dev Note
    *info
    *This function will get called via isprofilebuiltin
    */
    void SendData();

}
