#!/usr/bin/env perl

BEGIN { use lib 't'; require 'testlib.pl'; }

capture_ok("./z88dk-z80asm-pp", <<END);
z80asm preprocessor (c) Paulo Custodio 2011-2021

Usage: z88dk-z80asm-pp [options] files...

  options:
  -Dvar[=value] define variable, default value is 1
  -Idir         apppend directory to include path
  -ucase        convert identifiers to upper case
  -float=type   use type when converting floats
                type: ieee32, ieee64, zx81, zx
  -v            verbose
END

unlink_testfiles;
done_testing;
