#pragma once

#include "misc.h"

import std;
import rapidjson;

// CLI settings
static const char * cli_keys[] = {"--verbose", "--generate_map16", "--meowmeow",
                                  "--slots", "--use_maxtile",
                                  "--custom_method_name", "--bypass_ram_check"};

// JSON settings
static const char * json_keys[] = {"verbose", "generate_map16", "meowmeow",
                                   "slots", "use_maxtile",
                                   "custom_method_name", "bypass_ram_check"};

// Settings which happen to be boolean
static const char * bool_keys[] = {"verbose", "generate_map16", "meowmeow", "use_maxtile", "bypass_ram_check"};

/*
    Functions for BOWSIE settings
    ---
    Check the readme for more info
*/

bool deserialize_json(rapidjson::Document*, std::map<std::string, std::variant<bool, int, std::string>>&, std::string*);
void parse_cli_settings(std::vector<std::string>&, std::map<std::string, std::variant<bool, int, std::string>>&);

