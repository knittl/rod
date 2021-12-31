#!/bin/sh

summary() {
	printf "%d tests, %d failures\n" "$tests" "$failures";
	test "$failures" -eq 0;
}

trap 'summary; exit $?' EXIT

tests=0
failures=0

_test() {
	tests=$((tests+1))
	input="$1"
	desc="${2:-"$1"}"
	actual="$(printf '%s' "$input" | od ${endianness:+"--endian=$endianness"} | ./rod ${endianness:+"--endian=$endianness"} | xxd -p)"
	expected="$(printf '%s' "$input" | xxd -p)"
	if test "$actual" = "$expected"; then
		printf 'OK   %s\n' "$desc"
	else
		printf 'FAIL %s -- "%s" != "%s"\n' "$desc" "$actual" "$expected"
		failures=$((failures+1))
	fi
}

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

	_test "$(printf 000102030400 | xxd -r -p)" 'binary'
	_test "$(printf 000000000000 | xxd -r -p)" 'nul bytes'

	_test "$(head -c100 /dev/urandom | base64)" 'base64'
	_test "$(dd if=/dev/urandom bs=1M count=17)" 'large offsets'
	_test "$(dd if=/dev/zero bs=1M count=17)" 'large repeat'
done
