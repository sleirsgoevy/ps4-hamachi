TOOLCHAIN   := $(OO_PS4_TOOLCHAIN)
PROJDIR     := $(shell basename $(CURDIR))
INTDIR      := $(PROJDIR)/x64/Debug

# Libraries linked into the ELF.
LIBS        := -lc -lkernel -lSceNet -lc++ -lSceVideoOut -lSceFreeType -lSceSysmodule -lSceUserService -lSceSystemService -lScePad -lSceImeDialog

# Compiler options. You likely won't need to touch these.
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
		CC      := clang++
		LD      := ld.lld
		CDIR    := linux
endif
ifeq ($(UNAME_S),Darwin)
		CC      := /usr/local/opt/llvm/bin/clang++
		LD      := /usr/local/opt/llvm/bin/ld.lld
		CDIR    := macos
endif
ODIR        := $(INTDIR)
SDIR        := $(PROJDIR)
IDIRS       := -I$(TOOLCHAIN)/include -I$(TOOLCHAIN)/include/c++/v1
LDIRS       := -L$(TOOLCHAIN)/lib
CFLAGS      := -cc1 -triple x86_64-pc-freebsd-elf -munwind-tables $(IDIRS) -fuse-init-array -emit-obj -debugger-tuning=gdb -debug-info-kind=limited
LFLAGS      := -error-limit=0 -m elf_x86_64 -pie --script link.x --eh-frame-hdr $(LDIRS) $(LIBS) $(TOOLCHAIN)/lib/crt1.o

CFILES      := $(wildcard $(SDIR)/*.c) $(wildcard $(SDIR)/*/*.c)
DIRS        := $(patsubst $(SDIR)/%, $(ODIR)/%, $(wildcard $(SDIR)/*/))
CPPFILES    := $(wildcard $(SDIR)/*.cpp)
OBJS        := $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(CFILES)) $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(CPPFILES)) ../ldr/ldr ../tun/tunldr.o

TARGET = eboot.bin

# Create the intermediate directory incase it doesn't already exist.
_unused := $(shell mkdir -p $(INTDIR))

# Make rules
$(TARGET): mkdir $(ODIR) $(OBJS)
	$(LD) -o $(ODIR)/$(PROJDIR).elf $(OBJS) $(LFLAGS)
	python3 fix_entry.py
	$(TOOLCHAIN)/bin/$(CDIR)/create-fself -in=$(ODIR)/$(PROJDIR).elf -out=$(ODIR)/$(PROJDIR).oelf -eboot=eboot.bin --paid 0x3800000000000011

mkdir:
	mkdir -p $(DIRS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -o $@ $<

$(ODIR):
	@mkdir $@

.PHONY: clean

clean:
	rm -f $(TARGET) $(ODIR)/*.o $(ODIR)/$(PROJDIR).elf $(ODIR)/$(PROJDIR).oelf libc.a libc++.a
