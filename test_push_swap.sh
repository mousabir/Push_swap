#!/usr/bin/env bash
# Test script for push_swap.
#
# Usage:
#   ./test_push_swap.sh
#
# Options (environment variables):
#   PS=./push_swap                        path to your program
#   STRATS="--simple --medium"            strategies to test
#   STRATS="--simple --medium --complex --adaptive"   when you have all four
#   CHECKER=./checker_linux               use the school checker instead of the built-in one
#   TIMEOUT=10                            seconds before a test counts as an infinite loop
#   V=1                                   also print every passed test
#
# The built-in checker is a small awk program that runs the moves on the
# numbers and tells if stack a is sorted and stack b is empty.

PS="${PS:-./push_swap}"
STRATS="${STRATS:-"--simple --medium"}"
CHECKER="${CHECKER:-}"
TIMEOUT="${TIMEOUT:-10}"
V="${V:-0}"

TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT
OUT="$TMP/out"
ERR="$TMP/err"

PASS=0
FAIL=0
RC=0
MOVES=0

if [ -t 1 ]; then
	G=$'\033[32m'; R=$'\033[31m'; Y=$'\033[33m'; B=$'\033[1m'; N=$'\033[0m'
else
	G=""; R=""; Y=""; B=""; N=""
fi

if [ ! -x "$PS" ]; then
	echo "${R}Cannot find $PS. Compile first (make), or set PS=path.${N}"
	exit 2
fi

# ---------------------------------------------------------------- helpers

ok()
{
	PASS=$((PASS + 1))
	[ "$V" = "1" ] && echo "${G}ok${N}   $1"
}

bad()
{
	FAIL=$((FAIL + 1))
	echo "${R}FAIL${N} $1"
	[ -n "$2" ] && echo "       $2"
}

section()
{
	echo
	echo "${B}== $1 ==${N}"
}

# prints the arguments, or saves them in a file when there are many
show_args()
{
	if [ "$#" -le 12 ]; then
		echo "args: $*"
	else
		local f="ps_fail_${FAIL}.txt"
		echo "$*" > "$f"
		echo "args saved in $f   (run: $PS $STRAT_SHOWN \$(cat $f))"
	fi
}

run_ps()
{
	timeout "$TIMEOUT" "$PS" "$@" > "$OUT" 2> "$ERR"
	RC=$?
}

crashed()
{
	[ "$RC" -eq 124 ] || [ "$RC" -ge 128 ]
}

why()
{
	if [ "$RC" -eq 124 ]; then
		echo "timeout after ${TIMEOUT}s (infinite loop?)"
	elif [ "$RC" -ge 128 ]; then
		echo "crashed with signal $((RC - 128)) (segfault = 11)"
	else
		echo "exit code $RC"
	fi
}

# verify <numbers...> : reads the moves from $OUT, prints OK or KO
verify()
{
	if [ -n "$CHECKER" ] && [ -x "$CHECKER" ]; then
		"$CHECKER" "$@" < "$OUT" 2>/dev/null
		return
	fi
	awk -v nums="$*" '
	function sw_a(  t) { t = A[1]; A[1] = A[2]; A[2] = t }
	function sw_b(  t) { t = B[1]; B[1] = B[2]; B[2] = t }
	function ro_a(  i, t) { t = A[1]; for (i = 1; i < na; i++) A[i] = A[i+1]; A[na] = t }
	function ro_b(  i, t) { t = B[1]; for (i = 1; i < nb; i++) B[i] = B[i+1]; B[nb] = t }
	function rr_a(  i, t) { t = A[na]; for (i = na; i > 1; i--) A[i] = A[i-1]; A[1] = t }
	function rr_b(  i, t) { t = B[nb]; for (i = nb; i > 1; i--) B[i] = B[i-1]; B[1] = t }
	function pu_a(  i) { for (i = na; i >= 1; i--) A[i+1] = A[i]; A[1] = B[1]; na++;
	                     for (i = 1; i < nb; i++) B[i] = B[i+1]; delete B[nb]; nb-- }
	function pu_b(  i) { for (i = nb; i >= 1; i--) B[i+1] = B[i]; B[1] = A[1]; nb++;
	                     for (i = 1; i < na; i++) A[i] = A[i+1]; delete A[na]; na-- }
	BEGIN { na = split(nums, A, " "); nb = 0; bad = 0 }
	{
		op = $0
		if (op == "sa") { if (na > 1) sw_a() }
		else if (op == "sb") { if (nb > 1) sw_b() }
		else if (op == "ss") { if (na > 1) sw_a(); if (nb > 1) sw_b() }
		else if (op == "pa") { if (nb > 0) pu_a() }
		else if (op == "pb") { if (na > 0) pu_b() }
		else if (op == "ra") { if (na > 1) ro_a() }
		else if (op == "rb") { if (nb > 1) ro_b() }
		else if (op == "rr") { if (na > 1) ro_a(); if (nb > 1) ro_b() }
		else if (op == "rra") { if (na > 1) rr_a() }
		else if (op == "rrb") { if (nb > 1) rr_b() }
		else if (op == "rrr") { if (na > 1) rr_a(); if (nb > 1) rr_b() }
		else { bad = 1; badop = op }
	}
	END {
		good = (!bad && nb == 0)
		for (i = 1; i < na; i++)
			if (A[i] + 0 > A[i+1] + 0) good = 0
		if (bad) print "KO unknown move: [" badop "]"
		else if (good) print "OK"
		else print "KO (a not sorted or b not empty)"
	}' "$OUT"
}

# gen <n> : n different random numbers, with negatives
gen()
{
	shuf -i 0-$(($1 * 4)) -n "$1" | awk -v o=$(($1 * 2)) '{ print $1 - o }'
}

# ---------------------------------------------------------------- tests

# t_error <name> <strat> <args...> : must print Error on stderr and nothing on stdout
t_error()
{
	local name="$1" strat="$2"
	shift 2
	STRAT_SHOWN="$strat"
	run_ps $strat "$@"
	if crashed; then
		bad "$name $strat" "$(why)"
	elif [ -s "$OUT" ]; then
		bad "$name $strat" "stdout is not empty (it must be empty on error)"
	elif [ "$(cat "$ERR")" != "Error" ]; then
		bad "$name $strat" "stderr is [$(head -c 60 "$ERR")], expected [Error]"
	else
		ok "$name $strat"
	fi
}

# t_sort <name> <strat> <numbers...> : the moves must sort the stack
t_sort()
{
	local name="$1" strat="$2" res
	shift 2
	STRAT_SHOWN="$strat"
	MOVES=0
	run_ps $strat "$@"
	if crashed; then
		bad "$name $strat" "$(why)"
		show_args "$@"
		return 1
	fi
	if [ -s "$ERR" ]; then
		bad "$name $strat" "unexpected stderr: [$(head -c 60 "$ERR")]"
		show_args "$@"
		return 1
	fi
	res="$(verify "$@")"
	if [ "$res" != "OK" ]; then
		bad "$name $strat" "$res"
		show_args "$@"
		return 1
	fi
	MOVES=$(wc -l < "$OUT" | tr -d ' ')
	ok "$name $strat ($MOVES moves)"
	return 0
}

# t_zero <name> <strat> <numbers...> : sorted input must give 0 moves
t_zero()
{
	local name="$1" strat="$2"
	shift 2
	if t_sort "$name" "$strat" "$@"; then
		if [ "$MOVES" -ne 0 ]; then
			bad "$name $strat" "already sorted but $MOVES moves printed (expected 0)"
		fi
	fi
}

# ---------------------------------------------------------------- 1. errors

section "1. Errors (must print Error on stderr, nothing on stdout)"
for S in $STRATS; do
	t_error "letters"            "$S" abc
	t_error "letter in list"     "$S" 1 2 a 3
	t_error "word one"           "$S" 0 one 2 3
	t_error "duplicate"          "$S" 3 2 3
	t_error "duplicate far"      "$S" 99 88 75 -2 99 88 3 22 7
	t_error "duplicate 0 and -0" "$S" 0 -0
	t_error "above INT_MAX"      "$S" 2147483648
	t_error "below INT_MIN"      "$S" -2147483649
	t_error "huge number"        "$S" 99999999999999999999
	t_error "lone plus"          "$S" +
	t_error "lone minus"         "$S" -
	t_error "empty string"       "$S" ""
	t_error "1 2 empty 3"        "$S" 1 2 "" 3
	t_error "number with space"  "$S" "1 2" 3
	t_error "double sign"        "$S" --5
	t_error "sign in middle"     "$S" 5-3
	t_error "float"              "$S" 1.5
	t_error "unknown option"     "$S" --semple 1 2 3
done

section "2. No arguments (must print nothing)"
run_ps
if crashed; then
	bad "no arguments" "$(why)"
elif [ -s "$OUT" ] || [ -s "$ERR" ]; then
	bad "no arguments" "something was printed"
else
	ok "no arguments"
fi

# ---------------------------------------------------------------- 3. small

section "3. Small and edge cases"
for S in $STRATS; do
	t_zero "one number"        "$S" 42
	t_zero "sorted 5"          "$S" 1 2 3 4 5
	t_zero "sorted negatives"  "$S" -5 -3 0 4 9
	t_sort "two reversed"      "$S" 2 1
	t_sort "three 3 2 1"       "$S" 3 2 1
	t_sort "three 2 1 3"       "$S" 2 1 3
	t_sort "three 1 3 2"       "$S" 1 3 2
	t_sort "three 3 1 2"       "$S" 3 1 2
	t_sort "three 2 3 1"       "$S" 2 3 1
	t_sort "reversed 5"        "$S" 5 4 3 2 1
	t_sort "subject example"   "$S" 2 1 3 6 5 8
	t_sort "four numbers"      "$S" 99 88 75 -2
	t_sort "INT limits"        "$S" 2147483647 -2147483648 0
	t_sort "INT limits 2"      "$S" -2147483648 2147483647
	t_sort "plus sign"         "$S" +3 +1 +2
	t_sort "zeros and neg"     "$S" 0 -1 1 -2 2
	t_sort "reversed 100"      "$S" $(seq 100 -1 1)
	t_zero "sorted 100"        "$S" $(seq 1 100)
done

section "4. Option position and several options"
for S in $STRATS; do
	STRAT_SHOWN="$S"
	run_ps 3 2 1 $S
	if crashed; then
		bad "option after numbers $S" "$(why)"
	elif [ "$(verify 3 2 1)" != "OK" ]; then
		bad "option after numbers $S" "result is not sorted"
	else
		ok "option after numbers $S"
	fi
done

# ---------------------------------------------------------------- 5. random

section "5. Random tests (different numbers every run)"
for S in $STRATS; do
	for n in 2 3 4 5 6 7 8 9 10 11 15 20 30 50; do
		fails_before=$FAIL
		for i in $(seq 1 20); do
			mapfile -t NUMS < <(gen "$n")
			t_sort "random n=$n" "$S" "${NUMS[@]}"
		done
		if [ "$FAIL" -ne "$fails_before" ]; then
			echo "${R}  $S n=$n: $((FAIL - fails_before)) failed out of 20${N}"
		fi
	done
done
echo "random tests done"

# ---------------------------------------------------------------- 6. perf

section "6. Performance (100 numbers x10, 500 numbers x5)"
for S in $STRATS; do
	for spec in "100 10 2000 1500 700" "500 5 12000 8000 5500"; do
		set -- $spec
		n=$1; reps=$2; lim_pass=$3; lim_good=$4; lim_exc=$5
		sum=0; max=0; done_ok=0
		for i in $(seq 1 "$reps"); do
			mapfile -t NUMS < <(gen "$n")
			if t_sort "perf n=$n" "$S" "${NUMS[@]}"; then
				done_ok=$((done_ok + 1))
				sum=$((sum + MOVES))
				[ "$MOVES" -gt "$max" ] && max=$MOVES
			fi
		done
		if [ "$done_ok" -eq 0 ]; then
			echo "${R}$S n=$n: no run finished correctly${N}"
			continue
		fi
		avg=$((sum / done_ok))
		if [ "$S" = "--simple" ]; then
			echo "$S n=$n: average $avg, worst $max   (no limit checked for the simple sort)"
			continue
		fi
		if [ "$max" -ge "$lim_pass" ]; then
			bad "$S n=$n too many moves" "average $avg, worst $max, limit to pass: under $lim_pass"
		else
			if [ "$max" -lt "$lim_exc" ]; then grade="${G}excellent${N}"
			elif [ "$max" -lt "$lim_good" ]; then grade="${G}good${N}"
			else grade="${Y}pass${N}"
			fi
			ok "$S n=$n moves"
			echo "$S n=$n: average $avg, worst $max -> $grade (pass <$lim_pass, good <$lim_good, excellent <$lim_exc)"
		fi
	done
done

# ---------------------------------------------------------------- 7. leaks

section "7. Memory (valgrind)"
if command -v valgrind > /dev/null 2>&1; then
	for S in $STRATS; do
		mapfile -t NUMS < <(gen 30)
		valgrind -q --leak-check=full --errors-for-leak-kinds=all \
			--error-exitcode=42 "$PS" $S "${NUMS[@]}" > /dev/null 2> "$TMP/vg"
		if [ "$?" -eq 42 ]; then
			bad "valgrind valid input $S" "see: valgrind --leak-check=full $PS $S 3 2 1"
			head -n 15 "$TMP/vg"
		else
			ok "valgrind valid input $S"
		fi
		valgrind -q --leak-check=full --errors-for-leak-kinds=all \
			--error-exitcode=42 "$PS" $S 1 2 1 > /dev/null 2> "$TMP/vg"
		if [ "$?" -eq 42 ]; then
			bad "valgrind error path $S" "leak or memory error when the input is invalid"
			head -n 15 "$TMP/vg"
		else
			ok "valgrind error path $S"
		fi
	done
else
	echo "valgrind is not installed, skipped (compile with -fsanitize=address as another way)"
fi

# ---------------------------------------------------------------- result

echo
echo "${B}Result: $PASS passed, $FAIL failed${N}"
if [ "$FAIL" -eq 0 ]; then
	echo "${G}ALL TESTS PASSED${N}"
	exit 0
fi
echo "${R}SOME TESTS FAILED${N}"
exit 1
