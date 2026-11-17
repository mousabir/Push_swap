#!/usr/bin/env bash
# ============================================================
#  push_swap tester  (simple / medium / complex)
#  Usage:   ./tester.sh            -> all tests
#           MEM=1 ./tester.sh      -> also run memory check (valgrind or leaks)
#           BIN=./push_swap ./tester.sh   -> choose the binary
#  Works without the official checker: it has its own stack simulator.
# ============================================================

BIN=${BIN:-./push_swap}
MODES=(--simple --medium --complex)

G=$'\033[32m'; R=$'\033[31m'; Y=$'\033[33m'; B=$'\033[36m'; N=$'\033[0m'
PASS=0; FAIL=0

if [ ! -x "$BIN" ]; then
	echo "${R}Binary $BIN not found. Run make first.${N}"
	exit 1
fi

# ---------- stack simulator (awk) ----------
AWKPROG=$(cat <<'EOF'
function sz(k) { return hi[k] - lo[k] + 1 }
function pushf(k, v) { lo[k]--; s[k, lo[k]] = v }
function popf(k,   v) { v = s[k, lo[k]]; delete s[k, lo[k]]; lo[k]++; return v }
function rot(k,   v) { if (sz(k) < 2) return; v = popf(k); hi[k]++; s[k, hi[k]] = v }
function rrot(k,   v) { if (sz(k) < 2) return; v = s[k, hi[k]]; delete s[k, hi[k]]; hi[k]--; pushf(k, v) }
function swp(k,   t) { if (sz(k) < 2) return; t = s[k, lo[k]]; s[k, lo[k]] = s[k, lo[k] + 1]; s[k, lo[k] + 1] = t }
BEGIN {
	n = split(nums, arr, " ")
	lo[0] = 1; hi[0] = n
	for (i = 1; i <= n; i++) s[0, i] = arr[i] + 0
	lo[1] = 1; hi[1] = 0
	bad = 0
}
NF == 0 { next }
{
	if (NF > 1) { bad = 1; next }
	op = $1
	if (op == "sa") swp(0)
	else if (op == "sb") swp(1)
	else if (op == "ss") { swp(0); swp(1) }
	else if (op == "pa") { if (sz(1) > 0) pushf(0, popf(1)) }
	else if (op == "pb") { if (sz(0) > 0) pushf(1, popf(0)) }
	else if (op == "ra") rot(0)
	else if (op == "rb") rot(1)
	else if (op == "rr") { rot(0); rot(1) }
	else if (op == "rra") rrot(0)
	else if (op == "rrb") rrot(1)
	else if (op == "rrr") { rrot(0); rrot(1) }
	else bad = 1
}
END {
	if (bad) { print "BADOP"; exit }
	if (sz(1) != 0 || sz(0) != n) { print "KO"; exit }
	for (i = lo[0]; i < hi[0]; i++)
		if (s[0, i] + 0 > s[0, i + 1] + 0) { print "KO"; exit }
	print "OK"
}
EOF
)

# ---------- helpers ----------
ok()   { PASS=$((PASS + 1)); }
bad()  { FAIL=$((FAIL + 1)); echo "${R}FAIL${N} $1"; [ -n "$2" ] && echo "       $2"; }

run_ps() {
	local m=$1; shift
	if [ "$m" = "--default" ]; then "$BIN" "$@"; else "$BIN" "$m" "$@"; fi
}

short() { local s="$*"; [ ${#s} -gt 70 ] && s="${s:0:70}..."; echo "$s"; }

gen_random() {  # $1 = how many (unique, with negatives)
	awk -v n="$1" -v seed="$RANDOM$RANDOM" 'BEGIN {
		srand(seed)
		while (c < n) {
			v = int(rand() * 200000) - 100000
			if (!(v in s)) { s[v] = 1; printf "%d ", v; c++ }
		}
	}'
}

gen_sorted()   { awk -v n="$1" 'BEGIN { for (i = 1; i <= n; i++) printf "%d ", i - n / 2 }'; }
gen_reverse()  { awk -v n="$1" 'BEGIN { for (i = n; i >= 1; i--) printf "%d ", i - n / 2 }'; }
gen_nearly()   { # sorted list with a few swaps
	awk -v n="$1" -v seed="$RANDOM" 'BEGIN {
		srand(seed)
		for (i = 1; i <= n; i++) a[i] = i
		k = int(n / 20) + 1
		for (j = 0; j < k; j++) {
			x = int(rand() * n) + 1; y = x + 1; if (y > n) y = n
			t = a[x]; a[x] = a[y]; a[y] = t
		}
		for (i = 1; i <= n; i++) printf "%d ", a[i]
	}'
}

# sets globals LAST_RES (OK/KO/BADOP) and LAST_OPS
simulate() {
	local m=$1; shift
	local out
	out=$(run_ps "$m" "$@" 2>/dev/null)
	LAST_RES=$(printf '%s\n' "$out" | awk -v nums="$*" "$AWKPROG")
	LAST_OPS=$(printf '%s\n' "$out" | grep -c .)
}

check_sort() { # name mode args...
	local name=$1 m=$2; shift 2
	simulate "$m" "$@"
	if [ "$LAST_RES" = "OK" ]; then ok
	else bad "$name  [$m]" "result: $LAST_RES   input: $(short "$@")"; fi
}

check_zero_ops() { # name mode args...
	local name=$1 m=$2; shift 2
	simulate "$m" "$@"
	if [ "$LAST_RES" = "OK" ] && [ "$LAST_OPS" -eq 0 ]; then ok
	else bad "$name  [$m]" "already sorted: expected 0 moves, got $LAST_OPS (result $LAST_RES)"; fi
}

check_error() { # name mode args...
	local name=$1 m=$2; shift 2
	local so se
	so=$(run_ps "$m" "$@" 2>/tmp/ps_err.$$)
	se=$(cat /tmp/ps_err.$$); rm -f /tmp/ps_err.$$
	if [ "$se" = "Error" ] && [ -z "$so" ]; then ok
	else bad "$name  [$m]" "expected only 'Error' on stderr. stdout='$(short "$so")' stderr='$(short "$se")'"; fi
}

label() { # n avg
	local n=$1 avg=$2 p g e
	if [ "$n" -eq 100 ]; then p=2000; g=1500; e=700; else p=12000; g=8000; e=5500; fi
	if   [ "$avg" -lt "$e" ]; then echo "${G}excellent${N}"
	elif [ "$avg" -lt "$g" ]; then echo "${G}good${N}"
	elif [ "$avg" -lt "$p" ]; then echo "${Y}pass${N}"
	else echo "${R}TOO MANY${N}"; fi
}

# ============================================================
echo "${B}== 1. Errors (stderr = Error, stdout empty) ==${N}"
for m in "${MODES[@]}"; do
	check_error "letters"          "$m" 0 one 2 3
	check_error "duplicates"       "$m" 3 2 3
	check_error "dup negatives"    "$m" -1 5 -1
	check_error "above INT_MAX"    "$m" 1 2147483648
	check_error "below INT_MIN"    "$m" 1 -2147483649
	check_error "mixed chars"      "$m" 1 2 3a
	check_error "only a sign"      "$m" 1 - 2
	check_error "empty string"     "$m" ""
	check_error "huge number"      "$m" 99999999999999999999
done

echo "${B}== 2. No arguments (nothing printed) ==${N}"
out=$("$BIN" 2>&1); [ -z "$out" ] && ok || bad "no args" "printed: $(short "$out")"
for m in --simple --medium --complex; do
	out=$("$BIN" "$m" 2>&1)
	[ -z "$out" ] && ok || bad "flag only [$m]" "printed: $(short "$out")"
done

echo "${B}== 3. Edge cases (sorted = 0 moves) ==${N}"
for m in "${MODES[@]}"; do
	check_zero_ops "one number"       "$m" 42
	check_zero_ops "two sorted"       "$m" 1 2
	check_zero_ops "sorted 5"         "$m" 1 2 3 4 5
	check_zero_ops "sorted negatives" "$m" -5 -3 -1 0 2
	check_zero_ops "sorted 100"       "$m" $(gen_sorted 100)
	check_sort "two reversed"   "$m" 2 1
	check_sort "three 2 1 0"    "$m" 2 1 0
	check_sort "INT limits"     "$m" 2147483647 -2147483648 0
	check_sort "reverse 5"      "$m" 5 4 3 2 1
	check_sort "reverse 100"    "$m" $(gen_reverse 100)
	check_sort "nearly sorted"  "$m" $(gen_nearly 100)
	check_sort "nearly sorted 7" "$m" $(gen_nearly 7)
done

echo "${B}== 4. All permutations of 3 and 4 numbers ==${N}"
perms() { # print all permutations of the given words, one per line
	if [ $# -le 1 ]; then echo "$*"; return; fi
	local i rest
	for ((i = 1; i <= $#; i++)); do
		rest=("${@:1:i-1}" "${@:i+1}")
		for p in $(perms "${rest[@]}" | tr ' ' ','); do
			echo "${!i} ${p//,/ }"
		done
	done
}
for m in "${MODES[@]}"; do
	while read -r line; do check_sort "perm" "$m" $line; done < <(perms 1 2 3)
	while read -r line; do check_sort "perm" "$m" $line; done < <(perms 4 1 3 2)
done

echo "${B}== 5. Option position ==${N}"
for m in --simple --medium --complex; do
	check_sort "flag first" "$m" 3 1 2 5 4
	simulate_out=$("$BIN" 3 1 2 5 4 "$m" 2>/dev/null)
	res=$(printf '%s\n' "$simulate_out" | awk -v nums="3 1 2 5 4" "$AWKPROG")
	[ "$res" = "OK" ] && ok || bad "flag last [$m]" "result: $res (ok to ignore if your parser forbids it)"
done

echo "${B}== 6. Random tests (different every run) ==${N}"
for m in "${MODES[@]}"; do
	for n in 2 3 4 5 6 7 8 10 15 20 50; do
		for _ in 1 2 3 4 5; do check_sort "random $n" "$m" $(gen_random "$n"); done
	done
	for _ in 1 2 3; do check_sort "random 100" "$m" $(gen_random 100); done
	check_sort "random 500" "$m" $(gen_random 500)
done

echo "${B}== 7. Performance (100 numbers x10, 500 numbers x5) ==${N}"
for m in --simple --medium --complex; do
	for cfg in "100 10" "500 5"; do
		n=${cfg% *}; runs=${cfg#* }
		sum=0; worst=0; broken=0
		for ((r = 0; r < runs; r++)); do
			simulate "$m" $(gen_random "$n")
			[ "$LAST_RES" = "OK" ] || broken=1
			sum=$((sum + LAST_OPS))
			[ "$LAST_OPS" -gt "$worst" ] && worst=$LAST_OPS
		done
		avg=$((sum / runs))
		line="$m n=$n: average $avg, worst $worst -> $(label "$n" "$avg")"
		echo "   $line"
		if [ "$broken" -eq 1 ]; then bad "perf run not sorted [$m n=$n]"
		else ok; fi
	done
done
echo "   (move counts are shown for information; only a wrong sort counts as a fail)"

echo "${B}== 8. Memory (only with MEM=1) ==${N}"
if [ "$MEM" = "1" ]; then
	args=$(gen_random 50)
	for m in --simple --medium --complex; do
		if command -v valgrind >/dev/null 2>&1; then
			valgrind --leak-check=full --error-exitcode=42 -q "$BIN" "$m" $args >/dev/null 2>/tmp/ps_mem.$$
			[ $? -ne 42 ] && ok || bad "valgrind [$m]" "$(head -3 /tmp/ps_mem.$$)"
			valgrind --leak-check=full --error-exitcode=42 -q "$BIN" "$m" 1 a 3 >/dev/null 2>/tmp/ps_mem.$$
			[ $? -ne 42 ] && ok || bad "valgrind on error input [$m]"
			rm -f /tmp/ps_mem.$$
		elif command -v leaks >/dev/null 2>&1; then
			leaks --atExit -- "$BIN" "$m" $args >/tmp/ps_mem.$$ 2>&1
			grep -q "0 leaks for 0 total leaked bytes" /tmp/ps_mem.$$ && ok || bad "leaks [$m]" "run: leaks --atExit -- $BIN $m <numbers>"
			rm -f /tmp/ps_mem.$$
		else
			echo "   no valgrind or leaks found, skipped"; break
		fi
	done
else
	echo "   skipped (run with: MEM=1 ./tester.sh)"
fi

echo
echo "Result: ${G}$PASS passed${N}, ${R}$FAIL failed${N}"
[ "$FAIL" -eq 0 ] && echo "${G}ALL TESTS PASSED${N}" || echo "${R}SOME TESTS FAILED${N}"
[ "$FAIL" -eq 0 ]
