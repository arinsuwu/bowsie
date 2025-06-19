BOWSIE v1.10
---
### Features
___BOWSIE___
- Added support for arbitrarily sized extra bytes, which you may know from level sprites as the "Extension" field. Thanks to FuSoYa for adding this feature to Lunar Magic!
  - This feature needs Lunar Magic 3.51 or newer to work.
  - BOWSIE will now enable two 8-bit extra bytes to form a full-sized extra byte, the way OW Revolution sprites work.
  - Extra bytes used are controlled by the `!extra` define in a sprite's ASM file. For example, `!extra = 5` will enable five extra bytes.
  - Added two tables, `!ow_sprite_extra_1` and `!ow_sprite_extra_2`, to access four 8-bit extra bytes.
  - You're limited to eight extra bytes.
- Added meOWmeOW, which will detect changes in extra bytes, similar to Akaginite's MeiMei.
  - Added the `fix_extra` setting, which allows you to enable or disable meOWmeOW if so desired.
  - meOWmeOW is disabled with OW Revolution. Sorry! Run PIXI after BOWSIE to fix any potential extra byte changes via MeiMei.
- Added `%finish_oam_write()`, an adaptation of Akaginite's routine to set tile sizes and the X position high byte in the OAM size table `$0420`. While normally this isn't necessary, due to the side border, using this routine fixes the odd quirks of sprites disappearing when using Medic's Widescreen Overworld patch.
- `%sub_horz_pos()` and `%sub_vert_pos()` now store the distance between player and sprite in scratch RAM `$0E`.
- You can now pass BOWSIE settings as arguments to the program directly, in case you don't wish to use `bowsie-config.json`. Run `bowsie -h` for more info.
- Removed the support for the vanilla sprite system, in anticipation of yoshifanatic's disassemblies. Please use those instead, they don't have the odd restriction the vanilla sprites have either way.
  - `omtre_detect` was also removed as a consequence, as everything fits in vanilla freespace now!

___OW Revolution___
- The Map16 handler can now generate regular sprite Map16 data.
- Added a missing table, `!ow_sprite_load_index`, which is the equivalent to level sprites' `!161A`: the index to the table used to determine whether a sprite should respawn.
- Added `%erase_sprite()`. In it's simplest form, it just sets a sprite number to zero, but when using OW Revolution, it also marks the sprite to be reloaded correctly.

___Non-OW Revolution___
- __Katrina__: this system actually works now LOL

### Fixes
- Fixed a crash in the VLDC9 system when using sprite numbers `55-7F` -- the high byte of the index to the pointers was being lost.
- Fixed reading garbage when the pointer to sprite data not inserted (equal to `$000000`). This was a regression issue introduced when I cleaned the code in my modified PIXI builds.
- Fixed the vanilla shadow macro using a non-assigned OAM slot. Thanks to simoncaio for reporting this and helping me test the fix!
- Fixed `%interact()` not taking a sprite's Z position into account. Thanks to simoncaio for making me realize this was the case!

BOWSIE v1.10 RELEASE CANDIDATE 2
---
### Features
- Added a macro library. By default it includes two macros:
  - A macro to draw a shadow, like the VLDC9 sprites which use it.
  - A macro to draw the vanilla game's shadow.

BOWSIE v1.10 RELEASE CANDIDATE 1
---
### Features
- The tool now pauses after an error or when sprites insert successfully... [Turns out regular people run BOWSIE from outside the console just like they'd do PIXI huh](https://i.kym-cdn.com/photos/images/original/002/086/808/90f.gif)

___OW Revolution___
- Upgraded to Overworld Revolution v1.1.1.
- `slots` can now be up to 32 when using `owrev`, without needing to disable RAM checks.
  - Due to this, some tables have moved RAM address locations.
- `owrev` now inserts its main handler at the pointer located at `$0480D2` instead of overwriting OW Revolution code.
- `owrev` now hacks `$02A861` directly for sprite load instead of overwriting OW Revolution code.
- Added the sprite table `!ow_sprite_props`, corresponding to OW Revolution's `!RAM_SMW_OWSpr_Properties`. It's used to know when to process an sprite even if it's offscreen. Due to overworld map sizes, it's pretty necessary.
  - Updated `%sub_offscreen()` to account for this fact.
- Removed BOWSIE pointers for previously overwritten functions.
- The bank 4 freespace finder now stops if we reach BOWSIE's default area. This is done in advance of potential future OW Revolution upgrades, as to know when to update the `JSR : NOP` to an `autoclean JSL`.
- Special thanks to yoshifanatic for all his support to make this update possible. 😃

Overworld Revolution support can now be considered mature.

___Non-OW Revolution___
- __Katrina__: added `!bowsie_contiguous` so the user can tell when a sprite is using contiguous sprite tables, like Katrina's system does.
  - Updated `%spawn_sprite()` to account for this fact.

The following features apply to both VLDC9 and Katrina's systems.
- Separated the main sprite handler into its own separate function.
- Sprites now appear during fade-in instead of awkwardly appearing out of nowhere.
### Fixes
- __OW Revolution__: now the extension bytes are set for all 256 sprites with extra bit 1, instead of just the first hundred. Oops!
- __VLDC9/Katrina__: fixed RAM addresses not clearing on level load. This was very noticeable with, eg. Yoshi spawning turning around.
- Changed `warnpc` to `assert pc()` and removed `math pri off`, in advance of Asar 2.0.
- Fixed an issue where BOWSIE couldn't find the ASM and sprite directory when calling the tool using relative paths.

BOWSIE v1.03
---
### Features
- Added the `%distance()` shared subroutine, used in VLDC9 sprites to get the absolute positional difference between the player and a sprite.
- Modified `%interact()` so it accounts for player 2 as well.
- The list file can now contain blank lines.
- The list file can now contain _comment lines_, which are lines that begin with a semicolon `;`.
### Fixes
- Fixed various bugs which were causing incorrect sprite Map16 generation:
  - The generator was miscounting when using 16x16 tiles. This was most noticeable when using json files which defined more than 1 16x16 tile.
  - Y flip was being ignored.
  - Priority was using two bits instead of one. I forget Map16 data is _YXPCCCTT_, and not _YXPPCCCT_... [silly me](https://i.kym-cdn.com/photos/images/original/002/086/808/90f.gif).
- Fixed an anomaly regarding some spawn X positions due a rogue carry in the VLDC9 system.
- Fixed a bug with `%get_draw_info()` where carry, set when off-screen, was being lost when asking for priority, making sprites get drawn even when off-screen.
- Now the sprite spawn routine clears the init state flag.
- Made the VLDC9 system read the pointer directly from ROM instead of using an Asar read. This _should_ fix issues related to crashing when not re-running the tool, at the expense of three bytes and some cycles. Thanks go to Thomas/kaizoman for suggesting this alternative!

Note that since OW Revolution support is still immature, I'm not pushing the version number to v1.10 yet.

BOWSIE v1.02
---
### Features
- Added experimental support for yoshifanatics' Overworld Revolution. Note that the tool will attempt to detect the patch on its own.  
The following additions are all related to this above feature:
  - Added the `owrev` sprite system, which modifies yoshifanatic's system with a variant of Lui's system. It's included at `owrev.asm`.
  - Added reads in the tool to find the current freespace in bank 4, where BOWSIE installs some of its code, and the routine `SMW_ProcessOWPlayers_DrawOverworldPlayerAndBorderSprites`, as the original jump is overwritten.
  - Added an `RTL` at `$04FFFF` so the uninserted sprites have a correct pointer.
  - Note that for now, overworld sprite Map16 data is still generated instead of regular sprite Map16. This will be corrected at a later date.
  - Note that the shadow drawing routine from OW Revolution is erased.
  - Note that _BOWSIE_ won't remove OW Revolution's shared subroutines, wasting space.
  - Note that this replaces yoshifanatic's own sprites with _BOWSIE_'s! Don't insert sprites with OW Revolution.
- This support is extremely experimental. Once it has been tested properly, the version number will be pushed to 1.10.

BOWSIE v1.01
---
### Features
- Moved back the OAM priority further so sprites which need priority can actually go behind the player.
- Added the shared subroutine `%get_ow_tile()`, which a sprite can call to get the index to the 16x16 tile the sprite is currently on, via a call to `$049885`. This routine is commonly used by carol's sprites.
### Fixes
- Fixed an issue in the VLDC9 system where the player's graphics routine, which in the vanilla game runs _after the sprites_, could overwrite certain sprite tiles, if the user didn't ask priority from `%get_draw_info()`. This happens whether _OPSE_ is used or not.
- Fixed an issue where `%get_draw_info()` was always writing the next OAM index to `!ow_sprite_oam`. This caused conflicts when using sprites with and without priority calls at the same time.
- Fixed an issue where BOWSIE failed to find the relevant directories when calling the tool from the command line as an absolute path. Thanks to Stivi for reporting this!

BOWSIE v1.00
---
- Initial release.