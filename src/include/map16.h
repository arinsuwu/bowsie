#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <format>

#include <fmt/base.h>

#include <nlohmann/json.hpp>

#define MAP16_SIZE 0x100*8

/*
    struct Map16: OW sprite Map16 generation
    ---
*/
struct Map16
{
    std::string tooltip;
    int no_tiles;
    std::vector<int> x_offset;
    std::vector<int> y_offset;
    int map16_number;

    std::vector<bool> is_16x16;
    std::vector<int> tile_num;

    std::vector<int> y_flip;
    std::vector<int> x_flip;
    std::vector<int> priority;
    std::vector<int> palette;
    std::vector<int> second_page;

    char * map16_page = new char [MAP16_SIZE] { 0x00 };
    std::ifstream s16ov;
    std::ofstream sscov;

    // I/O functions
    bool deserialize_json(nlohmann::json&, std::string*);
    void open_s16ov(const char*);
    void open_sscov(const char*);
    void done(const char*);

    // Sprite map16 tilemap functions
    int get_map16_tile(int);
    bool write_single_map16_tile(int, int, int);
    int* write_map16_tiles(std::string*);

    // Tooltip (and main) function
    bool write_tooltip(int, std::string*);
};

bool destroy_map16(std::string filename);

