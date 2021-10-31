#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
exitm
END_ASM
$test.asm:1: error: structure nesting
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
macro m1
exitm !
endm
m1
END_ASM
$test.asm:4: error: end of line expected
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
macro m1
inc a
exitm
inc b
endm
m1
END_ASM
line 6,"$test.asm"
inc a
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
macro m1
inc a
if 0
exitm
endif
inc b
endm
m1
END_ASM
line 8,"$test.asm"
inc a
line 8
inc b
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
macro m1
inc a
if 1
exitm
endif
inc b
endm
m1
END_ASM
line 8,"$test.asm"
inc a
END_I

unlink_testfiles;
done_testing;
