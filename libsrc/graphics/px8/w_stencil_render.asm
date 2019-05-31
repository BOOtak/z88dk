;
;	z88dk GFX library for the PX8
;
;	Render the "stencil".
;	The dithered horizontal lines base their pattern on the Y coordinate
;	and on an 'intensity' parameter (0..11).
;	Basic concept by Rafael de Oliveira Jannone
;	
;	Machine code version by Stefano Bodrato, 22/4/2009
;
;	stencil_render(unsigned char *stencil, unsigned char intensity)
;

	INCLUDE	"graphics/grafix.inc"

	SECTION code_graphics
	PUBLIC	stencil_render
	PUBLIC	_stencil_render
	EXTERN	dither_pattern


	EXTERN    subcpu_call

;	
;	$Id: w_stencil_render.asm $
;

.stencil_exit
		pop	ix	
		ret
	

.stencil_render
._stencil_render
		push	ix
		ld	ix,4
		add	ix,sp

		ld	bc,maxy
		push	bc
.yloop		pop	bc
		dec	bc
		ld	a,b
		and	c
		cp 255
		jr	z,stencil_exit
		push	bc
		
		ld	d,b
		ld	e,c

		ld	l,(ix+2)	; stencil
		ld	h,(ix+3)

		add	hl,bc
		add	hl,bc
		ld	e,(hl)
		inc hl
		ld	d,(hl)
		dec hl
		;ex	(sp),hl

		ld	a,d			; check left side for current Y position..
		and	e
		cp	127
		jr	z,yloop		; ...loop if nothing to be drawn
		
		ld	bc,maxy*2
		add	hl,bc
		ld	a,(hl)
		inc hl
		ld	h,(hl)
		ld	l,a

		pop bc
		push bc

		push hl
		
		ld	a,(ix+0)	; intensity
		push de		; X1
		call	dither_pattern
		pop hl		; X1
		
		; align pattern byte to the left X coordinate
		ld	e,a
		ld	a,7
		and	l
		ld	d,a
		ld	a,e
shift:
		rlca
		dec d
		jr nz,shift
		
		; save the pattern byte
		ex	af,af

			ld	a,b
			ld	(y0coord),a
			ld	(y1coord),a
			ld	a,c
			ld	(y0coord+1),a
			ld	(y1coord+1),a

			ld	a,h
			ld	(x0coord),a
			ld	a,l
			ld	(x0coord+1),a

			pop hl
			
			ld	a,h
			ld	(x1coord),a
			ld	a,l
			ld	(x1coord+1),a
			
			; blank horizontal row to clean the background
			ld	hl,65535
			ld	(oper),hl
			xor	a		; 'res' mode
			ld	(draw_mode),a
			
			ld	hl,packet
			call subcpu_call

			
			; get the pattern byte
			ex	af,af
			
			ld	(oper),a
			ld	(oper+1),a
			
			; dithered horizontal row, same coordinates as above
			ld	a,2		; 'plot' mode
			ld	(draw_mode),a
			
			ld	hl,packet
			call subcpu_call
			
			
			jr	yloop
			
			




	SECTION	data_clib


packet:
	defw	sndpkt
	defw	12		; packet sz
	defw	oper	; packet addr expected back from the slave CPU (useless)
	defw	1		; size of the expected packet being received ('bytes'+1)


sndpkt:
	defb	$29		; slave CPU command to draw a line
x0coord:
	defb	0		; MSB
	defb	0		; LSB
y0coord:
	defb	0		; MSB
	defb	0		; LSB
x1coord:
	defb	0		; MSB
	defb	0		; LSB
y1coord:
	defb	0		; MSB
	defb	0		; LSB
oper:
	defb	255		; draw operation mask (used for dotted lines)
	defb	255
draw_mode:
	defb	0		; 0:res, 2:plot, 3:xor
