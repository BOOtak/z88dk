#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
#define
END_ASM
$test.asm:1: error: identifier expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
#define abc
#define abc
END_ASM
$test.asm:2: error: duplicate definition: abc
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
#define abc(a,b,a) a+b+a
END_ASM
$test.asm:1: error: duplicate definition: a
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
#define abc(a,b a+b
END_ASM
$test.asm:1: error: ')' or ',' expected
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
#define abc (123)
abc
abc 1,2
abc##456
END_ASM
line 2,"$test.asm"
(123)
(123)1,2
(123)456
END_I

asmpp_ok("", "", <<END_ERR, <<END_ASM, <<END_I);
$test.asm:2: warning: wrong number of macro arguments: abc
END_ERR
#define abc(a,b) a+b
abc
abc 1(2)3,4(5)6
abc 1(2)3,4(5)6)
abc(1(2)3,4(5)6)
END_ASM
line 2,"$test.asm"
+
1(2)3+4(5)6
1(2)3+4(5)6)
1(2)3+4(5)6
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
#define pusha \\
		push bc \\ \\
		push de \\ \\
		push hl
pusha
END_ASM
line 5,"$test.asm"
push bc
line 5
push de
line 5
push hl
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
#define COMMA ,
#define LOAD(a) ld a
LOAD(a COMMA b)
END_ASM
line 3,"$test.asm"
ld a,b
END_I

unlink_testfiles;
done_testing;
