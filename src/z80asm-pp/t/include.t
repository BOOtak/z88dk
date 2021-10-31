#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

# file not found
unlink "$test.asm";
capture_nok("./z88dk-z80asm-pp ${test}.asm", <<END_ERR);
z88dk-z80asm-pp: error: file not found: $test.asm
END_ERR

# include syntax error
asmpp_nok("", <<END_ASM, <<END_ERR);
include
END_ASM
$test.asm:1: error: string expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
include 123
END_ASM
$test.asm:1: error: string expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
include "$test.inc" extra
END_ASM
$test.asm:1: error: end of line expected
END_ERR

# include file not found
unlink "$test.inc";
asmpp_nok("", <<END_ASM, <<END_ERR);
include "$test.inc" ; comment
END_ASM
$test.asm:1: error: file not found: $test.inc
END_ERR

# recursive include
asmpp_nok("", <<END_ASM, <<END_ERR);
include "$test.asm" ; comment
END_ASM
$test.asm:1: error: recursive include: $test.asm
END_ERR

# include
path("$test.inc")->spew(<<END_ASM);
	ld a,1
END_ASM
asmpp_ok("", "", "", <<END_ASM, <<END_I);
	xor a
	include "$test.inc"
	inc a
.l1	include "$test.inc"
l2:	include "$test.inc"
l3	include "$test.inc"
#include "$test.inc"
	dec a \\ include "$test.inc" \\ inc a \\ include \\
"$test.inc" ; comment
END_ASM
line 1,"$test.asm"
xor a
line 1,"$test.inc"
ld a,1
line 3,"$test.asm"
inc a
l1:
line 1,"$test.inc"
ld a,1
line 5,"$test.asm"
l2:
line 1,"$test.inc"
ld a,1
line 6,"$test.asm"
l3:
line 1,"$test.inc"
ld a,1
line 1
ld a,1
line 8,"$test.asm"
dec a
line 1,"$test.inc"
ld a,1
line 8,"$test.asm"
inc a
line 1,"$test.inc"
ld a,1
END_I

# include from directory
path("$test.dir")->mkpath;
path("$test.dir/test.inc")->spew(<<END_ASM);
	xor a
END_ASM

asmpp_nok("", <<END_ASM, <<END_ERR);
	include "test.inc" ; comment
END_ASM
$test.asm:1: error: file not found: test.inc
END_ERR

asmpp_ok("-I$test.dir", "", "", <<END_ASM, <<END_I);
	include "test.inc" ; comment
END_ASM
line 1,"$test.dir/test.inc"
xor a
END_I

# relative include
path("$test.dir/test.asm")->spew(<<END_ASM);
	ld a,1
	include "test.inc"
	ld a,2
END_ASM

capture_ok("./z88dk-z80asm-pp $test.dir/test.asm", "");
check_txt_file("$test.dir/test.i", <<END_I);
line 1,"$test.dir/test.asm"
ld a,1
line 1,"$test.dir/test.inc"
xor a
line 3,"$test.dir/test.asm"
ld a,2
END_I


path("$test.dir")->remove_tree;
unlink_testfiles;
done_testing;
