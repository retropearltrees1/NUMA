NAME = numa
BUILD_DIR = output
SRC = src/main.c
CC = arm-none-eabi-gcc
NWLINK ?= nwlink

LAUNCHER_SRC = numplay_repo/launcher/src
COMMON_SRC = numplay_repo/games/common

CFLAGS = -std=c11 $(shell $(NWLINK) eadk-cflags-device)
CFLAGS += -Os -Wall -Wextra -Wno-unused-parameter
CFLAGS += -I. -I$(LAUNCHER_SRC) -I$(COMMON_SRC)
CFLAGS += -fno-tree-loop-distribute-patterns -flto -fno-fat-lto-objects -fvisibility=internal

LDFLAGS = -Wl,--relocatable -nostartfiles --specs=nano.specs
LDFLAGS += -Wl,-e,main -Wl,-u,eadk_app_name -Wl,-u,eadk_app_icon -Wl,-u,eadk_api_level
LDFLAGS += -Wl,--gc-sections -flinker-output=nolto-rel

OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/gfx.o $(BUILD_DIR)/font_data.o $(BUILD_DIR)/xfont.o $(BUILD_DIR)/icon.o

all: $(BUILD_DIR)/$(NAME).nwa

check: $(BUILD_DIR)/$(NAME).nwa
	$(NWLINK) nwa-bin $< $(BUILD_DIR)/$(NAME).bin
	@echo "VALIDATION REUSSIE : $(BUILD_DIR)/$(NAME).bin est valide pour NumWorks !"

$(BUILD_DIR)/main.o: $(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/gfx.o: $(LAUNCHER_SRC)/gfx.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/font_data.o: $(LAUNCHER_SRC)/font_data.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/xfont.o: $(LAUNCHER_SRC)/xfont.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/icon.o: src/icon.png | $(BUILD_DIR)
	$(NWLINK) png-icon-o $< $@

$(BUILD_DIR)/$(NAME).nwa: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	arm-none-eabi-strip --strip-unneeded $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)
