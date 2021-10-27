#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

path("$test.1.asm")->spew("");
path("$test.2.asm")->spew("");
path("$test.3.asm")->spew("");


capture_ok("./z88dk-z80asm-pp $test.1.asm $test.2.asm $test.3.asm", <<END);
END

capture_ok("./z88dk-z80asm-pp -v $test.1.asm $test.2.asm $test.3.asm", <<END);
Processing file: $test.1.asm
Processing file: $test.2.asm
Processing file: $test.3.asm
END

$ENV{Z88DK_Z80ASM_PP} = "-v";
capture_ok("./z88dk-z80asm-pp $test.1.asm $test.2.asm $test.3.asm", <<END);
Processing environment variable: Z88DK_Z80ASM_PP=-v
Processing file: $test.1.asm
Processing file: $test.2.asm
Processing file: $test.3.asm
END

unlink_testfiles;
done_testing;
