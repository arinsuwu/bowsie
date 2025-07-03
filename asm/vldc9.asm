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

org $009AA4         ;   either prepare MaxTile or nuke jump to original ow sprite load
    if !vldc9_maxtile
        JSL maxtile_setup
    else
        BRA $02 : NOP #2
    endif

;---

org $04840D         ;   swap the order in which the player is drawn and the sprites are processed
    JSR $862E
    JMP $F708
org $04827E
    JMP $840D

;---

org $0091CA         ;   these addresses aren't cleared in the vanilla game, but we use them, so let's clear this
    JSL prepare_clear_ram

;---

org $00A165         ;   jump to new ow sprite load (this one will run in gamemode $0C)
    JML custom_ow_sprite_load_gm
org $04DBA3         ;   jump to new ow sprite load (this one will run in map transitions)
    JMP.w custom_ow_sprite_load_sm

org $04F625         ;   nuke original ow sprite load (which runs in gamemode $05)
    pad $04F6F8

;---

org $048292         ;   redirect OW ending to sync MaxTile buffer correctly (or just return, if disabled)
    JMP.w run_ow_sprite_done_running

org $00A169         ;   sync OAM correctly in map transitions (or restore code)
    if !vldc9_maxtile
        JSL maxtile_sync_buffer
    elseif not(!sa1)
        LDA #$F0
        STA $3F
    endif

;---

; LM Flags

; Enable arbitrarily sized extra bytes
org $0DE18C
    autoclean dl extra_byte_table
    db $42

; Disable LM from writing to OW sprite area
org $0FFFE0
    db read1($0FFFE0)&$F7

;---

; main hijack, within vanilla freespace
org $04F625|!bank
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
    LDA !mario_map,x           ; | submap of current player (times 2) into X for index to offset table.
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
    LDY #$00                    ;   loop counter = 0
.sprite_load_loop               ;   loop for decoding sprite data and spawning sprite.
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

    LDA [$6B],y                 ;\  get 'middle' word of sprite data (zzzz zyyy  yyyx xxxx)
    AND #$07E0                  ; | mask out y bits:      -----yyy yyy-----
    LSR #2                      ; | shift y bits down by 2 (same as y multiplied by 8 to get pixels from 8x8)
    STA $04                     ;/  store y position (in pixel) in $04

    LDA [$6B],y                 ;\  get 'middle' word of sprite data (zzzz zyyy  yyyx xxxx)
    AND #$F800                  ; | mask out z bits: zzzzz--- --------
    XBA                         ; | swap bytes:        -------- zzzzz---
    STA $06                     ;/  store z position (in pixel) in $06

    PHY
    %spawn_sprite()             ;   attempt to spawn the sprite
    STY $09
    PLY
    BCC .end_spawning           ;   return if no more slots where to spawn a sprite at

    INY #2                      ;   move to the extra bytes
    LDX $00                     ;\
    LDA.l extra_byte_table,x    ; |
    AND #$00FF                  ; | if there's no extra bytes (so only three bytes),
    SBC #$0003                  ; | move on
    CLC                         ; |
    BEQ .sprite_load_loop       ;/
    CMP #$0005                  ;\  if there's more than 4 extra bytes,
    BCS .extra_byte_ptr         ;/  put a pointer instead

.regular
    STZ $00
    STZ $02
    SEP #$20
    STA $08

    LDX #$00                    ;\
-   LDA [$6B],y                 ; |
    STA $00,x                   ; | loop through the amount of extra bytes:
    INY                         ; | extract them in $00-$03
    INX                         ; | the non-filled values initialize to 00
    CPX $08                     ; |
    BCC -                       ;/

    LDX $09                     ;\
    REP #$21                    ; |
    LDA $00                     ; | store retrieved extra bytes in the extra byte tables
    STA !ow_sprite_extra_1,x    ; |
    LDA $02                     ; |
    STA !ow_sprite_extra_2,x    ;/

    BRA .sprite_load_loop

.end_spawning
    SEP #$20
    PLB
    if !sa1
        RTL
    else
        RTS
    endif

.extra_byte_ptr
    STA $00
    LDX $09
    TYA                         ;\
    CLC                         ; |
    ADC $6B                     ; |
    STA !ow_sprite_extra_1,x    ; | insert pointer to extra bytes:
    LDA $6D                     ; | now contained in the extra byte tables
    ADC #$0000                  ; |
    AND #$00FF                  ; |
    STA !ow_sprite_extra_2,x    ;/
    TYA                         ;\
    ADC $00                     ; | offset the next slots adequately
    TAY                         ;/
.done_extra
    JMP .sprite_load_loop

assert pc() <= $04F6F8|!bank

;---

if !vldc9_maxtile
    org $04F8A6|!bank
        incsrc "maxtile.asm"

    pad $058000|!bank
endif

org $04F76E|!bank
run_ow_sprite:
; Main handler
.main
    REP #$21
    if not(!bowsie_maxtile)
        LDA #!oam_start
        STA !ow_sprite_oam
        LDA #!oam_start_p
        STA !ow_sprite_oam_p
    endif

    LDX.b #!bowsie_ow_slots*2-2     ;\
-   LDA !ow_sprite_num,x            ; | Main loop.
    BEQ ..no_sprite                 ; | Call execute_ow_sprite for all sprites where
    LDA !ow_sprite_init,x           ; | !ow_sprite_num,x is not zero.
    BNE +                           ;/
    JSR execute_ow_sprite_init      ;\
    INC !ow_sprite_init,x           ; | Or, in case !ow_sprite_init,x is still zero, call execute_ow_sprite_init and then INC it.
    BRA ..no_sprite                 ;/

+   JSR execute_ow_sprite
..no_sprite
    DEX #2
    BPL -

    SEP #$20

.done_running
    ; We need to get ahead of UberASMTool's jump to $008494.
    ; This is a JMP. A JSR would return to $048413, which is PLB : RTL.
    ; So, we simply restore the PLB and either jump to MaxTile syncing the buffer or
    ; add the RTL directly.
    PLB
    if !vldc9_maxtile
        JML maxtile_sync_buffer
    else
        RTL
    endif

.transition
    PHB
    REP #$21

    LDX.b #!bowsie_ow_slots*2-2     ;\
-   LDA !ow_sprite_num,x            ; |
    BEQ ..no_sprite                 ; | Main loop.
    JSR execute_ow_sprite_init      ; | Call execute_ow_sprite for all sprites where !ow_sprite_num,x is not zero.   
    INC !ow_sprite_init,x           ; |
    LDA !ow_sprite_num,x            ; |
    JSR execute_ow_sprite           ;/
..no_sprite
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
    JSR run_ow_sprite_transition
    JSL $04D6E9|!bank
    JML $00A169|!bank
.sm
    JSL clear_ram
    PHX
    JSR custom_ow_sprite_load_main
    JSR run_ow_sprite_transition
    PLX
    LDA !mario_map,x
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

;---

assert pc() <= $04F882|!bank

;---

freedata
extra_byte_table:
    db $03                      ;   sprite 00 (fixes lm bug)
    incbin "extra_size.bin"

