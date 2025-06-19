;====================================
; A basic interaction routine
; By Erik, originally for the fish
; and goomba sprites
;====================================

    LDY $0DD6|!addr
    LDA $1F17|!addr,y
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
    LDA $1F19|!addr,y
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
