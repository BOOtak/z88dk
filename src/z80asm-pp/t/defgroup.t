#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
defgroup
END_ASM
$test.asm:2: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defgroup {
END_ASM
$test.asm:2: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defgroup { start
END_ASM
$test.asm:2: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defgroup { start=asmpp }
END_ASM
$test.asm:1: error: constant expression expected
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1 defgroup { a1,a2=2,a3 }
if a1
not in
endif
if a2
in
endif
END_ASM
line 1,"$test.asm"
l1:
line 1
defc a1=0
line 1
defc a2=2
line 1
defc a3=3




in
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1: defgroup { 
a2=2 
}
END_ASM
line 1,"$test.asm"
l1:
defc a2=2
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
.l1 defgroup { 
a2=2 
}
END_ASM
line 1,"$test.asm"
l1:
defc a2=2
END_I

unlink_testfiles;
done_testing;
