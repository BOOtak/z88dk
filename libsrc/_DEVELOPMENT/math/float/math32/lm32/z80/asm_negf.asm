
; float _negf (float number) __z88dk_fastcall

SECTION code_clib
SECTION code_fp_math32

PUBLIC asm_negf, asm_dneg

EXTERN m32_fsneg_fastcall

    ; negate sccz80 floats
    ;
    ; enter : stack = ret
    ;          DEHL = sccz80_float number
    ;
    ; exit  :  DEHL = sccz80_float(-number)
    ;
    ; uses  : af, bc, de, hl

DEFC  asm_negf = m32_fsneg_fastcall             ; enter stack = ret
                                                ;        DEHL = d32_float
                                                ; return DEHL = d32_float

DEFC  asm_dneg = m32_fsneg_fastcall             ; enter stack = ret
                                                ;        DEHL = d32_float
                                                ; return DEHL = d32_float
