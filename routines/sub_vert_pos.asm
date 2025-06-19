;=======================================================
; sub_vert_pos: Check sprite's vertical position
;               relative to player's
;
; Output:
;  $0E contains the distance between player and sprite
;    Y is 0 if player is above, 1 otherwise
;=======================================================

    LDY $0DD6|!addr
    LDA $1F19|!addr,y
    LDY #$00
    SEC
    SBC #$0008
    SEC
    SBC !ow_sprite_y_pos,x
    STA $0E
    BPL $01
    INY
    RTL
