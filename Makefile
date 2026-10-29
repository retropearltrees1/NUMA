NAME = numa
SRCS = src/main.c

CFLAGS = -Wall -Wextra -Os -fno-common -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-d16
CFLAGS += -I. -Inumplay_repo/launcher/src -Inumplay_repo/games/common

CC = arm-none-eabi-gcc
NWLINK = nwlink

all: output/$(NAME).nwa

output/$(NAME).elf: $(SRCS)
	@mkdir -p output
	$(CC) $(CFLAGS) -Wl,-flinker-output=nolto-rel -Wl,-relocatable $^ -o $@

output/$(NAME).nwa: output/$(NAME).elf
	$(NWLINK) nwa-bin $^ $@

clean:
	rm -rf output
