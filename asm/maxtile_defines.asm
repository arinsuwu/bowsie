includeonce

if !sa1
    ;   Labels for SA-1's MaxTile
    maxtile_get_slot                = $0084B0|!bank

    ;   Memory definitions for SA-1's MaxTile
    !maxtile_oam_buffer_index_1    #= $3100
    !maxtile_oam_buffer_index_2    #= !maxtile_oam_buffer_index_1+2

    !maxtile_ptr_pri_0             #= $410180
    !maxtile_ptr_pri_1             #= !maxtile_ptr_pri_0+$10
    !maxtile_ptr_pri_2             #= !maxtile_ptr_pri_1+$10
    !maxtile_ptr_pri_3             #= !maxtile_ptr_pri_2+$10

    !maxtile_mirror_pri_0          #= $6180
    !maxtile_mirror_pri_1          #= !maxtile_mirror_pri_0+$10
    !maxtile_mirror_pri_2          #= !maxtile_mirror_pri_1+$10
    !maxtile_mirror_pri_3          #= !maxtile_mirror_pri_2+$10

    !maxtile_oam_buffer            #= $400000
    !maxtile_tilesize_buffer       #= $400000

    !maxtile_tilesize_buffer_pri_0 #= $40B600
    !maxtile_tilesize_buffer_pri_1 #= !maxtile_tilesize_buffer_pri_0+$80
    !maxtile_tilesize_buffer_pri_2 #= !maxtile_tilesize_buffer_pri_1+$80
    !maxtile_tilesize_buffer_pri_3 #= !maxtile_tilesize_buffer_pri_2+$80

    !maxtile_oam_buffer_pri_0      #= !maxtile_tilesize_buffer_pri_3+$80
    !maxtile_oam_buffer_pri_1      #= !maxtile_oam_buffer_pri_0+$0200
    !maxtile_oam_buffer_pri_2      #= !maxtile_oam_buffer_pri_1+$0200
    !maxtile_oam_buffer_pri_3      #= !maxtile_oam_buffer_pri_2+$0200

    !vanilla_oam_buffer            #= $6200
    !vanilla_oam_tilesize_buffer   #= $6420

elseif !bowsie_owrev
    ;   Labels for yoshifanatic's MaxTile
    maxtile_get_slot                = $048162|!bank

    ;   Memory definitions for yoshifanatic's MaxTile
    !maxtile_oam_buffer_index_1    #= $0B01
    !maxtile_oam_buffer_index_2    #= !maxtile_oam_buffer_index_1+2

    !maxtile_ptr_pri_0             #= $0B05
    !maxtile_ptr_pri_1             #= !maxtile_ptr_pri_0+$10
    !maxtile_ptr_pri_2             #= !maxtile_ptr_pri_1+$10
    !maxtile_ptr_pri_3             #= !maxtile_ptr_pri_2+$10

    !maxtile_mirror_pri_0          #= $0B05
    !maxtile_mirror_pri_1          #= !maxtile_mirror_pri_0+$10
    !maxtile_mirror_pri_2          #= !maxtile_mirror_pri_1+$10
    !maxtile_mirror_pri_3          #= !maxtile_mirror_pri_2+$10

    !maxtile_oam_buffer            #= $7F9A7B
    !maxtile_tilesize_buffer       #= $7F9A7B

    !maxtile_tilesize_buffer_pri_0 #= $7F9A7B
    !maxtile_tilesize_buffer_pri_1 #= !maxtile_tilesize_buffer_pri_0+$80
    !maxtile_tilesize_buffer_pri_2 #= !maxtile_tilesize_buffer_pri_1+$80
    !maxtile_tilesize_buffer_pri_3 #= !maxtile_tilesize_buffer_pri_2+$80

    !maxtile_oam_buffer_pri_0      #= !maxtile_tilesize_buffer_pri_3+$80
    !maxtile_oam_buffer_pri_1      #= !maxtile_oam_buffer_pri_0+$0200
    !maxtile_oam_buffer_pri_2      #= !maxtile_oam_buffer_pri_1+$0200
    !maxtile_oam_buffer_pri_3      #= !maxtile_oam_buffer_pri_2+$0200

    !vanilla_oam_buffer            #= $0200
    !vanilla_oam_tilesize_buffer   #= $0420

else
    ;   Requirements
    if read1($04E6FA) != $96
        error "MaxTile requires you to disable the event path fade effect for speed reasons."
        print "Go to Overworld -> Extra Options... and enable 'Disable event path fade effect'."
    endif

        !maxtile_opse   = 0
    if read1($04862E) == $AD
        print "MaxTile detected OPSE. The patch won't touch any player related GFX."
        print "Please ask the maintainer to add MaxTile support."
        print "Proceed under your own risk!"

        !maxtile_opse   = 1
    endif

    ;   Labels
    maxtile_get_slot                = $04F8A6|!bank

    ;   Memory definitions for BOWSIE's MaxTile
    ;   Follows SA-1 conventions: 0 is highest priority, 3 is lowest.
    !maxtile_oam_buffer_index_1    #= $0EB1
    !maxtile_oam_buffer_index_2    #= !maxtile_oam_buffer_index_1+2

    !maxtile_ptr_pri_0             #= $0EB5
    !maxtile_ptr_pri_1             #= !maxtile_ptr_pri_0+$10
    !maxtile_ptr_pri_2             #= !maxtile_ptr_pri_1+$10
    !maxtile_ptr_pri_3             #= !maxtile_ptr_pri_2+$10

    !maxtile_mirror_pri_0          #= $0EB5
    !maxtile_mirror_pri_1          #= !maxtile_mirror_pri_0+$10
    !maxtile_mirror_pri_2          #= !maxtile_mirror_pri_1+$10
    !maxtile_mirror_pri_3          #= !maxtile_mirror_pri_2+$10

    !maxtile_oam_buffer            #= $7F9A7B
    !maxtile_tilesize_buffer       #= $7F9A7B

    !maxtile_tilesize_buffer_pri_0 #= $7F9A7B
    !maxtile_tilesize_buffer_pri_1 #= !maxtile_tilesize_buffer_pri_0+$80
    !maxtile_tilesize_buffer_pri_2 #= !maxtile_tilesize_buffer_pri_1+$80
    !maxtile_tilesize_buffer_pri_3 #= !maxtile_tilesize_buffer_pri_2+$80

    !maxtile_oam_buffer_pri_0      #= !maxtile_tilesize_buffer_pri_3+$80
    !maxtile_oam_buffer_pri_1      #= !maxtile_oam_buffer_pri_0+$0200
    !maxtile_oam_buffer_pri_2      #= !maxtile_oam_buffer_pri_1+$0200
    !maxtile_oam_buffer_pri_3      #= !maxtile_oam_buffer_pri_2+$0200

    !vanilla_oam_buffer            #= $0200
    !vanilla_oam_tilesize_buffer   #= $0420

    ;   MaxTile macros
    macro copy_priority(pointer, offset)
        LDA.l <pointer>+4
        TAX
        LDA.l <pointer>+0
        BMI ?+
        INX #<offset>
        MVP !vanilla_oam_buffer>>16,!maxtile_oam_buffer>>16
    ?+
    endmacro

    macro restrict_buffer(pointer)
        LDA.l <pointer>+$04
        SEC
        SBC.l <pointer>+$00
        CMP $00
        BCC ?+
        CLC
        LDA $00
    ?+
        DEC
        STA.l <pointer>+$00
        EOR #$FFFF
        ADC $00
        STA $00
    endmacro

endif


;---


