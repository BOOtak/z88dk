; CALLER linkage for function pointers

SECTION code_clib
PUBLIC screensize
PUBLIC _screensize
EXTERN screensize_callee
EXTERN ASMDISP_SCREENSIZE_CALLEE

.screensize
._screensize

   pop af
   pop de
   pop bc
   push bc
   push de
   push af
   
   jp screensize_callee + ASMDISP_SCREENSIZE_CALLEE
