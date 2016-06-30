
dnl############################################################
dnl##        ZX_CRT_IF2.M4 - IF2 CARTRIDGE TARGET            ##
dnl############################################################
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;                zx spectrum if2 cartridge                  ;;
;;     generated by target/zx/startup/zx_crt_if2.m4          ;;
;;                                                           ;;
;;   16k ROM in 0-16k area, ram placement per pragmas        ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; CRT AND CLIB CONFIGURATION ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include "../crt_defaults.inc"
include "crt_target_defaults.inc"
include "../crt_rules.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; SET UP MEMORY MODEL ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include "memory_model.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; GLOBAL SYMBOLS ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include "../clib_constants.inc"
include "clib_target_constants.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; INSTANTIATE DRIVERS ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
dnl
dnl############################################################
dnl## LIST OF AVAILABLE DRIVERS WITH STATIC INSTANTIATORS #####
dnl############################################################
dnl
dnl## input terminals
dnl
dnl#include(../driver/terminal/zx_01_input_kbd_inkey.m4)dnl
dnl#include(../driver/terminal/zx_01_input_kbd_lastk.m4)dnl
dnl
dnl## output terminals
dnl
dnl#include(../driver/terminal/zx_01_output_char_32.m4)dnl
dnl#include(../driver/terminal/zx_01_output_char_32_tty_z88dk.m4)dnl
dnl#include(../driver/terminal/zx_01_output_char_64.m4)dnl
dnl#include(../driver/terminal/zx_01_output_char_64_tty_z88dk.m4)dnl
dnl#include(../driver/terminal/zx_01_output_fzx.m4)dnl
dnl#include(../driver/terminal/zx_01_output_fzx_tty_z88dk.m4)dnl
dnl
dnl## file dup
dnl
dnl#include(../../m4_file_dup.m4)dnl
dnl
dnl## empty fd slot
dnl
dnl#include(../../m4_file_absent.m4)dnl
dnl
dnl############################################################
dnl## INSTANTIATE DRIVERS #####################################
dnl############################################################

ifndef CRT_FONT

   PUBLIC CRT_FONT
   EXTERN _ff_ind_Termino
   defc CRT_FONT = _ff_ind_Termino

endif

include(../../clib_instantiate_begin.m4)

include(../driver/terminal/zx_01_input_kbd_inkey.m4)dnl
m4_zx_01_input_kbd_inkey(_stdin, __i_fcntl_fdstruct_1, 0x03b0, 64, 1, 500, 15)dnl

include(../driver/terminal/zx_01_output_fzx_tty_z88dk.m4)dnl
m4_zx_01_output_fzx_tty_z88dk(_stdout, 0x2370, 0, 0, 0, 32, 0, 24, 0, CRT_FONT, 56, 0, 56, 0, 256, 0, 192, 1, 0, 3, 0)dnl

include(../../m4_file_dup.m4)dnl
m4_file_dup(_stderr, 0x80, __i_fcntl_fdstruct_1)dnl

include(../../clib_instantiate_end.m4)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; STARTUP ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SECTION CODE

PUBLIC __Start, __Exit

EXTERN _main

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; PAGE ZERO ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

IF __crt_org_code = 0

   include "../crt_page_zero.inc"

ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; CRT INIT ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

__Start:

   di                          ; if warm reset

   ; set stack address
   
   IF __register_sp = -1
   
      ld sp,0
   
   ELSE
   
      ld sp,__register_sp
   
   ENDIF
   
   ; commandline
   
   IF __crt_enable_commandline
   
      ld hl,0
      push hl                  ; argv[argc] = NULL
      add hl,sp
      push hl                  ; argv[0] = ""
      dec hl
      dec hl                   ; hl = argv
      ld bc,1                  ; bc = argc = 1
      
      IF __SDCC | __SDCC_IX | __SDCC_IY

         push hl               ; argv
         push bc               ; argc
      
      ELSE
      
         push bc               ; argc
         push hl               ; argv

      ENDIF
   
   ENDIF

   ; initialize sections

   include "../clib_init_data.inc"
   include "../clib_init_bss.inc"

SECTION code_crt_init          ; user and library initialization
SECTION code_crt_main

   ; call user program
   
   call _main                  ; hl = return status

   ; run exit stack

   IF __clib_exit_stack_size > 0
   
      EXTERN asm_exit
      jp asm_exit              ; exit function jumps to __Exit
   
   ENDIF

__Exit:

SECTION code_crt_exit          ; user and library cleanup
SECTION code_crt_return

   ; close files
   
   include "../clib_close.inc"

   ; terminate
   
IF __crt_enable_restart

   ; restart the program
   
   jp __Start

ELSE

   ; loop forever
   
   jr ASMPC

ENDIF

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; RUNTIME VARS ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include "../clib_variables.inc"
include "clib_target_variables.inc"

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; CLIB STUBS ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include "../clib_stubs.inc"
