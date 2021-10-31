#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

asmpp_nok("", <<END_ASM, <<END_ERR);
defvars
END_ASM
$test.asm:2: error: structure nesting
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defvars asmpp
END_ASM
$test.asm:1: error: constant expression expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defvars .
END_ASM
$test.asm:1: error: integer or symbol expected
END_ERR

asmpp_nok("", <<END_ASM, <<END_ERR);
defvars 0 {
END_ASM
$test.asm:2: error: structure nesting
END_ERR

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defvars 0 {
a1 ds.b 1
a2 ds.b 1
}
END_ASM
line 2,"$test.asm"
defc a1=0
defc a2=1
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1 defvars 0 {
a1 ds.b 1
a2 ds.b 1
}
END_ASM
line 1,"$test.asm"
l1:
defc a1=0
defc a2=1
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
.l1 defvars 0 {
a1 ds.b 1
a2 ds.b 1
}
END_ASM
line 1,"$test.asm"
l1:
defc a1=0
defc a2=1
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
l1: defvars 0 {
a1 ds.b 1
a2 ds.b 1
}
END_ASM
line 1,"$test.asm"
l1:
defc a1=0
defc a2=1
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defvars 0 {
   ds.b 1
a2 ds.b 1
}
END_ASM
line 3,"$test.asm"
defc a2=1
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defvars 0 {
   ds.w 1
a2 ds.b 1
}
END_ASM
line 3,"$test.asm"
defc a2=2
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defvars 0 {
   ds.p 1
a2 ds.b 1
}
END_ASM
line 3,"$test.asm"
defc a2=3
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defvars 0 {
   ds.q 1
a2 ds.b 1
}
END_ASM
line 3,"$test.asm"
defc a2=4
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defvars 0 {
   ds.b 2
a2 ds.b 1
}
END_ASM
line 3,"$test.asm"
defc a2=2
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defvars 0 {
   ds.w 2
a2 ds.b 1
}
END_ASM
line 3,"$test.asm"
defc a2=4
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defvars 0 {
   ds.p 2
a2 ds.b 1
}
END_ASM
line 3,"$test.asm"
defc a2=6
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defvars 0 {
   ds.q 2
a2 ds.b 1
}
END_ASM
line 3,"$test.asm"
defc a2=8
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defvars 100 {
a1 ds.b 1
a2 ds.b 1
}
defvars 0 {
b1 ds.b 1
b2 ds.b 1
}
defvars -1 {
a3 ds.b 1
a4 ds.b 1
}
END_ASM
line 2,"$test.asm"
defc a1=100
defc a2=101


defc b1=0
defc b2=1


defc a3=102
defc a4=103
END_I

asmpp_ok("", "", "", <<END_ASM, <<END_I);
defvars 100 {
line 1
line 1,"x.asm"
c_line 1
c_line 1,"x.c"
a1 ds.b 1
}
END_ASM
line 1,"x.asm"
c_line 1
c_line 1,"x.c"
defc a1=100
END_I

unlink_testfiles;
done_testing;
