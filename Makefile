# Define our compiler tools
CC = i686-elf-gcc
AS = i686-elf-as

# Define our flags
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib

# What files we need to build
OBJS = boot.o kernel.o

# The default target when you just type 'make'
all: myos.iso

boot.o: boot.s
	$(AS) boot.s -o boot.o

kernel.o: kernel.c font.h
	$(CC) -c kernel.c -o kernel.o $(CFLAGS)

myos: $(OBJS)
	$(CC) $(LDFLAGS) -o myos $(OBJS)

myos.iso: myos grub.cfg
	mkdir -p isodir/boot/grub
	cp myos isodir/boot/myos
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir

# A command to clean up your directory
clean:
	rm -f $(OBJS) myos myos.iso
	rm -rf isodir

run: myos.iso
	qemu-system-i386 -cdrom myos.iso
