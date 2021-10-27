#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
repti
END_ASM
$test.asm:1: error: identifier expected
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
repti var
END_ASM
$test.asm:1: error: ',' expected
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
repti var,
END_ASM
$test.asm:1: error: argument expected
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
repti var,"hello"
END_ASM
$test.asm:2: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
repti var,"hello"
repti var,"hello"
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

asmpp_ok("", "", "", <<END_ASM, <<END_I);
repti var,123,abc,"hello"
defb var
endr
END_ASM
line 3,"$test.asm"
defb 123
line 3
defb abc
line 3
defb 104,101,108,108,111
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
#define STR abc
repti var,STR
defb var
endr
END_ASM
line 4,"$test.asm"
defb abc
END_I

unlink_testfiles;
done_testing;
