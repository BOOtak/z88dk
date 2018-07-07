;       Bandai RX78 CRT0
;


        MODULE  rx78_crt0

        
;--------
; Include zcc_opt.def to find out some info
;--------

        defc    crt0 = 1
        INCLUDE "zcc_opt.def"

;--------
; Some scope definitions
;--------

        EXTERN    _main           ; main() is always external to crt0 code

        PUBLIC    cleanup         ; jp'd to by exit()
        PUBLIC    l_dcal          ; jp(hl)



;--------
; Set an origin for the application (-zorg=) default to 32768
;--------

	defc	CRT_ORG_CODE = 0x2000
	defc	CRT_ORG_BSS = 0xb000

	defc	CONSOLE_ROWS = 24
	defc	CONSOLE_COLUMNS = 23

;        defc    TAR__fputc_cons_generic = 1
        defc    TAR__clib_exit_stack_size = 0
        defc    TAR__register_sp = 0xebff
	defc	__CPU_CLOCK = 4090909
        INCLUDE "crt/classic/crt_rules.inc"

        org     CRT_ORG_CODE

	defb	0x01
	defb	0
	defb	0

start:
	di
        INCLUDE "crt/classic/crt_init_sp.asm"
        INCLUDE "crt/classic/crt_init_atexit.asm"

	call	crt0_init_bss
	ei

; Optional definition for auto MALLOC init
; it assumes we have free space between the end of 
; the compiled program and the stack pointer
	IF DEFINED_USING_amalloc
		INCLUDE "crt/classic/crt_init_amalloc.asm"
	ENDIF


        call    _main           ; Call user program
cleanup:
;
;       Deallocate memory which has been allocated here!
;
        push    hl				; return code

IF CRT_ENABLE_STDIO = 1
        EXTERN     closeall
        call    closeall
ENDIF


cleanup_exit:

        pop     bc				; return code (still not sure it is teh right one !)

end:	jp	0


l_dcal: jp      (hl)            ;Used for function pointer calls


        defm    "Small C+ RX78"   ;Unnecessary file signature
        defb    0
        INCLUDE "crt/classic/crt_runtime_selection.asm"

        defc    __crt_org_bss = CRT_ORG_BSS
        IF DEFINED_CRT_MODEL
            defc __crt_model = CRT_MODEL
        ELSE
            defc __crt_model = 1
        ENDIF
        INCLUDE "crt/classic/crt_section.asm"



