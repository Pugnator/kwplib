PLATFORM?=WIN32
BUILDTYPE?=RELEASE

MAKE:=make

ifeq ($(PLATFORM),WIN32)
WINRES:=windres
RM:= rm -rf
STRIP:=strip
GPP:=g++
CP = cp
MKDIR_P = mkdir -p

else ifeq ($(PLATFORM), STM32)

GCCPRFX:=arm-none-eabi
AR:=$(GCCPRFX)-ar
GPP:=$(GCCPRFX)-g++
ASM:=$(GCCPRFX)-as
OCOPY:=$(GCCPRFX)-objcopy
STRIP:=$(GCCPRFX)-strip
SIZE:=$(GCCPRFX)-size
READELF:=$(GCCPRFX)-readelf
CXXFILT:=$(GCCPRFX)-c++filt
RM:=rm -rf
CP = cp
MKDIR_P = mkdir -p

else
$(error bad platform specified)
endif

SRCDIR:=src
OBJDIR:=obj_$(PLATFORM)
OUTDIR:=bin

SRC:=\
    $(SRCDIR)/client.cc \
    $(SRCDIR)/message.cc \
    $(SRCDIR)/commands.cc \
	$(SRCDIR)/dtc.cc \
	$(SRCDIR)/ids.cc

OBJ:=$(SRC:%.cc=$(OBJDIR)/%.o)
ifeq ($(PLATFORM),WIN32)
OBJ+=$(OBJDIR)/client_win32.o
OBJ+=$(OBJDIR)/main.o
else ifeq ($(PLATFORM), STM32)
OBJ+=$(OBJDIR)/client_stm32.o
endif

KWPEXEC:=$(OUTDIR)/kwp.exe
KWPLIB:=$(OUTDIR)/kwp.a


CXXFLAGS+= -Iinclude \
	--std=gnu++14 \
	-DSTM32F10X_MD -D_GNU_SOURCE -D__STM32__\
	-D_REENT_SMALL -D_REENT_GLOBAL_ATEXIT \
	-Wno-write-strings \
	-fno-rtti \
	-fno-exceptions \
	-Dassert_param="" \
	-fpermissive \
	-fno-builtin \
	-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float

LDFLAGS:=-Wl,--gc-sections


ifeq ($(BUILDTYPE), DEBUG)
	CXXFLAGS+=-Og -g3 -D__DEBUG=1
else ifeq ($(BUILDTYPE), RELEASE)
	CXXFLAGS+= -Os -s -D__DEBUG=0 -Werror
else
$(error bad buildtype specified)
endif

.PHONY: all
all: dirs
ifeq ($(PLATFORM),WIN32)
all: $(KWPEXEC) | silent
else ifeq ($(PLATFORM), STM32)
all: $(KWPLIB) | silent
endif


.PHONY: lib
lib: CXXFLAGS+=-Os -static
lib: dirs
lib: $(KWPLIB)

.PHONY: dirs
dirs: 
	@mkdir -p $(OBJDIR) 
	@mkdir -p $(OUTDIR) 
 
ifeq ($(PLATFORM),WIN32)
$(OBJDIR)/kwp.res: kwp.rc
	$(MKDIR_P) `dirname $@`
	$(WINRES) $< -O coff -o $@

$(OBJDIR)/client_win32.o: $(SRCDIR)/client_win32.cc
	$(MKDIR_P) `dirname $@`
	$(GPP) -c -o $@ $< $(CXXFLAGS)

$(OBJDIR)/main.o: $(SRCDIR)/main.cc
	$(MKDIR_P) `dirname $@`
	$(GPP) -c -o $@ $< $(CXXFLAGS)
else ifeq ($(PLATFORM), STM32)
$(OBJDIR)/client_stm32.o: $(SRCDIR)/client_stm32.cc
	$(MKDIR_P) `dirname $@`
	$(GPP) -c -o $@ $< $(CXXFLAGS)
endif


$(OBJDIR)/%.o: %.cc
	$(MKDIR_P) `dirname $@`
	$(GPP) -c -o $@ $< $(CXXFLAGS)

$(OBJDIR):
	$(MKDIR_P) $(OBJDIR)

$(OUTDIR):
	$(MKDIR_P) $(OUTDIR)

ifeq ($(PLATFORM),WIN32)
$(OBJ)+=$(OBJDIR)/kwp.res
endif

$(KWPEXEC): $(OBJ)
	$(GPP) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)
	$(STRIP) -s $(KWPEXEC)

$(KWPLIB): $(OBJ)
	ar rcs $@ $+
	#$(STRIP) -s $(KWPLIB)

.PHONY: clean
clean:
	$(RM) $(OBJ)
	$(RM) obj*
	$(RM) $(OUTDIR)
	
.PHONY: silent
silent:
    @: