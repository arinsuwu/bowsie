;===================================================
; get_draw_info: Find free OAM slots to draw tiles
;
; Input:
;    A contains the amount of tiles to draw,
;      minus 1
;    Clear carry to ask priority
;
; Output:
;    16-bit X/Y
;    carry set if off screen, clear otherwise
;    Y contains the oam index to use
;    $00 contains the X position, screen relative
;    $02 contains the Y position, screen relative
;    $04-$09 are destroyed (non-MaxTile)
;===================================================

    REP #$10

    if !bowsie_maxtile
        INC
        STA $04
        LDA #$0000
        BCC +
        LDA #$0003
    +   STA $06

        %sub_offscreen()
        BCS .return

        LDY $04
        LDA $06
        JSL maxtile_get_slot
        BCC .no_slot
        LDY !maxtile_oam_buffer_index_1
        CLC
        RTL

    .no_slot
        SEC
    .return
        RTL

    else
        PHP
        BCS +
        LDY !ow_sprite_oam_p
        BRA .start
    +   LDY !ow_sprite_oam
    .start
        ASL #2
        STA $04
        STZ $06
        %sub_offscreen()
        BCS .return
        SEP #$20

    .oam_loop
        CPY #!oam_limit
        BCC +
    .loop_back
        LDY #!oam_start
        LDA $06
        BNE .found_all_slots
        INC $06
    +   LDA.w oam_buffer[$00].y_pos,y
        CMP #$F0
        BEQ .check_tile_amount
    .oam_next
        INY #4
        BRA .oam_loop

    .check_tile_amount
        REP #$21
        TYA
        ADC $04
        STA $08
        SEP #$20
    .tile_amount_loop
        CPY $08
        BEQ .found_all_slots
        INY #4
        CPY #!oam_limit
        BCS .loop_back
        LDA.w oam_buffer[$00].y_pos,y
        CMP #$F0
        BEQ .tile_amount_loop
        BRA .oam_next

    .found_all_slots
        REP #$21
        TYA
        ADC #$0004
        PLP
        BCS +
        STA !ow_sprite_oam_p
        RTL

    +   STA !ow_sprite_oam
        CLC
        RTL
    .return
        PLP
        SEC
        RTL
    endif

