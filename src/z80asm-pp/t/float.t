#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

# floats
my $floats = <<'END';
defb 1.
defb -1.
defb 255.
defb -255.
defb 256.
defb -256.
defb 65535.
defb -65535.
defb 65536.
defb -65536.
defb 5.5
defb -5.5
defb 5.5e1
defb 5.e-1
END

for my $opt ("", "-float=ieee32") {
	asmpp_ok($opt, "", "", $floats, <<END);
line 1,"$test.asm"
defb 0,0,128,63
defb 0,0,128,191
defb 0,0,127,67
defb 0,0,127,195
defb 0,0,128,67
defb 0,0,128,195
defb 0,255,127,71
defb 0,255,127,199
defb 0,0,128,71
defb 0,0,128,199
defb 0,0,176,64
defb 0,0,176,192
defb 0,0,92,66
defb 0,0,0,63
END
}

asmpp_ok("-float=ieee64", "", "", $floats, <<END);
line 1,"$test.asm"
defb 0,0,0,0,0,0,240,63
defb 0,0,0,0,0,0,240,191
defb 0,0,0,0,0,224,111,64
defb 0,0,0,0,0,224,111,192
defb 0,0,0,0,0,0,112,64
defb 0,0,0,0,0,0,112,192
defb 0,0,0,0,224,255,239,64
defb 0,0,0,0,224,255,239,192
defb 0,0,0,0,0,0,240,64
defb 0,0,0,0,0,0,240,192
defb 0,0,0,0,0,0,22,64
defb 0,0,0,0,0,0,22,192
defb 0,0,0,0,0,128,75,64
defb 0,0,0,0,0,0,224,63
END

asmpp_ok("-float=zx81", "", "", $floats, <<END);
line 1,"$test.asm"
defb 129,0,0,0,0
defb 129,128,0,0,0
defb 136,127,0,0,0
defb 136,255,0,0,0
defb 137,0,0,0,0
defb 137,128,0,0,0
defb 144,127,255,0,0
defb 144,255,255,0,0
defb 145,0,0,0,0
defb 145,128,0,0,0
defb 131,48,0,0,0
defb 131,176,0,0,0
defb 134,92,0,0,0
defb 128,0,0,0,0
END

asmpp_ok("-float=zx", "", "", $floats, <<END);
line 1,"$test.asm"
defb 0,0,1,0,0
defb 0,255,255,255,0
defb 0,0,255,0,0
defb 0,255,1,255,0
defb 0,0,0,1,0
defb 0,255,0,255,0
defb 0,0,255,255,0
defb 0,255,1,0,0
defb 145,0,0,0,0
defb 145,128,0,0,0
defb 131,48,0,0,0
defb 131,176,0,0,0
defb 0,0,55,0,0
defb 128,0,0,0,0
END

unlink_testfiles;
done_testing;
