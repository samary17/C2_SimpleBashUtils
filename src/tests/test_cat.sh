#!/bin/bash
# Mac BSD cat va Linux GNU cat farqlarini hisobga oladi
set -u
CAT="../cat/s21_cat"
PASS=0
FAIL=0

if [ ! -x "$CAT" ]; then
  echo "Avval 'make' orqali s21_cat ni qurish kerak"
  exit 1
fi

# OS aniqlash: Mac da -E, -T va ba'zi -be holatlari ishlamaydi
IS_MAC=0
if [ "$(uname)" = "Darwin" ]; then
  IS_MAC=1
fi

TMP=$(mktemp -d)
cat > "$TMP/a.txt" << 'EOF'
birinchi satr
	tab bilan boshlangan satr
ikkinchi satr

bo'sh satrdan keyin


ko'p bo'sh satrdan keyin
oxirgi satr
EOF

cat > "$TMP/b.txt" << 'EOF'
ikkinchi fayl
yana satrlar
EOF

> "$TMP/empty.txt"
printf 'normal\n\tindented\n\x01control\n' > "$TMP/special.txt"

run_test() {
  local desc="$1"
  shift
  local expected actual
  expected=$(cat "$@" 2>&1)
  actual=$("$CAT" "$@" 2>&1)
  if [ "$expected" = "$actual" ]; then
    echo "  [OK]   $desc"
    PASS=$((PASS + 1))
  else
    echo "  [XATO] $desc"
    FAIL=$((FAIL + 1))
  fi
}

skip_test() {
  echo "  [SKIP] $1 (Mac BSD cat bu bayroqni qo'llab-quvvatlamaydi)"
}

echo "== s21_cat testlari =="
run_test "bayroqsiz"         "$TMP/a.txt"
run_test "-n"           -n   "$TMP/a.txt"
run_test "-b"           -b   "$TMP/a.txt"
run_test "-e"           -e   "$TMP/a.txt"
if [ $IS_MAC -eq 1 ]; then skip_test "-E"; else run_test "-E" -E "$TMP/a.txt"; fi
run_test "-t"           -t   "$TMP/a.txt"
if [ $IS_MAC -eq 1 ]; then skip_test "-T"; else run_test "-T" -T "$TMP/a.txt"; fi
run_test "-s"           -s   "$TMP/a.txt"
run_test "-v"           -v   "$TMP/special.txt"
if [ $IS_MAC -eq 1 ]; then skip_test "-be"; else run_test "-be" -be "$TMP/a.txt"; fi
if [ $IS_MAC -eq 1 ]; then skip_test "-nE"; else run_test "-nE" -nE "$TMP/a.txt"; fi
run_test "-etv"         -etv "$TMP/a.txt"
run_test "-bs"          -bs  "$TMP/a.txt"
run_test "ko'p fayl"         "$TMP/a.txt" "$TMP/b.txt"
run_test "bo'sh fayl"   -n   "$TMP/empty.txt"

rm -rf "$TMP"
echo
echo "Natija: $PASS muvaffaqiyatli, $FAIL muvaffaqiyatsiz"
[ "$FAIL" -eq 0 ]
