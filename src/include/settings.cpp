#include "settings.h"

namespace ranges = std::ranges;

/*
    deserialize_json(Document* json, string* err_str) -> bool: Parse settings
    ---
    Input:
    * json is a pointer to the (rapidjson) parsed JSON file
    * err_str is a pointer to a string to hold the error data

    Output:
    * true if settings were parsed correctly, false if not
    * err_str now contains any issues found
*/
bool deserialize_json(rapidjson::Document* json, std::map<std::string, std::variant<bool, int, std::string>>& bowsie_settings, std::string* err_str)
{
    bool status = true;
    *err_str = "Couldn't find key(s):\t\t\t\t";
    for(const char * key : json_keys)
    {
        if(!(*json).HasMember(key))
        {
            (*err_str).append(key).append(", ");
            status = false;
        }
    }
    (*err_str) = status ? "" : (*err_str).erase((*err_str).size()-2, 2).append("\n");

    for(const char * key : bool_keys)
    {
        if(!(*json)[key].IsBool())
        {
            (*err_str).append(std::format("Incorrect data type for {}:\t\texpected Boolean\n", key));
            status = false;
        }
        else
            bowsie_settings[key] = (*json)[key].GetBool();
    }
    if(!(*json)["slots"].IsUint())
    {
        (*err_str).append("Incorrect data type for slots:\t\t\texpected Number (Unsigned Integer)\n");
        status = false;
    }
    else
        bowsie_settings["slots"] = (*json)["slots"].GetInt();
    if(!( (*json)["custom_method_name"].IsNull() || (*json)["custom_method_name"].IsString()))
    {
        (*err_str).append("Incorrect data type for custom_method_name:\t\texpected Null or String\n");
        status = false;
    }
    else
        bowsie_settings["custom_method_name"] = (*json)["custom_method_name"].GetType() ? (*json)["custom_method_name"].GetString() : "";

    return status;
}

/*
    parse_cli_settings(vector<string>& cli_settings, map<string, variant<bool, int, string>>& bowsie_settings) -> void: Parse settings from CLI
    ---
    Input:
    - cli_settings is a vector which holds the command line arguments
    - bowsie_settings is a vector to hold the settings

    Output:
    - exits with error if a setting does not exist
    - bowsie_settings now contains the processed settings
*/
void parse_cli_settings(std::vector<std::string>& cli_settings, std::map<std::string, std::variant<bool, int, std::string>>& bowsie_settings)
{
    if(ranges::contains(cli_settings, "-h") || ranges::contains(cli_settings, "--help"))
    {
        std::println("\t bowsie [switches] [rom] [list]\n");
        std::println("Command line-specific switches");
        std::println("  -h, --help\t\tDisplay this message and quit\n");
        std::println("Command line or configuration file switches (bowsie-config.json)");
        std::println("Pass as a command line argument in the form --<setting_name>=<value>");
        std::println("See the readme for valid values");
        std::println("  verbose\t\tDisplay all info per sprite inserted");
        std::println("  generate_map16\tCreate .s16ov and .sscov files for LM display");
        std::println("  meowmeow\t\tFix extra byte changes using meOWmeOW");
        std::println("  slots\t\t\tAmount of OW sprites (max. 24)");
        std::println("  use_maxtile\t\tEnable MaxTile granphics routines");
        std::println("  custom_dir\t\tPath to the asm file which handles the new OW sprites");
        std::println("  bypass_ram_check\tIgnore RAM boundaries (and by extension, sprite limit)");

        std::exit(0);
    }

    std::string setting;
    std::string param;
    for(auto full_setting : cli_settings)
    {
        setting = full_setting.contains("=") ? std::string(full_setting.begin(), full_setting.begin()+full_setting.find_first_of("=")) : full_setting;
        param = full_setting.contains("=") ? std::string(full_setting.begin()+full_setting.find_first_of("=")+1, full_setting.end()) : "";

        if(!ranges::contains(cli_keys, setting))
            std::exit(error("Unknown setting {}", setting));

        std::string setting_name = std::string(setting.begin()+2, setting.end());
        if(ranges::contains(bool_keys, setting_name))
        {
            if(!(param=="true" || param=="false"))
                std::exit(error("Unknown parameter for {}: extected true/false", setting));
            else
                bowsie_settings[setting_name] = param=="true" ? true : false;
        }
        else if(setting_name=="slots")
        {
            try
            {
                std::size_t pos {};
                int slots = stoi(param, &pos);
                bowsie_settings[setting_name] = slots;
            }
            catch(std::exception const & err)
            {
                std::exit(error("Error parsing amount of slots given."));
            }
        }
        else if(setting_name=="custom_method_name")
            bowsie_settings[setting_name] = param;
    }
}

