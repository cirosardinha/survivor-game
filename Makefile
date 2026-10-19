# Build incremental (Windows mingw-w64 / Linux gcc)
#
# make                  compila e roda
# make compile          só compila
# make run              roda (compila antes, se precisar)
# make debug            compila com -g -O0 em build/debug
# make clean            remove build/
# make cleanAndCompile  limpa e recompila do zero
# make android          gera o APK debug
# make android-install  instala o APK no aparelho conectado

CC = gcc

SRC_DIR     = src
INCLUDE_DIR = src/include
BUILD_DIR   = build
ANDROID_DIR = android
GRADLE      ?= gradle

# Tudo que muda entre Windows e Linux fica neste bloco.
ifeq ($(OS),Windows_NT)
LIB_DIR     = lib/win64_mingw-w64
EXE_EXT     = .exe
LDLIBS      = -lraylib -lopengl32 -lgdi32 -lwinmm
MKDIR_BUILD = @if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
RUN_CMD     = $(BUILD_DIR)\$(TARGET_EXEC)
CLEAN_CMD   = @if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
else
LIB_DIR     = lib/linux_amd64
EXE_EXT     =
LDLIBS      = -lraylib -lm -lpthread -ldl -lrt -lX11
MKDIR_BUILD = @mkdir -p "$(BUILD_DIR)"
RUN_CMD     = ./$(BUILD_DIR)/$(TARGET_EXEC)
CLEAN_CMD   = @rm -rf $(BUILD_DIR)
ifeq ($(wildcard $(LIB_DIR)/libraylib.a),)
$(warning $(LIB_DIR)/libraylib.a nao encontrada, usando a raylib do sistema. Veja o README)
endif
endif

TARGET_NAME := $(notdir $(CURDIR))
TARGET_EXEC := $(TARGET_NAME)$(EXE_EXT)

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# -MMD -MP: recompila um .c também quando um header incluído por ele muda
CFLAGS  = -I $(INCLUDE_DIR) -I $(INCLUDE_DIR)/raylib -MMD -MP \
          -O1 -Wall -Wextra -Wno-unused-parameter -pedantic-errors \
          -std=c99 -Wno-missing-braces

ifdef DEBUG
CFLAGS += -g -O0
endif

LDFLAGS = -L $(LIB_DIR) $(LDLIBS)

.PHONY: all compile run debug clean cleanAndCompile android android-install

all: compile run

compile: $(BUILD_DIR)/$(TARGET_EXEC)

cleanAndCompile: clean compile

debug:
	$(MAKE) compile BUILD_DIR=build/debug DEBUG=1

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(MKDIR_BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

run:
	$(RUN_CMD)

android:
	$(GRADLE) -p $(ANDROID_DIR) assembleDebug

android-install:
	$(GRADLE) -p $(ANDROID_DIR) installDebug

clean:
	$(CLEAN_CMD)

-include $(DEPS)
