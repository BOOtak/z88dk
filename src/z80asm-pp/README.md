# Preprocessor directives

## Continuation lines

Lines ending in a backslash are concatenated with the next line.

Lines containing a backslash are split in logical lines.

The example below has two lines:

```
line \
1 \ line 2
```

## Floating point constants

Floating point constants are accepted in `DEFB` statements and are  translated into the 
list of byte values representing the constant in the current format defined by command 
line option `-float=xxx`. The following formats are accepted:

| Option | Description
|---     |---
| -float=ieee32 | 32-bit IEEE 754 (default)
| -float=ieee64 | 64-bit IEEE 754 
| -float=zx81   | 40-bit ZX-81 format
| -float=zx     | 40-bit ZX-Spectrum format

Example:
```
defb 1.0        <=> defb 0,0,128,63
```

## INCLUDE

```
#include "string"
[label] include "string"
```

Include the file in the current parse location.

## #DEFINE

```
#define macro text
```
or
```
#define macro(arg1,...) text
```
Define a macro without (first case) or with (second case) arguments.

The macro is called with the arguments inside parens, or up to the end of the line.
```
#define macro(a,b) a+b
macro(1,2)      => expands to 1+2
macro 1,2       => expands to 1+2
```

## #DEFCONT
```
#define pusha    push bc
#defcont         push de
#defcont         push hl
```
Continue previous `#define` to create multi-line macros. Inserts a newline before the given text.

## #UNDEF

```
#undef macro
```
Undefine a macro if it exists.

## DEFC|EQU|=
```
[label] defc name = expression
name    equ         expression
name    =           expression
```
Define a name as the result of evaluating the expression.

## IF ... ELIF ... ELSE ... ENDIF
```
[label] if expression
        ...
[label] elif expression
        ...
[label] else
        ...
[label] endif
```
Evaluate the expressions and assemble the blocks depending on the result. Undefined values
or values depending on the linker are considered false. The `ELIF` and `ELSE` clauses are optional.

## IFDEF|IFNDEF name ... ELIFDEF|ELIFNDEF name ... ELSE ... ENDIF
```
[label] ifdef|ifndef name
        ...
[label] elifdef|elifndef name
        ...
[label] else
        ...
[label] endif
```
Check if the given symbols exist/don\'t exist and assemble the blocks depending on the result.

## MACRO ... ENDM
```
name macro param,...
     ...
     endm
```
or
```
     macro name param,...
     ...
     endm
```
Define a macro with the given name and parameters, which is expanded when name
appears in the place of an opcode or directive.

## LOCAL
```
local name,...
```
When used inside a macro, declares the symbols that should have different 
names in each macro instance.

## EXITM
```
macro name
...
exitm
...
endm
```
Exits the expansion of the current macro.

## REPT ... ENDR
```
rept expression
...
endr
```
Repeat the text between `rept` and `endr` N times, where N is the result
of evaluating the constant expression.

## REPTC ... ENDR
```
reptc var,ident|"string"
...
endr
```
Repeat the text between `rept` and `endr` once for each character of the
given identifier or string, setting each time the `var` variable to the
character code.

## REPTI ... ENDR
```
repti var,args...
...
endr
```
Repeat the text between `rept` and `endr` once for each item of the 
given arguments, setting each time the `var` variable to the argument.

## DEFL
```
name defl expression
```
or
```
defl name = expression
```
Define or redefine a symbol. The expression can refer to the same name
to allow a symbol to refer to its previous value.

## DEFGROUP { name[=expression], ... }

Define a set of symbols like the C enum statement. The symbol values
start at zero and are incremented for each consecutive symbol, or a
new value can be defined with the optional constant expression, e.g.
```
defgroup  
{  
   sym_null  
   sym_ten = 10, sym_eleven, sym_twelve  
}
```

## DEFVARS origin { [name] size_id count, ... }

Define a set of symbols like the C struct statement.

origin is a constant expression that defines the base address of the 
structure elements. If origin is -1, then the structure elements follow 
the values of the previous DEFVARS statement that had an origin 
different from zero. The origin=0 structures are not continued 
with -1.

name is the symbol that is defined with the structure offset, 
i.e. origin for the first symbol, and incremented by the size 
of each element for the subsequent elements.

size_id is one of the following identifiers:

| size_id | size |
|---      |---
|DS.B|8-bits
|DS.W|16-bits
|DS.P|24-bits
|DS.Q|32-bits

count is the number of size_id elements stored at the location.

Example:
```
defvars $4000 
{
  v1    ds.b 1          ; v1 = $4000
  v2    ds.b 3          ; v2 = $4001
  v3    ds.w 1          ; v3 = $4004
        ds.w 1          ; reserve one word
}
defvars 0               ; stand-alone struct
{
  prev  ds.w 1          ; prev = 0
  next  ds.w 1          ; next = 2
  data  ds.w 1          ; data = 4
}
defvars -1              ; continue $4000 struct
{
  v4    ds.q 1          ; v4 = $4008
}
```

## BINARY|INCBIN "filename"

Include a binary file at the current place in the assembly.

## LINE|C_LINE line_num[, "filename"]

Tells the assembler that the next input line is from the given source file and line number. This information is used for error messages.

LINE is used to synchronize the assembly source line number.

C_LINE is used by the C-compiler to point the error messages to the original C source file.
