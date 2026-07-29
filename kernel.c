#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "font.h"


/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif;

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;

    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
} __attribute__((packed)) multiboot_info_t;

int actual_line = 0;

void draw_character(char c, uint32_t x, uint32_t y, uint32_t color, multiboot_info_t* mbd) {
    if (c < 32 || c > 126) return; 

    uint32_t* fb = (uint32_t*)(uint32_t)mbd->framebuffer_addr;
    uint32_t pitch = mbd->framebuffer_pitch / 4;
    const uint8_t* glyph = font8x8[c - 32];

    for (int cy = 0; cy < 8; cy++) {
        for (int cx = 0; cx < 8; cx++) {
            if (glyph[cy] & (1 << (7 - cx))) fb[(y + cy) * pitch + (x + cx)] = color;
        }
    }
}

int calc_text_size(char* text){
	int length = 0;
	while(text[length] != '\0') length++;
	return length;
}

void draw_text(char* text, multiboot_info_t* mbd){
	int text_size = calc_text_size(text);
	for(int i = 0; i < text_size; i++) draw_character(text[i], i * 8, 8 * actual_line, 0xFFFFFF, mbd);
	actual_line++;
}

void value_into_logs_buffer(char* logs_buffer, uint32_t mbd_value){
	if(mbd_value == 0){
		logs_buffer[0] = '0';
		logs_buffer[1] = '\0';
		return;
	}

	int i = 0;
	while(mbd_value != 0){
		logs_buffer[i] = (mbd_value % 10) + '0';
		i++;
		mbd_value /= 10;
	}
	logs_buffer[i] = '\0';

	int start = 0;
	int end = i - 1;
	while(start < end){
		char temp = logs_buffer[start];
		logs_buffer[start] = logs_buffer[end];
		logs_buffer[end] = temp;
		start++;
		end--;
	}
}

void draw_mbd_info(multiboot_info_t* mbd){
	char logs_buffer[16];

	draw_text("						Multiboot Data ", mbd);
	draw_text("				Framebuffer Data", mbd);

	value_into_logs_buffer(logs_buffer, mbd->framebuffer_width);
	draw_text("		width: ", mbd);
	draw_text(logs_buffer, mbd);

	value_into_logs_buffer(logs_buffer, mbd->framebuffer_height);
	draw_text("		height: ", mbd);
	draw_text(logs_buffer, mbd);

	value_into_logs_buffer(logs_buffer, mbd->framebuffer_pitch);
	draw_text("		pitch: ", mbd);
	draw_text(logs_buffer, mbd);

	draw_text("				Memory Maps", mbd);
	value_into_logs_buffer(logs_buffer, mbd->mem_lower);
	draw_text("		memory lower: ", mbd);
	draw_text(logs_buffer, mbd);

	value_into_logs_buffer(logs_buffer, mbd->mem_upper);
	draw_text("		memory upper: ", mbd);
	draw_text(logs_buffer, mbd);


}

void kernel_main(multiboot_info_t* mbd){
	if (!(mbd->flags & (1 << 12))) return;

	draw_mbd_info(mbd);
}
