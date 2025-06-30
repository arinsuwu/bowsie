#pragma once

import std;
import rapidjson;

using namespace std;

/*
    Auxiliary functions BOWSIE uses
    ---
*/

void cleanup_str(string*);
void acquire_rom(string*);
void acquire_list(string*);
bool cleanup(string tool_folder);

/*
    error(format_string msg, Args... args) -> int: Print error message
    ---
    Input:
    * msg is a format string, args its arguments

    Output:
    * always returns 1. Intended as an exit code
*/
template<class... Args> int error(const format_string<Args...> msg, Args &&... args)
{
    println(cerr, "ERROR: {}",format(msg, args...));
    #if defined(_WIN32)
        system("pause");
    #else
        printf("Press Enter to continue");
        getchar();
    #endif
    return 1;
}

