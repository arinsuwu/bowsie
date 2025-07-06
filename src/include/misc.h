#pragma once

import std;
import rapidjson;

/*
    Auxiliary functions BOWSIE uses
    ---
*/

void cleanup_str(std::string*);
void acquire_rom(std::string*);
void acquire_list(std::string*);
bool cleanup(std::string tool_folder);

/*
    error(format_string msg, Args... args) -> int: Print error message
    ---
    Input:
    * msg is a format string, args its arguments

    Output:
    * always returns 1. Intended as an exit code
*/
template<class... Args> int error(const std::format_string<Args...> msg, Args &&... args)
{
    std::println(std::cerr, "ERROR: {}", std::format(msg, args...));
    #if defined(_WIN32)
        std::system("pause");
    #else
        std::print("Press Enter to continue");
        getchar();
    #endif
    return 1;
}

