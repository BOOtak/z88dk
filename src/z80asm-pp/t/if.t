#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
if
END_ASM
$test.asm:1: error: integer or symbol expected
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
if 1,
END_ASM
$test.asm:1: error: end of line expected
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
elif 1
END_ASM
$test.asm:1: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
else
END_ASM
$test.asm:1: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
endif
END_ASM
$test.asm:1: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
if 1
else
else
endif
END_ASM
$test.asm:3: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
if 1
else 1
endif 1
END_ASM
$test.asm:2: error: end of line expected
$test.asm:3: error: end of line expected
$test.asm:4: error: structure nesting
z88dk-z80asm-pp found 3 errors.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
if 1
END_ASM
$test.asm:2: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
ifdef name
END_ASM
$test.asm:2: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
ifndef name
END_ASM
$test.asm:2: error: structure nesting
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
ifdef AA,
END_ASM
$test.asm:1: error: end of line expected
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
if 1
elifdef AA,
endif
END_ASM
$test.asm:2: error: end of line expected
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
if 1
elifndef AA,
endif
END_ASM
$test.asm:2: error: end of line expected
z88dk-z80asm-pp found 1 error.
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
if 1
  1
  if 1
    11
  else
    12
  endif
elif 1
  2
else
  3
endif
END_ASM
line 2,"$test.asm"
1

11
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
#define AA 1
if AA
  1
else
  0
endif
END_ASM
line 3,"$test.asm"
1
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
if AA
  1
else
  0
endif
END_ASM
line 4,"$test.asm"
0
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defc AA = 1
if AA
  1
else
  0
endif
END_ASM
line 1,"$test.asm"
defc AA=1

1
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defc AA = 0
if AA
  1
else
  0
endif
END_ASM
line 1,"$test.asm"
defc AA=0



0
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
if 1+AA
  1
else
  0
endif
END_ASM
line 4,"$test.asm"
0
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
ifdef AA
  1
elifdef BB
  2
else
  3
endif
END_ASM
line 6,"$test.asm"
3
END_I

asmpp_ok("-DAA", "", "", <<END_ASM, <<END_I);
ifdef AA
  1
elifdef BB
  2
else
  3
endif
END_ASM
line 2,"$test.asm"
1
END_I

asmpp_ok("-DBB", "", "", <<END_ASM, <<END_I);
ifdef AA
  1
elifdef BB
  2
else
  3
endif
END_ASM
line 4,"$test.asm"
2
END_I

asmpp_ok("-DBB", "", "", <<END_ASM, <<END_I);
defc BB=1
ifdef AA
  1
elifdef BB
  2
else
  3
endif
END_ASM
line 1,"$test.asm"
defc BB=1



2
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
if 1
1
elif 2
2
else
3
endif
END_ASM
line 2,"$test.asm"
1
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1 if 1
1
l2 elif 2
2
l3 else
3
l4 endif
END_ASM
line 1,"$test.asm"
l1:
1
l2:
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
.l1 if 1
1
.l2 elif 2
2
.l3 else
3
.l4 endif
END_ASM
line 1,"$test.asm"
l1:
1
l2:
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1: if 1
1
l2: elif 2
2
l3: else
3
l4: endif
END_ASM
line 1,"$test.asm"
l1:
1
l2:
END_I

asmpp_ok("-DAA", "", "", <<END_ASM, <<END_I);
l1 ifdef AA
1
l2 elifdef BB
2
l3 else
3
l4 endif
END_ASM
line 1,"$test.asm"
l1:
1
l2:
END_I

asmpp_ok("-DAA", "", "", <<END_ASM, <<END_I);
.l1 ifdef AA
1
.l2 elifdef BB
2
.l3 else
3
.l4 endif
END_ASM
line 1,"$test.asm"
l1:
1
l2:
END_I

asmpp_ok("-DAA", "", "", <<END_ASM, <<END_I);
l1: ifdef AA
1
l2: elifdef BB
2
l3: else
3
l4: endif
END_ASM
line 1,"$test.asm"
l1:
1
l2:
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1 ifndef AA
1
l2 elifndef BB
2
l3 else
3
l4 endif
END_ASM
line 1,"$test.asm"
l1:
1
l2:
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
.l1 ifndef AA
1
.l2 elifndef BB
2
.l3 else
3
.l4 endif
END_ASM
line 1,"$test.asm"
l1:
1
l2:
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1: ifndef AA
1
l2: elifndef BB
2
l3: else
3
l4: endif
END_ASM
line 1,"$test.asm"
l1:
1
l2:
END_I


unlink_testfiles;
done_testing;

__END__
