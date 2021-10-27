#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
rept
END_ASM
$test.asm:1: error: integer or symbol expected
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
rept 2!
END_ASM
$test.asm:1: error: end of line expected
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
rept 2
END_ASM
$test.asm:2: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
endr
END_ASM
$test.asm:1: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
rept 3
rept 3
END_ASM
$test.asm:2: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
rept asmpp
END_ASM
$test.asm:1: error: constant expression expected
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
rept 3
nop
endr
END_ASM
line 3,"$test.asm"
nop
line 3
nop
line 3
nop
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1 rept 3
nop
endr
END_ASM
line 1,"$test.asm"
l1:

nop
line 3
nop
line 3
nop
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1: rept 3
nop
endr
END_ASM
line 1,"$test.asm"
l1:

nop
line 3
nop
line 3
nop
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
.l1 rept 3
nop
endr
END_ASM
line 1,"$test.asm"
l1:

nop
line 3
nop
line 3
nop
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
#define COUNT 3
rept COUNT
nop
endr
END_ASM
line 4,"$test.asm"
nop
line 4
nop
line 4
nop
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defc COUNT = 3
rept COUNT
nop
endr
END_ASM
line 1,"$test.asm"
defc COUNT=3


nop
line 4
nop
line 4
nop
END_I

unlink_testfiles;
done_testing;
