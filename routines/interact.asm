;====================================
; A basic interaction routine
; By Ari, originally for the fish
; and goomba sprites
;====================================

    LDY $0DD6|!addr
    LDA !mario_x_pos_lo,y
    SEC
    SBC #$0008
    AND #$FFFE
    CMP !ow_sprite_x_pos,x
    BNE .return
    LDA !ow_sprite_y_pos,x
    SEC
    SBC !ow_sprite_z_pos,x
    AND #$FFFE
    STA $00
    LDA !mario_y_pos_lo,y
    SEC
    SBC #$000C
    AND #$FFFE
    CMP $00
    BNE .return
    SEC
    RTL
.return
    CLC
    RTL
