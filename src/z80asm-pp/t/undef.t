#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
#undef
END_ASM
$test.asm:1: error: identifier expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
#undef abc,
END_ASM
$test.asm:1: error: end of line expected
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
#undef abc
#define abc 123
abc
#undef abc
#define abc 456
abc
END_ASM
line 3,"$test.asm"
123


456
END_I

unlink_testfiles;
done_testing;
