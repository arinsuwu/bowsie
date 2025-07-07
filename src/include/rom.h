#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

#include "asar/asar.h"

#define HEADER_SIZE 512
#define MAX_SIZE 1024*1024*16

/*
    struct Rom: SMW ROM data
    ---
*/
struct Rom
{
    std::string rom_path;
    std::ifstream rom_data;
    mappertype mapper=lorom;
    int rom_size;
    char * raw_rom_data;
    char * old_extra_bytes;
    char * new_extra_bytes;
    bool first_time;

    bool open_rom();
    bool reload();
    void done(bool);
    bool inline_patch(std::string, const char *);

    /*
        read<int bytes>(int addr, bool little_endian = false) -> uint: Read bytes from ROM
        ---
        Input:
        * addr is the SNES address to read
        * bytes is the amount of bytes to read

        Output:
        * res contains the bytes read (unsigned -1 in failure)
    */
    template<int bytes> unsigned int read(int addr, bool little_endian)
    {
        unsigned int res = 0x0;
        int shift;

        rom_data.clear();
        rom_data.seekg(snestopc_pick(mapper, addr)+HEADER_SIZE);
        for(int i=0;i<bytes;++i)
        {
            shift = little_endian ? i*8 : (bytes-1-i)*8;
            res|=(rom_data.get()&0xFF)<<shift;
        }
        return res;
    }

    template<int bytes> unsigned int read(int addr)
    {
        return read<bytes>(addr, false);
    }
};

