;===========================================================================
; MaxTile
;
; Acknowledgements:
; - SA-1 MaxTile is 2020-21 Vitor Vilela
; - OR MaxTile is 2023 yoshifanatic
; - Hacks to the vanilla draw routines to use MaxTile are 2025 Erik/Arinsu
;===========================================================================

; This only inserts under a specific set of conditions.
; - MaxTile is requested to be inserted.
; - SA-1 is not in use. In SA-1's case, we use Vitor's native MaxTile.
; - OW Revolution is not in use. In OR's case, we use yoshifanatic's native MaxTile.

incsrc "maxtile_defines.asm"

if and(and(not(!sa1), not(!bowsie_owrev)), !bowsie_maxtile)
    ; MaxTile functions
    maxtile_get_slot_main:
        STY !maxtile_oam_buffer_index_2
        ASL #4
        TAY

        LDA !maxtile_oam_buffer_index_2
        ASL #2
        STA !maxtile_oam_buffer_index_1

        LDA.w !maxtile_mirror_pri_0+0,y
        SEC
        SBC !maxtile_oam_buffer_index_1
        CMP.w !maxtile_mirror_pri_0+8,y
        BCC .no_slots
        STA.w !maxtile_mirror_pri_0+0,y
        ADC #$0003
        STA !maxtile_oam_buffer_index_1

        LDA.w !maxtile_mirror_pri_0+2,y
        SEC
        SBC !maxtile_oam_buffer_index_2
        STA.w !maxtile_mirror_pri_0+2,y
        INC
        STA !maxtile_oam_buffer_index_2

        SEC
    .no_slots
        RTL

    maxtile_sync_buffer:
        PEA.w $0000|(bank(!maxtile_oam_buffer))
        PLB
        REP #$30

        LDA #$0200
        STA $00
        %restrict_buffer(!maxtile_ptr_pri_0)
        %restrict_buffer(!maxtile_ptr_pri_1)
        %restrict_buffer(!maxtile_ptr_pri_2)
        %restrict_buffer(!maxtile_ptr_pri_3)

        LDA #$0080
        STA $00
        %restrict_buffer(!maxtile_ptr_pri_0+2)
        %restrict_buffer(!maxtile_ptr_pri_1+2)
        %restrict_buffer(!maxtile_ptr_pri_2+2)
        %restrict_buffer(!maxtile_ptr_pri_3+2)

        LDY.w #!vanilla_oam_buffer+$01FF
        %copy_priority(!maxtile_ptr_pri_3, 3)
        %copy_priority(!maxtile_ptr_pri_2, 3)
        %copy_priority(!maxtile_ptr_pri_1, 3)
        %copy_priority(!maxtile_ptr_pri_0, 3)

        LDY.w #!vanilla_oam_tilesize_buffer+$007F
        %copy_priority(!maxtile_ptr_pri_3+2, 0)
        %copy_priority(!maxtile_ptr_pri_2+2, 0)
        %copy_priority(!maxtile_ptr_pri_1+2, 0)
        %copy_priority(!maxtile_ptr_pri_0+2, 0)

        ;   Corresponds to oam_reset_tables in SA-1's oam.asm
        LDA !maxtile_oam_buffer_pri_0+4
        STA !maxtile_oam_buffer_pri_0+0
        LDA !maxtile_oam_buffer_pri_1+4
        STA !maxtile_oam_buffer_pri_1+0
        LDA !maxtile_oam_buffer_pri_2+4
        STA !maxtile_oam_buffer_pri_2+0
        LDA !maxtile_oam_buffer_pri_3+4
        STA !maxtile_oam_buffer_pri_3+0

        LDA !maxtile_oam_buffer_pri_0+6
        STA !maxtile_oam_buffer_pri_0+2
        LDA !maxtile_oam_buffer_pri_1+6
        STA !maxtile_oam_buffer_pri_1+2
        LDA !maxtile_oam_buffer_pri_2+6
        STA !maxtile_oam_buffer_pri_2+2
        LDA !maxtile_oam_buffer_pri_3+6
        STA !maxtile_oam_buffer_pri_3+2

        SEP #$30
        PLB
        RTL

    ;---

    maxtile_setup:
        ;   Extremely bizarre here as I hack $7F8000 to redirect to MaxTile's OAM flush.
        ;   It's, uh, more user friendly than asking them to edit UberASM LOL
        LDA #$5C
        STA $7F8000
        LDA.b #maxtile_flush
        STA $7F8001
        LDA.b #maxtile_flush>>8
        STA $7F8002
        LDA.b #maxtile_flush>>16
        STA $7F8003
        ; RTL

    ;-

    maxtile_flush:
        LDA $0100|!addr
        CMP #$0C
        BCC .dont_flush_maxtile
        CMP #$0F
        BCS .dont_flush_maxtile

        ;   Corresponds to oam_init_tables in SA-1's oam.asm
        REP #$20

        LDA.w #!maxtile_oam_buffer_pri_0-4
        STA !maxtile_ptr_pri_0+8
        LDA.w #!maxtile_oam_buffer_pri_0+$01FC
        STA !maxtile_ptr_pri_0+0
        STA !maxtile_ptr_pri_0+4

        LDA.w #!maxtile_oam_buffer_pri_1-4
        STA !maxtile_ptr_pri_1+8
        LDA.w #!maxtile_oam_buffer_pri_1+$01FC
        STA !maxtile_ptr_pri_1+0
        STA !maxtile_ptr_pri_1+4

        LDA.w #!maxtile_oam_buffer_pri_2-4
        STA !maxtile_ptr_pri_2+8
        LDA.w #!maxtile_oam_buffer_pri_2+$01FC
        STA !maxtile_ptr_pri_2+0
        STA !maxtile_ptr_pri_2+4

        LDA.w #!maxtile_oam_buffer_pri_3-4
        STA !maxtile_ptr_pri_3+8
        LDA.w #!maxtile_oam_buffer_pri_3+$01FC
        STA !maxtile_ptr_pri_3+0
        STA !maxtile_ptr_pri_3+4

        LDA.w #!maxtile_tilesize_buffer_pri_0+$7F
        STA !maxtile_ptr_pri_0+2
        STA !maxtile_ptr_pri_0+6

        LDA.w #!maxtile_tilesize_buffer_pri_1+$7F
        STA !maxtile_ptr_pri_1+2
        STA !maxtile_ptr_pri_1+6

        LDA.w #!maxtile_tilesize_buffer_pri_2+$7F
        STA !maxtile_ptr_pri_2+2
        STA !maxtile_ptr_pri_2+6

        LDA.w #!maxtile_tilesize_buffer_pri_3+$7F
        STA !maxtile_ptr_pri_3+2
        STA !maxtile_ptr_pri_3+6

        SEP #$20

    ;   The regular, boring, bad clear OAM routine.
    ;   SA-1's variant is called oam_clear.
    .dont_flush_maxtile
        LDA #$F0
        STA $0201|!addr
        JML $7F8005
    .end

    ;---

    ; Replaced graphics routines

    ; Border around the player
    draw_border_around_player_maxtile:
        STA $01
        REP #$30

        LDY #$0010
        LDA #$0000
        JSL maxtile_get_slot_main
        BCC .no_slots

    .slots_gotten
        SEP #$20
        LDX !maxtile_oam_buffer_index_2
        STX $02
        LDX !maxtile_oam_buffer_index_1
        LDY #$0000
        PEA.w (bank(draw_border_around_player)<<8)|(bank(!maxtile_oam_buffer))
        PLB
        JMP.w draw_border_around_player_loop

    .end_loop
        INC $02
        CPY #$0010
        BEQ .done_border_around_player
        JMP.w draw_border_around_player_loop

    .no_slots
        SEP #$30
        RTS

    .done_border_around_player
        PLB
        SEP #$30

    ;-

    ; Player in OW border
    draw_player_in_border_hack:
        LDY #$00
        LDA #$F0
        CMP.l $03F9|!addr
        BEQ +
        INY
    +   CMP.l $03ED|!addr
        BEQ +
        INY
    +   CMP.l $03E9|!addr
        BEQ +
        INY
    +

        CPY #$00
        BNE +
        RTS

    +
        REP #$30
        LDA #$0000
        JSL maxtile_get_slot_main
        BCS .slots_gotten

    .no_slots
        SEP #$30
        RTS

    .slots_gotten
        LDX !maxtile_oam_buffer_index_1
        LDY !maxtile_oam_buffer_index_2
        SEP #$20

        PEA.w (bank(draw_player_in_border_hack)<<8)|(bank(!maxtile_oam_buffer))
        PLB

        LDA.l $03F9|!addr
        CMP #$F0
        BEQ +
        STA.w oam_buffer[$00].y_pos,x
        LDA.l $03F8|!addr
        STA.w oam_buffer[$00].x_pos,x
        LDA.l $03FA|!addr
        STA.w oam_buffer[$00].tile,x
        LDA.l $03FB|!addr
        STA.w oam_buffer[$00].props,x
        LDA.l $049E|!addr
        STA.w oam_tilesize_buffer[$00].tile_sx,y
        INX #4
        INY
    +

        LDA.l $03ED|!addr
        CMP #$F0
        BEQ +
        STA.w oam_buffer[$00].y_pos,x
        LDA.l $03EC|!addr
        STA.w oam_buffer[$00].x_pos,x
        LDA.l $03EE|!addr
        STA.w oam_buffer[$00].tile,x
        LDA.l $03EF|!addr
        STA.w oam_buffer[$00].props,x
        LDA.l $049B|!addr
        STA.w oam_tilesize_buffer[$00].tile_sx,y
        INX #4
        INY
    +

        LDA.l $03E9|!addr
        CMP #$F0
        BEQ +
        STA.w oam_buffer[$00].y_pos,x
        LDA.l $03E8|!addr
        STA.w oam_buffer[$00].x_pos,x
        LDA.l $03EA|!addr
        STA.w oam_buffer[$00].tile,x
        LDA.l $03EB|!addr
        STA.w oam_buffer[$00].props,x
        LDA.l $049A|!addr
        STA.w oam_tilesize_buffer[$00].tile_sx,y
    +

        PLB
        SEP #$30
        RTS

    ;-

    ; Lives exchanger
    draw_lives_exchanger_maxtile:
        REP #$30
        LDY #$0002
        LDA #$0000
        JSL maxtile_get_slot_main
        BCC .no_slots

    .slots_gotten
        LDY !maxtile_oam_buffer_index_1

        PEA.w (bank(draw_lives_exchanger)<<8)|(bank(!maxtile_oam_buffer))
        PLB

        LDA #$7848
        JMP.w draw_lives_exchanger_do_draw

    .no_slots
        SEP #$30
        JMP.w draw_lives_exchanger_dont_draw

    draw_lives_exchanger_finish_drawing:
        PLB

        LDX !maxtile_oam_buffer_index_2
        STA.l oam_tilesize_buffer[$00].tile_sx,x
        STA.l oam_tilesize_buffer[$01].tile_sx,x

        SEP #$10
        JMP.w draw_lives_exchanger_dont_draw

    ;-

    if not(!maxtile_opse)
        ; Player, no Yoshi
        draw_player_not_riding_yoshi_maxtile:
            REP #$30
            PHY

            LDY #$0004
            LDA #$0001
            JSL maxtile_get_slot_main
            PLY
            BCC .no_slots

        .slots_gotten
            LDA !maxtile_oam_buffer_index_2
            STA $CE                                             ;   scratch
            JMP.w draw_player_not_riding_yoshi_do_draw

        .no_slots
            SEP #$30
            RTS

        draw_player_not_riding_yoshi_tilesize:
            PHX
            LDX $CE
            INC $CE
            SEP #$20
            LDA #$00
            STA.l oam_tilesize_buffer[$00].tile_sx,x
            PLX
            INX #4
            RTS

        ; Player, riding Yoshi
        draw_player_riding_yoshi_maxtile:
            LDA #$07
            STA $8C

            REP #$30
            PHY

            LDY #$0008
            LDA #$0001
            JSL maxtile_get_slot_main
            PLY
            BCC .no_slots

        .slots_gotten
            LDA !maxtile_oam_buffer_index_2
            STA $CE                                             ;   scratch
            LDX !maxtile_oam_buffer_index_1
            JMP.w draw_player_riding_yoshi_do_draw

        draw_player_riding_yoshi_end_loop:
            PHX
            LDX $CE
            INC $CE
            SEP #$20
            LDA #$00
            STA.l oam_tilesize_buffer[$00].tile_sx,x
            PLX

            INX #4
            INY #2
            DEC $8C
            BMI .done_looping
            JMP.w draw_player_riding_yoshi_draw_loop

        .done_looping
        draw_player_riding_yoshi_maxtile_no_slots:
            SEP #$30
            RTS

        ; Player, halo
        draw_player_halo_maxtile:
            LDY #$0002
            LDA #$0001
            JSL maxtile_get_slot_main
            BCC .no_slots

        .slots_gotten
            LDY !maxtile_oam_buffer_index_1

            PEA.w (bank(draw_player_halo_maxtile)<<8)|(bank(!maxtile_oam_buffer))
            PLB

            SEP #$21
            LDA $8A
            JMP draw_player_halo_do_draw

        .no_slots
            PLA
            STA $8A
            RTS

        draw_player_halo_finish_drawing:
            STA.w oam_buffer[$01].props,y

            PLB
            LDX !maxtile_oam_buffer_index_2

            LDA #$00
            STA.l oam_tilesize_buffer[$00].tile_sx,x
            STA.l oam_tilesize_buffer[$01].tile_sx,x

            REP #$20
            PLA
            STA $8A
            RTS

    endif

    ;---

    ; Hijacks
    pushpc

        ; Border around the player and player in border
        org $0485EE|!bank
        draw_border_around_player:
            JMP.w .maxtile
            BRA $00

        .loop
        ;   This is an adaptation of the original code (which is horrible btw)
        ;   Data bank is $7F
            skip 2
            STA.w oam_buffer[$00].x_pos,x
            skip 5
            skip 2
            STA.w oam_buffer[$00].y_pos,x
            skip 2
            STA.w oam_buffer[$00].tile,x
            skip 2
            STA.w oam_buffer[$00].props,x
            PHX
            LDX $02
            STZ.w oam_tilesize_buffer[$00].tile_sx,x
            PLX
            INY
            TYA
            AND #$03
            BNE ..no_next_row
            LDA #$18
            STA 00
            LDA $01
            CLC
            ADC #$08
            STA $01
        ..no_next_row
            INX #4
            JMP.w draw_border_around_player_maxtile_end_loop
        .return
            RTS

        pad $04862E|!bank
        assert pc() == $04862E|!bank

        ; Lives exchanger
        org $04F56C|!bank
        draw_lives_exchanger:
            JMP.w .maxtile
            BRA $00
        .do_draw
            STA.w oam_buffer[$00].x_pos,y
            skip 3
            STA.w oam_buffer[$01].x_pos,y
            skip 3
            STA.w oam_buffer[$00].tile,y
            skip 3
            STA.w oam_buffer[$01].tile,y
            skip 2
            skip 2

            JMP.w .finish_drawing
            NOP #3
        .dont_draw

        if not(!maxtile_opse)
            ; Player, draw order
            org $048681|!bank
                STA $8D                                             ;   scratch
            org $048688|!bank
                STA $8E                                             ;   scratch
            org $048698|!bank
            change_player_draw_order:
                REP #$20
                LDA $0DD6|!addr
                XBA
                LSR
                EOR #$0200
                STA $04
                SEP #$20

            .check_draw_player_2
                LDA $1F11|!addr
                LDY $13D9|!addr
                CPY #$0A
                BNE +
                EOR #$01
            +   CMP $1F12|!addr
                BNE .draw_player_1

                LDA $0DD6|!addr
                LSR
                EOR #$02
                TAY

                LDA #$03
                STA $8C
                LDA $02
                STA $06
                STA $8A
                LDA $03
                STA $07
                STA $8B

                LDA $1F13|!addr,y
                CMP #$12
                BEQ ..dont_offset_pos
                CMP #$07
                BCC ..offset_pos
                CMP #$0F
                BCC ..dont_offset_pos
            ..offset_pos
                LDA $8B
                SEC
                SBC #$05
                STA $8B
                STA $07

            ..dont_offset_pos
                REP #$30
                LDA $0DB2|!addr
                AND #$00FF
                BEQ .draw_player_1
                LDA $0C
                CMP #$00F0
                BCS .draw_player_1
                LDA $0E
                CMP #$00F0
                BCS .draw_player_1
                JSR $8789
                LDA $0DD6|!addr
                LSR
                EOR #$0002
                TAY
                JSR $894F

            .draw_player_1
                SEP #$30

                LDA $0DD6|!addr
                LSR
                TAY

                LDA #$03
                STA $8C
                LDA $8D
                STA $06
                STA $8A
                LDA $8E
                STA $07
                STA $8B

                LDA $1F13|!addr,y
                CMP #$12
                BEQ ..dont_offset_pos
                CMP #$07
                BCC ..offset_pos
                CMP #$0F
                BCC ..dont_offset_pos
            ..offset_pos
                LDA $8B
                SEC
                SBC #$05
                STA $8B
                STA $07

            ..dont_offset_pos
                REP #$30
                LDA $04
                EOR #$0200
                STA $04
                JSR $8789
                LDA $0DD6|!addr
                LSR
                TAY
                JSR $894F
                SEP #$30
                RTS

            pad $048786|!bank
            assert pc() == $048786|!bank

            ; Player, no Yoshi
            org $048962|!bank
            draw_player_not_riding_yoshi:
                PLY
                JMP.w .maxtile

            .do_draw
                LDA $1F13|!addr,y
                ASL #4
                STA $00
                LDA $13
                AND #$0018
                ADC $00                                             ;   carry clear after ASLs
                TAY
                LDA $04
                XBA
                LSR
                TAX
                LDA $0DB3|!addr,x
                BPL .alive
                LDA $00
                TAY
                BRA .begin_draw

            .alive
                LDA $01,s                                           ;\
                CMP #$874E                                          ; | return at $04874F = current player
                BNE .begin_draw                                     ;/
                LDA $13D9|!addr
                CMP #$000B
                BNE .begin_draw

            ..star_road
                LDA $13
                AND #$000C
                LSR #2
                TAY
                LDA $894B,y
                AND #$00FF
                TAY

            .begin_draw
                LDX !maxtile_oam_buffer_index_1
            .draw_loop
                REP #$21
                LDA $8A
                STA.l oam_buffer[$00].x_pos,x
                LDA $87CB,y
                ADC $04                                             ;   carry clear after REP #$21
                STA.l oam_buffer[$00].tile,x
                SEP #$21
                JSR.w .tilesize
                INY #2
                LDA $8A
                ADC #$07                                            ;   add 8, carry set after SEP #$21
                STA $8A
                DEC $8C
                LDA $8C
                LSR
                BCC +
                LDA $06
                STA $8A
                LDA $8B
                ADC #$07                                            ;   add 8, carry set after LSR
                STA $8B
            +
                LDA $8C
                BPL .draw_loop
            .return
                RTS
            pad $0489DE|!bank
            assert pc() == $0489DE|!bank

            ; Player, riding Yoshi
            org $048CE6|!bank
            draw_player_riding_yoshi:
                JMP.w .maxtile
            ; We need to save bytes by any means necessary
            .do_draw
                LDA $1F13|!addr,y
                ASL #4
                STA $00
                LDA $13
                AND #$0008
                ASL
                ADC $00
                TAY

                LDA $01,s                                           ;\
                CMP #$874E                                          ; | return at $04874F = current player
                BNE ..no_star_road                                  ;/
                LDA $13D9|!addr
                CMP #$000B
                BNE ..no_star_road

            ..star_road
                LDA $13
                AND #$000C
                LSR #2
                TAY
                LDA $894B,y
                AND #$00FF
                TAY
            ..no_star_road

            .draw_loop
                REP #$20
                PHY
                TYA
                LSR
                TAY
                SEP #$20
                LDA $8B5E,y
                CLC
                ADC $8A
                STA.l oam_buffer[$00].x_pos,x
                LDA $8C1E,y
                CLC
                ADC $8B
                STA.l oam_buffer[$00].y_pos,x
                PLY
                REP #$20
                LDA $89DE,y
                CMP #$FFFF
                BEQ .skip_tile
                STA $00
                AND #$0F00
                CMP #$0200
                BNE .offset_tile
                STY $08
                LDA $0E
                SBC #$0004                                          ;   BNE branched = carry set
                TAY
                LDA $00
                AND #$F0FF
                ORA $8CDE,y
                LDY $08
                BRA .store_tile

            .offset_tile
                LDA $00
                CLC
                ADC $04
            .store_tile
                STA.l oam_buffer[$00].tile,x
            .skip_tile
                JMP.w .end_loop
            .return
                SEP #$30
                RTS

            pad $048D74|!bank
            assert pc() == $048D74|!bank

            ; Player, halo
            org $04879B|!bank
            draw_player_halo:
                JMP.w .maxtile
                NOP

            ;   Try respect the original code as much as possible
            .do_draw
                STA.w oam_buffer[$00].x_pos,y
                skip 3
                STA.w oam_buffer[$01].x_pos,y
                skip 2
                skip 3
                STA.w oam_buffer[$00].y_pos,y
                STA.w oam_buffer[$01].y_pos,y
                skip 2
                STA.w oam_buffer[$00].tile,y
                STA.w oam_buffer[$01].tile,y
                skip 2
                STA.w oam_buffer[$00].props,y
                skip 2
                JMP.w .finish_drawing

        endif

    pullpc
else
    error "You aren't supposed to insert this variant of MaxTile."
endif

