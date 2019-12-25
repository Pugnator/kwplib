WINRES:=windres
RM:= rm -rf
STRIP:=strip
MAKE:=make

ifeq ($(shell uname), Linux)
  CC:=gcc
  CPP:=g++
  CP = cp
else
  CC:=gcc
  CPP:=g++
  WINRES:=windres
  CP = cp
endif
MKDIR_P = mkdir -p

SRCDIR:=src
OBJDIR:=obj
OUTDIR:=bin


SRC:=\
    $(SRCDIR)/client.cc \
    $(SRCDIR)/message.cc \
    $(SRCDIR)/commands.cc \
		$(SRCDIR)/ids.cc

OBJ:=$(SRC:%.cc=$(OBJDIR)/%.o)

KWPEXEC:=$(OUTDIR)/kwp.exe

FLAGS:=-ffunction-sections -fms-extensions -fdata-sections -I.
CFLAGS+=-std=gnu++11 $(FLAGS) -Iinclude
LDFLAGS:=-Wl,--gc-sections

.PHONY: all
all: FLAGS+=-O0
all: dirs
all: $(KWPEXEC) #strip

.PHONY: debug
debug: FLAGS+=-O0 -g
debug: dirs
debug: $(KWPEXEC)

.PHONY: dirs
dirs: 
	mkdir -p $(OBJDIR) 
	mkdir -p $(OUTDIR) 
 
$(OBJDIR)/kwp.res: kwp.rc
	$(MKDIR_P) `dirname $@`
ifeq ($(OS), Windows_NT)
	$(WINRES) $< -O coff -o $@
endif

$(OBJDIR)/%.o: %.cc
	$(MKDIR_P) `dirname $@`
	$(CPP) -c -o $@ $< $(CFLAGS)

$(OBJDIR):
	$(MKDIR_P) $(OBJDIR)

$(OUTDIR):
	$(MKDIR_P) $(OUTDIR)

ifeq ($(OS), Windows_NT)
  $(KWPEXEC): $(OBJ) $(OBJDIR)/kwp.res
		$(CPP) -o $@ $^ $(CFLAGS) $(LDFLAGS)  
else
  $(KWPEXEC): $(OBJ) $(ASMOBJ) $(DASMOBJ)
		$(CPP) -o $@ $^ $(CFLAGS) $(LDFLAGS) 
endif

.PHONY: strip
strip:
	-$(STRIP) -s $(KWPEXEC)
	
.PHONY: clean
clean:
	$(RM) $(OBJ)
	$(RM) $(OBJDIR)
	$(RM) $(OUTDIR)
	
