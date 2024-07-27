PROJECT = $(notdir $(CURDIR))
DEBUG = target/debug/$(PROJECT)
RELEASE = target/release/$(PROJECT)

all: mkdir debug

mkdir:
	mkdir -p ./target/debug
	mkdir -p ./target/release

SRC = $(wildcard src/*.c)
CC = gcc
FLAGS = -Wall -pipe -std=c2x
FLAGS += -I"$(CURDIR)/raylib-5.0/src/" -L"$(CURDIR)/target/raylib/" -I"$(CURDIR)/raygui-4.0/" -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
debug: OUTPUT = $(DEBUG)
debug: FLAGS += -g
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
