#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
line
END_ASM
$test.asm:1: error: integer or symbol expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
c_line
END_ASM
$test.asm:1: error: integer or symbol expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
line 123:
END_ASM
$test.asm:1: error: end of line expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
c_line 123:
END_ASM
$test.asm:1: error: end of line expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
line 123,
END_ASM
$test.asm:1: error: string expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
c_line 123,
END_ASM
$test.asm:1: error: string expected
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
nop
line 5
ret
END_ASM
line 1,"$test.asm"
nop



ret
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
nop
line 5,"x.asm"
ret
END_ASM
line 1,"$test.asm"
nop
line 5,"x.asm"
ret
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
nop
c_line 5
ret
END_ASM
line 1,"$test.asm"
nop
c_line 5
ret
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
nop
c_line 5,"x.c"
ret
END_ASM
line 1,"$test.asm"
nop
c_line 5,"x.c"
ret
END_I

unlink_testfiles;
done_testing;
