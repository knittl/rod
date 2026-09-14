#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* support offsets up to 07777777777777777777777 (>64 bit) */
#define SIZE 96

static uint64_t offset_from = 0UL;
static uint64_t offset_to = 0UL;
static char line[SIZE];

static uint16_t buf[16] = { 0U };
static uint16_t *octets = buf;
static uint16_t *buffer = buf + 8;

static char endian_shift = 0;

static void usage(void);
static void parse_options(char **argv);

static char *read_line(void);
static int parse_line(void);
static uint64_t parse_octal(char **str);
static void write_octets(void);
static void swap_buffers(void);

int main(int argc, char **argv) {
	(void)argc; /* unused */
	parse_options(argv + 1);

	while (read_line()) {
		if (parse_line() > 0) {
			write_octets();
			swap_buffers();
		}
	}

	return 0;
}

static void parse_options(char **argv) {
	char *arg;
	while ((arg = *argv++)) {
		if (strstr(arg, "--help") == arg) {
			usage();
			exit(0);
		} else if (strstr(arg, "--endian=big") == arg) {
			endian_shift = 8;
		}
	}
}

static void usage(void) {
	printf(
			"Usage: rod [--endian=big]\n"
			"\n"
			"Read \"od\" octal representation from standard input.\n"
			"Reverse it into its original binary form and write to standard output.\n"
			"This program only supports the default format of \"od\".\n");
}

static char *read_line(void) {
	return fgets(line, SIZE, stdin);
}

static int parse_line(void) {
	uint8_t i;
	char *p = line;
	if (*p == '*') return 0;
	offset_to = parse_octal(&p);
	++p; /* skip space */
	for (i = 0; i < 8; ++i) {
		buffer[i] = parse_octal(&p);
		++p; /* skip space */
	}
	return 1;
}

static uint64_t parse_octal(char **str) {
	uint64_t value = 0;
	char *p;
	for (p = *str; *p >= '0' && *p <= '7'; ++p) {
		value = value * 8 + (*p - '0');
	}
	*str = p;
	return value;
}

static void write_octets(void) {
	uint64_t i;
	for (i = offset_from; i < offset_to; ++i) {
		uint16_t b = octets[(i >> 1) & 0x7];
		b = (b >> endian_shift) | (b << endian_shift);
		putchar(b >> ((i & 1) << 3));
	}
}

static void swap_buffers(void) {
	uint16_t *tmp;
	offset_from = offset_to;
	tmp = octets;
	octets = buffer;
	buffer = tmp;
}
