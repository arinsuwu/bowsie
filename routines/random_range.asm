; A = max+1 -> random int in range [0, max]
; (you aren't getting "negative" results with negative mod)

    PHA
    SEP #$20
    JSL $01ACF9|!bank
if !sa1
    LDA #$01      ; perform mod
    STA $2250
    REP #$20
    LDA $148D|!addr
    LSR
    STA $2251
    PLA
    INC
    STA $2253
    NOP
    LDA $2308
else            ;   fuck sanity
    REP #$20
    BPL +
    EOR #$FFFF
    INC
+   STA $4204
    PLA
    SEP #$20    ;   loool no 16-bit divisors
    STA $4206
    LDA ($00,s),y   ;
    LDA ($00,s),y   ;   waste 16 cycles
    NOP         ;
    REP #$20
    LDA $4216
endif
    RTL

