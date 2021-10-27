#!/usr/bin/env perl

use Modern::Perl;
use Test::More;
use Path::Tiny;

use vars '$test';
$test = "test_".(($0 =~ s/\.t$//r) =~ s/[\.\/]/_/gr);

sub check_bin_file {
    my($got_file, $exp) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;
    
    run_ok("xxd -g1 $got_file > $got_file.hex");
    
    my $exp_file = ($got_file =~ s/\.\w+$/.exp/ir);
    path($exp_file)->spew_raw($exp);
    run_ok("xxd -g1 $exp_file > $exp_file.hex");
    
    run_ok("diff $exp_file.hex $got_file.hex");
}

sub check_txt_file {
    my($got_file, $exp) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;
    
    my $exp_file = ($got_file =~ s/\.\w+$/.exp/ir);
    path($exp_file)->spew($exp);
    
    run_ok("dos2unix -q $exp_file $got_file");
    run_ok("diff $exp_file $got_file");
}

sub asmpp_ok {
    my($options, $exp_out, $exp_err, $asm, $i) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;
    
    # save asm file
    my $asm_file = "${test}.asm";
    my $i_file = "${test}.i";
    path($asm_file)->spew($asm);
    unlink($i_file);
    
    # preprocess
    $options ||= "";
    run_ok("./z88dk-z80asm-pp $options ${test}.asm > ${test}.out 2> ${test}.err");
    check_txt_file($i_file, $i);
    check_txt_file("${test}.out", $exp_out) if $exp_out;
    check_txt_file("${test}.err", $exp_err) if $exp_err;
}

sub asmpp_nok {
    my($options, $asm, $exp_err) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;
    
    # save asm file
    my $asm_file = "${test}.asm";
    path($asm_file)->spew($asm);
    
    # preprocess
    $options ||= "";
    capture_nok("./z88dk-z80asm-pp $options ${test}.asm", $exp_err);
}

sub capture_ok {
    my($cmd, $exp_out) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;
    
    run_ok($cmd." > ${test}.out");
    check_txt_file("${test}.out", $exp_out);
}

sub capture_nok {
    my($cmd, $exp_err) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    run_nok($cmd." 2> ${test}.err");
    check_txt_file("${test}.err", $exp_err);
}

sub run_ok {
    my($cmd) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;
    ok 0==system($cmd), $cmd;
}

sub run_nok {
    my($cmd) = @_;
    local $Test::Builder::Level = $Test::Builder::Level + 1;
    ok 0!=system($cmd), $cmd;
}

sub unlink_testfiles {
    unlink(<${test}.*>) 
        if Test::More->builder->is_passing;
}

1;
