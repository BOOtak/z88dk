#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

# numbers, strings, comments
asmpp_ok("", "", "", <<'END_IN', <<END_OUT);
	ld a, 19     ; comment
	ld a, 19d    ; comment
	ld a, 19D    ; comment
	ld a, 1fh    ; comment
	ld a, 1FH    ; comment
	ld a, $1F    ; comment
	ld a, 0x1F   ; comment
	ld a, 0X1F   ; comment
	ld a, 10b    ; comment
	ld a, 10B    ; comment
	ld a, %10    ; comment
	ld a, @10    ; comment
	ld a, 0b10   ; comment
	ld a, 0B10   ; comment
	ld a, %"#-"  ; comment
	ld a, @"#-"  ; comment
	ld a, '0'    ; comment
	ld a, '\a'   ; comment
	ld a, '\b'   ; comment
	ld a, '\e'   ; comment
	ld a, '\f'   ; comment
	ld a, '\n'   ; comment
	ld a, '\r'   ; comment
	ld a, '\t'   ; comment
	ld a, '\v'   ; comment
	ld a, '\1'   ; comment
	ld a, '\17'  ; comment
	ld a, '\107' ; comment
	ld a, '\x1f' ; comment
	ld a, '\x1F' ; comment
	defb ""
	defb "\a\b\e\f\n\r\t\v\1\17\107\x1f\x1F"
	defb "0123456789"
	defb "123",123,"456",456
	defb '1',123,'2',456
END_IN
line 1,"$test.asm"
ld a,19
ld a,19
ld a,19
ld a,31
ld a,31
ld a,31
ld a,31
ld a,31
ld a,2
ld a,2
ld a,2
ld a,2
ld a,2
ld a,2
ld a,2
ld a,2
ld a,48
ld a,7
ld a,8
ld a,27
ld a,12
ld a,10
ld a,13
ld a,9
ld a,11
ld a,1
ld a,15
ld a,71
ld a,31
ld a,31
defb""
defb 7,8,27,12,10,13,9,11,1,15,71,31,31
defb 48,49,50,51,52,53,54,55,56,57
defb 49,50,51,123,52,53,54,456
defb 49,123,50,456
END_OUT

# single-quoted character
asmpp_nok("", <<'END_IN', <<END_ERR);
	ld a, ''
END_IN
$test.asm:1: error: invalid single quoted character
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<'END_IN', <<END_ERR);
	ld a, ''
	ld a, '12'
	defb ``
END_IN
$test.asm:1: error: invalid single quoted character
$test.asm:2: error: invalid single quoted character
$test.asm:3: error: invalid character
z88dk-z80asm-pp found 3 errors.
END_ERR

# symbols
asmpp_ok("", "", "", <<'END_IN', <<END_OUT);
!!# #$$f$$g%%1%%2%%a&&&()***++,,--..//::<<< <= = ==!=<> >>> >=??[]^^^{}|||~~
END_IN
line 1,"$test.asm"
!!# #\$ 15\$\$g% 1%% 2%%a&& &()** *++,,--..//::<< < <= == ==!=!=>> > >=??[]^^ ^{}|| |~~
END_OUT

# concatenation
asmpp_ok("", "", "", <<'END_IN', <<END_OUT);
  ab  ##  cd
END_IN
line 1,"$test.asm"
abcd
END_OUT

# labels
asmpp_ok("", "", "", <<'END_IN', <<END_OUT);
.label .word
label:word:
END_IN
line 1,"$test.asm"
label:.word
label:word:
END_OUT

asmpp_ok("-ucase", "", "", <<'END_IN', <<END_OUT);
.label .word
label:word:
END_IN
line 1,"$test.asm"
LABEL:.WORD
LABEL:WORD:
END_OUT

unlink_testfiles;
done_testing;
