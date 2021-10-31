#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
#defcont
END_ASM
$test.asm:1: error: structure nesting
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
#define pusha 	push bc
#defcont		push de 
#defcont		push hl
pusha
END_ASM
line 4,"$test.asm"
push bc
line 4
push de
line 4
push hl
END_I

unlink_testfiles;
done_testing;
