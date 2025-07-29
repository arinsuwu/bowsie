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
bool deserialize_json(nlohmann::json& json, std::map<std::string, std::variant<bool, int, std::string>>& bowsie_settings, std::string* err_str)
{
    bool status = true;
    *err_str = "Couldn't find key(s):\t";
    for(const char * key : json_keys)
    {
        if(!json.contains(key))
        {
            (*err_str).append(key).append(", ");
            status = false;
        }
    }
    (*err_str) = status ? "" : (*err_str).erase((*err_str).size()-2, 2).append("\n");
    // if( !status ) return status;    // Crashes otherwise

    try
    {
        for(const char * key : bool_keys)
        {
            if(!json[key].is_boolean())
            {
                (*err_str).append(std::format("Incorrect data type for {}:\t\texpected Boolean\n", key));
                status = false;
            }
            else
                bowsie_settings[key] = json[key].get<bool>();
        }
        if(!json["slots"].is_number_unsigned())
        {
            (*err_str).append("Incorrect data type for slots:\t\t\texpected Number (Unsigned Integer)\n");
            status = false;
        }
        else
            bowsie_settings["slots"] = json["slots"].get<int>();

        if(!( json["custom_method_name"].is_null() || json["custom_method_name"].is_string()))
        {
            (*err_str).append("Incorrect data type for custom_method_name:\t\texpected Null or String\n");
            status = false;
        }
        else
            bowsie_settings["custom_method_name"] = json.value("custom_method_name", "");
    }
    catch(...) {}

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
    if( (ranges::find(cli_settings, "-h")!=ranges::end(cli_settings) ) || ( ranges::find(cli_settings, "--help")!=ranges::end(cli_settings) ) )
    {
        fmt::println("\t bowsie [switches] [rom] [list]\n");
        fmt::println("Command line-specific switches");
        fmt::println("  -h, --help\t\tDisplay this message and quit\n");
        fmt::println("Command line or configuration file switches (bowsie-config.json)");
        fmt::println("Pass as a command line argument in the form --<setting_name>=<value>");
        fmt::println("See the readme for valid values");
        fmt::println("  verbose\t\tDisplay all info per sprite inserted");
        fmt::println("  generate_map16\tCreate .s16ov and .sscov files for LM display");
        fmt::println("  meowmeow\t\tFix extra byte changes using meOWmeOW");
        fmt::println("  slots\t\t\tAmount of OW sprites (max. 24)");
        fmt::println("  use_maxtile\t\tEnable MaxTile granphics routines");
        fmt::println("  custom_dir\t\tPath to the asm file which handles the new OW sprites");
        fmt::println("  bypass_ram_check\tIgnore RAM boundaries (and by extension, sprite limit)");

        std::exit(0);
    }

    std::string setting;
    std::string param;
    for(auto full_setting : cli_settings)
    {
        setting = ( full_setting.find("=")!=std::string::npos ) ? std::string(full_setting.begin(), full_setting.begin()+full_setting.find_first_of("=")) : full_setting;
        param = ( full_setting.find("=")!=std::string::npos ) ? std::string(full_setting.begin()+full_setting.find_first_of("=")+1, full_setting.end()) : "";

        if(ranges::find(cli_keys, setting)==ranges::end(cli_keys))
            exit(error("Unknown setting {}", setting));

        std::string setting_name = std::string(setting.begin()+2, setting.end());
        if(ranges::find(bool_keys, setting_name)!=ranges::end(bool_keys))
        {
            if(!(param=="true" || param=="false"))
                exit(error("Unknown parameter for {}: extected true/false", setting));
            else
                bowsie_settings[setting_name] = param=="true" ? true : false;
        }
        else if(setting_name=="slots")
        {
            try
            {
                size_t pos {};
                int slots = stoi(param, &pos);
                bowsie_settings[setting_name] = slots;
            }
            catch(std::exception const & err)
            {
                exit(error("Error parsing amount of slots given."));
            }
        }
        else if(setting_name=="custom_method_name")
            bowsie_settings[setting_name] = param;
    }
}

