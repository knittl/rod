#include <stdio.h>
#include <string.h>

#define SIZE 65

static unsigned long offsets[2] = { 0UL };
static char line[SIZE];

static unsigned int octetts[8] = { 0U };
static unsigned int buffer[8] = { 0U };

static int parse_line(void) {
	offsets[0] = offsets[1];
	memcpy(octetts, buffer, sizeof(octetts));
	return sscanf(
			line,
			"%lo %6o %6o %6o %6o %6o %6o %6o %6o",
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

int main(int argc, char **argv) {
	char flip_endian = 0;
	unsigned int b;
	long unsigned int i;
	if (argc > 1) {
		flip_endian = strstr(argv[1], "--endian=big") == argv[1];
	}

	while (fgets(line, SIZE, stdin)) {
		if (parse_line() <= 0) continue;

		for (i = offsets[0]; i < offsets[1]; ++i) {
			b = octetts[i >> 1 & 0x7];
			b = (b >> (flip_endian << 3)) | (b << (flip_endian << 3));
			putchar(b >> ((i & 1) << 3));
		}
	}

	return 0;
}
