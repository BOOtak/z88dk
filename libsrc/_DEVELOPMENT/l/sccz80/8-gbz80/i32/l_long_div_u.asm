;       Z88 Small C+ Run Time Library
;       Long functions
;
;       feilipu 10/2021


SECTION code_clib
SECTION code_l_sccz80

PUBLIC  l_long_div_u, l_long_div_u_0

EXTERN  l_long_rl_mhl, l_long_cp_mhl, l_long_sub_mhl

;quotient = primary / secondary
;enter with secondary (divisor) in dehl, primary (dividend | quotient) on stack
;exit with quotient in dehl

.l_long_div_u
    ld a,d                      ;check for divide by zero
    or e
    or h
    or l                        ;clear Carry to quotient
    jp Z, divide_by_zero

    push    de                  ;put secondary (divisor) on stack
    push    hl

    ld      bc,0                ;establish remainder on stack
    push    bc
    push    bc

    push    bc                  ;save null sign info

    call    l_long_div_u_0      ;unsigned division

    ;tidy up with quotient to dehl

    ld      hl,sp+14            ;get quotient MSW
    ld      c,(hl)
    inc     hl
    ld      b,(hl)

    ld      hl,sp+10            ;get return from stack
    ld      e,(hl)
    inc     hl
    ld      d,(hl)
    ld      hl,sp+14            ;place return on stack
    ld      (hl),e
    inc     hl
    ld      (hl),d

    ld      hl,sp+2             ;get remainder LSB (for __printf_number)
    ld      a,(hl)

    ld      hl,sp+12            ;get quotient LSW
    ld      e,(hl)
    inc     hl
    ld      d,(hl)

;   ld      hl,sp+14
    inc     hl                  ;point to return again
    ld      sp,hl               ;remove stacked parameters

    ex      de,hl               ;quotient LSW
    ld      de,bc               ;quotient MSW

    ret


.l_long_div_u_0
    ld      b,32                ;set up div_loop counter

.div_loop
    rra                         ;save Carry
    ld      hl,sp+4             ;remainder (hl)
    ex      de,hl
    ld      hl,sp+14            ;dividend (hl)
    rla                         ;restore Carry

IF 1
    call    l_long_rl_mhl       ;rotate left dividend + quotient Carry
    ex      de,hl
    call    l_long_rl_mhl       ;rotate left remainder + dividend Carry

    ld      hl,sp+4             ;compare (remainder - divisor)
    ex      de,hl
    ld      hl,sp+8
    call    l_long_cp_mhl

    jp      C,skip_subtract     ;skip if remainder < divisor

    ld      hl,sp+4             ;subtract (remainder - divisor)
    ex      de,hl
    ld      hl,sp+8
    call    l_long_sub_mhl

ELSE
    ;rotate left dividend + quotient Carry
    ld      a,(hl)
    rla
    ld      (hl+),a
    ld      a,(hl)
    rla
    ld      (hl+),a
    ld      a,(hl)
    rla
    ld      (hl+),a
    ld      a,(hl)
    rla
    ld      (hl),a

    ex      de,hl

    ;rotate left remainder + dividend Carry
    ld      a,(hl)
    rla
    ld      (hl+),a
    ld      a,(hl)
    rla
    ld      (hl+),a
    ld      a,(hl)
    rla
    ld      (hl+),a
    ld      a,(hl)
    rla
    ld      (hl),a

    ;compare (remainder - divisor)
    ld      hl,sp+4
    ex      de,hl
    ld      hl,sp+8

    ld      a,(de)
    sub     a,(hl)
    inc     de
    inc     hl
    ld      a,(de)
    sbc     a,(hl)
    inc     de
    inc     hl
    ld      a,(de)
    sbc     a,(hl)
    inc     de
    inc     hl
    ld      a,(de)
    sbc     a,(hl)

    jp      C,skip_subtract     ;skip if remainder < divisor
    
    ;subtract (remainder - divisor)
    ld      hl,sp+4 
    ex      de,hl
    ld      hl,sp+8

    ld      a,(de)
    sub     a,(hl)
    ld      (de),a
    inc     de
    inc     hl
    ld      a,(de)
    sbc     a,(hl)
    ld      (de),a
    inc     de
    inc     hl
    ld      a,(de)
    sbc     a,(hl)
    ld      (de),a
    inc     de
    inc     hl
    ld      a,(de)
    sbc     a,(hl)
    ld      (de),a
ENDIF

.skip_subtract
    ccf                         ;prepare Carry for quotient

    dec     b
    jp      NZ,div_loop

    rra                         ;save Carry
    ld      hl,sp+14
    rla                         ;restore Carry
    call    l_long_rl_mhl       ;rotate left quotient Carry

    ret


.divide_by_zero
    pop     bc                  ;pop return
    pop     hl                  ;pop dividend
    pop     de
    push    bc                  ;replace return

    ld      de,$ffff            ;return ULONG_MAX
    ld      hl,de

    ret
