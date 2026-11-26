#!/usr/bin/env python3
"""
push_swap FULL tester  -  follows subject v1.2

Quick start (run it from your project folder, next to the Makefile):
    python3 push_swap_tester.py                 core tests (fast)
    python3 push_swap_tester.py --all           everything (needs valgrind / norminette for some parts)

Groups of tests (the name in brackets is used with --only / --skip):
    [make]        Makefile rules, flags, relink, clean / fclean / re          (--make)
    [static]      README rules, forbidden functions, globals, norm            (--static)
    [errors]      bad input must print "Error\\n" on stderr only
    [quiet]       cases where nothing must be printed
    [formats]     many args, one quoted string, signs, INT_MIN/INT_MAX, subject examples
    [flags]       --simple --medium --complex --adaptive and the default
    [perms]       every permutation of 2..5 (and 6) numbers, for every strategy
    [output]      stdout format, determinism, --bench must not change stdout
    [bench]       --bench text: disorder %, strategy, complexity, total, per-op counts
    [regimes]     adaptive must use O(n^2) / O(n sqrt n) / O(n log n) by disorder
    [special]     sorted, reversed, almost sorted, tiny sizes
    [random]      100 and 500 random numbers graded with the subject limits
    [complexity]  measured growth of every strategy (n^2, n sqrt n, n log n)
    [stress]      1000 / 2000 / 5000 numbers and timing                       (--stress)
    [valgrind]    leaks and invalid memory access                              (--valgrind)
    [checker]     bonus checker                                                (--bonus)

Other options: -p PROGRAM, -c CHECKER, --ref-checker PATH, -r RUNS, --seed N,
               --quick, -q (hide PASS lines), --only a,b  --skip a,b, --list, --no-color
"""
import argparse
import glob
import itertools
import math
import os
import random
import re
import shlex
import shutil
import statistics
import subprocess
import sys
import time
from collections import namedtuple

INT_MIN = -2147483648
INT_MAX = 2147483647
OPS = ["sa", "sb", "ss", "pa", "pb", "ra", "rb", "rr", "rra", "rrb", "rrr"]
VALID_OPS = set(OPS)
STRATS = ["--simple", "--medium", "--complex", "--adaptive"]
KNOWN_FLAGS = set(STRATS + ["--bench"])
STRAT_CLASS = {"--simple": "n2", "--medium": "nsqrtn", "--complex": "nlogn"}
CLASS_NAME = {"n2": "O(n^2)", "nsqrtn": "O(n sqrt n)", "nlogn": "O(n log n)"}

# size -> (excellent, good, pass); the number of operations must be STRICTLY lower
BENCH = {100: (700, 1500, 2000), 500: (5500, 8000, 12000)}

Res = namedtuple("Res", "out err rc dt")

OPT = None
COL = {"G": "\033[92m", "R": "\033[91m", "Y": "\033[93m", "B": "\033[96m", "X": "\033[0m"}
RESULTS = {}          # section -> [passed, failed, warned]
FAILS = []            # (section, name, info, repro)
MOVE_STATS = []       # (label, size, min, avg, max, grade)
CUR = ["-"]
REPRO_N = [0]


# =============================================================== reporting
def _bump(i):
    RESULTS.setdefault(CUR[0], [0, 0, 0])[i] += 1


def report(name, ok, info="", args=None):
    if ok:
        _bump(0)
        if not OPT.quiet:
            print(f"{COL['G']}[PASS]{COL['X']} {name} {info}")
        return
    _bump(1)
    print(f"{COL['R']}[FAIL]{COL['X']} {name} {info}")
    rp = repro(args) if args is not None else None
    if rp:
        print(f"       reproduce: {rp}")
    FAILS.append((CUR[0], name, info, rp))


def warn(name, info=""):
    _bump(2)
    print(f"{COL['Y']}[WARN]{COL['X']} {name} {info}")


def info(text):
    print(f"{COL['B']}[INFO]{COL['X']} {text}")


def section(title):
    print(f"\n{COL['Y']}== {title} =={COL['X']}")


def repro(args):
    cmd = shlex.join([OPT.program] + [str(a) for a in args])
    if len(cmd) <= 300:
        return cmd
    os.makedirs("tester_failed_inputs", exist_ok=True)
    REPRO_N[0] += 1
    path = f"tester_failed_inputs/case_{REPRO_N[0]}.txt"
    flags = [a for a in args if a in KNOWN_FLAGS]
    nums = [a for a in args if a not in KNOWN_FLAGS]
    with open(path, "w") as f:
        f.write(" ".join(nums))
    return f"{OPT.program} {' '.join(flags)} $(cat {path})".replace("  ", " ")


def short(args, limit=70):
    s = str(list(args))
    return s if len(s) <= limit else s[:limit - 3] + "..."


# ================================================================ helpers
def run(prog, args, stdin=None, timeout=None):
    t = time.perf_counter()
    try:
        p = subprocess.run([prog] + [str(a) for a in args], capture_output=True, text=True,
                           errors="replace", input=stdin, timeout=timeout or OPT.timeout)
    except subprocess.TimeoutExpired:
        return None
    except OSError as e:
        return Res("", str(e), 127, 0.0)
    return Res(p.stdout, p.stderr, p.returncode, time.perf_counter() - t)


def disorder(nums):
    """Disorder exactly as defined in the subject (0..1)."""
    mistakes = 0
    total = 0
    n = len(nums)
    for i in range(n):
        for j in range(i + 1, n):
            total += 1
            if nums[i] > nums[j]:
                mistakes += 1
    return mistakes / total if total else 0.0


def simulate(numbers, moves):
    """Replay the moves (top of a stack = index 0). Return (ok, message)."""
    a = list(numbers)
    b = []
    for m in moves:
        if m not in VALID_OPS:
            return False, f"invalid instruction {m!r}"
        if m in ("sa", "ss") and len(a) > 1:
            a[0], a[1] = a[1], a[0]
        if m in ("sb", "ss") and len(b) > 1:
            b[0], b[1] = b[1], b[0]
        if m == "pa" and b:
            a.insert(0, b.pop(0))
        if m == "pb" and a:
            b.insert(0, a.pop(0))
        if m in ("ra", "rr") and len(a) > 1:
            a.append(a.pop(0))
        if m in ("rb", "rr") and len(b) > 1:
            b.append(b.pop(0))
        if m in ("rra", "rrr") and len(a) > 1:
            a.insert(0, a.pop())
        if m in ("rrb", "rrr") and len(b) > 1:
            b.insert(0, b.pop())
    if b:
        return False, "stack b is not empty at the end"
    if a != sorted(numbers):
        return False, "stack a is not sorted at the end"
    return True, "ok"


def solve(prog, numbers, flags=(), one_string=False):
    """Run push_swap, replay its output, return a dict with the verdict."""
    args = list(flags)
    if one_string:
        args.append(" ".join(map(str, numbers)))
    else:
        args += [str(n) for n in numbers]
    d = dict(ok=False, n=0, msg="", err="", moves=[], out="", args=args, dt=0.0)
    res = run(prog, args)
    if res is None:
        d["msg"] = f"timeout (> {OPT.timeout}s)"
        return d
    d.update(err=res.err, out=res.out, dt=res.dt)
    if res.rc < 0:
        d["msg"] = f"crashed (signal {-res.rc})"
        return d
    moves = res.out.split("\n")
    if moves and moves[-1] == "":
        moves.pop()
    d["moves"] = moves
    d["n"] = len(moves)
    ok, msg = simulate(numbers, moves)
    if ok and res.out and not res.out.endswith("\n"):
        ok, msg = False, "output does not end with a newline"
    if ok and "--bench" not in flags and res.err:
        ok, msg = False, f"unexpected stderr without --bench: {res.err.strip()[:80]!r}"
    if not ok and msg.startswith("invalid instruction") and "--bench" in flags:
        msg += " -> bench text seems to be printed on stdout, it must go to stderr"
    d["ok"], d["msg"] = ok, msg
    return d


def report_solve(name, r, extra=""):
    report(name, r["ok"], extra if r["ok"] else r["msg"], r["args"])


def label(flag):
    return flag if flag else "default"


def grade(size, worst):
    ex, good, ps = BENCH[size]
    if worst < ex:
        return "excellent", True
    if worst < good:
        return "good", True
    if worst < ps:
        return "pass only", True
    return f"OVER the pass limit ({ps})", False


def gen_random(size, extremes=True):
    pool = random.sample(range(-100000, 100000), size)
    if extremes and size >= 5 and random.random() < 0.3:
        pool[0], pool[1] = INT_MIN, INT_MAX
    return pool


def data_with_inversions(n, k):
    """A list of n distinct numbers with EXACTLY k inversions (disorder = k / (n(n-1)/2))."""
    total = n * (n - 1) // 2
    k = max(0, min(k, total))
    caps = [n - 1 - i for i in range(n)]
    c = caps[:] if k == total else [0] * n
    rem = 0 if k == total else k
    while rem > 0:
        i = random.randrange(n)
        if c[i] < caps[i]:
            c[i] += 1
            rem -= 1
    avail = list(range(n))
    perm = [avail.pop(ci) for ci in c]
    vals = sorted(random.sample(range(-100000, 100000), n))
    return [vals[p] for p in perm]


def data_with_disorder(n, frac):
    total = n * (n - 1) // 2
    return data_with_inversions(n, int(round(frac * total)))


# ------------------------------------------------------- bench text parsing
def op_count_in(err, op):
    p1 = rf"(?<![a-z]){op}(?![a-z])\s*[:=\-]*\s*(\d+)"
    p2 = rf"(\d+)\s*[x*]?\s*(?<![a-z]){op}(?![a-z])"
    m = re.search(p1, err) or re.search(p2, err)
    return int(m.group(1)) if m else None


def detect_class(text):
    """Which complexity classes does the bench text mention? subset of n2 / nsqrtn / nlogn."""
    found = set()
    for g in re.findall(r"o\(([^\n]*?)\)", text.lower()):
        if "log" in g:
            found.add("nlogn")
        elif "sqrt" in g or "\u221a" in g or "1.5" in g or "3/2" in g:
            found.add("nsqrtn")
        elif re.search(r"n\s*(\^|\*\*)\s*2|n\u00b2|n\s*\*\s*n|n2", g):
            found.add("n2")
    return found


def check_bench(err, numbers, moves, flag):
    """Return (problems, warnings) for the --bench text written on stderr."""
    low = err.lower()
    problems, warnings = [], []
    m = re.search(r"(\d+\.\d{2})\s*%", err)
    if not m:
        problems.append("disorder is not shown as a percentage with two decimals")
    else:
        shown = float(m.group(1))
        expected = disorder(numbers) * 100
        if abs(shown - expected) > 0.011:
            problems.append(f"disorder shown {shown}% but expected {expected:.2f}%")
    if "o(" not in low:
        problems.append("no complexity class (like O(n^2)) found")
    elif flag in STRAT_CLASS:
        cls = detect_class(low)
        if cls and STRAT_CLASS[flag] not in cls:
            problems.append(f"{flag} should show {CLASS_NAME[STRAT_CLASS[flag]]} but shows {sorted(cls)}")
        elif not cls:
            warnings.append("could not recognise the complexity class text")
    if flag in STRAT_CLASS:
        word = flag[2:]
        if word not in low:
            warnings.append(f"strategy name does not contain '{word}' (fine if your names differ)")
    tm = re.search(r"total[^\n\d]*(\d+)", low)
    if tm:
        if int(tm.group(1)) != len(moves):
            problems.append(f"total shown {tm.group(1)} but {len(moves)} operations printed")
    elif str(len(moves)) not in re.findall(r"\d+", low):
        problems.append("total number of operations not found")
    for op in OPS:
        shown = op_count_in(low, op)
        real = moves.count(op)
        if shown is None:
            problems.append(f"count for {op} not found")
        elif shown != real:
            problems.append(f"{op}: shown {shown} but real {real}")
    return problems, warnings


# ============================================================ [make] tests
def sh(cmd, timeout=300):
    return subprocess.run(cmd, capture_output=True, text=True, errors="replace", timeout=timeout)


def mtime(path):
    return os.path.getmtime(path) if os.path.exists(path) else None


def test_make(prog):
    section("Makefile")
    if not os.path.exists("Makefile"):
        report("Makefile exists", False)
        return
    name = os.path.basename(prog)
    for rule in ("all", "clean", "fclean", "re", name):
        p = sh(["make", "-n", rule])
        report(f"rule '{rule}' exists", p.returncode == 0)
    sh(["make", "fclean"])
    p = sh(["make", "-n"])
    out = p.stdout
    report("compiles with cc", bool(re.search(r"(^|\s)cc\s", out)) and not re.search(r"\b(gcc|clang)\b", out))
    report("flags -Wall -Wextra -Werror", all(f in out for f in ("-Wall", "-Wextra", "-Werror")))
    b = sh(["make"])
    report("make builds the program", b.returncode == 0 and os.path.exists(prog),
           "" if b.returncode == 0 else b.stderr.strip()[-200:])
    if not os.path.exists(prog):
        return
    t1 = mtime(prog)
    time.sleep(1.1)
    b2 = sh(["make"])
    relinked = bool(re.search(r"\b(cc|gcc|clang)\b", b2.stdout)) or mtime(prog) != t1
    report("second 'make' does not relink", b2.returncode == 0 and not relinked)
    srcs = [f for f in glob.glob("*.c") + glob.glob("src/**/*.c", recursive=True)]
    if srcs:
        os.utime(srcs[0], None)
        time.sleep(1.1)
        b3 = sh(["make"])
        report("touching a .c file rebuilds", b3.returncode == 0 and mtime(prog) != t1)
    sh(["make", "clean"])
    objs = [f for f in glob.glob("**/*.o", recursive=True) if "libft" not in f]
    report("make clean removes the .o files", not objs)
    report("make clean keeps the program", os.path.exists(prog))
    sh(["make", "fclean"])
    report("make fclean removes the program", not os.path.exists(prog))
    sh(["make", "re"])
    report("make re rebuilds the program", os.path.exists(prog))
    t2 = mtime(prog)
    time.sleep(1.1)
    b4 = sh(["make", "all"])
    report("'make all' after build does not relink", mtime(prog) == t2 and not re.search(r"\b(cc|gcc|clang)\b", b4.stdout))
    if OPT.bonus_enabled:
        chk = OPT.checker
        p = sh(["make", "-n", "bonus"])
        report("rule 'bonus' exists", p.returncode == 0)
        sh(["make", "bonus"])
        report("make bonus builds the checker", os.path.exists(chk))
        t3 = mtime(chk)
        time.sleep(1.1)
        b5 = sh(["make", "bonus"])
        report("second 'make bonus' does not relink",
               mtime(chk) == t3 and not re.search(r"\b(cc|gcc|clang)\b", b5.stdout))
        bonus_files = glob.glob("**/*_bonus.c", recursive=True)
        if not bonus_files:
            warn("no *_bonus.c files found", "bonus code must be in _bonus.{c,h} files")
        sh(["make", "fclean"])
        sh(["make"])
        if os.path.exists(chk):
            warn("plain 'make' also built the checker", "bonus should only be built by 'make bonus'")
        sh(["make", "bonus"])   # leave both programs built for the next tests


# ============================================================ [static] tests
def read_text(path):
    try:
        with open(path, encoding="utf-8", errors="replace") as f:
            return f.read()
    except OSError:
        return None


def test_readme():
    text = read_text("README.md")
    if text is None:
        report("README.md exists at the root", False)
        return
    report("README.md exists at the root", True)
    lines = text.splitlines()
    first = lines[0].strip() if lines else ""
    phrase = "This project has been created as part of the 42 curriculum by"
    italic = bool(re.fullmatch(r"(\*|_)(?![\s*_]).*(?<![\s*_])(\*|_)", first)) and not first.startswith(("**", "__"))
    report("first line is italic", italic, f"first line is: {first[:70]!r}" if not italic else "")
    report("first line has the required sentence", phrase in first)
    m = re.search(re.escape(phrase) + r"\s*(.+?)[*_]*$", first)
    logins = re.split(r"\s*,\s*|\s+and\s+", m.group(1).strip("*_ .")) if m else []
    report("first line lists the logins", len([x for x in logins if x]) >= 1, f"found {logins}")
    if len([x for x in logins if x]) < 2:
        warn("only one login on the first line", "this is a group project of exactly 2 learners")
    for head in ("description", "instructions", "resources"):
        found = re.search(rf"^#{{1,6}}\s*.*{head}", text, re.I | re.M)
        report(f"README section '{head.capitalize()}'", bool(found))
    report("Resources explains how AI was used", bool(re.search(r"\bAI\b|chatgpt|claude|copilot|gemini|llm", text, re.I)))
    for word in ("simple", "medium", "complex", "adaptive"):
        if not re.search(word, text, re.I):
            warn(f"README does not mention '{word}'")
    report("README gives complexity classes", len(re.findall(r"O\(", text)) >= 3)
    thr = "0.2" in text and "0.5" in text
    report("README explains the thresholds 0.2 and 0.5", thr)
    if not re.search(r"disorder", text, re.I):
        warn("README does not mention the disorder metric")
    if not re.search(r"contribut", text, re.I):
        warn("README has no contributions of the two learners", "subject asks for it")


def test_static(prog):
    section("Static checks (files, README, forbidden functions, norm)")
    srcs = [f for f in glob.glob("**/*.c", recursive=True) if ".git" + os.sep not in f]
    hdrs = [f for f in glob.glob("**/*.h", recursive=True) if ".git" + os.sep not in f]
    report("Makefile present", os.path.exists("Makefile"))
    report("has .c files", bool(srcs))
    report("has .h files", bool(hdrs))
    mk = read_text("Makefile") or ""
    if "libft" in mk:
        report("libft folder exists (Makefile uses it)", os.path.isdir("libft"))
    for stray in ("a.out",):
        if os.path.exists(stray):
            warn(f"stray file {stray}")
    if shutil.which("git") and os.path.isdir(".git"):
        p = sh(["git", "ls-files"])
        bad = [f for f in p.stdout.split("\n") if f.endswith((".o", ".a", ".out")) or
               os.path.basename(f) in ("push_swap", "checker")]
        if bad:
            warn("build files are tracked by git", ", ".join(bad[:5]))
    test_readme()

    # forbidden functions: undefined symbols of the binary
    if os.path.exists(prog) and shutil.which("nm"):
        p = sh(["nm", "-u", prog])
        syms = []
        for line in p.stdout.splitlines():
            parts = line.split()
            if not parts:
                continue
            name = parts[-1].split("@")[0]
            if sys.platform == "darwin" and name.startswith("_"):
                name = name[1:]
            if name.startswith("_") or name.startswith("dyld") or name.startswith("ITM"):
                continue
            syms.append(name)
        allowed = {"read", "write", "malloc", "free", "exit"}
        hard = {"printf", "fprintf", "dprintf", "sprintf", "snprintf", "puts", "putchar", "atoi", "atol",
                "strtol", "strlen", "strcmp", "strncmp", "strdup", "calloc", "realloc", "fopen", "getenv",
                "strcpy", "strncpy", "strchr", "strcat", "memcmp", "abs", "pow", "sqrt", "qsort"}
        extra = sorted(set(syms) - allowed)
        bad = [s for s in extra if s in hard]
        soft = [s for s in extra if s not in hard]
        report("binary uses only authorized libc functions", not bad,
               f"forbidden: {bad}" if bad else "")
        if soft:
            warn("binary imports other libc symbols", f"{soft} (the compiler may add memcpy/memset; check your code)")
    # global variables: look at the object files
    objs = [f for f in glob.glob("**/*.o", recursive=True)]
    if objs and shutil.which("nm"):
        gl, st = [], []
        for o in objs:
            p = sh(["nm", "--defined-only", o])
            for line in p.stdout.splitlines():
                parts = line.split()
                if len(parts) >= 3:
                    typ, nm = parts[-2], parts[-1]
                    if typ in ("B", "D", "C"):
                        gl.append(f"{nm} ({os.path.basename(o)})")
                    elif typ in ("b", "d") and not nm.startswith(("completed", ".")):
                        st.append(f"{nm} ({os.path.basename(o)})")
        report("no global variables (object files)", not gl, f"found: {gl[:5]}" if gl else "")
        if st:
            warn("static variables found", f"{st[:5]} (global variables are forbidden, check them)")
    elif not objs:
        info("no .o files found, run 'make' first to check global variables")
    # norm
    if shutil.which("norminette"):
        p = sh(["norminette"])
        errs = [l for l in p.stdout.splitlines() if "Error" in l]
        report("norminette", p.returncode == 0 and not errs,
               "" if p.returncode == 0 and not errs else f"{len(errs)} errors, first: {errs[0] if errs else ''}")
    else:
        warn("norminette not installed", "light scan of forbidden constructs instead")
        for f in srcs + hdrs:
            if "libft" in f:
                continue
            txt = read_text(f) or ""
            body = re.sub(r"/\*.*?\*/", "", txt, flags=re.S)
            for pat, what in ((r"\bfor\s*\(", "for loop"), (r"\bswitch\s*\(", "switch"),
                              (r"\bgoto\b", "goto"), (r"\bdo\s*\{", "do-while"),
                              (r"\w\s*\?\s*[^:\n]+:", "ternary")):
                if re.search(pat, body):
                    warn(f"{f}: possible {what}", "forbidden by the Norm")


# ============================================================ [errors] tests
def long_valid(n=200):
    return [str(x) for x in random.sample(range(-5000, 5000), n)]


def test_errors(prog):
    section("Error cases (stderr must be exactly 'Error\\n', stdout empty)")
    bad = [
        ["1", "a", "3"], ["12abc"], ["abc"], ["1", "2", "2"], ["1", "1"],
        ["2147483648"], ["-2147483649"], ["99999999999999999999"], ["9" * 50], ["-" + "9" * 50],
        [str(INT_MAX), str(INT_MAX + 1)], [str(INT_MIN - 1), "0"],
        ["+"], ["-"], [""], [" "], ["\t"], ["1", "", "3"], ["1 2 2"], ["1", "--2"], ["1", "-+2"],
        ["1", "2", "3a"], ["0", "-0"], ["1", "+1"], ["0001", "1"], ["1,2"], ["1.5"], ["1e3"], ["0x10"],
        ["--"], ["--foo", "1", "2"], ["-", "1"], ["1", "+"], ["- 1"], ["1 - 2"],
        ["--adaptive", "0", "one", "2", "3"], ["--simple", "3", "2", "3"], ["--medium", "1", "1"],
        ["--complex", "2147483648"], ["--adaptive", "--simple", "a"],
        ["--bench", "1", "a"], ["--bench", "--simple", "5", "5"], ["--bench", "--medium", "x", "1"],
    ]
    v = long_valid(200)
    bad.append(v + ["x"])
    bad.append(["x"] + v)
    bad.append(v + [v[10]])
    bad.append(v[:100] + ["99999999999"] + v[100:])
    bad.append(["--complex"] + v + ["3.5"])
    for args in bad:
        nm = f"error {short(args)}"
        res = run(prog, args)
        if res is None:
            report(nm, False, "timeout", args)
        elif res.rc < 0:
            report(nm, False, f"crashed (signal {-res.rc})", args)
        else:
            ok = res.err == "Error\n" and res.out == ""
            report(nm, ok, "" if ok else f"stdout={res.out[:40]!r} stderr={res.err[:40]!r}", args)


def test_quiet(prog):
    section("Quiet cases (nothing must be printed)")
    cases = [[], ["42"], ["1", "2", "3"], ["-5", "0", "7", "100"], ["1 2 3"],
             [str(INT_MIN), str(INT_MAX)], ["--simple", "1", "2", "3"], ["--adaptive", "7"],
             ["--medium", "1", "2", "3", "4", "5"], ["--complex", "-3", "-2", "-1"],
             sorted(str(x) for x in range(0, 0))]
    cases.append([str(x) for x in range(100)])
    cases.append([str(x) for x in range(-250, 250)])
    for args in cases:
        res = run(prog, args)
        nm = f"quiet {short(args)}"
        if res is None:
            report(nm, False, "timeout", args)
            continue
        ok = res.out == "" and res.err == "" and res.rc >= 0
        report(nm, ok, "" if ok else f"stdout={res.out[:40]!r} stderr={res.err[:40]!r} rc={res.rc}", args)
    for f in STRATS:
        res = run(prog, [f])
        if res is None or res.rc < 0:
            report(f"flag alone {f}", False, "timeout or crash", [f])
        elif res.out == "" and res.err in ("", "Error\n"):
            report(f"flag alone {f} prints no operations", True)
        else:
            warn(f"flag alone {f}", f"stdout={res.out[:30]!r} stderr={res.err[:30]!r}")


# ============================================================ [formats] tests
def accepted_or_error(prog, args, numbers, name):
    """Either a correct sort or a clean 'Error' is acceptable (subject does not say)."""
    res = run(prog, args)
    if res is None or res.rc < 0:
        report(name, False, "timeout or crash", args)
        return
    if res.err == "Error\n" and res.out == "":
        report(name, True, "(rejected with Error)")
        return
    moves = [m for m in res.out.split("\n") if m != ""]
    ok, msg = simulate(numbers, moves)
    report(name, ok and res.err == "", "" if ok else msg, args)


def test_formats(prog):
    section("Input formats and subject examples")
    nums = [3, -1, 2, 0, 5]
    for one in (False, True):
        r = solve(prog, nums, one_string=one)
        report_solve("one quoted string" if one else "many arguments", r, f"{r['n']} moves")
    report_solve("INT_MIN / INT_MAX", solve(prog, [INT_MIN, INT_MAX, 0, -1, 1]))
    report_solve("INT_MAX first", solve(prog, [INT_MAX, 5, 4, INT_MIN]))
    res = run(prog, ["+3", "-2", "01", "0"])
    if res is None or res.rc < 0:
        report("signs and leading zeros (+3 -2 01 0)", False, "timeout or crash")
    else:
        ok, msg = simulate([3, -2, 1, 0], [m for m in res.out.split("\n") if m])
        report("signs and leading zeros (+3 -2 01 0)", ok and res.err == "", "" if ok else msg)
    accepted_or_error(prog, ["3 2", "1"], [3, 2, 1], "mixed quoted + separate args")
    accepted_or_error(prog, [" 3 2 1 "], [3, 2, 1], "spaces around a quoted string")
    accepted_or_error(prog, ["3   2    1"], [3, 2, 1], "several spaces inside a quoted string")
    report_solve("subject: 2 1 3 6 5 8", solve(prog, [2, 1, 3, 6, 5, 8]))
    r = solve(prog, [4, 67, 3, 87, 23], ("--adaptive",))
    report_solve("subject: --adaptive 4 67 3 87 23", r, f"{r['n']} moves")
    report_solve("subject: --simple 5 4 3 2 1", solve(prog, [5, 4, 3, 2, 1], ("--simple",)))
    report_solve("subject: --complex 4 67 3 87 23", solve(prog, [4, 67, 3, 87, 23], ("--complex",)))
    report_solve("2 numbers reversed", solve(prog, [2, 1]))
    report_solve("1000 numbers in one quoted string", solve(prog, gen_random(1000), one_string=True))
    for f in STRATS:
        report_solve(f"{f} with one quoted string", solve(prog, gen_random(30), (f,), one_string=True))


# ============================================================== [flags] tests
def test_flags(prog):
    section("Strategy flags")
    nums = random.sample(range(-100, 100), 12)
    for flag in STRATS:
        r = solve(prog, nums, (flag,))
        report_solve(f"{flag} sorts 12 numbers", r, f"{r['n']} moves")
    d = solve(prog, nums)
    a = solve(prog, nums, ("--adaptive",))
    report("default behaves like --adaptive", d["ok"] and d["out"] == a["out"], "", d["args"])
    for flag in STRATS:
        for size in (1, 2, 3, 4):
            data = random.sample(range(-50, 50), size)
            report_solve(f"{flag} on {size} number(s)", solve(prog, data, (flag,)))
    for flag in STRATS:
        for size in (100, 500):
            r = solve(prog, gen_random(size), (flag,))
            report_solve(f"{flag} works on {size} numbers (any size, any disorder)", r, f"{r['n']} moves")
    base = random.sample(range(1000), 20)
    r1 = solve(prog, base, ("--simple", "--bench"))
    r2 = solve(prog, base, ("--bench", "--simple"))
    report("flags can be in any order (--simple --bench / --bench --simple)",
           r1["ok"] and r2["ok"] and r1["out"] == r2["out"], r1["msg"] or r2["msg"], r1["args"])


# =============================================================== [perms] tests
def test_perms(prog):
    section("All permutations (sizes 2 to 5, 6 for the default) for every strategy")
    for flag in [None] + STRATS:
        flags = (flag,) if flag else ()
        sizes = [2, 3, 4, 5] + ([6] if flag is None and not OPT.quick else [])
        for size in sizes:
            worst, bad = 0, None
            for perm in itertools.permutations(range(size)):
                r = solve(prog, list(perm), flags)
                worst = max(worst, r["n"])
                if not r["ok"]:
                    bad = r
                    break
            nm = f"{label(flag)} size {size}"
            if bad:
                report(nm, False, bad["msg"], bad["args"])
            else:
                report(nm, True, f"worst = {worst} moves")


# ============================================================== [output] tests
def test_output(prog):
    section("Output format, determinism, --bench must not change stdout")
    for flag in STRATS:
        nums = random.sample(range(-1000, 1000), 25)
        plain = solve(prog, nums, (flag,))
        again = solve(prog, nums, (flag,))
        bench = solve(prog, nums, (flag, "--bench"))
        report_solve(f"{flag} plain run is valid", plain)
        report(f"{flag} is deterministic", plain["out"] == again["out"], "", plain["args"])
        report(f"{flag} --bench keeps stdout identical", plain["ok"] and bench["ok"] and plain["out"] == bench["out"],
               bench["msg"] if not bench["ok"] else "", bench["args"])
        if plain["out"]:
            clean = all(l in VALID_OPS for l in plain["out"].split("\n")[:-1]) and plain["out"].endswith("\n") \
                and "\r" not in plain["out"] and "  " not in plain["out"] and "\n\n" not in plain["out"]
            report(f"{flag} operations are separated by a single \\n", clean, "", plain["args"])
    ranks = random.sample(range(30), 30)
    big = [r * 70000 - 1000000000 for r in ranks]
    r1 = solve(prog, ranks)
    r2 = solve(prog, big)
    if r1["ok"] and r2["ok"] and r1["out"] != r2["out"]:
        warn("same order but different values give different operations",
             "usually the algorithm should work on ranks, not on raw values")
    else:
        report("output depends only on the order (ranks), not on the values", r1["ok"] and r2["ok"],
               r1["msg"] or r2["msg"])


# =============================================================== [bench] tests
def test_bench(prog):
    section("--bench mode")
    nums = random.sample(range(-1000, 1000), 30)
    for flag in STRATS:
        for order in ((flag, "--bench"), ("--bench", flag)):
            r = solve(prog, nums, order)
            nm = " ".join(order)
            if not r["ok"]:
                report(nm, False, r["msg"], r["args"])
                continue
            problems, warnings = check_bench(r["err"], nums, r["moves"], flag)
            report(nm, not problems, "; ".join(problems), r["args"])
            for w in warnings:
                warn(nm, w)
    r = solve(prog, nums, ("--bench",))
    if r["ok"]:
        problems, _ = check_bench(r["err"], nums, r["moves"], None)
        report("--bench alone (default strategy)", not problems, "; ".join(problems), r["args"])
        report("bench text is on stderr only", r["err"] != "" and all(l in VALID_OPS for l in r["out"].split("\n") if l),
               "", r["args"])
    else:
        report("--bench alone (default strategy)", False, r["msg"], r["args"])
    for label_, data in (("sorted list", [1, 2, 3, 4]), ("single number", [7])):
        res = run(prog, ["--bench"] + [str(x) for x in data])
        if res is None or res.rc < 0:
            report(f"--bench on {label_}", False, "timeout or crash", ["--bench"] + data)
        elif res.out != "":
            report(f"--bench on {label_}", False, f"stdout should be empty, got {res.out[:30]!r}", ["--bench"] + data)
        elif not res.err:
            warn(f"--bench on {label_}", "no bench text at all (the subject says bench appears after sorting)")
        else:
            report(f"--bench on {label_}", True)
    res = run(prog, ["--bench", "1", "x"])
    report("--bench with bad input prints only Error", res is not None and res.err == "Error\n" and res.out == "")
    nums = random.sample(range(-100000, 100000), 100)
    d = disorder(nums) * 100
    r = solve(prog, nums, ("--bench",))
    if r["ok"]:
        m = re.search(r"(\d+\.\d{2})\s*%", r["err"])
        report("disorder is measured BEFORE any move (100 numbers)", bool(m) and abs(float(m.group(1)) - d) < 0.011,
               f"expected {d:.2f}%" + (f", shown {m.group(1)}%" if m else ""), r["args"])


# ============================================================ [regimes] tests
def regime_case(prog, name, data, expected, exact=False):
    for flags in (("--adaptive",), ()):
        r = solve(prog, data, flags + ("--bench",))
        nm = f"{name} [{label(flags[0] if flags else None)}]"
        if not r["ok"]:
            report(nm, False, r["msg"], r["args"])
            continue
        cls = detect_class(r["err"])
        if cls == {expected}:
            report(nm, True, f"{r['n']} moves, {CLASS_NAME[expected]}")
        elif not cls:
            warn(nm, "sorted OK, but no complexity class could be read from the bench text")
        elif len(cls) > 1:
            warn(nm, f"bench text lists several classes {sorted(cls)}, cannot tell which one was used")
        else:
            msg = f"expected {CLASS_NAME[expected]} but the bench says {CLASS_NAME[next(iter(cls))]}"
            if exact:
                warn(nm, msg + " (exact boundary: check < and >= against 0.2 / 0.5, and float rounding)")
            else:
                report(nm, False, msg, r["args"])


def test_regimes(prog):
    section("Adaptive: the method must follow the disorder (<0.2 n^2, <0.5 n sqrt n, else n log n)")
    sizes = (100,) if OPT.quick else (100, 500)
    for n in sizes:
        total = n * (n - 1) // 2
        for frac, exp in ((0.02, "n2"), (0.10, "n2"), (0.19, "n2"),
                          (0.21, "nsqrtn"), (0.35, "nsqrtn"), (0.49, "nsqrtn"),
                          (0.51, "nlogn"), (0.75, "nlogn"), (1.0, "nlogn")):
            data = data_with_disorder(n, frac)
            regime_case(prog, f"n={n} disorder {frac:.2f} -> {CLASS_NAME[exp]}", data, exp)
        for k, exp, tag in ((int(round(0.2 * total)) - 1, "n2", "just below 0.2"),
                            (int(round(0.2 * total)), "nsqrtn", "exactly 0.2"),
                            (int(round(0.5 * total)) - 1, "nsqrtn", "just below 0.5"),
                            (int(round(0.5 * total)), "nlogn", "exactly 0.5")):
            data = data_with_inversions(n, k)
            regime_case(prog, f"n={n} {tag} ({k}/{total}) -> {CLASS_NAME[exp]}", data, exp, exact="exactly" in tag)
    info("moves of every strategy on the same input (100 numbers):")
    for frac in (0.05, 0.35, 0.75):
        data = data_with_disorder(100, frac)
        row = []
        for f in STRATS:
            r = solve(prog, data, (f,))
            row.append(f"{f[2:]}={r['n'] if r['ok'] else 'FAIL'}")
        print(f"       disorder {frac:.2f}: " + "  ".join(row))


# ============================================================== [special] tests
def test_special(prog):
    section("Special orders and small sizes")
    for size in (100, 500):
        base = random.sample(range(-100000, 100000), size)
        asc, desc = sorted(base), sorted(base, reverse=True)
        almost = asc[:]
        for _ in range(5):
            i, j = random.randrange(size), random.randrange(size)
            almost[i], almost[j] = almost[j], almost[i]
        rotated = asc[size // 3:] + asc[:size // 3]
        for nm, data in (("sorted", asc), ("reversed", desc), ("almost sorted", almost), ("rotated sorted", rotated)):
            for flag in [None] + STRATS[:3]:
                flags = (flag,) if flag else ()
                r = solve(prog, data, flags)
                tag = f"{nm} ({size}) {label(flag)}"
                if not r["ok"]:
                    report(tag, False, r["msg"], r["args"])
                elif nm == "sorted":
                    report(tag, r["n"] == 0, f"{r['n']} moves (must be 0)", r["args"])
                elif flag is None:
                    g, good = grade(size, r["n"])
                    report(tag, good, f"{r['n']} moves, {g}", r["args"])
                else:
                    report(tag, True, f"{r['n']} moves")
    for flag in [None] + STRATS:
        flags = (flag,) if flag else ()
        for size in range(1, 11):
            ok_all, bad = True, None
            for _ in range(20):
                r = solve(prog, gen_random(size), flags)
                if not r["ok"]:
                    ok_all, bad = False, r
                    break
            report(f"{label(flag)} random size {size} x20", ok_all, bad["msg"] if bad else "", bad["args"] if bad else None)


# ============================================================== [random] tests
def test_random(prog):
    section(f"Random tests ({OPT.runs} runs for the default, {max(3, OPT.runs // 5)} for forced strategies)")
    configs = [(None, OPT.runs)] + [(f, max(3, OPT.runs // 5)) for f in STRATS]
    for flag, runs in configs:
        flags = (flag,) if flag else ()
        for size in (3, 5, 100, 500):
            counts, fails, first = [], 0, None
            for _ in range(runs):
                data = gen_random(size)
                r = solve(prog, data, flags)
                counts.append(r["n"])
                if not r["ok"]:
                    fails += 1
                    first = first or (data, r)
            nm = f"{label(flag)} random {size} x{runs}"
            inf = f"min={min(counts)} avg={statistics.mean(counts):.0f} max={max(counts)}"
            graded = flag in (None, "--adaptive") and size in BENCH
            if fails:
                report(nm, False, f"{fails} wrong results. {inf} -> {first[1]['msg']}", first[1]["args"])
                continue
            if graded:
                g, good = grade(size, max(counts))
                ex, gd, ps = BENCH[size]
                under = [sum(1 for c in counts if c < t) for t in (ex, gd, ps)]
                report(nm, good, f"{g}. {inf}  [<{ex}: {under[0]}/{runs}, <{gd}: {under[1]}/{runs}, <{ps}: {under[2]}/{runs}]", None)
                MOVE_STATS.append((label(flag), size, min(counts), statistics.mean(counts), max(counts), g))
            else:
                report(nm, True, inf)
                MOVE_STATS.append((label(flag), size, min(counts), statistics.mean(counts), max(counts), "-"))


# =========================================================== [complexity] tests
def slope(points):
    xs = [math.log(n) for n, _ in points]
    ys = [math.log(max(m, 1)) for _, m in points]
    mx, my = statistics.mean(xs), statistics.mean(ys)
    den = sum((x - mx) ** 2 for x in xs)
    return sum((x - mx) * (y - my) for x, y in zip(xs, ys)) / den if den else 0.0


def test_complexity(prog):
    section("Measured growth of the operation count (log-log slope: n^2 ~ 2.0, n sqrt n ~ 1.5, n log n ~ 1.1-1.3)")
    sizes = [100, 200, 400] if OPT.quick else [100, 200, 400, 800]
    reps = 2 if OPT.quick else 3
    windows = {"--simple": (1.6, 2.4), "--medium": (1.2, 1.8), "--complex": (0.9, 1.45)}
    for flag in STRATS[:3]:
        pts, ok_all = [], True
        for n in sizes:
            vals = []
            for _ in range(reps):
                r = solve(prog, gen_random(n, False), (flag,))
                if not r["ok"]:
                    report(f"{flag} n={n}", False, r["msg"], r["args"])
                    ok_all = False
                    break
                vals.append(r["n"])
            if not ok_all:
                break
            pts.append((n, statistics.mean(vals)))
        if not ok_all:
            continue
        s = slope(pts)
        lo, hi = windows[flag]
        table = " ".join(f"n={n}:{m:.0f}" for n, m in pts)
        if lo <= s <= hi:
            report(f"{flag} grows like {CLASS_NAME[STRAT_CLASS[flag]]}", True, f"slope {s:.2f}  ({table})")
        else:
            warn(f"{flag} slope {s:.2f}", f"expected {lo}-{hi} for {CLASS_NAME[STRAT_CLASS[flag]]} ({table})")


# =============================================================== [stress] tests
def test_stress(prog):
    section("Stress (big inputs and timing)")
    for size in (1000, 2000, 5000):
        data = gen_random(size)
        r = solve(prog, data)
        report_solve(f"default {size} numbers", r, f"{r['n']} moves in {r['dt']:.2f}s")
        if r["ok"] and r["dt"] > 10:
            warn(f"default {size} numbers is slow", f"{r['dt']:.1f}s")
    for flag in STRATS[:3]:
        r = solve(prog, gen_random(1000), (flag,))
        report_solve(f"{flag} 1000 numbers", r, f"{r['n']} moves in {r['dt']:.2f}s")
    r = solve(prog, list(range(5000, 0, -1)))
    report_solve("5000 reversed", r, f"{r['n']} moves in {r['dt']:.2f}s")
    r = solve(prog, list(range(1, 5001)))
    report("5000 sorted prints nothing", r["ok"] and r["n"] == 0, r["msg"], r["args"])


# ============================================================= [valgrind] tests
def test_valgrind(prog):
    section("Memory (valgrind): leaks and invalid accesses")
    if not shutil.which("valgrind"):
        warn("valgrind not found", "skipped")
        return
    data = [str(x) for x in random.sample(range(-1000, 1000), 60)]
    cases = [("default", data), ("--simple", ["--simple"] + data), ("--medium", ["--medium"] + data),
             ("--complex", ["--complex"] + data), ("--adaptive", ["--adaptive"] + data),
             ("--bench", ["--bench"] + data), ("sorted input", [str(i) for i in range(20)]),
             ("no arguments", []), ("one quoted string", [" ".join(data)]),
             ("error: letters", ["1", "a"]), ("error: duplicate", ["1", "2", "1"]),
             ("error: overflow", ["99999999999"]), ("error late in the list", data + ["x"]),
             ("error with flag", ["--simple", "1", "1"])]
    for nm, args in cases:
        try:
            p = subprocess.run(["valgrind", "--leak-check=full", "--show-leak-kinds=all",
                                "--errors-for-leak-kinds=all", "--error-exitcode=42", prog] + args,
                               capture_output=True, text=True, errors="replace", timeout=300)
        except subprocess.TimeoutExpired:
            report(f"valgrind {nm}", False, "timeout", args)
            continue
        ok = p.returncode != 42
        why = ""
        if not ok:
            lines = [l.strip("= ") for l in p.stderr.splitlines()
                     if re.search(r"lost|Invalid|uninitialised|still reachable", l)]
            why = "; ".join(lines[:3])
        report(f"valgrind {nm}", ok, why, args)


# =============================================================== [checker] tests
def test_checker(prog):
    section("Bonus: checker")
    chk = OPT.checker
    if not os.path.exists(chk):
        report("checker exists", False, f"{chk} not found (run 'make bonus')")
        return
    fixed = [
        (["3", "2", "1", "0"], "rra\npb\nsa\nrra\npa\n", "OK\n", ""),
        (["3", "2", "1", "0"], "sa\nrra\npb\n", "KO\n", ""),
        (["1", "2", "3"], "", "OK\n", ""),
        (["3", "2", "1"], "", "KO\n", ""),
        (["5"], "", "OK\n", ""),
        (["2", "1"], "sa\n", "OK\n", ""),
        (["3", "2", "one", "0"], "", "", "Error\n"),
        (["", "1"], "", "", "Error\n"),
        (["1", "2", "2"], "", "", "Error\n"),
        (["2147483648"], "", "", "Error\n"),
        (["3", "2", "1"], "xx\n", "", "Error\n"),
        (["3", "2", "1"], "sa \n", "", "Error\n"),
        (["3", "2", "1"], " sa\n", "", "Error\n"),
        (["3", "2", "1"], "SA\n", "", "Error\n"),
        (["3", "2", "1"], "sax\n", "", "Error\n"),
        (["3", "2", "1"], "sa\r\n", "", "Error\n"),
        (["3", "2", "1"], "sa ra\n", "", "Error\n"),
        (["1", "x"], "sa\n", "", "Error\n"),
        ([], "", "", ""),
        ([], "sa\nra\n", "", ""),
    ]
    for args, stdin, eo, ee in fixed:
        res = run(chk, args, stdin)
        nm = f"checker {short(args, 40)} <- {stdin!r}"[:100]
        if res is None or res.rc < 0:
            report(nm, False, "timeout or crash")
            continue
        ok = res.out == eo and res.err == ee
        report(nm, ok, "" if ok else f"stdout={res.out!r} stderr={res.err!r}")
    for stdin, nm in (("sa\n\nra\n", "empty line in the middle"), ("sa", "last instruction without newline")):
        res = run(chk, ["3", "2", "1"], stdin)
        if res is None or res.rc < 0:
            report(f"checker {nm}", False, "timeout or crash")
        elif res.err == "Error\n" or res.out in ("OK\n", "KO\n"):
            report(f"checker {nm} gives a clean answer", True, f"(stdout={res.out!r} stderr={res.err!r})")
        else:
            warn(f"checker {nm}", f"stdout={res.out!r} stderr={res.err!r}")
    # semantics of the 11 operations: compare with this tester's own simulator
    bad = 0
    total = 0
    for _ in range(400 if not OPT.quick else 100):
        size = random.randint(2, 5)
        nums = random.sample(range(-9, 10), size)
        ops = [random.choice(OPS) for _ in range(random.randint(0, 9))]
        exp = "OK\n" if simulate(nums, ops)[0] else "KO\n"
        res = run(chk, [str(x) for x in nums], "".join(o + "\n" for o in ops))
        total += 1
        if res is None or res.out != exp or res.err:
            bad += 1
            if bad == 1:
                first = (nums, ops, exp, res)
    report(f"checker semantics on {total} random operation lists", bad == 0,
           "" if bad == 0 else f"{bad} differ, first: stack={first[0]} ops={first[1]} expected={first[2]!r} got={(first[3].out, first[3].err) if first[3] else None}")
    # push_swap output piped to the checker
    for flag in [None] + STRATS:
        flags = [flag] if flag else []
        data = gen_random(100)
        res = run(prog, flags + [str(n) for n in data])
        if res is None:
            report(f"push_swap {label(flag)} | checker", False, "push_swap timeout")
            continue
        c = run(chk, [str(n) for n in data], res.out)
        report(f"push_swap {label(flag)} | checker -> OK", c is not None and c.out == "OK\n" and c.err == "")
        c = run(chk, [str(n) for n in data], res.out + "pb\n")
        report(f"push_swap {label(flag)} + extra pb | checker -> KO", c is not None and c.out == "KO\n")
        if res.out:
            cut = res.out.split("\n")[:-2]
            exp = "OK\n" if simulate(data, cut)[0] else "KO\n"
            c = run(chk, [str(n) for n in data], "".join(l + "\n" for l in cut))
            report(f"push_swap {label(flag)} minus last operation | checker matches simulator", c is not None and c.out == exp)
    big = gen_random(500)
    res = run(prog, ["--simple"] + [str(n) for n in big])
    if res is not None:
        c = run(chk, [str(n) for n in big], res.out)
        report(f"checker reads {res.out.count(chr(10))} lines quickly", c is not None and c.out == "OK\n",
               f"{c.dt:.2f}s" if c else "timeout")
    if OPT.ref_checker:
        ref = OPT.ref_checker
        diff = 0
        for _ in range(100):
            size = random.randint(2, 6)
            nums = random.sample(range(-9, 10), size)
            ops = [random.choice(OPS) for _ in range(random.randint(0, 10))]
            s = "".join(o + "\n" for o in ops)
            a = run(chk, [str(x) for x in nums], s)
            b = run(ref, [str(x) for x in nums], s)
            if a is None or b is None or a.out != b.out:
                diff += 1
        report("same OK/KO answers as the reference checker (100 random cases)", diff == 0, f"{diff} differ" if diff else "")


# =================================================================== main
SECTIONS = [
    ("make", test_make, "make"),
    ("static", test_static, "static"),
    ("errors", test_errors, "core"),
    ("quiet", test_quiet, "core"),
    ("formats", test_formats, "core"),
    ("flags", test_flags, "core"),
    ("perms", test_perms, "core"),
    ("output", test_output, "core"),
    ("bench", test_bench, "core"),
    ("regimes", test_regimes, "core"),
    ("special", test_special, "core"),
    ("random", test_random, "core"),
    ("complexity", test_complexity, "core"),
    ("stress", test_stress, "stress"),
    ("valgrind", test_valgrind, "valgrind"),
    ("checker", test_checker, "bonus"),
]


def main():
    global OPT
    ap = argparse.ArgumentParser(description="push_swap full tester (subject v1.2)")
    ap.add_argument("-p", "--program", default="./push_swap")
    ap.add_argument("-c", "--checker", default="./checker")
    ap.add_argument("--ref-checker", default=None, help="provided checker (e.g. ./checker_linux) to compare with")
    ap.add_argument("-r", "--runs", type=int, default=30, help="random runs for the default strategy")
    ap.add_argument("--seed", type=int, default=None)
    ap.add_argument("--timeout", type=int, default=30)
    ap.add_argument("--quick", action="store_true", help="smaller runs")
    ap.add_argument("-q", "--quiet", action="store_true", help="hide PASS lines")
    ap.add_argument("--all", action="store_true", help="enable make, static, stress, valgrind and bonus")
    ap.add_argument("--make", action="store_true")
    ap.add_argument("--static", action="store_true")
    ap.add_argument("--stress", action="store_true")
    ap.add_argument("--valgrind", action="store_true")
    ap.add_argument("--bonus", action="store_true")
    ap.add_argument("--only", default="", help="comma separated list of sections")
    ap.add_argument("--skip", default="", help="comma separated list of sections")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--no-color", action="store_true")
    OPT = ap.parse_args()

    if OPT.list:
        for key, fn, grp in SECTIONS:
            print(f"{key:12} group={grp:9} {(fn.__doc__ or '').strip()}")
        return
    if OPT.no_color or not sys.stdout.isatty() or os.environ.get("NO_COLOR"):
        for k in COL:
            COL[k] = ""
    if OPT.quick:
        OPT.runs = min(OPT.runs, 10)
    seed = OPT.seed if OPT.seed is not None else random.randrange(10 ** 6)
    random.seed(seed)
    print(f"seed = {seed}  (use --seed {seed} to repeat exactly this run)")

    enabled = {"core"}
    for grp in ("make", "static", "stress", "valgrind", "bonus"):
        if OPT.all or getattr(OPT, grp):
            enabled.add(grp)
    OPT.bonus_enabled = "bonus" in enabled
    only = {s for s in OPT.only.split(",") if s}
    skip = {s for s in OPT.skip.split(",") if s}
    prog = OPT.program

    for key, fn, grp in SECTIONS:
        if only and key not in only:
            continue
        if key in skip or (not only and grp not in enabled):
            continue
        if key not in ("make", "static") and not os.path.exists(prog):
            print(f"{COL['R']}Cannot find {prog}. Compile first (make).{COL['X']}")
            break
        CUR[0] = key
        fn(prog)
        if key == "make" and not os.path.exists(prog):
            print(f"{COL['R']}The Makefile did not produce {prog}, stopping.{COL['X']}")
            break

    # ----------------------------------------------------------- summary
    print(f"\n{COL['Y']}== Move statistics (random lists) =={COL['X']}")
    if MOVE_STATS:
        print(f"{'strategy':12}{'size':>6}{'min':>8}{'avg':>9}{'max':>8}   grade")
        for lab, size, mn, avg, mx, g in MOVE_STATS:
            print(f"{lab:12}{size:>6}{mn:>8}{avg:>9.0f}{mx:>8}   {g}")
    print(f"\n{COL['Y']}== Summary per section =={COL['X']}")
    tp = tf = tw = 0
    for key, (p, f, w) in RESULTS.items():
        tp, tf, tw = tp + p, tf + f, tw + w
        mark = COL["G"] + "ok  " if f == 0 else COL["R"] + "FAIL"
        print(f"{mark}{COL['X']} {key:12} passed {p:4}  failed {f:3}  warnings {w:3}")
    print(f"\n{COL['G']}passed: {tp}{COL['X']}   {COL['R']}failed: {tf}{COL['X']}   {COL['Y']}warnings: {tw}{COL['X']}")
    if FAILS:
        with open("tester_failures.log", "w") as f:
            for sec, nm, inf, rp in FAILS:
                f.write(f"[{sec}] {nm}\n    {inf}\n" + (f"    reproduce: {rp}\n" if rp else ""))
        print(f"All {len(FAILS)} failures with reproduce commands are saved in tester_failures.log")
        print("First failures:")
        for sec, nm, inf, rp in FAILS[:8]:
            print(f"  [{sec}] {nm} {inf[:100]}")
    sys.exit(1 if tf else 0)


if __name__ == "__main__":
    main()
