; In Lui's system, all sprite tables are 16 bit, with the sprite index in X
; and discontiguous tables, just like in the base vanilla sprites.

incsrc bowsie_defines.asm       ; don't delete this! it's created during the tool run.

;   Whether to install MaxTile
;   If this is a SA-1 ROM, there's no need to install anything
;   THIS IS AN INTERNAL DEFINE. Use !bowsie_maxtile to detect differences between systems!
!vldc9_maxtile #= and(not(!sa1), !bowsie_maxtile)

;   Macros
macro define_ow_sprite_table(name, address)
    !<name>    #= $<address>|!addr
    !<address> #= !<name>

    <name>      = $000000+!<address>
endmacro

;   OAM definitions
if !bowsie_maxtile
    if !sa1
        !oam_buffer            #= $400000
        !oam_tilesize_buffer   #= $400000
    else
        !oam_buffer            #= $7F9A7B
        !oam_tilesize_buffer   #= $7F9A7B
    endif

    !next_oam_slot              = "INY #4"
    !next_oam_tilesize_slot     = "INY"
    !adjacent_oam_slot          = $01
else
    !oam_buffer                #= $000200|!addr
    !oam_tilesize_buffer       #= $000420|!addr

    !oam_start_p               #= $0000
    !oam_start                 #= $0100
    !oam_limit                 #= $01E8

    !next_oam_slot              = "DEY #4"
    !next_oam_tilesize_slot     = "DEY"
    !adjacent_oam_slot          = -$01
endif

;   Expanded levels and events definitions...
;   ...except there's no such thing in the traditional OW, for now.
!level_status_flags    #= $1EA2|!addr
!event_flags           #= $1F02|!addr
!mario_map             #= $1F11|!addr
!luigi_map             #= !mario_map+1
!mario_anim_state      #= $1F13|!addr
!luigi_anim_state      #= $1F15|!addr
!mario_x_pos_lo        #= $1F17|!addr
!mario_y_pos_lo        #= !mario_x_pos_lo+2
!luigi_x_pos_lo        #= !mario_x_pos_lo+4
!luigi_y_pos_lo        #= !mario_x_pos_lo+6
!events_triggered      #= $1F2E|!addr

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

;   Tables
%define_ow_sprite_table(ow_sprite_num, 14C8)
%define_ow_sprite_table(ow_sprite_speed_x, 14F8)
%define_ow_sprite_table(ow_sprite_speed_y, 1528)
%define_ow_sprite_table(ow_sprite_speed_z, 1558)
%define_ow_sprite_table(ow_sprite_x_pos, 1588)
%define_ow_sprite_table(ow_sprite_y_pos, 15B8)
%define_ow_sprite_table(ow_sprite_z_pos, 15E8)
%define_ow_sprite_table(ow_sprite_timer_1, 1618)
%define_ow_sprite_table(ow_sprite_timer_2, 1648)
%define_ow_sprite_table(ow_sprite_timer_3, 1678)
%define_ow_sprite_table(ow_sprite_misc_1, 16A8)
%define_ow_sprite_table(ow_sprite_misc_2, 16D8)
%define_ow_sprite_table(ow_sprite_misc_3, 1708)
%define_ow_sprite_table(ow_sprite_misc_4, 1738)
%define_ow_sprite_table(ow_sprite_misc_5, 1768)
%define_ow_sprite_table(ow_sprite_speed_x_acc, 17C8)
%define_ow_sprite_table(ow_sprite_speed_y_acc, 17F8)
%define_ow_sprite_table(ow_sprite_speed_z_acc, 1828)
%define_ow_sprite_table(ow_sprite_init, 188C)
%define_ow_sprite_table(ow_sprite_extra_1, 0DE0)
%define_ow_sprite_table(ow_sprite_extra_2, 0E10)

;   Flags
%define_ow_sprite_table(ow_sprite_index, 1858)
%define_ow_sprite_table(ow_sprite_oam, 185A)
%define_ow_sprite_table(ow_sprite_oam_p, 185C)

;   Misc.
!ow_sprite_extra_bits  #= !ow_sprite_extra_1            ;   this is kept for backwards compatibility

