;==============================================================
; erase_sprite: clear sprite status from load table as needed
; By yoshifanatic
;==============================================================

    if !bowsie_owrev
        LDY !ow_sprite_load_index,x
        CPY #$FF
        BEQ .off_screen_kill_sprite
        SEP #$20
        if or(equal(!sa1, 1), equal(read4($02FFE2), $44535453))
            TYX
            LDA #$00
            STA !ow_sprite_load_table,x
            LDX !ow_sprite_index
        else
            LDA #$00
            STA !ow_sprite_load_table,y
        endif
        REP #$20
    endif
.off_screen_kill_sprite
    STZ !ow_sprite_num,x
    RTL