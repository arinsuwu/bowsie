/*
 * BOWSIE - Better Overworld Sprite Insertion Engine
 * ---
 * BOWSIE's C++ code is (c) 2024-25 Erik Rios (aka Arinsu)
 * licensed under the BSD 2-clause license
 * 
 * 65816 ASM code is due to various authors:
 * - vldc9.asm and its routines originally by Lui37, Medic et al. in 2016
 *   modifications done by JackTheSpades in 2018 and Erik in 2018, 2024-25
 * - katrina.asm originally by Katrina in 2018 building upon vldc9.asm
 *   modifications done by Erik in 2024-25
 * - owrev.asm originally by Erik in 2024-25
 *   with some code and pointers by yoshifanatic
 * - MaxTile is (c) 2020-21 Vitor Vilela
 *   LoROM OR variant by yoshifanatic in 2023
 *   BOWSIE LoROM implementation by Erik in 2025, builds upon the above two
 * - All other ASM code (eg. macros, non-VLDC routines) is by Erik
 * 
 * Asar is (c) 2011 Alcaro, RPG Hacker, trillian/randomdude999 et al.
 * rapidjson is (c) 2015 Tencent
 * meOWmeOW is (c) 2025 Erik, named after MeiMei, which is (c) 2019 Akaginite/33953YoShI
*/

#include "include/misc.h"
#include "include/map16.h"
#include "include/rom.h"
#include "include/settings.h"

import std;
import rapidjson;
import asar;
import meowmeow;

#define VERSION 1
#define SUBVER 10

using namespace std;
using namespace rapidjson;
using namespace asar;
using namespace meOWmeOW;

// Constants
#define BOWSIE_USED_PTR 0x04EF3E        // must be freespace in bank 4
#define MAGIC_CONSTANT 0x00CAC705       // 00cactus - refer to FE!N by Jacques Webster and Jordan Carter
#define MAGIC_CONSTANT_WRITE ((MAGIC_CONSTANT&0xFF)<<24)|((MAGIC_CONSTANT&0xFF00)<<8)|((MAGIC_CONSTANT&0xFF0000)>>8)|((MAGIC_CONSTANT&0xFF000000)>>24)

#define OWREV_HANDLER_PTR 0x0480F6      // space OR leaves for a jump to a custom sprite system
#define OWREV_LOAD_HACK_PTR 0x02A861    // where the load sprites hijack for OR is located

int main(int argc, char *argv[])
{
    println("BOWSIE - Better Overworld Sprite Insertion Engine");
    println("\t v{}.{:0>2}", VERSION, SUBVER);
    println("\t By Erik\n");

    Rom rom;

    string list_path;
    map<string, variant<bool, int, string>> bowsie_settings;
    bowsie_settings["verbose"] = false;
    bowsie_settings["generate_map16"] = true;
    bowsie_settings["meowmeow"] = true;
    bowsie_settings["slots"] = -1;
    bowsie_settings["use_maxtile"] = true;
    bowsie_settings["custom_method_name"] = "";
    bowsie_settings["bypass_ram_check"] = false;

    bool cli_settings = false;
    bool rom_passed;
    bool list_passed;

    /*
        Parse inputs
    */
    switch(argc)
    {
        case 0:
        case 1:
            acquire_rom(&rom.rom_path);
            acquire_list(&list_path);
            break;
        default:
            vector<string> clean_argv;
            bool rom_acquired = false;
            bool list_acquired = false;

            for(int i=0;i<argc-1;++i)
            {
                string tmp = argv[i+1];
                cleanup_str(&tmp);
                if(tmp.ends_with(".smc") || tmp.ends_with(".sfc"))
                {
                    rom.rom_path = tmp;
                    rom_acquired = true;
                }
                else if(tmp.ends_with(".txt"))
                {
                    list_path = tmp;
                    list_acquired = true;
                }
                else
                    clean_argv.push_back(tmp);
            }

            if(clean_argv.size()>0)
            {
                parse_cli_settings(clean_argv, bowsie_settings);
                cli_settings = true;
            }

            if(!rom_acquired)
                acquire_rom(&rom.rom_path);
            if(!list_acquired)
                acquire_list(&list_path);
    }

    /*
        Existence checks
    */

    string method;

    if(!rom.open_rom())
        exit(error("Couldn't open ROM file {}", rom.rom_path));
    ifstream list(list_path);
    if(!list)
        exit(error("Couldn't open list file {}", list_path));

    /*
        Parse settings
    */
    bool settings_json_exists = false;
    Document settings_json;

    // First we look for bowsie-config.json in the ROM path
    // if not there, we look in the root folder
    {
        string tmp(filesystem::relative(rom.rom_path).string());
        #ifdef _WIN32
            string settings_path(tmp.begin(), tmp.begin()+tmp.find_last_of("\\")+1);
        #else
            string settings_path(tmp.begin(), tmp.begin()+tmp.find_last_of("/")+1);
        #endif
        settings_path.append("bowsie-config.json");

        bool process_json = false;
    
        ifstream ifs(settings_path);
        if(!ifs)
        {
            tmp = argv[0];
            settings_path = tmp.append("/../bowsie-config.json");

            ifs = ifstream(settings_path);
            if(!(!ifs))
            {
                if(cli_settings)
                    println("WARNING: bowsie-config.json exists but CLI settings were also passed. The CLI settings will be ignored.");
                process_json = true;
            }
        }
        else
        {
            if(cli_settings)
                println("WARNING: bowsie-config.json exists but CLI settings were also passed. The CLI settings will be ignored.");
            process_json = true;
        }

        if(process_json)
        {
            BasicIStreamWrapper isw(ifs);

            settings_json.ParseStream(isw);
            if(settings_json.HasParseError())
                exit(error("A problem occurred while parsing bowsie-config.json. Please ensure the file isn't corrupted and has a valid JSON format. Check the readme for more information."));
            else
                settings_json_exists = true;
        }
    }

    if(settings_json_exists)
    {
        string settings_err;
        if(!deserialize_json(&settings_json, bowsie_settings, &settings_err))
            exit(error("A problem occurred while parsing specific settings. Details:\n{}", settings_err));
    }

    bool verbose = get<bool>(bowsie_settings["verbose"]);
    bool generate_map16 = get<bool>(bowsie_settings["generate_map16"]);
    bool run_meowmeow = get<bool>(bowsie_settings["meowmeow"]);
    int slots = get<int>(bowsie_settings["slots"]);
    bool use_maxtile = get<bool>(bowsie_settings["use_maxtile"]);
    string custom_method_name = get<string>(bowsie_settings["custom_method_name"]);
    bool bypass_ram_check = get<bool>(bowsie_settings["bypass_ram_check"]);

    // OW Revolution check
    bool ow_rev;
    if(rom.read<2>(0x048000)==0x5946 && rom.read<2>(0x048002)==0x4F52)
    {
        if(verbose)
            println("Overworld Revolution detected.\n  meOwmeOW has been disabled.\n  MaxTile has been enabled.");

        ow_rev = true;
        method = "owrev";

        // Settings hardcoded to OR
        run_meowmeow = false;
        use_maxtile = true;
        slots = slots == -1 ? 32 : slots;
    }
    else
    {
        ow_rev = false;
        method = custom_method_name=="" ? "vldc9" : "custom";
        slots = slots == -1 ? 24 : slots;
    }

    /*
        Verify settings
    */
    if(slots<=0)
        exit(error("Can't have zero or less slots. It makes no sense."));
    if(!ow_rev)
        if(slots>24 && !bypass_ram_check)
            exit(error("Can't have more than 24 slots due to memory limitations."));
    else
        if(slots>32 && !bypass_ram_check)
            exit(error("Can't have more than 32 slots due to memory limitations."));
    if( bypass_ram_check && method!="custom" && slots>255 )
        exit(error("Can't have more than 255 slots without a custom insertion handler."));
    if(method!="vldc9" && method!="owrev" && method!="custom")
        exit(error("Unknown insertion method: {}", method));

    // Misc. ROM checks
    if(rom.rom_size<0x100000)
        exit(error("This ROM is clean. Please edit it in Lunar Magic."));
    if((rom.read<3>(0x00FFC0))!=0x535550) // SUP
        exit(error("Title mismatch. Is this ROM headered?"));

    if(verbose)
    {
        println("\nVerbose mode enabled.");
        println("Running BOWSIE with");
        println("Slots:\t\t\t\t{}", slots);
        println("Insertion method:\t\t{} {}", method, method=="custom" ? "("+custom_method_name+")" : "");
        println("MaxTile:\t\t\t{}", use_maxtile ? "Enabled" : "Disabled");
        println("meOWmeOW:\t\t\t{}", run_meowmeow ? "Enabled" : "Disabled");
        println("RAM checks:\t\t\t{}", bypass_ram_check ? "Ignored (!)" : "Enabled");
        println("Asar version: v{}.{}{}\n", (int)(((asar_version()%100000)-(asar_version()%1000))/10000),
                                            (int)(((asar_version()%1000)-(asar_version()%10))/100),
                                            (int)(asar_version()%10));
    }
    else
        println("");

    int asar_errcount = 0;
    string tool_folder = filesystem::absolute(argv[0]).parent_path().string()+"/";
    string rom_name = filesystem::absolute(rom.rom_path).parent_path().string()+"/"+filesystem::path(rom.rom_path).stem().string();

    string full_patch(format("incsrc \"{1}asm/{0}_defines.asm\"\nincsrc \"{1}asm/ssr.asm\"\nincsrc \"{1}asm/macro.asm\"\nincsrc \"{1}asm/maxtile_defines.asm\"\n\n", method, tool_folder));
    {
        string tmp;
        // Insert the method
        {
            ifstream method_patch;
            if(method=="custom")
            {
                method_patch.open(tool_folder+"asm/"+custom_method_name+".asm", ios::ate);
                if(!method_patch)
                    exit(error("Couldn't open ASM files for custom method {0}. Make sure {0}.asm is in the asm folder.", custom_method_name));
            }
            else
            {
                method_patch.open(tool_folder+"asm/"+method+".asm", ios::ate);
                if(!method_patch)
                    exit(error("Couldn't open ASM files for insertion method {0}. Make sure {0}.asm is in the asm folder.", method));
            }
            method_patch.seekg(0);
            getline(method_patch, tmp, '\0');
        }
        full_patch.append(tmp);
    }

    // Figure out whether the tool's been used and clean-up if so
    if( (rom.read<2>(BOWSIE_USED_PTR)==(MAGIC_CONSTANT&0xFFFF0000)>>16) && (rom.read<2>(BOWSIE_USED_PTR+2)==(MAGIC_CONSTANT&0xFFFF)) )
    {
        if(verbose)
            println("Performing clean-up of a previous execution...");
        destroy_map16(rom_name);

        string clean_patch;
        int clean_offset = 4;
        while(1)
        {
            if(rom.read<3>(BOWSIE_USED_PTR+clean_offset)==0x555555)
            {
                clean_patch.append(format("org ${:0>6X}\n    dl $FFFFFF", BOWSIE_USED_PTR+clean_offset));
                break;
            }
            clean_patch.append(format("autoclean read3(${0:0>6X})\norg ${0:0>6X}\n    dl $FFFFFF\n", BOWSIE_USED_PTR+clean_offset));
            clean_offset+=3;
        }
        if(ow_rev)
            if(rom.read<1>(OWREV_HANDLER_PTR)==0x20)
                {
                    clean_patch.append(format("\norg $04{:0>4X}\n    padbyte $00 : pad ${:0>6X}",
                                              rom.read<1>(OWREV_LOAD_HACK_PTR+1)|((rom.read<1>(OWREV_LOAD_HACK_PTR+2)<<8)&0xFF00), BOWSIE_USED_PTR));
                }
        if(!rom.inline_patch(tool_folder, clean_patch.c_str()))
            exit(error("An error ocurred while cleaning up. Details:\n  {}", asar_geterrors(&asar_errcount)->fullerrdata));
        if(!rom.reload())
            exit(error("An error ocurred while cleaning up."));
        else if(verbose)
            println("Clean-up done.\n");

    }
    else
    {
        println("First time using BOWSIE! Thank you. :)\n");
        rom.inline_patch(tool_folder, format("org ${:0>6X} : dd ${:0>8X}", BOWSIE_USED_PTR, MAGIC_CONSTANT_WRITE).c_str());
    }

    // BOWSIE-specific defines
    string defines(format("\
!bowsie_ow_slots = {}\n\
!bowsie_version = {}\n\
!bowsie_subversion = {}\n\n\
!bowsie_contiguous = {}\n\n\
!bowsie_maxtile = {}\n\n\
if read1($00FFD5) == $23\n\
    if read1($00FFD7) == $0D\n\
        fullsa1rom\n\
    else\n\
        sa1rom\n\
    endif\n\
    !sa1 = 1\n\
    !dp = $3000\n\
    !addr = $6000\n\
    !bank = $000000\n\
else\n\
    lorom\n\
    !sa1 = 0\n\
    !dp = $0000\n\
    !addr = $0000\n\
    !bank = $800000\n\
endif\n\n\
!bowsie_owrev = 0\n\
if read4($048000) == $524F4659\n\
    !bowsie_owrev = 1\n\
endif\n\n\
!bowsie_widescreen_ow = equal(read1($04FB2A), $5C)\n\
if !bowsie_owrev\n\
    !bowsie_widescreen_ow = not(equal(read1($0480D3)&$30, $00))\n\
endif\n\n\
optimize dp always\n\
dpbase !dp\n\n\
optimize address mirrors\n\
bank auto\n\n", slots, VERSION, SUBVER, method=="katrina" ? '1' : '0', use_maxtile ? '1' : '0'));

    /*
        These loops locate where certain code is inserted by Overworld Revolution.
        Since the patch has many variables, these hijack spots are NEVER fixed.
    */
    if(ow_rev)
    {
        // Freespace
        for(int i=0;;++i)
        {
            if(rom.read<4>(0x04ACD0+i)==0x00000000 && rom.read<4>(0x04ACD0+i+4)==0x00000000)
            {
                defines.append(format("!owrev_bank_4_freespace = ${:0>6X}\n", 0x04ACD0+i));
                if(verbose)
                    println("Using OW Revolution bank 4 freespace located at ${:0>6X}\n", 0x04ACD0+i);
                break;
            }
            if(0x04ACD0+i>=BOWSIE_USED_PTR)
                exit(error("Found no OW Revolution freespace in bank 4."));
        }
    }
    ofstream(tool_folder+"asm/bowsie_defines.asm").write(defines.c_str(), defines.size());

    // Shared sub-routines
    string routine_macro(format("incsrc {}_defines.asm\nincsrc maxtile_defines.asm\n\n", method));
    string routine_content("freecode cleaned\n\n");
    vector<string> routine_names;
    for(auto routine : filesystem::directory_iterator(tool_folder+"routines"))
    {
        string routine_path(routine.path().string());
        string routine_name(routine.path().stem().string());
        routine_names.push_back(routine_name);
        routine_macro.append(format("macro {0}()\n    JSL {0}_{0}\nendmacro\n\n", routine_name));
        routine_content.append(format("namespace {0}\n    {0}:\n        incsrc \"{1}\"\n    print \"Shared subroutine %{0}() inserted at $\", hex({0})\nnamespace off\n\n",\
        routine_name, routine_path));
    }

    int offset = 0;
    if(!rom.inline_patch(tool_folder, (routine_macro+routine_content).c_str()))
        exit(error("An error ocurred while inserting shared subroutines. Details:\n  {}", asar_geterrors(&asar_errcount)->fullerrdata));
    else
    {
        routine_macro = "";
        routine_content = "";
        if(verbose)
            println("===========================================================");
        auto routine_print = asar_getprints(&asar_errcount);
        for(int i=0;i<asar_errcount;++i)
        {
            string routine_addr(routine_print[i]);
            routine_addr = string(routine_addr.begin()+routine_addr.find_first_of("$"), routine_addr.end());
            routine_macro.append(format("macro {}()\n    JSL {}\nendmacro\n\n", routine_names[i], routine_addr));
            routine_content.append(format("org ${:0>6X}\n    dl {}\n", BOWSIE_USED_PTR+4+(i*3), routine_addr));
            if(verbose)
            {
                println("{}", routine_print[i]);
                println("-----------------------------------------------------------");
            }
        }
        println("{} shared subroutines inserted.{}", asar_errcount, verbose ? "\n===========================================================\n" : "");
        offset = asar_errcount;
    }
    ofstream(tool_folder+"asm/ssr.asm").write(routine_macro.c_str(), routine_macro.size());
    rom.inline_patch(tool_folder, routine_content.c_str());

    // Pointers
    unsigned char * ow_init_ptrs = new unsigned char[0x7E*3] {};
    unsigned char * ow_main_ptrs = new unsigned char[0x7E*3] {};
    rom.new_extra_bytes = new char[0x7F] {};

    rom.new_extra_bytes[0x7E] = 0x03;
    for(int i=0;i<0x7E;++i)
    {
        if(!ow_rev)
        {
            ow_init_ptrs[i*3] = 0x14;
            ow_main_ptrs[i*3] = 0x14;
            ow_init_ptrs[1+i*3] = 0x84;
            ow_main_ptrs[1+i*3] = 0x84;
            ow_init_ptrs[2+i*3] = 0x04;
            ow_main_ptrs[2+i*3] = 0x04;
        }
        else
        {
            ow_init_ptrs[i*3] = 0xFF;
            ow_main_ptrs[i*3] = 0xFF;
            ow_init_ptrs[1+i*3] = 0xFF;
            ow_main_ptrs[1+i*3] = 0xFF;
            ow_init_ptrs[2+i*3] = 0x04;
            ow_main_ptrs[2+i*3] = 0x04;
        }
        rom.new_extra_bytes[i] = 0x03;
    }

    meowmeow meowmeow;
    if(run_meowmeow)
        if(!meowmeow.init_meowmeow(rom))
            exit(error("An error ocurred while initializing meOWmeOW. Aborting execution."));

    // Sprites
    int sprite_count = 0;
    Map16 map16;
    if(generate_map16)
    {
        map16.open_s16ov(string(rom_name+".s16ov").c_str());
        map16.open_sscov(string(rom_name+".sscov").c_str());
    }
    if(verbose)
        println("===========================================================");
    for(string sprite; getline(list, sprite);)
    {
        try
        {
            size_t pos {};
            if(sprite.find_first_not_of("\t ")==string::npos)
                continue;
            if(sprite.starts_with(";"))
                continue;
            int sprite_number = stoi(sprite, &pos, 16);
            if(sprite_number<0x01 || sprite_number>0x7F) throw out_of_range("");
            string sprite_filename(sprite.substr(sprite.find_first_not_of("\t ", pos)));
            cleanup_str(&sprite_filename);
            if(!sprite_filename.ends_with(".asm") && !sprite_filename.ends_with(".asm\""))
                exit(error("Unknown extension for sprite {}. (Remember the list file looks for the .asm file, NOT the .json tooltip!)", sprite_filename));
            string sprite_labelname(sprite_filename.substr(0, sprite_filename.find_first_of("."))+"_"+to_string(sprite_number));

            if(!ow_rev && (ow_init_ptrs[2+(sprite_number-1)*3]<<16 | ow_init_ptrs[1+(sprite_number-1)*3]<<8 | ow_init_ptrs[(sprite_number-1)*3])!=0x048414 ) throw invalid_argument("");
            if(ow_rev && (ow_init_ptrs[2+(sprite_number-1)*3]<<16 | ow_init_ptrs[1+(sprite_number-1)*3]<<8 | ow_init_ptrs[(sprite_number-1)*3])!=0x04FFFF ) throw invalid_argument("");

            ifstream curr_sprite(tool_folder+"sprites/"+sprite_filename);
            if(!curr_sprite)
                exit(error("Could not open sprite with number {:0>2X} and filename {}. Make sure the sprite exists and is in the sprites directory.", sprite_number, sprite_filename));
            string insert_sprites(format("incsrc {0}_defines.asm\n\
incsrc maxtile_defines.asm\n\
incsrc ssr.asm\n\
incsrc macro.asm\n\
namespace {1}\n\
    freecode cleaned\n\
    incsrc {4}\n\
    !extra ?= 2\n\
    print \"Sprite {3:0>2X} - {2}\"\n\
    print \"    Init routine inserted at: $\", hex({1}_init)\n\
    print \"    Main routine inserted at: $\", hex({1}_main)\n\
    print \"    Extra bytes: \", dec(!extra)\n\
namespace off\n", method, sprite_labelname, sprite_filename, sprite_number, ("\""+tool_folder+"sprites/"+sprite_filename+"\"")));
            if(!rom.inline_patch(tool_folder, insert_sprites.c_str()))
                exit(error("Could not insert sprite {}. Details: {}\n", sprite_filename, asar_geterrors(&asar_errcount)->fullerrdata));
            else
            {
                bool init = true;
                auto sprite_print = asar_getprints(&asar_errcount);
                size_t pos {};
                for(int i=0;i<asar_errcount;++i)
                {
                        string sprite_addr(sprite_print[i]);
                        if(verbose)
                            println("{}", sprite_addr);
                        if(sprite_addr.contains('$'))
                        {
                            sprite_addr = string(sprite_addr.begin()+sprite_addr.find_first_of("$")+1, sprite_addr.end());
                            if(init)
                            {
                                init = false;
                                ow_init_ptrs[2+(sprite_number-1)*3] = stoi(string(sprite_addr.begin(),sprite_addr.begin()+2), &pos, 16);
                                ow_init_ptrs[1+(sprite_number-1)*3] = stoi(string(sprite_addr.begin()+2,sprite_addr.begin()+4), &pos, 16);
                                ow_init_ptrs[0+(sprite_number-1)*3] = stoi(string(sprite_addr.begin()+4,sprite_addr.begin()+6), &pos, 16);
                            }
                            else
                            {
                                init = true;
                                ow_main_ptrs[2+(sprite_number-1)*3] = stoi(string(sprite_addr.begin(),sprite_addr.begin()+2), &pos, 16);
                                ow_main_ptrs[1+(sprite_number-1)*3] = stoi(string(sprite_addr.begin()+2,sprite_addr.begin()+4), &pos, 16);
                                ow_main_ptrs[0+(sprite_number-1)*3] = stoi(string(sprite_addr.begin()+4,sprite_addr.begin()+6), &pos, 16);
                            }
                        }
                        else if(sprite_addr.contains("Extra bytes:"))
                        {
                            sprite_addr = string(sprite_addr.begin()+sprite_addr.find_first_of(":")+2, sprite_addr.end());
                            try
                            {
                                int extra = stoi(sprite_addr);
                                if(extra<=-1 || extra>=9) throw out_of_range("");
                                rom.new_extra_bytes[sprite_number-1] = 0x03+extra;
                            }
                            catch(invalid_argument const & err)
                            {
                                exit(error("Error parsing the extra bytes for {}: could not parse extra byte define\n  {}\nMake sure the define is a number between 0 and 8.\n", sprite_filename, sprite_addr));
                            }
                            catch(out_of_range const & err)
                            {
                                exit(error("Error parsing the extra bytes for {}: invalid amount of extra bytes\n  {}\n", sprite_filename, sprite_addr));
                            }
                        }
                }
            }
            if(generate_map16)
            {
                string sprite_tooltip_path = tool_folder+"sprites/"+string(sprite_filename.begin(), sprite_filename.begin()+sprite_filename.find_first_of(".")+1)+"json";
                ifstream ifs(sprite_tooltip_path);
                if(!(!ifs))
                {
                    Document sprite_tooltip;
                    BasicIStreamWrapper isw(ifs);
                    string e;
                    if(verbose)
                        println("Parsing tooltip information for {}...", sprite_filename);
                    sprite_tooltip.ParseStream(isw);
                    if(sprite_tooltip.HasParseError())
                        exit(error("A problem occurred while parsing {}. Please ensure the file isn't corrupted and has a valid JSON format.", sprite_tooltip_path));
                    if(!map16.deserialize_json(&sprite_tooltip, &e))
                        exit(error("A problem occurred while parsing specific tooltips for {}. Details:\n{}", sprite_tooltip_path, e));
                    e = "";
                    if(!map16.write_tooltip(sprite_number, &e))
                        exit(error("A problem has ocurred while generating tooltips for {}. Details:\n{}", sprite_tooltip_path, e));
                    else if(verbose)
                        println("Done.");
                }
                else if(verbose)
                    println("{} has no tooltip information.", sprite_filename);
            }
            if(verbose)
                println("-----------------------------------------------------------");
            ++sprite_count;
        }
        catch(invalid_argument const & err)
        {
            exit(error("Error parsing the list file: duplicate sprite number\n  {}\n", sprite));
        }
        catch(out_of_range const & err)
        {
            exit(error("Error parsing the list file: incorrect sprite number\n  {}\n", sprite));
        }
    }
    println("{} sprites inserted.\n{}", sprite_count, verbose ? "===========================================================" : "");
    ofstream(tool_folder+"asm/init_ptrs.bin", ios::binary).write((char *)ow_init_ptrs, 0x7E*3);
    ofstream(tool_folder+"asm/main_ptrs.bin", ios::binary).write((char *)ow_main_ptrs, 0x7E*3);
    ofstream(tool_folder+"asm/extra_size.bin", ios::binary).write(rom.new_extra_bytes, 0x7F);

    // Sprite system
    full_patch.append(format("\norg ${:0>6X}\n\
    ow_sprite_init_ptrs:\n\
        incbin init_ptrs.bin\n\
    ow_sprite_main_ptrs:\n\
        incbin main_ptrs.bin\n\n\
    dl $555555", BOWSIE_USED_PTR+4+(offset*3)));
    if(ow_rev)
        full_patch.append("\npushpc\norg $04FFFF\n    RTL\npullpc");
    if(!rom.inline_patch(tool_folder, full_patch.c_str()))
    {
        if(verbose)
        {
            auto system_prints = asar_getprints(&asar_errcount);
            for(int i=0;i<asar_errcount;++i)
                println("Captured print while inserting the sprite system: {}", system_prints[i]);
        }
        exit(error("Something went wrong while applying the sprite system. Details:\n  {}", asar_geterrors(&asar_errcount)->fullerrdata));
    }

    // meOWmeOW, if asked
    if(run_meowmeow)
    {
        vector<uint8_t> new_sprite_data;
        if(!meowmeow.execute_meowmeow(rom, tool_folder, new_sprite_data))
            exit(error("Something went wrong while applying meOWmeOW. Details:\n  {}", asar_geterrors(&asar_errcount)->fullerrdata));
    }
        
    // Done
    println("All sprites inserted successfully!\nRemember to run the tool again when you insert a custom OW sprite in Lunar Magic.");
    map16.done(string(rom_name+".s16ov").c_str());
    rom.done(run_meowmeow);
    if(!cleanup(tool_folder))
        exit(error("Error cleaning up temporary files. Exiting... (Your ROM was still saved anyway)"));
    #if defined(_WIN32)
        system("pause");
    #else
        printf("Press Enter to continue");
        getchar();
    #endif
    exit(0);
}

// cl /nologo /Febowsie /std:c++latest /EHsc /O2 /W2 /reference "std=src/std.ifc" /reference "rapidjson=src/rapidjson.ifc" /reference "asar=src/asar.ifc" /reference "meowmeow=src/meowmeow.ifc" src/bowsie.cpp src/include/misc.cpp src/include/map16.cpp src/include/rom.cpp src/include/settings.cpp src/std.obj src/rapidjson.obj src/asar.obj /link src/bowsie.res
// rc /nologo bowsie.rc

// for_each(rom_path.begin()+rom_path.size()-3,rom_path.end(),[](char &c){c = toupper(c);});

// #define LOWERCASE(str) for_each(str.begin(),str.end(),[](char & c) \
// { c = tolower(static_cast<unsigned char>(c));})