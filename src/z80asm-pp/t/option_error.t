#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

capture_nok("./z88dk-z80asm-pp -x -y", <<END);
z88dk-z80asm-pp: error: invalid option: -x
z88dk-z80asm-pp found 1 error.
END

unlink_testfiles;
done_testing;
