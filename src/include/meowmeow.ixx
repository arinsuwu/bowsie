/*
 *  meOWmeOW
 *  (c) 2025 Erik "Arinsu" Rios - eselerik@outlook.com
*/

module;

#include "rom.h"

export module meowmeow;

#pragma warning(push)
#pragma warning(disable: 5244)

import asar;
import std;

using namespace std;
using namespace asar;

#define HEADER_SIZE 512
#define OW_SPRITE_EXTRA_BYTES_PTR 0x0DE18C
#define OW_SPRITE_DATA_PTR 0x0EF55D
#define SUBMAPS 7

namespace meOWmeOW
{
    /*
        struct meowmeow: detect and correct extra byte changes
        ---
    */
    export struct meowmeow
    {
        /*
            init_meowmeow(Rom& rom) -> bool: initialize meOWmeOW
            ---  
            Input:  rom is a reference to a Rom object  
            Output: true if the ROM data could be read successfully, false otherwise
        */
        bool init_meowmeow(Rom& rom)
        {
            // Verify if there is an extra byte table already in ROM.
            rom.old_extra_bytes = new char[0x80] { 0x03 };
            const int extra_byte_loc = snestopc_pick( (rom.read<1>(OW_SPRITE_EXTRA_BYTES_PTR)&0xFF) | \
                                                      (rom.read<1>(OW_SPRITE_EXTRA_BYTES_PTR+1)&0xFF)<<8 | \
                                                      (rom.read<1>(OW_SPRITE_EXTRA_BYTES_PTR+2)&0xFF)<<16 )+HEADER_SIZE;
            if(rom.read<1>(OW_SPRITE_EXTRA_BYTES_PTR+3) == 0x42)
            {
                // Extra byte table exists, acquire it from the ROM.
                rom.rom_data.clear();
                rom.rom_data.seekg(extra_byte_loc);
                rom.rom_data.read(rom.old_extra_bytes, 0x80);

                if(rom.rom_data.rdstate() != ios::goodbit)
                    return false;
            }
            else
            {
                // Extra byte table does not exist, initialize it to 0x04 for backwards compatibility (three bytes for number/position + 1 extra)
                for(int i=1;i<0x80;++i)
                    rom.old_extra_bytes[i] = 0x04;
            }

            return true;
        }

        /*
            execute_meowmeow(Rom& rom, string tool_folder, vector<uint8_t>& new_sprite_data) -> run meOWmeOW on ROM
            ---  
            Input:  rom is a reference to a Rom object  
                    tool_folder is the path to BOWSIE  
                    new_sprite_data is a reference to a vector of 1-byte integers where to push the corrected data
            Output: true if the ROM data could be fixed (or meOWmeOW didn't need to run), false otherwise
        */
        bool execute_meowmeow(Rom& rom, string tool_folder, vector<uint8_t>& new_sprite_data)
        {
            // Verify whether we need meOWmeOW, by any extra byte changes
            bool run_meowmeow = false;
            for(int i=0;i<0x7F;++i)
            {
                if(rom.old_extra_bytes[i+1] != rom.new_extra_bytes[i])
                {
                    run_meowmeow = true;
                    break;
                }
            }

            if(run_meowmeow)
            {
                println("Extra byte changes detected. Running meOWmeOW to align data.");

                const int ow_sprite_data = (rom.read<1>(OW_SPRITE_DATA_PTR)&0xFF) | \
                                           (rom.read<1>(OW_SPRITE_DATA_PTR+1)&0xFF)<<8 | \
                                           (rom.read<1>(OW_SPRITE_DATA_PTR+2)&0xFF)<<16;
                int submaps_processed = 0;
                int bytes_processed = 0;
                int submap_offset[SUBMAPS] = { (rom.read<1>(ow_sprite_data)&0xFF) | \
                                               (rom.read<1>(ow_sprite_data+1)&0xFF)<<8,
                                               (rom.read<1>(ow_sprite_data+2)&0xFF) | \
                                               (rom.read<1>(ow_sprite_data+3)&0xFF)<<8,
                                               (rom.read<1>(ow_sprite_data+4)&0xFF) | \
                                               (rom.read<1>(ow_sprite_data+5)&0xFF)<<8,
                                               (rom.read<1>(ow_sprite_data+6)&0xFF) | \
                                               (rom.read<1>(ow_sprite_data+7)&0xFF)<<8,
                                               (rom.read<1>(ow_sprite_data+8)&0xFF) | \
                                               (rom.read<1>(ow_sprite_data+9)&0xFF)<<8,
                                               (rom.read<1>(ow_sprite_data+10)&0xFF) | \
                                               (rom.read<1>(ow_sprite_data+11)&0xFF)<<8,
                                               (rom.read<1>(ow_sprite_data+12)&0xFF) | \
                                               (rom.read<1>(ow_sprite_data+13)&0xFF)<<8 };


                // Prepare ROM data by putting the needle in the first map's data.
                rom.rom_data.clear();
                rom.rom_data.seekg(snestopc_pick(ow_sprite_data+submap_offset[0])+HEADER_SIZE);
                while(1)
                {
                    // These two are always present: they define a sprite number and its position.
                    uint8_t sprite_xnnnnnnn = rom.rom_data.get();
                    uint8_t sprite_yyyxxxxx = rom.rom_data.get();

                    new_sprite_data.push_back(sprite_xnnnnnnn);
                    new_sprite_data.push_back(sprite_yyyxxxxx);
                    bytes_processed += 2;

                    if(sprite_xnnnnnnn == 0x00 && sprite_yyyxxxxx == 0x00)
                    {
                        // Stop when all maps have been processed.
                        if(++submaps_processed==SUBMAPS)
                            break;

                        // Move to next map's sprite data.
                        rom.rom_data.clear();
                        rom.rom_data.seekg(snestopc_pick(ow_sprite_data+submap_offset[submaps_processed])+HEADER_SIZE);

                        // Store the new offset for new map and move on.
                        // The very first map (Main Map in LM) will *always* be 0x000E -- notice this is SUBMAPS*2.
                        submap_offset[submaps_processed] = bytes_processed+(SUBMAPS*2);
                        continue;
                    }

                    // Save third always present byte.
                    uint8_t sprite_zzzzzyyy = rom.rom_data.get();
                    new_sprite_data.push_back(sprite_zzzzzyyy);

                    // Verify if this is a sprite which changed.
                    int sprite_number = sprite_xnnnnnnn&0x7F;
                    int old_extra = rom.old_extra_bytes[sprite_number]-0x03;
                    int new_extra = rom.new_extra_bytes[sprite_number-1]-0x03;

                    if(old_extra <= new_extra)
                    {
                        // The same amount of extra bytes: copy the same data
                        for(int i=0; i<old_extra; ++i)
                            new_sprite_data.push_back((uint8_t)(rom.rom_data.get()));

                        // More extra bytes: fill new with 00
                        for(int i=old_extra; i<new_extra; ++i)
                            new_sprite_data.push_back(0x00);
                    }
                    else
                    {
                        // Less extra bytes: only copy the required ones
                        for(int i=0; i<new_extra; ++i)
                            new_sprite_data.push_back((uint8_t)(rom.rom_data.get()));

                        // Discard the rest of the old extra bytes
                        for(int i=new_extra; i<old_extra; ++i)
                            rom.rom_data.get();
                    }

                    bytes_processed += new_extra+1;
                }
                
                // Save table of corrected sprites
                ofstream(tool_folder+"asm/new_sprite_data.bin", ios::binary).write((char *)new_sprite_data.data(), new_sprite_data.size());

                string meowmeow_patch = format("if read1($00FFD5) == $23\n\
    if read1($00FFD7) == $0D\n\
        fullsa1rom\n\
    else\n\
        sa1rom\n\
    endif\n\
else\n\
    lorom\n\
endif\n\
autoclean read3(${0:0>6X})\n\
org ${0:0>6X}\n\
    autoclean dl new_sprite_data\n\n\
freedata\n\
new_sprite_data:\n\
    dw ${1:0>4X}\n\
    dw ${2:0>4X}\n\
    dw ${3:0>4X}\n\
    dw ${4:0>4X}\n\
    dw ${5:0>4X}\n\
    dw ${6:0>4X}\n\
    dw ${7:0>4X}\n\n\
    incbin \"{8}asm/new_sprite_data.bin\"", OW_SPRITE_DATA_PTR, submap_offset[0],
                                                                 submap_offset[1],
                                                                 submap_offset[2],
                                                                 submap_offset[3],
                                                                 submap_offset[4],
                                                                 submap_offset[5],
                                                                 submap_offset[6],
                                                                 tool_folder);

                return rom.inline_patch(tool_folder, meowmeow_patch.c_str());
            }

            return true;
        }

    };

}

export using meOWmeOW::meowmeow;

#pragma warning(pop)

