;===========================================================
; Custom Overworld Sprites: Lui's sprites on OW Revolution
;---------------------------
; Adapted by Erik
; This couldn't be done without resources/stuff made by:
; - Lui37, Medic et al.: original "custom ow sprites"
;   used in the ninth VLDC.
;   See https://smwc.me/1413938
; - yoshifanatic: author of OW Revolution
;   See https://smwc.me/t/128236
;===========================================================

assert read4($048000) == $524F4659, "This needs OW Revolution to work."

org $02A861                         ;   loads the actual OW sprites
    JML custom_ow_sprite_load

org read3($0480D2)                  ;   pointer for the start of the OW sprite code in OW Revolution
    JSR run_ow_sprite               ;\  process sprites
    NOP                             ;/  four bytes. *for now* we can afford to NOP this.

if read1($0EF30F) == $42
    org read3($0EF30C)+256          ;   enable 2 extra bytes (if the table exists only)
        for i = 0..256 : db $05 : endfor
endif

;---

org !owrev_bank_4_freespace         ;   as I said above, right now we can afford bank 4 freespace
; Sprite loader
custom_ow_sprite_load:
; this part is basically OWRev
	LDA [$CE],y                     ;\  restore code
	STA $05                         ;/
	LDA $0100|!addr                 ;\
	SEC                             ; |
	SBC #$0C                        ; | verify that we're actually on the OW
	CMP #$03                        ; |
	BCC .main                       ;/
    LDA $05
    JML $02A865|!bank

.main
    DEY
; actual custom code to load begins here
    PHX
if !sa1
    LDX.w #(!bowsie_ow_slots-1)*2
else
    LDX.b #(!bowsie_ow_slots-1)*2
endif
-   LDA.w !ow_sprite_num,x
    BEQ .free_slot
    DEX #2
    BPL -
    PLX
    JML $02A846|!bank

.free_slot
    DEY                             ;   get Y position
    LDA [$CE],y                     ;\
    PHA                             ; |
    AND #$F0                        ; | store Y position low byte
    STA !ow_sprite_y_pos,x          ; | 
    PLA                             ; |
    AND #$01                        ; | store Y position high byte
    ORA $0A                         ; |
    STA.w !ow_sprite_y_pos+1,x      ;/
    REP #$20
    LDA $00                         ;\  store X position (wow, if only Y pos were this simple!)
    STA !ow_sprite_x_pos,x          ;/
    STZ !ow_sprite_speed_x,x        ;\
    STZ !ow_sprite_speed_y,x        ; |
    STZ !ow_sprite_speed_z,x        ; |
    STZ !ow_sprite_z_pos,x          ; |
    STZ !ow_sprite_timer_1,x        ; |
    STZ !ow_sprite_timer_2,x        ; |
    STZ !ow_sprite_timer_3,x        ; |
    STZ !ow_sprite_misc_1,x         ; | clear sprite tables
    STZ !ow_sprite_misc_2,x         ; |
    STZ !ow_sprite_misc_3,x         ; |
    STZ !ow_sprite_misc_4,x         ; |
    STZ !ow_sprite_misc_5,x         ; |
    STZ !ow_sprite_speed_x_acc,x    ; |
    STZ !ow_sprite_speed_y_acc,x    ; |
    STZ !ow_sprite_speed_z_acc,x    ; |
    STZ !ow_sprite_init,x           ;/
    INY #3                          ;   get the first two extension bytes
    LDA [$CE],y                     ;\  store the extra byte. when using OWRev we actually have a word
    STA !ow_sprite_extra_bits,x     ;/
    DEY #2                          ;   correct this offset, for later
    SEP #$20
    LDA $05                         ;\  store the sprite number
    STA !ow_sprite_num,x            ;/
    PLX
    JML $02A846|!bank               ;   return (we're back pretty soon either way)

;---

; Actual handler
run_ow_sprite:
    PHB
    REP #$21
    LDA $9D                         ;\
    AND #$0002                      ; | OWRev uses bit 1 of $9D to skip sprite code completely
    BNE .skip_running_sprites       ;/
    LDA #!oam_start                 ;\
    STA !ow_sprite_oam              ; | set our start OAM slot values
    LDA #!oam_start_p               ; |
    STA !ow_sprite_oam_p            ;/

    LDX.b #(!bowsie_ow_slots-1)*2   ;\
-   LDA !ow_sprite_num,x            ; | Main loop.
    BEQ .no_sprite                  ; | Call execute_ow_sprite for all sprites where     
    LDA !ow_sprite_init,x           ; | !ow_sprite_num,x is not zero.
    BNE +                           ;/
    JSR execute_ow_sprite_init      ;\
    INC !ow_sprite_init,x           ; | Or, in case !ow_sprite_init,x is still zero,
    BRA .no_sprite                  ; | call execute_ow_sprite_init and then INC it.
+                                   ;/
    JSR execute_ow_sprite
.no_sprite
    DEX #2
    BPL -
.skip_running_sprites
    PLB
return:
    RTS

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
    TAX                             ;/

    LDA.l ow_sprite_main_ptrs-3,x   ;\ 
    STA $00                         ; | Get sprite main pointer in $00
    SEP #$20                        ; | Sprite number 00 is <end> so the table
    LDA.l ow_sprite_main_ptrs-1,x   ; | is actually 1 indexed (hence those subtractions)
    STA $02                         ;/

    PHA                             ;\ 
    PLB                             ; | Setup bank (value still in A)
    REP #$20                        ; | A in 16 bit
    TYX                             ;/

    PHK                             ;\
    PEA.w return-1                  ;/ workaround for JSL [$0000]
    JML.w [!dp]

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
    TAX                             ;/

    LDA.l ow_sprite_init_ptrs-3,x   ;\ 
    STA $00                         ; | Get sprite init pointer in $00
    SEP #$20                        ; | sprite number 00 is <end> so the table
    LDA.l ow_sprite_init_ptrs-1,x   ; | is actually 1 indexed (hence those subtractions)
    STA $02                         ;/
    
    PHA                             ;\ 
    PLB                             ; | Setup bank (value still in A)
    REP #$20                        ; | A in 16 bit
    TYX                             ;/
    PHK                             ;\
    PEA.w return-1                  ;/ workaround for JSL [$0000]
    JML.w [!dp]

;---

assert pc() <= $04EF3E

