#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
macro
END_ASM
$test.asm:1: error: identifier expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
macro m1 aa,
END_ASM
$test.asm:1: error: identifier expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
macro m1 aa aa
END_ASM
$test.asm:1: error: end of line expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
macro m1
macro m2
END_ASM
$test.asm:2: error: structure nesting
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
m1 macro
m2 macro
END_ASM
$test.asm:2: error: structure nesting
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
endm
END_ASM
$test.asm:1: error: structure nesting
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
macro m1
endm
m1 macro
END_ASM
$test.asm:3: error: duplicate definition: m1
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
macro m1
l1: endm
m2 macro
END_ASM
$test.asm:3: error: structure nesting
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
macro m1
endm ,
END_ASM
$test.asm:2: error: end of line expected
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
macro m1
inc b
inc c
endm
m1 \\ m1
END_ASM
line 5,"$test.asm"
inc b
line 5
inc c
line 5
inc b
line 5
inc c
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
m1 macro
inc b
inc c
endm
m1 \\ m1
END_ASM
line 5,"$test.asm"
inc b
line 5
inc c
line 5
inc b
line 5
inc c
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
macro m1
inc b
inc c
endm
l1: m1 \\ m1
END_ASM
line 5,"$test.asm"
l1:
line 5
inc b
line 5
inc c
line 5
inc b
line 5
inc c
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
macro m1
inc b
inc c
endm
.l1 m1 \\ m1
END_ASM
line 5,"$test.asm"
l1:
line 5
inc b
line 5
inc c
line 5
inc b
line 5
inc c
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
macro m1 a1,a2
ld a1,a2
endm
m1 b,d
m1 c,e
END_ASM
line 4,"$test.asm"
ld b,d
ld c,e
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
macro m1 a1,a2
ld a1,a2
endm
l1: m1 b,d
.l2 m1 c,e
END_ASM
line 4,"$test.asm"
l1:
line 4
ld b,d
l2:
line 5
ld c,e
END_I

unlink_testfiles;
done_testing;
