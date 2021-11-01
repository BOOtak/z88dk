#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
local
END_ASM
$test.asm:1: error: identifier expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
local as!
END_ASM
$test.asm:1: error: end of line expected
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
local abc,def
abc: def
END_ASM
line 2,"$test.asm"
abc__1:def__2
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1: local abc
abc: ret
END_ASM
line 1,"$test.asm"
l1:
abc__1:ret
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
.l1 local abc
abc: ret
END_ASM
line 1,"$test.asm"
l1:
abc__1:ret
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
macro m1
local abc
abc:ret
endm
abc:ret
m1
m1
abc:ret
END_ASM
line 5,"$test.asm"
abc:ret
abc__1:ret
abc__2:ret
abc:ret
END_I

unlink_testfiles;
done_testing;
