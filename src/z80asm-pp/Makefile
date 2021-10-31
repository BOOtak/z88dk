#------------------------------------------------------------------------------
# z80asm preprocessor
# Copyright (C) Paulo Custodio, 2011-2021
# License: GPL3 https://www.gnu.org/licenses/gpl-3.0.html
#------------------------------------------------------------------------------

PROJ	= z88dk-z80asm-pp

ifeq ($(OS),Windows_NT)
  EXESUFFIX	= .exe
else
  EXESUFFIX	?=
endif

CXX		?= g++
OPT		= -O3
#OPT		= -g

CXXFLAGS+= -Wall -Wextra -Werror -pedantic-errors -std=gnu++17 -MMD $(OPT)
RE2C	= re2c -W --tags --no-debug-info --no-generation-date 
INSTALL	?= install
ASTYLE	= astyle --style=attach --pad-oper --align-pointer=type \
		  --keep-one-line-blocks --keep-one-line-statements \
		  --break-closing-braces --remove-braces --attach-return-type \
		  --max-code-length=120 --lineend=linux --formatted

SRCS	= $(wildcard *.cpp)
OBJS	= $(SRCS:.cpp=.o)
DEPENDS	= $(SRCS:.cpp=.d)

#------------------------------------------------------------------------------
.PHONY: all clean astyle test install

#------------------------------------------------------------------------------
define MAKE_EXE
all: $(1)$(EXESUFFIX)

$(1)$(EXESUFFIX): $(2)
	$(CXX) $(CXXFLAGS) $(2) $(LDFLAGS) -o $(1)$(EXESUFFIX)
	
clean::
	$(RM) $(1) $(1)$(EXESUFFIX) $(2)
endef

#------------------------------------------------------------------------------
$(eval $(call MAKE_EXE,$(PROJ),$(OBJS)))

#------------------------------------------------------------------------------
define MAKE_RE2C
$(PROJ)$(EXESUFFIX): $(1).o

$(1).cpp: $(1).re Makefile
	$(RE2C) -o $(1).cpp $(1).re
# call astyle because re2c creates inconsistent line ends
#	$(ASTYLE) $(1).cpp
endef

#------------------------------------------------------------------------------
$(eval $(call MAKE_RE2C,cmdline))
$(eval $(call MAKE_RE2C,lex))

#------------------------------------------------------------------------------
astyle:
	$(ASTYLE) *.cpp,*.h,*.re

clean::
	$(RM) *.orig

#------------------------------------------------------------------------------
test: $(PROJ)$(EXESUFFIX)
	perl -S prove -It --state=slow,save -j9 t/*.t

#------------------------------------------------------------------------------
install: $(PROJ)$(EXESUFFIX)
	$(INSTALL) $(PROJ)$(EXESUFFIX) $(PREFIX)/bin/$(PROJ)$(EXESUFFIX)

#------------------------------------------------------------------------------
clean::
	$(RM) *.o *.d

-include $(DEPENDS)
