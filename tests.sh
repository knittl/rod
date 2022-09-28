#!/bin/sh

summary() {
	printf "%d tests, %d failures\n" "$tests" "$failures";
	test "$failures" -eq 0;
}

trap 'summary; exit $?' EXIT

tests=0
failures=0

echo() { printf '%s' "$@"; }
hex() { xxd -p; }
unhex() { xxd -p -r; }

_roundtrip() {
	od ${endianness:+"--endian=$endianness"} | ./rod ${endianness:+"--endian=$endianness"}
}

_test() {
	__test cat hex "$1" "$2"
}

_test_hex() {
	__test unhex cat "$1" "$2"
}

_test_stdin() {
	_test_hex "$(hex)" "$1"
}

__test() {
	tests=$((tests+1))
	unhex="$1"
	hex="$2"
	input="$3"
	desc="${4:-$input}"
	actual="$(echo "$input" | $unhex | _roundtrip | hex)"
	expected="$(echo "$input" | $hex)"
	if _eq "$actual" "$expected"; then
		printf 'OK   %s\n' "$desc"
	else
		printf 'FAIL %s -- "%s" != "%s"\n' "$desc" "$actual" "$expected"
		failures=$((failures+1))
	fi
}

_eq() { test "$1" = "$2"; }

for endianness in '' big little; do
	printf 'Endian: %s\n' "${endianness:-default}"

	_test '' 'empty'
	_test '1' 'odd bytes'
	_test '123' 'odd bytes'
	_test '12' 'even bytes'
	_test '1234' 'even bytes'

	_test '0123456789abcdef' '16 bytes'
	_test '0123456789abcdefx' '17 bytes'
	_test 'abcdefghijklmnop0123456789abcdef' '2x16 bytes'
	_test '0123456789abcdef0123456789abcdef' 'repeated'
	_test '0123456789abcdef0123456789abcdefxxxxxxxxxxxxxxxxfedcba9876543210fedcba9876543210fedcba9876543210fedcba9876543210//' 'repeated, normal, repeated'

	_test_hex '000102030400' 'binary'
	_test_hex '000000000000' 'nul bytes'

	_test "$(head -c100 /dev/urandom | base64)" 'base64'
	dd if=/dev/urandom bs=1M count=17 | _test_stdin 'large offsets'
	dd if=/dev/zero bs=1M count=17 | _test_stdin 'large repeat'
done
