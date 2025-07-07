#pragma once

#ifndef ASAR_SHARED
#define expectedapiversion 303

#include <stdbool.h>
#include <stddef.h> // for size_t

static int sa1banks[8]={0<<20, 1<<20, -1, -1, 2<<20, 3<<20, -1, -1};

//These structures are returned from various functions.
struct errordata {
	const char* fullerrdata;
	const char* rawerrdata;
	const char* block;
	const char* filename;
	int line;
	const char* callerfilename;
	int callerline;
	int errid;
};

struct labeldata {
	const char* name;
	int location;
};

struct definedata {
	const char* name;
	const char* contents;
};

struct writtenblockdata {
	int pcoffset;
	int snesoffset;
	int numbytes;
};

enum mappertype {
	invalid_mapper,
	lorom,
	hirom,
	sa1rom,
	bigsa1rom,
	sfxrom,
	exlorom,
	exhirom,
	norom
};

struct warnsetting {
	const char* warnid;
	bool enabled;
};

struct memoryfile {
	const char* path;
	const void* buffer;
	size_t length;
};

struct patchparams
{
	// The size of this struct. Set to (int)sizeof(patchparams).
	int structsize;

	// Same parameters as asar_patch()
	const char* patchloc;
	char* romdata;
	int buflen;
	int* romlen;

	// Include paths to use when searching files.
	const char** includepaths;
	int numincludepaths;

	// should everything be reset before patching? Setting it to false will make asar
	// act like the currently patched file was directly appended to the previous one.
	// note that you can't use the previous run's defines - you can use getalldefines()
	// and additional_defines for that.
	bool should_reset;

	// A list of additional defines to make available to the patch.
	const struct definedata* additional_defines;
	int additional_define_count;

	// Path to a text file to parse standard include search paths from.
	// Set to NULL to not use any standard includes search paths.
	const char* stdincludesfile;

	// Path to a text file to parse standard defines from.
	// Set to NULL to not use any standard defines.
	const char* stddefinesfile;

	// A list of warnings to enable or disable.
	// Specify warnings in the format "WXXXX" where XXXX = warning ID.
	const struct warnsetting* warning_settings;
	int warning_setting_count;

	// List of memory files to create on the virtual filesystem.
	const struct memoryfile* memory_files;
	int memory_file_count;

	// Set override_checksum_gen to true and generate_checksum to true/false
	// to force generating/not generating a checksum.
	bool override_checksum_gen;
	bool generate_checksum;
};

#ifdef __cplusplus
extern "C" {
#endif

	#undef snestopc
	#undef snestopc_pick
    static int snestopc_pick(mappertype mapper, int addr)
    {
		if (addr<0 || addr>0xFFFFFF) return -1;//not 24bit
		if (mapper==lorom)
		{
			// randomdude999: The low pages ($0000-$7FFF) of banks 70-7D are used
			// for SRAM, the high pages are available for ROM data though
			if ((addr&0xFE0000)==0x7E0000 ||//wram
				(addr&0x408000)==0x000000 ||//hardware regs, ram mirrors, other strange junk
				(addr&0x708000)==0x700000)//sram (low parts of banks 70-7D)
					return -1;
			addr=((addr&0x7F0000)>>1|(addr&0x7FFF));
			return addr;
		}
		if (mapper==hirom)
		{
			if ((addr&0xFE0000)==0x7E0000 ||//wram
				(addr&0x408000)==0x000000)//hardware regs, ram mirrors, other strange junk
					return -1;
			return addr&0x3FFFFF;
		}
		if (mapper==exlorom)
		{
			if ((addr&0xF00000)==0x700000 ||//wram, sram
				(addr&0x408000)==0x000000)//area that shouldn't be used in lorom
					return -1;
			if (addr&0x800000)
			{
				addr=((addr&0x7F0000)>>1|(addr&0x7FFF));
			}
			else
			{
				addr=((addr&0x7F0000)>>1|(addr&0x7FFF))+0x400000;
			}
			return addr;
		}
		if (mapper==exhirom)
		{
			if ((addr&0xFE0000)==0x7E0000 ||//wram
				(addr&0x408000)==0x000000)//hardware regs, ram mirrors, other strange junk
				return -1;
			if ((addr&0x800000)==0x000000) return (addr&0x3FFFFF)|0x400000;
			return addr&0x3FFFFF;
		}
		if (mapper==sfxrom)
		{
			// Asar emulates GSU1, because apparently emulators don't support the extra ROM data from GSU2
			if ((addr&0x600000)==0x600000 ||//wram, sram, open bus
				(addr&0x408000)==0x000000 ||//hardware regs, ram mirrors, rom mirrors, other strange junk
				(addr&0x800000)==0x800000)//fastrom isn't valid either in superfx
					return -1;
			if (addr&0x400000) return addr&0x3FFFFF;
			else return (addr&0x7F0000)>>1|(addr&0x7FFF);
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
			if ((addr&0xC00000)==0xC00000)//hirom
			{
				return (addr&0x3FFFFF)|0x400000;
			}
			if ((addr&0xC00000)==0x000000 || (addr&0xC00000)==0x800000)//lorom
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

	//Returns the version, in the format major*10000+minor*100+bugfix*1. This means that 1.2.34 would be
	// returned as 10234.
	int asar_version(void);

	//Returns the API version, format major*100+minor. Minor is incremented on backwards compatible
	// changes; major is incremented on incompatible changes. Does not have any correlation with the
	// Asar version.
	//It's not very useful directly, since asar_init() verifies this automatically.
	//Calling this one also sets a flag that makes asar_init not instantly return false; this is so
	// programs expecting an older API won't do anything unexpected.
	int asar_apiversion(void);

	//Clears out all errors, warnings and printed statements, and clears the file cache. Not really
	// useful, since asar_patch() already does this.
	bool asar_reset(void);

	//Applies a patch. The first argument is a filename (so Asar knows where to look for incsrc'd
	// stuff); however, the ROM is in memory.
	//This function assumes there are no headers anywhere, neither in romdata nor the sizes. romlen may
	// be altered by this function; if this is undesirable, set romlen equal to buflen.
	//The return value is whether any errors appeared (false=errors, call asar_geterrors for details).
	// If there is an error, romdata and romlen will be left unchanged.
	bool asar_patch(const char* patchloc, char* romdata, int buflen, int* romlen);

	// An extended version of asar_patch() with a future-proof parameter format.
	bool asar_patch_ex(const struct patchparams* params);

	//Returns the maximum possible size of the output ROM from asar_patch(). Giving this size to buflen
	// guarantees you will not get any buffer too small errors; however, it is safe to give smaller
	// buffers if you don't expect any ROMs larger than 4MB or something.
	int asar_maxromsize(void);

	//Get a list of all errors.
	//All pointers from these functions are valid only until the same function is called again, or until
	// asar_patch, asar_reset or asar_close is called, whichever comes first. Copy the contents if you
	// need it for a longer time.
	const struct errordata* asar_geterrors(int* count);

	//Get a list of all warnings.
	const struct errordata* asar_getwarnings(int* count);

	//Get a list of all printed data.
	const char* const* asar_getprints(int* count);

	//Get a list of all labels.
	const struct labeldata* asar_getalllabels(int* count);

	//Get the ROM location of one label. -1 means "not found".
	int asar_getlabelval(const char* name);

	//Gets the value of a define.
	const char* asar_getdefine(const char* name);

	//Gets the values and names of all defines.
	const struct definedata* asar_getalldefines(int* count);

	//Parses all defines in the parameter. The parameter controls whether it'll learn new defines in
	// this string if it finds any. Note that it may emit errors.
	const char* asar_resolvedefines(const char* data, bool learnnew);

	//Parses a string containing math. It automatically assumes global scope (no namespaces), and has
	// access to all functions and labels from the last call to asar_patch. Remember to check error to
	// see if it's successful (NULL) or if it failed (non-NULL, contains a descriptive string). It does
	// not affect asar_geterrors.
	double asar_math(const char* math, const char** error);

	//Get a list of all the blocks written to the ROM by calls such as asar_patch().
	const struct writtenblockdata* asar_getwrittenblocks(int* count);

	//Get the mapper currently used by Asar
	enum mappertype asar_getmapper(void);

	// Generates the contents of a symbols file for in a specific format.
	const char* asar_getsymbolsfile(const char* format);
#ifdef __cplusplus
}
#endif
#endif