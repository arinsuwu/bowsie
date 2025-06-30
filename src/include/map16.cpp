#include "map16.h"

import std;
import rapidjson;

using namespace std;
using namespace rapidjson;

// I/O functions

/*
    deserialize_json(Document* json, string* err_str) -> bool: Parse Map16 JSON
    ---
    Input:
    * json is a pointer to the (rapidjson) parsed JSON file
    * err_str is a pointer to a string to hold the error data

    Output:
    * true if Map16 tooltip information was parsed correctly, false if not.
    * err_str now contains any issues found
*/
bool Map16::deserialize_json(Document* json, string* err_str)
{
    bool status = true;
    static const char * keys[] = {"tooltip", "no_tiles"};
    static const char * tile_keys[] = {"is_16x16", "tile_num", "y_flip", "x_flip", "priority", "palette", "second_page"};
    *err_str = "Couldn't find key(s):\t\t\t\t";
    for(const char * key : keys)
    {
        if(!(*json).HasMember(key))
        {
            (*err_str).append(key).append(", ");
            status = false;
        }
    }
    (*err_str) = status ? "" : (*err_str).erase((*err_str).size()-2, 2).append("\n");

    // There's gotta be a better way to do this shit
    if(!(*json)["tooltip"].IsString())
    {
        (*err_str).append("Incorrect data type for tooltip:\t\texpected String\n");
        status = false;
    }
    else
        this->tooltip = (*json)["tooltip"].GetString();
    if(!(*json)["no_tiles"].IsInt())
    {
        (*err_str).append("Incorrect data type for no_tiles:\t\texpected Integer\n");
        status = false;
    }
    else
        this->no_tiles = (*json)["no_tiles"].GetInt();
    (*err_str) = status ? "Couldn't find tile(s):\t\t\t\t" : (*err_str).erase((*err_str).size()-2, 2).append("\nCouldn't find key(s):\t\t\t\t");

    for(int i=1;i<=no_tiles;++i)
    {
        string c = format("tile_{}", i);
        const char * curr = c.c_str();
        if(!(*json).HasMember(curr))
        {
            (*err_str).append(curr).append(", ");
            status = false;
            break;
        }
        else if(!(*json)[curr].IsObject())
        {
            (*err_str).append(format("\n{} is not a tile (so a JSON structure)\n", curr));
            status = false;
            break;
        }

        (*err_str) = "Couldn't find tile info:\t\t\t\t";

        for(const char * key : tile_keys)
        {
            if(!(*json)[curr].HasMember(key))
            {
                (*err_str).append(key).append(", ");
                status = false;
            }
        }
        if(!status)
            break;

        (*err_str) = "";
        if(!(*json)[curr]["is_16x16"].IsBool())
        {
            (*err_str).append("Incorrect data type for is_16x16:\t\texpected Boolean\n");
            status = false;
        }
        else
            this->is_16x16.push_back((*json)[curr]["is_16x16"].GetBool());
        if(!(*json)[curr]["tile_num"].IsInt())
        {
            (*err_str).append("Incorrect data type for tile_num:\t\texpected Integer\n");
            status = false;
        }
        else
            this->tile_num.push_back((*json)[curr]["tile_num"].GetInt());
        if(!(*json)[curr]["x_offset"].IsInt())
        {
            (*err_str).append("Incorrect data type for x_offset:\t\texpected Integer\n");
            status = false;
        }
        else
            this->x_offset.push_back((*json)[curr]["x_offset"].GetInt());
        if(!(*json)[curr]["y_offset"].IsInt())
        {
            (*err_str).append("Incorrect data type for y_offset:\t\texpected Integer\n");
            status = false;
        }
        else
            this->y_offset.push_back((*json)[curr]["y_offset"].GetInt());
        if(!(*json)[curr]["y_flip"].IsBool())
        {
            (*err_str).append("Incorrect data type for y_flip:\t\texpected Boolean\n");
            status = false;
        }
        else
            this->y_flip.push_back((*json)[curr]["y_flip"].GetBool());
        if(!(*json)[curr]["x_flip"].IsBool())
        {
            (*err_str).append("Incorrect data type for x_flip:\t\texpected Boolean\n");
            status = false;
        }
        else
            this->x_flip.push_back((*json)[curr]["x_flip"].GetBool());
        if(!(*json)[curr]["priority"].IsInt())
        {
            (*err_str).append("Incorrect data type for priority:\t\texpected Integer\n");
            status = false;
        }
        else
            this->priority.push_back((*json)[curr]["priority"].GetInt());
        if(!(*json)[curr]["palette"].IsInt())
        {
            (*err_str).append("Incorrect data type for palette:\t\texpected Integer\n");
            status = false;
        }
        else
            this->palette.push_back((*json)[curr]["palette"].GetInt());
        if(!(*json)[curr]["second_page"].IsBool())
        {
            (*err_str).append("Incorrect data type for second_page:\t\texpected Boolean\n");
            status = false;
        }
        else
            this->second_page.push_back((*json)[curr]["second_page"].GetBool());
    }

    return status;
}

/*
    open_s16ov(const char* filename) -> void: open .s16ov
    ---
    Input:
    * filename is the ROM filepath+name (no extension)

    Output:
    * s16ov is now an ifstream with the raw map16 data
*/
void Map16::open_s16ov(const char* filename)
{
    ofstream(filename, ios::binary).write(map16_page, MAP16_SIZE);
    s16ov = ifstream(filename, ios::binary);
}

/*
    open_sscov(const char* filename) -> void: open .sscov
    ---
    Input:
    * filename is the ROM filepath+name (no extension)

    Output:
    * sscov is now an ifstream with the custom sprite list data
*/
void Map16::open_sscov(const char* filename)
{
    ofstream(filename, ios::app).write("", 0);
    sscov = ofstream(filename, ios::app);
}

/*
    done(const char* filename) -> void: write .s16ov file to disk
    ---
    Input:
    * filename is the ROM filepath+name (no extension)
*/
void Map16::done(const char* filename)
{
    ofstream(filename, ios::binary).write(map16_page, MAP16_SIZE);
    delete[] map16_page;
}

// Sprite map16 tilemap functions

/*
    get_map16_tile(int step) -> int: get next free tile in .s16ov file
    ---
    Input:
    * step is the amount to jump (normally 2 for one 8x8 tile, 8 for one 16x16 tile)

    Output:
    * i is the offset where to put the tile. -1 if none
*/
int Map16::get_map16_tile(int step)
{
    for(int i=0;i<MAP16_SIZE;i+=step)
    {
        if((map16_page[i]|map16_page[i+1]|map16_page[i+2]|map16_page[i+3]|map16_page[i+4]|map16_page[i+5]|map16_page[i+6]|map16_page[i+7])==0x00)
            return i;
    }
    return -1;
}

/*
    write_single_map16_tile(int tile_number, int yxppccct, int pos) -> bool: Write map16 tile info to memory
    ---
    Input:
    * tile_number is the GFX position of the tile
    * yxppccct is the tile info (Y flip, X flip, priority, palette, page)
    * pos is where to write

    Output:
    * true in success, false otherwise
    * map16_page now contains the tile info
*/
bool Map16::write_single_map16_tile(int tile_number, int yxppccct, int pos)
{
    if(pos == -1 || pos >= MAP16_SIZE)
        return false;
    map16_page[pos] = tile_number;
    map16_page[pos+1] = yxppccct;
    return true;
}

/*
    write_map16_tiles(string* err_string) -> int *: Write set of map16 tiles
    ---
    Input:
    * deserialize_json has been run
    * err_string is a pointer to a string to hold error data
    Output:
    * the vectors with information must be empty
    * returns an integer array with the tile position in the page. if it contains -1, then an error occurred.
        * err_string contains said info
*/
int * Map16::write_map16_tiles(string* err_string)
{
    int * map16_numbers = new int [no_tiles] { 0x00 };
    for(int i=0; i<no_tiles; ++i)
    {
        int pos = get_map16_tile(is_16x16[i] ? 8 : 2);
        if(pos == -1 || pos > MAP16_SIZE-8)
        {
            map16_numbers[i] = -1;
            (*err_string).append(format("Not enough map16 space while starting to write a tile. Position is {}", pos));
            return map16_numbers;
        }
        int tile = tile_num[i];
        int yxppccct = (y_flip[i]<<7)|(x_flip[i]<<6)|((priority[i]&1)<<5)|((palette[i]%8)<<2)|(second_page[i]);
        tile_num.pop_back();
        y_flip.pop_back();
        x_flip.pop_back();
        priority.pop_back();
        palette.pop_back();
        second_page.pop_back();
        if(is_16x16[i])
        {
            bool tmp;
            if(x_flip[i] && y_flip[i])
                tmp = !write_single_map16_tile(tile+0x11, yxppccct, pos) ||\
                !write_single_map16_tile(tile+0x01, yxppccct, pos+2) ||\
                !write_single_map16_tile(tile+0x10, yxppccct, pos+4) ||\
                !write_single_map16_tile(tile, yxppccct, pos+6);
            else if(x_flip[i])
                tmp = !write_single_map16_tile(tile+0x01, yxppccct, pos) ||\
                !write_single_map16_tile(tile+0x11, yxppccct, pos+2) ||\
                !write_single_map16_tile(tile, yxppccct, pos+4) ||\
                !write_single_map16_tile(tile+0x10, yxppccct, pos+6);
            else if(y_flip[i])
                tmp =!write_single_map16_tile(tile+0x10, yxppccct, pos) ||\
                !write_single_map16_tile(tile, yxppccct, pos+2) ||\
                !write_single_map16_tile(tile+0x11, yxppccct, pos+4) ||\
                !write_single_map16_tile(tile+0x01, yxppccct, pos+6);
            else
                tmp = !write_single_map16_tile(tile, yxppccct, pos) ||\
                !write_single_map16_tile(tile+0x10, yxppccct, pos+2) ||\
                !write_single_map16_tile(tile+0x01, yxppccct, pos+4) ||\
                !write_single_map16_tile(tile+0x11, yxppccct, pos+6);
            if(tmp)
            {
                map16_numbers[i] = -1;
                (*err_string).append(format("Not enough map16 space while writing part of a 16x16 tile. Position is {}", pos));
                return map16_numbers;
            }
            map16_numbers[i] = (pos/8)+0x400;
        }
        else
        {
            if(!write_single_map16_tile(tile, yxppccct, pos))
            {
                map16_numbers[i] = -1;
                (*err_string).append(format("Not enough map16 space while writing an 8x8 tile. Position is {}", pos));
                return map16_numbers;
            }
            map16_numbers[i] = (pos/8)+0x400;
        }
    }
    return map16_numbers;
}

// Tooltip (and main) function

/*
    write_tooltip(int sprite_number, string* err_string) -> bool: Write tooltip for custom sprite
    ---
    Input:
    * sprite_number is the OW sprite slot
    * err_string is a pointer to a string to hold error data

    Output:
    * true in success, false otherwise
    * if false, err_string now contains the error info
*/
bool Map16::write_tooltip(int sprite_number, string* err_string)
{
    string str = format("{:X}\t10\t{}\n", sprite_number, tooltip);
    sscov.write(str.c_str(), str.size());
    int * map16_numbers = write_map16_tiles(err_string);
    str = format("{:X}\t12\t", sprite_number);
    for(int i=0;i<no_tiles;)
    {
        if(map16_numbers[i]==-1)
            return false;
        str.append(format("{},{},{:X}\t", x_offset[i], y_offset[i], map16_numbers[i]));
        x_offset.pop_back();
        y_offset.pop_back();
        if(!is_16x16[i])
            i+=4;
        else
            ++i;
    }
    if(no_tiles!=0)
        sscov.write((str+"\n").c_str(), str.size()+1);
    is_16x16.assign({});
    x_offset.assign({});
    y_offset.assign({});
    return true;
}

/*
    destroy_map16(string filename) -> void: delete OW sprite map16 files
    ---
    Input:
    * filename is the ROM filepath+name (no extension)

    Output:
    * sprite map16 files erased from disk (if they existed)
*/
void destroy_map16(string filename)
{
    if(filesystem::exists(filename+".s16ov"))
        filesystem::remove(filename+".s16ov");
    if(filesystem::exists(filename+".sscov"))
        filesystem::remove(filename+".sscov");
}
