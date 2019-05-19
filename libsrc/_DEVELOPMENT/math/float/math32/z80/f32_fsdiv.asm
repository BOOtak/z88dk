;
;  feilipu, 2019 May
;
;  This Source Code Form is subject to the terms of the Mozilla Public
;  License, v. 2.0. If a copy of the MPL was not distributed with this
;  file, You can obtain one at http://mozilla.org/MPL/2.0/.
;
;-------------------------------------------------------------------------
; m32_fsdiv - z80, z180, z80-zxn floating point divide
;-------------------------------------------------------------------------
; R = N/D = N * 1/D
;
; We calculate division of two floating point number by refining an
; estimate of the reciprocal of y using newton iterations.  Each iteration
; gives us just less than twice previous precision in binary bits (2n-2).
;
; Division is then done by multiplying by the reciprocal of the divisor.
;
;-------------------------------------------------------------------------
; m32_fsinv - z80, z180, z80-zxn floating point inversion (reciprocal)
;-------------------------------------------------------------------------
;
; Computes R the quotient of N and D
;
; Express D as M × 2e where 1 ≤ M < 2 (standard floating point representation)
;
; D' := D / 2e+1   // scale between 0.5 and 1
; N' := N / 2e+1
; X := 48/17 − 32/17 × D'   // precompute constants with same precision as D
;
; while
;    X := X + X × (1 - D' × X)
; return N' × X
;
;-------------------------------------------------------------------------
; FIXME clocks
;-------------------------------------------------------------------------

SECTION code_clib
SECTION code_math

EXTERN m32_fsmul, m32_fsmul_callee

EXTERN m32_fsmul32x32, m32_fsmul24x32, m32_fsadd32x32, m32_fsadd24x32
EXTERN m32_fsmin_fastcall, m32_fsmax_fastcall

PUBLIC m32_fsdiv, m32_fsdiv_callee
PUBLIC m32_fsinv_fastcall


.m32_fsdiv
    call m32_fsinv_fastcall
    jp m32_fsmul


.m32_fsdiv_callee
    call m32_fsinv_fastcall
    jp m32_fsmul_callee


.m32_fsinv_fastcall
    ex de,hl                    ; DEHL -> HLDE

    add hl,hl                   ; sign into C
    ld a,h
    push af                     ; save exponent and sign in C

    or a                        ; divide by zero?
    jp Z,m32_fsmax_fastcall

    ld h,0bfh                   ; scale to - 0.5 <= D' < 1.0
    srl l
    ex de,hl                    ; D' in DEHL

    push de                     ; D' msw on stack for D[2] calculation
    push hl                     ; D' lsw on stack for D[2] calculation
    push de                     ; D' msw on stack for D[1] calculation
    push hl                     ; D' lsw on stack for D[1] calculation

    sla e
    sla d                       ; get D' full exponent into d
    rr c                        ; put sign in c
    scf
    rr e                        ; put implicit bit for mantissa in ehl
    ld b,d                      ; unpack IEEE to expanded float 32-bit mantissa
    ld d,e
    ld e,h
    ld h,l
    ld l,0
;-------------------------------;
                                ; X = 48/17 − 32/17 × D'
    exx
    ld bc,04034h
    push bc
    ld bc,0B4B5h
    push bc                
    ld bc,03FF0h
    push bc
    ld bc,0F0F1h
    push bc
    exx
    call m32_fsmul24x32         ; (float) 32/17 × D'
    call m32_fsadd24x32         ; X = 48/17 − 32/17 × D'

;-------------------------------;
                                ; X := X + X × (1 - D' × X)
    exx
    pop hl                      ; D' for D[1] calculation
    pop de
    exx
    push bc                     ; X
    push de
    push hl
    push bc                     ; X
    push de
    push hl
    exx
    ld bc,03f80h                ; 1.0
    push bc
    ld bc,0
    push bc
    push de                      ; D' for D[1] calculation
    push hl
    exx
    call m32_fsmul24x32         ; (float) - D' × X
    call m32_fsadd24x32         ; (float) 1 - D' × X
    call m32_fsmul32x32         ; (float) X × (1 - D' × X)
    call m32_fsadd32x32         ; (float) X + X × (1 - D' × X)

;-------------------------------;
                                ; X := X + X × (1 - D' × X)
    exx
    pop hl                      ; D' for D[2] calculation
    pop de
    exx
    push bc                     ; X
    push de
    push hl
    push bc                     ; X
    push de
    push hl
    exx
    ld bc,03f80h                ; 1.0
    push bc
    ld bc,0
    push bc
    push de                      ; D' for D[2] calculation
    push hl
    exx
    call m32_fsmul24x32         ; (float) - D' × X
    call m32_fsadd24x32         ; (float) 1 - D' × X
    call m32_fsmul32x32         ; (float) X × (1 - D' × X)
    call m32_fsadd32x32         ; (float) X + X × (1 - D' × X)

;-------------------------------;

    ld a,l                      ; round using digi norm's method
    or a
    jr Z,fd0
    set 0,h

.fd0

    ld l,h                      ; align 32-bit mantissa to IEEE 24-bit mantissa
    ld h,e
    ld e,d

    pop af                      ; recover D exponent and sign in C
    rr c                        ; save sign in c
    sub a,07fh                  ; calculate new exponent for 1/D
    neg
    add a,07eh   

    sla e
    sla c                       ; recover sign from c
    rra
    rr e
    ld d,a
    ret                         ; return IEEE DEHL

