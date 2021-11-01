#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

path("$test.bin")->spew_raw(pack("C*", 0..255));
my $expected1 = "defb ".join(",", 0..63);
my $expected2 = "defb ".join(",", 64..127);
my $expected3 = "defb ".join(",", 128..191);
my $expected4 = "defb ".join(",", 192..255);

asmpp_nok("", <<END_ASM, <<END_ERR);
binary
END_ASM
$test.asm:1: error: string expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
incbin
END_ASM
$test.asm:1: error: string expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
binary "hello",
END_ASM
$test.asm:1: error: end of line expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
incbin "hello",
END_ASM
$test.asm:1: error: end of line expected
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
binary "$test.bin"
END_ASM
line 1,"$test.asm"
$expected1
line 1
$expected2
line 1
$expected3
line 1
$expected4
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
incbin "$test.bin"
END_ASM
line 1,"$test.asm"
$expected1
line 1
$expected2
line 1
$expected3
line 1
$expected4
END_I

path("$test.bin")->spew_raw("");

asmpp_ok("", "", "", <<END_ASM, <<END_I);
binary "$test.bin"
END_ASM
END_I

path("$test.bin")->spew_raw(pack("C*", 1..3));

asmpp_ok("", "", "", <<END_ASM, <<END_I);
binary "$test.bin"
END_ASM
line 1,"$test.asm"
defb 1,2,3
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
incbin "$test.bin"
END_ASM
line 1,"$test.asm"
defb 1,2,3
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
.l1 binary "$test.bin"
END_ASM
line 1,"$test.asm"
l1:
line 1
defb 1,2,3
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
.l1 incbin "$test.bin"
END_ASM
line 1,"$test.asm"
l1:
line 1
defb 1,2,3
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1: binary "$test.bin"
END_ASM
line 1,"$test.asm"
l1:
line 1
defb 1,2,3
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1: incbin "$test.bin"
END_ASM
line 1,"$test.asm"
l1:
line 1
defb 1,2,3
END_I

path("$test.dir")->mkpath;
path("$test.dir/test.bin")->spew_raw(pack("C*", 1..3));

asmpp_nok("", <<END_ASM, <<END_ERR);
binary "test.bin" ; comment
END_ASM
$test.asm:1: error: file not found: test.bin
END_ERR

asmpp_ok("-I$test.dir", "", "", <<END_ASM, <<END_I);
binary "test.bin" ; comment
END_ASM
line 1,"$test.asm"
defb 1,2,3
END_I

path("$test.dir")->remove_tree;
unlink_testfiles;
done_testing;
