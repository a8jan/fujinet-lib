        .export         _clock_set_tz

        .import         _fn_error
        .import         _memcpy
        .import         _sp_control
        .import         _sp_get_clock_id
        .import         _sp_payload
        .import         _strlen

        .import         incsp2
        .import         pusha
        .import         pushax
        .import         return1

        .include        "macros.inc"
        .include        "zp.inc"

; uint8_t clock_set_tz(char *tz);
_clock_set_tz:        
        axinto  tmp_tz_ptr              ; save the tz

        ; get the device id of the clock, this is stored in _sp_clock_id, but also returned so we can check if it failed (0 is error)
        jsr     _sp_get_clock_id
        bne     got_id

        ; no clock found, return 1 as an error (FN_ERR_IO_ERROR)
        jmp     return1

got_id:
        jsr     pusha                   ; store the destination device for call to sp_control

        ; copy timezone string into sp_payload, including the null terminator
        pushax  #(_sp_payload + 2)
        pushax  tmp_tz_ptr
        jsr     _strlen

        clc
        adc     #$01
        bcc     :+
        inx

:       sta     _sp_payload + 0
        stx     _sp_payload + 1
        jsr     _memcpy

        ; destination was stored on s/w stack already
        lda     #'T'                    ; set 'T'imezone
        jsr     _sp_control

        ; convert to fujinet error
        jmp     _fn_error


.bss
tmp_tz_ptr:     .res 2