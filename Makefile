PROJECT = $(notdir $(CURDIR))
DEBUG = target/debug/$(PROJECT)
RELEASE = target/release/$(PROJECT)

all: mkdir debug

mkdir:
	mkdir -p ./target/debug
	mkdir -p ./target/release

SRC = $(wildcard src/*.c)
CC = gcc
FLAGS = -Wall -pipe
FLAGS += -I"$(CURDIR)/raylib-5.0/src/" -L"$(CURDIR)/target/raylib/" -I"$(CURDIR)/raygui-4.0/" -lraylib -lgdi32 -lwinmm
debug: OUTPUT = $(DEBUG)
debug: executable

release: OUTPUT = $(RELEASE)
release: FLAGS += -O2
release: executable

executable: $(SRC)
	$(CC) $(SRC) -o $(OUTPUT) $(FLAGS) 

.PHONY: run clean install uninstall

# write "make run a="..." for commandline arguments"
run:
	./$(DEBUG) $(a)

clean:
	rm -f $(DEBUG) $(RELEASE)

# installs from release folder only
install:
	ln -s $(CURDIR)/$(RELEASE) ~/.local/bin/

uninstall:
	rm -f ~/.local/bin/$(PROJECT)
