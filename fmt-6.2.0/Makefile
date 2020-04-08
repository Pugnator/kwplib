MAKE:=make
WINRES:=windres
RM:=rm -rf
STRIP:=strip
AR:=ar
GPP:=g++

SRC:=format.cc os.cc

OBJ:=$(SRC:%.cc=%.o)

FMT:=libfmt.a

CXXFLAGS:= --std=gnu++17 -O3 -Iinclude

LDFLAGS:=-Wl,--gc-sections

.PHONY: all
all:
	$(MAKE) $(FMT)

%.o: %.c
	$(GPP) -c -o $@ $< $(CXXFLAGS)

$(FMT): $(OBJ)
	$(AR) rcs $@ $+

.PHONY: clean
clean:
	$(RM) $(OBJ) $(FMT)	
	
.PHONY: silent
silent:
    @: