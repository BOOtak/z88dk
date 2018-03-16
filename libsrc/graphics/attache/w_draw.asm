;
;       Z88 Graphics Functions - Small C+ stubs
;
;       Written around the Interlogic Standard Library
;
;       Stubs Written by D Morris - 30/9/98
;
;       Wide resolution (WORD based parameters) version by Stefano Bodrato
;
;	$Id: w_draw.asm $
;


        SECTION code_clib
                PUBLIC    draw
                PUBLIC    _draw
                EXTERN     swapgfxbk
                EXTERN     swapgfxbk1
                EXTERN	__graphics_end

                EXTERN     w_line_r_fast
                EXTERN     w_plotpixel


.draw
._draw
		push	ix
		ld	ix,4
		add	ix,sp
		ld	l,(ix+6)
		ld	h,(ix+7)
		ld	e,(ix+4)
		ld	d,(ix+5)
		
		push hl
		push de
		push ix

		;call    swapgfxbk
		call	w_plotpixel
		;call    swapgfxbk1

		pop ix
		ld	l,(ix+0)
		ld	h,(ix+1)
		pop bc
		or a
		sbc hl,bc
		ex de,hl
		
		ld	l,(ix+2)
		ld	h,(ix+3)
		pop bc
		or a
		sbc hl,bc

		;call    swapgfxbk
		;ld      ix,w_plotpixel
		call    w_line_r_fast
		jp      __graphics_end

