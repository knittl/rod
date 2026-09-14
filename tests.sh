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

	case "$1" in
		--hex)
			shift
			unhex() { xxd -r -p; }
			input="$1"
			;;
		--sh)
			shift
			unhex() { xxd -r -p; }
			input=$(sh -c "$1" | xxd -p)
			;;
		*)
			unhex() { cat; }
			input="$1"
			;;
	esac

	desc="${2:-"$1"}"
	actual="$(printf '%s' "$input" | unhex | od ${endianness:+"--endian=$endianness"} | ./rod ${endianness:+"--endian=$endianness"} | xxd -p)"
	expected="$(printf '%s' "$input" | unhex | xxd -p)"

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

	_test --hex "000102030400" 'binary'
	_test --hex "000000000000" 'nul bytes'

	_test --sh 'head -c100 /dev/urandom | base64' 'base64'
	_test --sh 'dd if=/dev/urandom bs=1M count=17' 'large offsets'
	_test --sh 'dd if=/dev/zero bs=1M count=17' 'large repeat'
done
