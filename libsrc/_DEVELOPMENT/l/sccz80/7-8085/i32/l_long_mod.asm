;       Z88 Small C+ Run Time Library
;       Long functions
;
;       feilipu 10/2021


SECTION code_clib
SECTION code_l_sccz80

PUBLIC  l_long_mod

EXTERN  l_long_div_u_0, l_long_neg_mhl

;remainder = primary % secondary
;enter with secondary (divisor) in dehl, primary (dividend | quotient) on stack
;exit with remainder in dehl

.l_long_mod
    ld a,d                      ;check for divide by zero
    or e
    or h
    or l                        ;clear Carry to quotient
    jr Z, divide_by_zero

    push    de                  ;put secondary (divisor) on stack
    push    hl

    ld      bc,0                ;establish remainder on stack
    push    bc
    push    bc

    ld      c,d                 ;sign of divisor

    ld      de,sp+13            ;sign of dividend
    ld      a,(de)
    ld      b,a

    push    bc                  ;save sign info

    ld      de,sp+12            ;dividend
    ex      de,hl
    or      a,a                 ;test sign of dividend
    call    M,l_long_neg_mhl    ;take absolute value of dividend

    ld      de,sp+6             ;divisor
    ex      de,hl
    ld      a,c                 ;sign of divisor
    or      a,a                 ;test sign of divisor
    call    M,l_long_neg_mhl    ;take absolute value of divisor

    call    l_long_div_u_0      ;unsigned division

    ;tidy up with remainder to dehl

    ; C standard requires that the result of division satisfy
    ; a = (a/b)*b + a%b
    ; remainder takes sign of the dividend

    pop     af                  ;restore sign info

    ld      de,sp+0             ;remainder
    ex      de,hl
    or      a,a                 ;test sign of dividend
    call    M,l_long_neg_mhl    ;negate remainder if dividend was negative

    ld      de,sp+8             ;get return from stack
    ld      hl,(de)
    ld      de,sp+12            ;place return on stack
    ld      (de),hl

    ld      de,sp+0             ;get remainder LSW
    ld      hl,(de)
    ld      bc,hl               ;remainder LSW

    ld      de,sp+2             ;get remainder MSW
    ld      hl,(de)

    ld      de,sp+12            ;point to return again
    ex      de,hl               ;remainder MSW <> return sp
    ld      sp,hl               ;remove stacked parameters

    ld      hl,bc               ;remainder LSW

    ret

.divide_by_zero
    pop     bc                  ;pop return
    pop     hl                  ;pop dividend
    pop     de
    push    bc                  ;replace return

    ld      de,0                ;return ZERO
    ld      hl,de

    ret
