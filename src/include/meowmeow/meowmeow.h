/*
 *  meOWmeOW
 *  (c) 2025 Erik "Arinsu" Rios - eselerik@outlook.com
*/
#pragma once


#include <iostream>
#include <vector>

#include <cstdint>

#include <fmt/base.h>

#include "../asar/asar.h"

#include "../misc.h"
#include "../rom.h"
#include "../settings.h"

using ios = std::ios;
using uint8_t = std::uint8_t;

#define HEADER_SIZE 512

// Traditional OW constants
#define OW_SPRITE_EXTRA_BYTES_PTR 0x0DE18C
#define OW_SPRITE_DATA_PTR 0x0EF55D
#define OW_SUBMAPS 7

// Overworld Revolution constants
#define OWREV_FIRST_MAP_LVL 0x0480D6
#define OWREV_SUBMAPS 0x0480D8
#define LVL_SPRITE_EXTRA_BYTES_PTR 0x0EF30C
#define LVL_SPRITE_DATA_PTR 0x05EC00
#define LVL_SPRITE_DATA_PTR_BANK 0x0EF100
#define EXTRA_BIT_OFFSET 0x100
#define SPRITE_SIZE_LIMIT 0x800

namespace meOWmeOW
{
    /*
        struct meowmeow: detect and correct extra byte changes
        ---
    */
    struct meowmeow
    {
        public:
            bool ow_rev;

            /*
                init_meowmeow(Rom& rom) -> bool: initialize meOWmeOW
                ---  
                Input:
                * rom is a reference to a Rom object

                Output:
                * true if the ROM data could be read successfully, false otherwise
            */
            bool init_meowmeow(Rom& rom);

            /*
                execute_meowmeow(Rom& rom, string tool_folder, vector<uint8_t>& new_sprite_data) -> run meOWmeOW on ROM
                ---  
                Input:
                * rom is a reference to a Rom object
                * tool_folder is the path to BOWSIE
                * new_sprite_data is a reference to a vector of 1-byte integers where to push the corrected data

                Output:
                * true if the ROM data could be fixed (or meOWmeOW didn't need to run), false otherwise
            */
            bool execute_meowmeow(Rom& rom, std::string tool_folder, std::vector<uint8_t>& new_sprite_data);

        private:
            bool exec_meowmeow_lvl(Rom& rom, std::string tool_folder, std::vector<uint8_t>& new_sprite_data);
            bool exec_meowmeow_ow(Rom& rom, std::string tool_folder, std::vector<uint8_t>& new_sprite_data);
    };

}

