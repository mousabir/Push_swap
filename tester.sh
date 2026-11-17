#!/bin/bash
# push_swap tester (no checker needed) - limits taken from the subject
# Usage: ./tester.sh            (default 20 runs per test)
#        RUNS=100 ./tester.sh   (more runs)

PS=./push_swap
RUNS=${RUNS:-20}
MODES=("" "--simple" "--medium" "--complex" "--adaptive")
BENCH_FLAG="--bench"

# Subject limits (operations must be LESS than these)
P100_EXCELLENT=700;   P100_GOOD=1500;  P100_PASS=2000
P500_EXCELLENT=5500;  P500_GOOD=8000;  P500_PASS=12000

GREEN="\033[32m"; RED="\033[31m"; YELLOW="\033[33m"; RESET="\033[0m"
PASS=0
FAIL=0
: > failed.log

ok() { PASS=$((PASS + 1)); printf "${GREEN}OK${RESET} %s\n" "$1"; }
ko() { FAIL=$((FAIL + 1)); printf "${RED}KO${RESET} %s\n" "$1"; echo "$1" >> failed.log; }
title() { printf "\n${YELLOW}== %s ==${RESET}\n" "$1"; }

gen() { # gen N -> N unique random numbers, negatives included
	seq 1 "$1" | shuf | awk -v n="$1" '{print $1 - int(n / 2)}' | tr '\n' ' '
}

is_sorted() { # is_sorted numbers... -> true if already ascending
	[ "$(echo "$@")" = "$(echo "$@" | tr ' ' '\n' | sort -n | xargs)" ]
}

set_mode() { # set_mode "--simple" -> sets MODE_ARGS and label
	MODE_ARGS=()
	[ -n "$1" ] && MODE_ARGS=("$1")
	label=${1:-default}
}

[ -x "$PS" ] || { echo "$PS not found. Run make first."; exit 1; }

# ---------- Errors ----------
title "Errors"
check_error() {
	local desc="$1"; shift
	local out
	out=$("$PS" "$@" 2>&1 >/dev/null)
	if echo "$out" | grep -q "^Error$"; then ok "$desc"; else ko "$desc: args [$*]"; fi
}
check_error "letter" 1 a 3
check_error "duplicate" 1 2 2 3
check_error "above INT_MAX" 1 2147483648
check_error "below INT_MIN" 1 -2147483649
check_error "lone plus" +
check_error "lone minus" -
check_error "empty string" ""
check_error "mixed junk" "1 2 x"
check_error "duplicate in one string" "1 2 1"
check_error "subject example: --adaptive 0 one 2 3" --adaptive 0 one 2 3
check_error "subject example: --simple 3 2 3" --simple 3 2 3

out=$("$PS" 2>&1)
if [ -z "$out" ]; then ok "no arguments prints nothing"; else ko "no arguments printed something"; fi

out=$("$PS" 3 2 1 2>&1 >/dev/null)
if [ -z "$out" ]; then ok "nothing on stderr when there is no bench flag"; else ko "stderr is not empty without $BENCH_FLAG"; fi

# ---------- Small inputs ----------
# No operation limit in the subject. We only check: no crash, no Error,
# and an unsorted input must not give 0 operations.
title "Small inputs"
small_test() { # small_test desc "args1" "args2" ...
	local desc="$1"; shift
	local max=0 a ops rc n bad=""
	for a in "$@"; do
		ops=$("$PS" "${MODE_ARGS[@]}" $a 2>/tmp/ps_err.txt); rc=$?
		n=$(printf '%s\n' "$ops" | grep -c .)
		if [ $rc -ne 0 ] || [ -s /tmp/ps_err.txt ]; then bad="exit $rc or stderr output for [$a]"; break; fi
		if ! is_sorted $a && [ "$n" -eq 0 ]; then bad="0 operations on unsorted input [$a]"; break; fi
		if is_sorted $a && [ "$n" -ne 0 ]; then bad="operations on sorted input [$a]"; break; fi
		[ "$n" -gt "$max" ] && max=$n
	done
	rm -f /tmp/ps_err.txt
	if [ -n "$bad" ]; then ko "[$label] $desc: $bad"; else ok "[$label] $desc (max $max operations)"; fi
}

for m in "${MODES[@]}"; do
	set_mode "$m"
	small_test "one number" 42
	small_test "two numbers, both orders" "1 2" "2 1"
	small_test "three numbers, all 6 orders" "1 2 3" "1 3 2" "2 1 3" "2 3 1" "3 1 2" "3 2 1"
	five=()
	for i in $(seq 1 "$RUNS"); do five+=("$(gen 5)"); done
	small_test "five numbers, $RUNS random lists" "${five[@]}"
done

# ---------- Big sorting ----------
title "Big sorting (limits from the subject)"
tier() { # tier max excellent good pass
	if [ "$1" -lt "$2" ]; then echo "excellent"
	elif [ "$1" -lt "$3" ]; then echo "good"
	elif [ "$1" -lt "$4" ]; then echo "pass"
	else echo "fail"; fi
}

big_test() { # big_test size excellent good pass check(yes|no)
	local size=$1 ex=$2 gd=$3 ps=$4 check=$5
	local min=999999999 max=0 sum=0 i args n worst=""
	for i in $(seq 1 "$RUNS"); do
		args=$(gen "$size")
		n=$("$PS" "${MODE_ARGS[@]}" $args 2>/dev/null | grep -c .)
		if [ "$n" -eq 0 ]; then
			echo "$args" > "failed_${size}_${label}.txt"
			ko "[$label] $size numbers: 0 operations on unsorted input (saved in failed_${size}_${label}.txt)"
			return
		fi
		[ "$n" -lt "$min" ] && min=$n
		if [ "$n" -gt "$max" ]; then max=$n; worst=$args; fi
		sum=$((sum + n))
	done
	local avg=$((sum / RUNS))
	if [ "$check" = "yes" ]; then
		local t
		t=$(tier "$max" "$ex" "$gd" "$ps")
		if [ "$t" = "fail" ]; then
			echo "$worst" > "failed_${size}_${label}.txt"
			ko "[$label] $size numbers: max $max is not under $ps (min $min, avg $avg, worst input in failed_${size}_${label}.txt)"
		else
			ok "[$label] $size numbers: max $max -> $t (min $min, avg $avg)"
		fi
	else
		ok "[$label] $size numbers: min $min, avg $avg, max $max (no limit checked for this mode)"
	fi
}

for m in "${MODES[@]}"; do
	set_mode "$m"
	check=no
	# The limits are checked for the default program, adaptive and complex.
	case "$m" in ""|--adaptive|--complex) check=yes ;; esac
	big_test 100 $P100_EXCELLENT $P100_GOOD $P100_PASS $check
	big_test 500 $P500_EXCELLENT $P500_GOOD $P500_PASS $check
done

# ---------- Bench output ----------
title "Bench output"
sorted_list="1 2 3 4 5 6 7 8 9 10"
reverse_list="10 9 8 7 6 5 4 3 2 1"

out=$("$PS" $BENCH_FLAG $sorted_list 2>&1)
if echo "$out" | grep -q "^0\.00%"; then ok "sorted list: disorder 0.00%"; else ko "sorted list: disorder is not 0.00%"; fi

out=$("$PS" $BENCH_FLAG $reverse_list 2>&1)
if echo "$out" | grep -q "^100\.00%"; then ok "reverse list: disorder 100.00%"; else ko "reverse list: disorder is not 100.00%"; fi

# strategy name and complexity must appear in each fixed mode
bench_complexity() { # bench_complexity flag expected_text
	out=$("$PS" $BENCH_FLAG "$1" $(gen 30) 2>&1 >/dev/null)
	if echo "$out" | grep -qF "$2"; then ok "bench with $1 shows '$2'"; else ko "bench with $1 does not show '$2'"; fi
}
bench_complexity --simple  "O(n^2)"
bench_complexity --medium  "O(n*sqrt(n))"
bench_complexity --complex "O(n log n)"

for i in $(seq 1 "$RUNS"); do
	args=$(gen 50)
	out=$("$PS" $BENCH_FLAG $args 2>&1)
	if ! echo "$out" | grep -Eq "^[0-9]+\.[0-9]{2}%"; then
		ko "percent format is not 'N.NN%' for: $args"; continue
	fi
	total=$(echo "$out" | awk '/total number of operations/ {print $NF}')
	sum=$(echo "$out" | awk '/^(sa|sb|ss|pa|pb|ra|rb|rr|rra|rrb|rrr) :/ {s += $3} END {print s + 0}')
	lines=$("$PS" $args 2>/dev/null | grep -c .)
	if [ "$total" = "$sum" ] && [ "$total" = "$lines" ]; then
		ok "bench #$i: total $total = sum of counters = printed lines"
	else
		ko "bench #$i: total $total, counters sum $sum, printed lines $lines for: $args"
	fi
done

# the subject says: benchmark goes to stderr, operations stay on stdout
"$PS" $BENCH_FLAG $(gen 20) 2>/dev/null > /tmp/ps_stdout.txt
if grep -q "total number of operations" /tmp/ps_stdout.txt; then
	ko "bench text is on stdout, the subject wants stderr"
else
	ok "bench text is not on stdout"
fi
rm -f /tmp/ps_stdout.txt

# ---------- Memory ----------
title "Valgrind"
if command -v valgrind > /dev/null; then
	vg() {
		local desc="$1"; shift
		local log
		log=$(valgrind --leak-check=full --errors-for-leak-kinds=all \
			"$PS" "$@" 2>&1 >/dev/null)
		if echo "$log" | grep -q "ERROR SUMMARY: 0 errors"; then ok "valgrind: $desc"; else ko "valgrind: $desc [$*]"; fi
	}
	vg "sorted" 1 2 3
	vg "random 5" $(gen 5)
	vg "random 100" $(gen 100)
	for f in --simple --medium --complex --adaptive; do
		vg "$f with 30 numbers" $f $(gen 30)
	done
	vg "error: duplicate" 1 1
	vg "error: letter" a
	vg "bench" $BENCH_FLAG $(gen 30)
else
	echo "valgrind not installed, skipped."
fi

# ---------- Result ----------
title "Result"
printf "Passed: ${GREEN}%d${RESET}   Failed: ${RED}%d${RESET}\n" "$PASS" "$FAIL"
[ "$FAIL" -gt 0 ] && echo "Failed tests are in failed.log"
exit $((FAIL > 0))