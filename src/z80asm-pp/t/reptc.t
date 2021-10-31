#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
reptc
END_ASM
$test.asm:1: error: identifier expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
reptc var
END_ASM
$test.asm:1: error: ',' expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
reptc var,
END_ASM
$test.asm:1: error: string expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
reptc var,hello!
END_ASM
$test.asm:1: error: end of line expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
reptc var,"hello"!
END_ASM
$test.asm:1: error: end of line expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
reptc var,"hello"
END_ASM
$test.asm:2: error: structure nesting
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
reptc var,"hello"
reptc var,"hello"
END_ASM
$test.asm:2: error: structure nesting
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
endr
END_ASM
$test.asm:1: error: structure nesting
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
reptc var,123
defb var
endr
END_ASM
line 3,"$test.asm"
defb 49
line 3
defb 50
line 3
defb 51
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
reptc var,abc
defb var
endr
END_ASM
line 3,"$test.asm"
defb 97
line 3
defb 98
line 3
defb 99
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
reptc var,"abc"
defb var
endr
END_ASM
line 3,"$test.asm"
defb 97
line 3
defb 98
line 3
defb 99
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
#define STR abc
reptc var,STR
defb var
endr
END_ASM
line 4,"$test.asm"
defb 97
line 4
defb 98
line 4
defb 99
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
#define STR 123
reptc var,STR
defb var
endr
END_ASM
line 4,"$test.asm"
defb 49
line 4
defb 50
line 4
defb 51
END_I

unlink_testfiles;
done_testing;
