#pragma once

#include "misc.h"

import std;
import rapidjson;

using namespace std;

/*
    cleanup_str(string* str) -> void: Parse path string
    ---
    Input:
    * str is a reference to a string containing a path

    Output:
    * in-place, now str contains a path completely in lowercase and without quotes
*/
void cleanup_str(string* str)
{
    string path;
    for(auto& c : *str)
        if(c!='\"')
            path.push_back( tolower(c) );
    *str = path;
}

/*
    acquire_rom(string* rom_path) -> void: Get path to ROM
    ---
    Input:
    * rom_path is a reference to a string which will contain the ROM path

    Output:
    * in-place, now rom_path contains the path to the ROM
*/
void acquire_rom(string* rom_path)
{
    while(1)
    {
        print("Select ROM file: ");
        getline(cin, *rom_path);
        cleanup_str(rom_path);
        if(!rom_path->ends_with(".smc") && !rom_path->ends_with(".sfc"))
            error("Not a valid ROM file: {}", *rom_path);
        else
            break;
    }
}

/*
    acquire_list(string* list_path) -> void: Get path to list file
    ---
    Input:
    * list_path is a reference to a string which will contain the list file path

    Output:
    * in-place, now list_path contains the path to the list file
*/
void acquire_list(string* list_path)
{
    while(1)
    {
        print("Select list file (.txt only): ");
        getline(cin, *list_path);
        cleanup_str(list_path);
        if(!list_path->ends_with(".txt"))
            error("Not a valid list file: {}", *list_path);
        else
            break;
    }
}

/*
    cleanup(string tool_folder) -> bool: Erase temporary files
    ---
    Input:
    * tool_folder is the path to BOWSIE 

    Output:
    * true if temporary files were removed, false otherwise
*/
bool cleanup(string tool_folder)
{
    try
    {
        filesystem::remove(tool_folder+"asm/tmp.asm");
        filesystem::remove(tool_folder+"asm/bowsie_defines.asm");
        filesystem::remove(tool_folder+"asm/ssr.asm");
        filesystem::remove(tool_folder+"asm/*.bin");
        return true;
    }
    catch(filesystem::filesystem_error const & err)
    {
        println("There was an error cleaning up temporary files. Details: {}", err.code().message());
        return false;
    }
}

