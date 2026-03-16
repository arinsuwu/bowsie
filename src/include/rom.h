#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

#ifdef ASAR_DYNAMIC_LINK
    #include "asar/asardll.h"
#else
    #include "asar/asar.h"
#endif

#define HEADER_SIZE 512
#define MAX_SIZE 1024*1024*16

static int sa1banks[8]={0<<20, 1<<20, -1, -1, 2<<20, 3<<20, -1, -1};

static int snestopc_pick(enum mappertype mapper, int addr)
{
    if (addr<0 || addr>0xFFFFFF) return -1;
    if (mapper==lorom)
    {
        if ((addr&0xFE0000)==0x7E0000 ||
            (addr&0x408000)==0x000000 ||
            (addr&0x708000)==0x700000)
                return -1;
        addr=((addr&0x7F0000)>>1|(addr&0x7FFF));
        return addr;
    }
    if (mapper==sa1rom)
    {
        if ((addr&0x408000)==0x008000)
        {
            return sa1banks[(addr&0xE00000)>>21]|((addr&0x1F0000)>>1)|(addr&0x007FFF);
        }
        if ((addr&0xC00000)==0xC00000)
        {
            return sa1banks[((addr&0x100000)>>20)|((addr&0x200000)>>19)]|(addr&0x0FFFFF);
        }
        return -1;
    }
    if (mapper==bigsa1rom)
    {
        if ((addr&0xC00000)==0xC00000)
        {
            return (addr&0x3FFFFF)|0x400000;
        }
        if ((addr&0xC00000)==0x000000 || (addr&0xC00000)==0x800000)
        {
            if ((addr&0x008000)==0x000000) return -1;
            return (addr&0x800000)>>2 | (addr&0x3F0000)>>1 | (addr&0x7FFF);
        }
        return -1;
    }
    if (mapper==norom)
    {
        return addr;
    }
    return -1;
}

/*
    struct Rom: SMW ROM data
    ---
*/
struct Rom
{
    std::string rom_path;
    std::ifstream rom_data;
    mappertype rom_mapper=lorom;
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
        rom_data.seekg(snestopc_pick(rom_mapper, addr)+HEADER_SIZE);
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
