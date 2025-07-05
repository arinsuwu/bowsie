#pragma once

import std;
import rapidjson;

using namespace std;
using namespace rapidjson;

#define MAP16_SIZE 0x100*8

/*
    struct Map16: OW sprite Map16 generation
    ---
*/
struct Map16
{
    string tooltip;
    int no_tiles;
    vector<int> x_offset;
    vector<int> y_offset;
    int map16_number;

    vector<bool> is_16x16;
    vector<int> tile_num;

    vector<int> y_flip;
    vector<int> x_flip;
    vector<int> priority;
    vector<int> palette;
    vector<int> second_page;

    char * map16_page = new char [MAP16_SIZE] { 0x00 };
    ifstream s16ov;
    ofstream sscov;

    // I/O functions
    bool deserialize_json(Document*, string*);
    void open_s16ov(const char*);
    void open_sscov(const char*);
    void done(const char*);

    // Sprite map16 tilemap functions
    int get_map16_tile(int);
    bool write_single_map16_tile(int, int, int);
    int* write_map16_tiles(string*);

    // Tooltip (and main) function
    bool write_tooltip(int, string*);
};

bool destroy_map16(string filename);

