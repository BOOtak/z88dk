#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
defc
END_ASM
$test.asm:1: error: identifier expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defc start
END_ASM
$test.asm:1: error: '=' expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defc start=
END_ASM
$test.asm:1: error: integer or symbol expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defc start=1+
END_ASM
$test.asm:1: error: integer or symbol expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defc start=1,
END_ASM
$test.asm:1: error: end of line expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defc start=1
defc start=1
END_ASM
$test.asm:2: error: duplicate definition: start
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
    defc c1=1
.l1 defc c2=2
l2: defc c3=3
l3  defc c4=4
END_ASM
line 1,"$test.asm"
defc c1=1
l1:
line 2
defc c2=2
l2:
line 3
defc c3=3
l3:
line 4
defc c4=4
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
c1  equ 1
.c2 equ 2
c3: equ 3
c4  = 4
.c5 = 5
c6: = 6
END_ASM
line 1,"$test.asm"
defc c1=1
defc c2=2
defc c3=3
defc c4=4
defc c5=5
defc c6=6
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
#define START 1
defc start=START
END_ASM
line 2,"$test.asm"
defc start=1
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
#define AA BB
#define BB AA
defc start=AA;comment
END_ASM
line 3,"$test.asm"
defc start=AA
END_I

unlink_testfiles;
done_testing;
