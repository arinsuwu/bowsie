; OW Revolution allows up to 32 sprite slots. The tables are rearranged for that purpose.
; This is a modification of its original system, though. Just like in Lui's system, the
; tables are discontiguous.

incsrc bowsie_defines.asm       ; don't delete this! it's created during the tool run.

;   OAM definitions
if !bowsie_maxtile
    if !sa1
        !oam_buffer            #= $400000
        !oam_tilesize_buffer   #= $400000
    else
        !oam_buffer            #= $7F0000
        !oam_tilesize_buffer   #= $7F0000
    endif

    !next_oam_slot              = "INY #4"
    !next_oam_tilesize_slot     = "INY"
    !adjacent_oam_slot          = $01
endif

;   Expanded levels and events definitions
if read1($0480D2)&$10 == $10
    !level_status_flags    #= $1EA2|!addr
    !event_flags           #= $1FA2|!addr
    !mario_map             #= $1FC2|!addr
    !luigi_map             #= !mario_map+2
    !mario_anim_state      #= $1FC6|!addr
    !luigi_anim_state      #= $1FC8|!addr
    !mario_x_pos_lo        #= $1FCA|!addr
    !luigi_x_pos_lo        #= !mario_x_pos_lo+2
    !mario_y_pos_lo        #= !mario_x_pos_lo+4
    !luigi_y_pos_lo        #= !mario_x_pos_lo+6
    !music_override        #= $1FD2|!addr
    !saved_game_flags      #= $1FD3|!addr
    !events_triggered      #= $1FD4|!addr
else
    !level_status_flags    #= $1EA2|!addr
    !event_flags           #= $1F02|!addr
    !mario_map             #= $1F11|!addr
    !luigi_map             #= !mario_map+2
    !mario_anim_state      #= $1F15|!addr
    !luigi_anim_state      #= $1F17|!addr
    !mario_x_pos_lo        #= $1F19|!addr
    !luigi_x_pos_lo        #= !mario_x_pos_lo+2
    !mario_y_pos_lo        #= !mario_x_pos_lo+4
    !luigi_y_pos_lo        #= !mario_x_pos_lo+6
    !music_override        #= $1F21|!addr
    !saved_game_flags      #= $1F22|!addr
    !events_triggered      #= $1F2D|!addr
endif
    !save_file_size        #= !events_triggered-!level_status_flags+4

;   Structs
struct oam_buffer !oam_buffer
    .x_pos: skip 1
    .y_pos: skip 1
    .tile:  skip 1
    .props: skip 1
endstruct align 4

struct oam_tilesize_buffer !oam_buffer
    .tile_sx:   skip 1
endstruct align 1

macro define_ow_sprite_table(name, address)
    !<name>     #= $<address>|!addr
    !<address>  #= !<name>

    <name>      = $000000+!<address>
endmacro

;   Tables
%define_ow_sprite_table(ow_sprite_num, 0DE5)
%define_ow_sprite_table(ow_sprite_speed_x, 0E25)
%define_ow_sprite_table(ow_sprite_speed_y, 0E65)
%define_ow_sprite_table(ow_sprite_speed_z, 0EA5)
%define_ow_sprite_table(ow_sprite_x_pos, 14B0)
%define_ow_sprite_table(ow_sprite_y_pos, 14F0)
%define_ow_sprite_table(ow_sprite_z_pos, 1530)
%define_ow_sprite_table(ow_sprite_timer_1, 1570)
%define_ow_sprite_table(ow_sprite_timer_2, 15B0)
%define_ow_sprite_table(ow_sprite_timer_3, 15F0)
%define_ow_sprite_table(ow_sprite_misc_1, 1630)
%define_ow_sprite_table(ow_sprite_misc_2, 1670)
%define_ow_sprite_table(ow_sprite_misc_3, 16B0)
%define_ow_sprite_table(ow_sprite_misc_4, 16F0)
%define_ow_sprite_table(ow_sprite_misc_5, 1730)
%define_ow_sprite_table(ow_sprite_speed_x_acc, 17B0)
%define_ow_sprite_table(ow_sprite_speed_y_acc, 17F0)
%define_ow_sprite_table(ow_sprite_speed_z_acc, 1830)
%define_ow_sprite_table(ow_sprite_init, 1870)
%define_ow_sprite_table(ow_sprite_extra_1, 1770)
%define_ow_sprite_table(ow_sprite_extra_2, 0110)

;   OW Revolution needs these
%define_ow_sprite_table(ow_sprite_props, 1E02)
%define_ow_sprite_table(ow_sprite_load_index, 1E42)

if or(equal(!sa1, 1), equal(read4($02FFE2), $44535453))
    if !sa1
        !ow_sprite_load_table  #= $7FAF00
    else
        !ow_sprite_load_table  #= $418A00
    endif
else
    !ow_sprite_load_table      #= $1938
endif

;   Flags
%define_ow_sprite_table(ow_sprite_index, 0DDE)

!ow_sprite_extra_bits  #= !ow_sprite_extra_1            ;   this is kept for backwards compatibility

