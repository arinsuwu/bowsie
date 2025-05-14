;===================================================
; finish_oam_write: Write to size table
; By Akaginite/33953YoShI, adapted by Erik
;
; Input:
;   16-bit AXY
;   A contains the number of tiles to draw - 1
;   X contains the size
;     * $00     - 8x8 tiles
;     * $02     - 16x16 tiles
;     * $80-$FF - manually set the size
;   Y contains the last OAM slot used. The GFX
;   routine has Y go down by four. This routine
;   has Y increase up by four.
;
; Output:
;   16-bit AXY
;   $00-$03 and $08-$0C are destroyed
;   the size table is now set correctly
;===================================================

    STX $0B
    STA $08
    LDX !ow_sprite_index
    LDA !ow_sprite_y_pos,x
    SEC
    SBC $1C
    STA $00
    LDA !ow_sprite_x_pos,x
    SEC
    SBC $1A
    STA $02
    TYA
    LSR #2
    TAX
    SEP #$21

.loop
    LDA $0200|!addr,y
    SBC $02
    REP #$21
    BPL +
    ORA #$FF00
+   ADC $02
    CMP #$0100
    TXA
    SEP #$20
    LDA $0B
    BPL +
    LDA $0420|!addr,x
    AND #$02
+   ADC #$00
    STA $0420|!addr,x
    LDA $0201|!addr,y
    SEC
    SBC $00
    REP #$21
    BPL +
    ORA #$FF00
+   ADC $00
    CLC
    ADC #$0010
    CMP #$0100
    BCC .next
    LDA #$00F0
    SEP #$20
    STA $0201|!addr,y
.next
    SEP #$21
    INY #4
    INX
    DEC $08
    BPL .loop
    LDX !ow_sprite_index
    REP #$20
    RTL

