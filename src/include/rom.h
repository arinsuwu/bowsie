#pragma once

import std;
import asar;

using namespace std;
using namespace asar;

#define HEADER_SIZE 512
#define MAX_SIZE 1024*1024*16

/*
    struct Rom: SMW ROM data
    ---
*/
struct Rom
{
    string rom_path;
    ifstream rom_data;
    int rom_size;
    char * raw_rom_data;
    char * old_extra_bytes;
    char * new_extra_bytes;
    bool first_time;

    bool open_rom();
    bool reload();
    void done(bool);
    bool inline_patch(string, const char *);

    /*
        read<int bytes>(int addr) -> uint: Read bytes from ROM
        ---
        Input:
        * addr is the SNES address to read
        * bytes is the amount of bytes to read

        Output:
        * res contains the bytes read (unsigned -1 in failure)
    */
    template<int bytes> unsigned int read(int addr)
    {
        unsigned int res = 0x0;
        rom_data.clear();
        rom_data.seekg(snestopc_pick(addr)+HEADER_SIZE);
        for(int i=0;i<bytes;++i)
            res|=(rom_data.get()&0xFF)<<((bytes-i-1)*8);
        return res;
    }
};

