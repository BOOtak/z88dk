#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
defl
END_ASM
$test.asm:1: error: identifier expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defl var
END_ASM
$test.asm:1: error: '=' expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defl var=
END_ASM
$test.asm:1: error: argument expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
var defl
END_ASM
$test.asm:1: error: argument expected
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
var defl var+1
defb var
var defl var+2
defb var
var defl var+3
defb var
defl var=var+4
defb var
END_ASM
line 2,"$test.asm"
defb+1

defb+1+2

defb+1+2+3

defb+1+2+3+4
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
prev defl 0
next macro lbl
lbl: defw prev
prev defl lbl
endm
next a1
next a2
END_ASM
line 6,"$test.asm"
a1:defw 0
a2:defw a1
END_I

unlink_testfiles;
done_testing;
