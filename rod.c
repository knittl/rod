#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* support offsets up to 07777777777777777777777 (>64 bit) */
#define SIZE 96

static uint64_t curr_offset = 0UL;
static uint64_t next_offset = 0UL;
static char line[SIZE];

static uint16_t buf[16] = { 0U };
static uint16_t *octets = buf;
static uint16_t *buffer = buf + 8;
static char big_endian = 0;

static void usage(void);
static void parse_options(char **argv);

static char *read_line(void);
static int parse_line(void);
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
			big_endian = 1;
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
	curr_offset = next_offset;
	return sscanf(
			line,
			"%"SCNo64
			" %6"SCNo16
			" %6"SCNo16
			" %6"SCNo16
			" %6"SCNo16
			" %6"SCNo16
			" %6"SCNo16
			" %6"SCNo16
			" %6"SCNo16,
			&next_offset,
			buffer + 0,
			buffer + 1,
			buffer + 2,
			buffer + 3,
			buffer + 4,
			buffer + 5,
			buffer + 6,
			buffer + 7);
}

static void write_octets(void) {
	uint64_t i;
	for (i = curr_offset; i < next_offset; ++i) {
		uint16_t b = octets[(i >> 1) & 0x7];
		b = (b >> (big_endian << 3)) | (b << (big_endian << 3));
		putchar(b >> ((i & 1) << 3));
	}
}

static void swap_buffers(void) {
	uint16_t *tmp;
	tmp = octets;
	octets = buffer;
	buffer = tmp;
}
