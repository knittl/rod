#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* support offsets up to 07777777777777777777777 (>64 bit) */
#define SIZE 96

static uint64_t offsets[2] = { 0UL };
static char line[SIZE];

static unsigned int octetts[8] = { 0U };
static unsigned int buffer[8] = { 0U };
static char big_endian = 0;

static char *read_line(void);
static int parse_line(void);
static void write_octetts(void);

int main(int argc, char **argv) {
	if (argc > 1) {
		big_endian = strstr(argv[1], "--endian=big") == argv[1];
	}

	while (read_line()) {
		if (parse_line() > 0) {
			write_octetts();
		}
	}

	return 0;
}

static char *read_line(void) {
	return fgets(line, SIZE, stdin);
}

static int parse_line(void) {
	offsets[0] = offsets[1];
	memcpy(octetts, buffer, sizeof(octetts));
	return sscanf(
			line,
			"%"SCNo64" %6o %6o %6o %6o %6o %6o %6o %6o",
			offsets + 1,
			buffer + 0,
			buffer + 1,
			buffer + 2,
			buffer + 3,
			buffer + 4,
			buffer + 5,
			buffer + 6,
			buffer + 7);
}

static void write_octetts(void) {
	uint64_t i;
	for (i = offsets[0]; i < offsets[1]; ++i) {
		unsigned int b = octetts[(i >> 1) & 0x7];
		b = (b >> (big_endian << 3)) | (b << (big_endian << 3));
		putchar(b >> ((i & 1) << 3));
	}
}
