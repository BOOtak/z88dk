#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
defgroup
END_ASM
$test.asm:2: error: structure nesting
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defgroup {
END_ASM
$test.asm:2: error: structure nesting
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defgroup { start
END_ASM
$test.asm:2: error: structure nesting
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defgroup { start=asmpp }
END_ASM
$test.asm:1: error: constant expression expected
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1: defgroup { 
a1=1,a2=2
a3=3,
}
END_ASM
line 1,"$test.asm"
l1:
defc a1=1
line 2
defc a2=2
defc a3=3
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

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defgroup { 
line 1
line 1,"x.asm"
c_line 1
c_line 1,"x.c"
a2=2 
}
END_ASM
line 1,"x.asm"
c_line 1
c_line 1,"x.c"
defc a2=2
END_I

unlink_testfiles;
done_testing;
