;=======================================================
; sub_horz_pos: Check sprite's horizontal position
;               relative to player's
;
; Output:
;  $0E contains the distance between player and sprite
;    Y is 0 if player is to the right, 1 otherwise
;=======================================================

    LDY $0DD6|!addr
    LDA !mario_x_pos_lo,y
    LDY #$00
    SEC
    SBC #$0008
    SEC
    SBC !ow_sprite_x_pos,x
    STA $0E
    BPL $01
    INY
    RTL
