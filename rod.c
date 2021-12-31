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

int main(void) {
	while (fgets(line, SIZE, stdin)) {
		parse_line();

		for (unsigned int i = offsets[0]; i < offsets[1]; ++i) {
			putchar(octetts[i >> 1 & 0x7] >> ((i & 1) << 3));
		}
	}

	return 0;
}
