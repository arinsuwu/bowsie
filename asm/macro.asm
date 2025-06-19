; Macro library for BOWSIE

; draw_shadow: draw a shadow tile based on z position
; parameters: shadow_tile   - tile for the shadow
;             shawdow_props - YXPPCCCT props for the shadow
; expects 8-bit A
; can be called before of after a DEY #4
macro draw_shadow(shadow_tile, shadow_props)
    LDA $02
    CLC
    ADC !ow_sprite_z_pos,x
    XBA
    LDA $00
    REP #$20
    STA $0200|!addr,y
    LDA.w #(<shadow_props><<16|<shadow_tile>)
    STA $0202|!addr,y

    PHY
    TYA
    LSR #2
    TAY
    SEP #$20
    LDA #$00
    STA $0420|!addr,y
    PLY
endmacro

; draw_shadow: draw the vanilla shadow tiles based on z position
; the vanilla shadow is tile $229 with props yxPPccct/yXPPccct
; expects 8-bit A
; can be called before of after a DEY #4
macro draw_vanilla_shadow()
    LDA $02
    CLC
    ADC !ow_sprite_z_pos,x
    STA $01FD|!addr,y
    STA $0201|!addr,y
    LDA $00
    STA $01FC|!addr,y
    CLC
    ADC #$08
    STA $0200|!addr,y
    REP #$20
    LDA #$3029
    STA $01FE|!addr,y
    LDA #$7029
    STA $0202|!addr,y

    PHY
    TYA
    LSR #2
    TAY
    LDA #$0000
    STA $041F|!addr,y
    SEP #$20
    PLY
endmacro

