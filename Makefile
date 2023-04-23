CC = clang++
RM = rm
MKDIR = mkdir
DELOBJ = $(OBJ)
SHELL := /bin/bash

## =================================================

MUTE = writable-strings uninitialized
CXXFLAGS = -Wall $(addprefix -Wno-,$(MUTE)) -I/usr/include/SDL2 -I/usr/include/GL -g
<<<<<<< HEAD
LDFLAGS = -lSDL2_test -lSDL2main -lSDL2 -lfreeimage -lGL
=======
LDFLAGS = -lSDL2_test -lSDL2main -lSDL2 -lSDL2_mixer -lfreeimage -lGL
>>>>>>> VisualStudio

APPNAME = whack-a-window
EXT = .cpp
SRCDIR = src
HEADERDIR = src
BINDIR = bin
OBJDIR = $(BINDIR)/obj

## =================================================

SRC = $(wildcard $(SRCDIR)/*$(EXT))
OBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o)
APP = $(BINDIR)/$(APPNAME)
DEP = $(OBJ:$(OBJDIR)/%.o=%.d)

OBJCOUNT_NOPAD = $(shell v=`echo $(OBJ) | wc -w`; echo `seq 1 $$(expr $$v)`)
OBJCOUNT = $(foreach v,$(OBJCOUNT_NOPAD),$(shell printf '%02d' $(v)))

## =================================================

.MAIN: $(APP)
.DEFAULT_GOAL := $(APP)

# Builds the app
$(APPNAME): $(APP)
$(APP): $(OBJ) | makedirs
	@printf "[final] compiling final product $(notdir $@)..."
	@$(CC) $(CXXFLAGS) -I$(HEADERDIR)/$(TARGET) -o $@ $^ $(LDFLAGS)
	@printf "\b\b done!\n"

# Building rule for .o files and its .c/.cpp in combination with all .h
# $(OBJDIR)/%.o: $(SRCDIR)/%$(EXT) | makedirs
$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT) | makedirs
	@printf "[$(word 1,$(OBJCOUNT))/$(words $(OBJ))] compiling $(notdir $<) into $(notdir $@)..."
	@$(CC) $(CXXFLAGS) -I$(HEADERDIR)/$(TARGET) $(INC_PCH_FLAG) -I $(HEADERDIR) -o $@ -c $<
	@printf "\b\b done!\n"
	$(eval OBJCOUNT = $(filter-out $(word 1,$(OBJCOUNT)),$(OBJCOUNT)))

## =================================================

font:
	rm assets/converted_font/out/*
	python assets/converted_font/convert.py

# Cleans complete project
.PHONY: clean
clean:
	@$(RM) -rf $(BINDIR)

.PHONY: makedirs
makedirs:
	@$(MKDIR) -p $(BINDIR)
	@$(MKDIR) -p $(OBJDIR)

.PHONY: remake
remake: clean $(APP)

