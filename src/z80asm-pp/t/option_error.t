#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

capture_nok("./z88dk-z80asm-pp -x -y", <<END);
z88dk-z80asm-pp: error: invalid option: -x
END

unlink_testfiles;
done_testing;
