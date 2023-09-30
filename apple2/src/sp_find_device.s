        .export     _sp_find_device
        .export     check_name

        .import     _sp_payload
        .import     _sp_status
        .import     pusha
        .import     return0

        .include    "macros.inc"
        .include    "zp.inc"

; int8_t sp_find_device(char *device_name);
;
; find the device number for the one matching given name
; return:
; errors are returned as negative values from the sp_status DIB call
; 0 = no errors, but no device found
; n = slot of given named device

; uses tmp7-tmp10
.proc _sp_find_device
        axinto  tmp9            ; the device name we're looking for

        ; find the device count - do we need to keep doing this?
        pusha   #$00            ; doubles up as both parameters
        jsr     _sp_status
        beq     :+
        
        ; convert to an error code by making negative
        eor     #$ff
        clc
        adc     #$01
        rts

:       lda     _sp_payload    ; number of devices in payload[0], 1 based
        sta     tmp8            ; device index, set to max (e.g. 6) initially

        mva     #$01, tmp7      ; this will be our device index to check

        ; get DIB for each device in turn looking for name
:       pusha   tmp7
        lda     #$03
        jsr     _sp_status
        bne     skip_next

        jsr     check_name
        beq     skip_next

        ; we found the device in slot tmp7
        ldx     #$00
        lda     tmp7
        rts

skip_next:
        ; keep looping until tmp7 (device index) is greater than device count
        inc     tmp7
        cmp     tmp8
        bcc     :-
        beq     :-

        ; didn't find device
        jmp     return0

.endproc

; tmp9/10 is pointer to device name we are comparing to.
.proc check_name
        ldx     #$00                    ; count of matching chars, and string length in match
        ldy     #$00

:       lda     (tmp9), y               ; device name searching for
        beq     end_string
        cmp     _sp_payload+5, y       ; payload[5+i]th character
        bne     not_found
        inx
        iny
        bne     :-

not_found:
        lda     #$00
        rts

end_string:
        ; if x > 0, we matched at least 1 char before hitting end of string, so it's a match
        ; simply move x into a and return, if it's 0 that's an error, otherwise it's strlen
        txa
        rts

.endproc