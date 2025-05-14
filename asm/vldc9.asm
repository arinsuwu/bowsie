;===========================================================
; Custom Overworld Sprites
;---------------------------
; This couldn't be done without resources/stuff made by:
; - Lui37, Medic et al.: original "custom ow sprites"
;   used in the ninth VLDC.
;   See https://smwc.me/1413938
; - JackTheSpades: code commenting and slight improvements
; - Katrina: she wrote the proposal for the new overworld
;   sprite design. I followed some of her guidelines and
;   implemented her two design proposals.
;   See https://bin.smwcentral.net/u/8006/owc.txt
; - FuSoYa: added the custom overworld sprite support into
;   Lunar Magic, based on the format used by VLDC9.
;===========================================================

padbyte $EA

org $009AA4         ;   nuke jump to original ow sprite load (always, even if kept. boy slow down running this bs routine...)
    BRA $02 : NOP #2

org $04840D         ;   swap the order in which the player is drawn and the sprites are processed
    JSR $862E
    JSR $F708
org $04827E
    JMP $840D

org $0091CA         ;   these addresses aren't cleared in the vanilla game, but we use them, so let's clear this
    JSL prepare_clear_ram

org $00A165         ;   jump to new ow sprite load (this one will run in gamemode $0C)
    JML custom_ow_sprite_load_gm
org $04DBA3         ;   jump to new ow sprite load (this one will run in map transitions)
    JMP.w custom_ow_sprite_load_sm

org $04F675     ;   nuke original ow sprite load (which runs in gamemode $05)
    pad $04F6F8

;---

; main hijack, within vanilla freespace
org $04F675|!bank
custom_ow_sprite_load_main:
    if !sa1
        LDA.b #.main
        STA $3180
        LDA.b #.main>>8
        STA $3181
        LDA.b #.main>>16
        STA $3182
        JSR $1E80
        RTS
    endif

.main
    PHB
    LDX $0DB3|!addr             ;\
    LDA $1F11|!addr,x           ; | submap of current player (times 2) into X for index to offset table.
    ASL                         ; |
    TAY                         ;/
    LDA.l $0EF55D+2             ;\  return if pointer is empty
    BEQ .end_spawning           ;/
    STA.b $6B+2                 ;\
    REP #$21                    ; |
    LDA $0EF55D                 ; | get pointer to OW sprite table
    STA $6B                     ; |
    ADC [$6B],y                 ; |
    STA $6B                     ;/

    CLC
    LDY #$00                    ; loop counter = 0
.sprite_load_loop               ; loop for decoding sprite data and spawning sprite.
    LDA [$6B],y                 ;\  get first word of sprite data (yyyx xxxx  xnnn nnnn)
    BEQ .end_spawning           ; | 0x0000 indicates end of data
    AND #$007F                  ; |
    STA $00                     ; | mask out n bits (sprite number) and store to $00
    LDA [$6B],y                 ; |
    AND #$1F80                  ; | mask out x bits: ---xxxxx x-------
    XBA                         ; | swap bytes in A: x------- ---xxxXX
    ASL                         ; |
    ROL                         ; | rotate left:     -------- --xxxxxx
    ASL #2                      ; | multiple by 8 because x is in 8x8 blocks, not pixels.
    STA $02                     ;/  store x position (in pixels) in $02
    INY

    LDA [$6B],y                 ;\ get 'middle' word of sprite data (zzzz zyyy  yyyx xxxx)
    AND #$07E0                  ; | mask out y bits:      -----yyy yyy-----
    LSR #2                      ; | shift y bits down by 2 (same as y multiplied by 8 to get pixels from 8x8)
    STA $04                     ;/ store y position (in pixel) in $04

    LDA [$6B],y                 ;\ get 'middle' word of sprite data (zzzz zyyy  yyyx xxxx)
    AND #$F800                  ; | mask out z bits: zzzzz--- --------
    XBA                         ; | swap bytes:        -------- zzzzz---
    STA $06                     ;/ store z position (in pixel) in $06
    
    INY #2                      ;\ 
    PHY                         ; | gotta save Y if we'll use this routine
    LDA [$6B],y                 ; | get high word or sprite data (____ ____  eeee eeee)
    STA $08                     ;/ store extra byte to $08 (and garbage data to $09)

                                ;\ Routine to first spawn the sprite and put data into sprite tables:
                                ; | Input (16 bit):
    %spawn_sprite()             ; |      $00 = Sprite number
                                ; |      $02 = Sprite X position (in pixel)
                                ; |      $04 = Sprite Y position (in pixel)
                                ; |      $06 = Sprite Z position (in pixel)
                                ; |      $08 = Extra Byte (not word!)
                                ; |
    PLY                         ; | Output:
    BCC .end_spawning           ; |      Carry: Clear = No Spawn, Set = Spawn
                                ;/      Y:      Sprite Index (for RAM addresses) 
    INY
    CLC
    BRA .sprite_load_loop

.end_spawning
    SEP #$20
    PLB
    if !sa1
        RTL
    else
        RTS
    endif

;---

org $04F76E|!bank
run_ow_sprite:

; Main handler
.main
    PHB
    REP #$21
    LDA #!oam_start
    STA !ow_sprite_oam
    LDA #!oam_start_p
    STA !ow_sprite_oam_p

    LDX.b #!bowsie_ow_slots*2-2     ;\
-   LDA !ow_sprite_num,x            ; | Main loop.
    BEQ .no_sprite                  ; | Call execute_ow_sprite for all sprites where     
    LDA !ow_sprite_init,x           ; | !ow_sprite_num,x is not zero.
    BNE +                           ;/
    JSR execute_ow_sprite_init      ;\
    INC !ow_sprite_init,x           ; | Or, in case !ow_sprite_init,x is still zero, call execute_ow_sprite_init and then INC it.
    BRA .no_sprite                  ;/

+   JSR execute_ow_sprite
.no_sprite
    DEX #2
    BPL -

    SEP #$20
    PLB
return:
    RTS

;---

custom_ow_sprite_load:
.gm
    JSR custom_ow_sprite_load_main
    JSR run_ow_sprite_main
    JSR run_ow_sprite_main
    JSL $04D6E9|!bank
    JML $00A169|!bank
.sm
    JSL clear_ram
    PHX
    JSR custom_ow_sprite_load_main
    JSR run_ow_sprite_main
    JSR run_ow_sprite_main
    PLX
    LDA $1F11|!addr,x
    JMP $DBA6

;---

;--------------------------------------------------------------------------------
; Routine that calls an OW sprite's main function
; Also reduces the sprite's timers by 1.
; Input:                X = sprite index
;        !ow_sprite_num,x = sprite number    
;--------------------------------------------------------------------------------
execute_ow_sprite:
    STX !ow_sprite_index

    LDA !ow_sprite_timer_1,x        ;\
    BEQ +                           ; |
    DEC !ow_sprite_timer_1,x        ; |
+   LDA !ow_sprite_timer_2,x        ; |
    BEQ +                           ; | Decrease timers 
    DEC !ow_sprite_timer_2,x        ; |
+   LDA !ow_sprite_timer_3,x        ; |
    BEQ +                           ; |
    DEC !ow_sprite_timer_3,x        ;/

+   LDA !ow_sprite_num,x            ;\
    ASL                             ; |
    ADC !ow_sprite_num,x            ; | Sprite number times 3 in x
    TXY                             ; |
    REP #$10                        ; |
    TAX                             ;/

    LDA.l ow_sprite_main_ptrs-3,x   ;\ 
    STA $00                         ; | Get sprite main pointer in $00
    SEP #$20                        ; | Sprite number 00 is <end> so the table
    LDA.l ow_sprite_main_ptrs-1,x   ; | is actually 1 indexed (hence those subtractions)
    STA $02                         ;/

    PHA                             ;\ 
    PLB                             ; | Setup bank (value still in A)
    REP #$20                        ; | A in 16 bit
    SEP #$10                        ; | 8-bit index
    TYX                             ;/

    PHK                             ;\
    PEA.w return-1                  ; | workaround for JSL [$0000]
    JML.w [!dp]                     ;/

;--------------------------------------------------------------------------------
; Routine that calls an OW sprite's init function
; Input:                X = sprite index
;        !ow_sprite_num,x = sprite number    
;--------------------------------------------------------------------------------
execute_ow_sprite_init:
    STX !ow_sprite_index

    LDA !ow_sprite_num,x            ;\
    ASL                             ; |
    ADC !ow_sprite_num,x            ; | Sprite number times 3 in x
    TXY                             ; |
    REP #$10                        ; |
    TAX                             ;/

    LDA.l ow_sprite_init_ptrs-3,x   ;\ 
    STA $00                         ; | Get sprite init pointer in $00
    SEP #$20                        ; | sprite number 00 is <end> so the table
    LDA.l ow_sprite_init_ptrs-1,x   ; | is actually 1 indexed (hence those subtractions)
    STA $02                         ;/
    
    PHA                             ;\ 
    PLB                             ; | Setup bank (value still in A)
    REP #$20                        ; | A in 16 bit
    SEP #$10                        ; | 8-bit index
    TYX                             ;/

    PHK                             ;\
    PEA.w return-1                  ; | workaround for JSL [$0000]
    JML.w [!dp]                     ;/

;---

; Clear RAM used in OW sprites, either in submap transitions or on level load
prepare_clear_ram:
    STA $00
    STZ $01
clear_ram:
    PHX
    REP #$30
    LDX.w #(!bowsie_ow_slots*2*21)+4
-   STZ !ow_sprite_num,x
    DEX #2
    BPL -
    SEP #$30
    PLX
    RTL


