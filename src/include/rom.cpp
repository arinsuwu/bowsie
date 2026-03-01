#include "rom.h"

using ios = std::ios;

namespace fs = std::filesystem;

// I/O functions

/*
    open_rom(void) -> bool: Set ROM file input stream
    ---
    Output: rom_data is an ifstream with the ROM data
            rom_size contains the ROM size in bytes (counting the header!)
            raw_rom_data is a 16MB character array initialized to zeroes
            returns true if the ROM opened successfully, false otherwise
*/
bool Rom::open_rom()
{
    rom_data = std::ifstream(rom_path, ios::binary | ios::ate);
    rom_size = rom_data.tellg();
    raw_rom_data = new char[MAX_SIZE] { 0x00 };
    first_time = true;
    if(rom_data.seekg(0x07FD5+HEADER_SIZE).get() == 0x23)
        rom_mapper = (rom_data.seekg(0x07FD7+HEADER_SIZE).get() == 0x0D) ? bigsa1rom : sa1rom;
    else
        rom_mapper = lorom;

    rom_data.seekg(HEADER_SIZE);
    return !(!rom_data);
}

bool Rom::reload()
{
    std::ofstream(rom_path, ios::binary).write(raw_rom_data, rom_size);
    return open_rom();
}

/*
    done(bool meowmeow) -> void: Close ROM input
    ---
    Input:
    * meowmeow indicates meOWmeOW ran

    Output:
    * patched ROM is now written to disk
    * raw_rom_data destroyed
*/
void Rom::done(bool meowmeow)
{
    std::ofstream(rom_path, ios::binary).write(raw_rom_data, rom_size);
    delete[] raw_rom_data;
    delete[] new_extra_bytes;
    if(meowmeow)
        delete[] old_extra_bytes;
}

// ROM data operations

/*
    inline_patch(const char * patch_content) -> bool: Patch ROM
    ---
    Input:
    * patch_content is a char with the patch to apply

    Output:
    * ./asm/tmp.asm contains the patch applied
    * returns true if the patch was successfully applied,
    * false otherwise
*/
bool Rom::inline_patch(std::string tool_folder, const char * patch_content)
{
    int new_size = rom_size - HEADER_SIZE;

    if(first_time)
    {
        rom_data.seekg(HEADER_SIZE);
        rom_data.read(&(raw_rom_data[HEADER_SIZE]), rom_size);
    }

    std::ofstream(tool_folder+"asm/tmp.asm").write(patch_content, std::strlen(patch_content));
    std::string tmp_path = fs::absolute(tool_folder+"asm/tmp.asm").string();
    bool patch_res = asar_patch(tmp_path.c_str(), &(raw_rom_data[HEADER_SIZE]), MAX_SIZE, &new_size);

    rom_size = new_size + HEADER_SIZE;

    if(patch_res)
        first_time = false;
    return patch_res;
}