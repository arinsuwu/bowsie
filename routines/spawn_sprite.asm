;====================================================
; spawn_sprite: Spawn Overworld sprite
; ---
;
; Input:
;    $00 contains the sprite number
;    $02 contains the x position
;    $04 contains the y position
;    $06 contains the z position
;    $08 contains the extra bytes (low byte only)
;
; Output:
;    Carry set if spawn successful, clear otherwise
;    Y contains the new sprite index, if successful
;====================================================

if !bowsie_contiguous
    LDX.w #$28*(!bowsie_ow_slots-1)
-   LDA !ow_sprite_num,x
    BEQ .found_slot
    TXA
    SEC
    SBC.w #$0028
    TAX
else
    LDX.b #(!bowsie_ow_slots-1)*2
-   LDA !ow_sprite_num,x
    BEQ .found_slot
    DEX #2
endif
    BPL -
    CLC
    LDX !ow_sprite_index
    RTL

.found_slot
    LDA $00
    STA !ow_sprite_num,x
    LDA $02
    STA !ow_sprite_x_pos,x
    LDA $04
    STA !ow_sprite_y_pos,x
    LDA $06
    STA !ow_sprite_z_pos,x
    if !bowsie_lmver < 351
        LDA $08
        STA !ow_sprite_extra_1,x
    endif
    STZ !ow_sprite_init,x
    STZ !ow_sprite_speed_x,x
    STZ !ow_sprite_speed_x_acc,x
    STZ !ow_sprite_speed_y,x
    STZ !ow_sprite_speed_y_acc,x
    STZ !ow_sprite_speed_z,x
    STZ !ow_sprite_speed_z_acc,x
    STZ !ow_sprite_timer_1,x
    STZ !ow_sprite_timer_2,x
    STZ !ow_sprite_timer_3,x
    STZ !ow_sprite_misc_1,x
    STZ !ow_sprite_misc_2,x
    STZ !ow_sprite_misc_3,x
    STZ !ow_sprite_misc_4,x
    STZ !ow_sprite_misc_5,x
    SEC
    TXY
    LDX !ow_sprite_index
    RTL
