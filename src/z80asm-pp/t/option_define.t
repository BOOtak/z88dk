#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

capture_nok("./z88dk-z80asm-pp -D", <<END);
z88dk-z80asm-pp: error: invalid option: -D
END

asmpp_ok("-Dstart", "", "", <<END_ASM, <<END_I);
	ld a,start
END_ASM
line 1,"$test.asm"
ld a,1
END_I

asmpp_ok("-Dstart=0x10", "", "", <<END_ASM, <<END_I);
	ld a,start
END_ASM
line 1,"$test.asm"
ld a,16
END_I

asmpp_ok("-Daa=1+bb -Dbb=1+cc -Dcc=1", "", "", <<END_ASM, <<END_I);
	ld a,aa
END_ASM
line 1,"$test.asm"
ld a,1+1+1
END_I

asmpp_ok("-Daa=bb -Dbb=aa", "", "", <<END_ASM, <<END_I);
	ld a,aa
END_ASM
line 1,"$test.asm"
ld a,aa
END_I

unlink_testfiles;
done_testing;
