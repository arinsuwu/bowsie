; In Katrina's system, all sprite tables are 16 bit, and instead of having
; the discontiguous tables like SMW, we have contiguous tables like a struct.

incsrc bowsie_defines.asm       ; don't delete this! it's created during the tool run.

!oam_start_p    = $0000
!oam_start      = $00BC
!oam_limit      = $01E8

macro define_ow_sprite_table(name, address)
        !<name>     #= <address>
        !<address>  #= !<name>
endmacro

struct ow_sprite_struct $14C8|!addr
    .number:        skip 2
    .x_speed:       skip 2
    .y_speed:       skip 2
    .z_speed:       skip 2
    .x_pos:         skip 2
    .y_pos:         skip 2
    .z_pos:         skip 2
    .timer_1:       skip 2
    .timer_2:       skip 2
    .timer_3:       skip 2
    .misc_1:        skip 2
    .misc_2:        skip 2
    .misc_3:        skip 2
    .misc_4:        skip 2
    .misc_5:        skip 2
    .extra_byte:    skip 2
    .x_speed_acc:   skip 2
    .y_speed_acc:   skip 2
    .z_speed_acc:   skip 2
    .init:          skip 2
endstruct

;   Tables
%define_ow_sprite_table(ow_sprite_num, ow_sprite_struct.number)
%define_ow_sprite_table(ow_sprite_speed_x, ow_sprite_struct.x_speed)
%define_ow_sprite_table(ow_sprite_speed_y, ow_sprite_struct.y_speed)
%define_ow_sprite_table(ow_sprite_speed_z, ow_sprite_struct.z_speed)
%define_ow_sprite_table(ow_sprite_x_pos, ow_sprite_struct.x_pos)
%define_ow_sprite_table(ow_sprite_y_pos, ow_sprite_struct.y_pos)
%define_ow_sprite_table(ow_sprite_z_pos, ow_sprite_struct.z_pos)
%define_ow_sprite_table(ow_sprite_timer_1, ow_sprite_struct.timer_1)
%define_ow_sprite_table(ow_sprite_timer_2, ow_sprite_struct.timer_2)
%define_ow_sprite_table(ow_sprite_timer_3, ow_sprite_struct.timer_3)
%define_ow_sprite_table(ow_sprite_misc_1, ow_sprite_struct.misc_1)
%define_ow_sprite_table(ow_sprite_misc_2, ow_sprite_struct.misc_2)
%define_ow_sprite_table(ow_sprite_misc_3, ow_sprite_struct.misc_3)
%define_ow_sprite_table(ow_sprite_misc_4, ow_sprite_struct.misc_4)
%define_ow_sprite_table(ow_sprite_misc_5, ow_sprite_struct.misc_5)
%define_ow_sprite_table(ow_sprite_extra_bits, ow_sprite_struct.extra_byte)
%define_ow_sprite_table(ow_sprite_speed_x_acc, ow_sprite_struct.x_speed_acc)
%define_ow_sprite_table(ow_sprite_speed_y_acc, ow_sprite_struct.y_speed_acc)
%define_ow_sprite_table(ow_sprite_speed_z_acc, ow_sprite_struct.z_speed_acc)
%define_ow_sprite_table(ow_sprite_init, ow_sprite_struct.init)
;   Flags
%define_ow_sprite_table(ow_sprite_index, 1888)
%define_ow_sprite_table(ow_sprite_oam, 188A)
%define_ow_sprite_table(ow_sprite_oam_p, 188C)
