#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

# continuation lines
asmpp_ok("", "", "", <<'END_IN', <<END_OUT);
ld a,b\ld b,d\ld \
d,a
ret
END_IN
line 1,"$test.asm"
ld a,b
line 1
ld b,d
line 1
ld d,a

ret
END_OUT

unlink_testfiles;
done_testing;
