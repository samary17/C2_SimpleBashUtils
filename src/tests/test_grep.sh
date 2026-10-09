#!/bin/bash
# s21_grep ni haqiqiy grep bilan solishtiruvchi integratsion testlar.

set -u
GREP="../grep/s21_grep"
PASS=0
FAIL=0

if [ ! -x "$GREP" ]; then
  echo "Avval 'make' orqali s21_grep ni qurish kerak"
  exit 1
fi

TMP=$(mktemp -d)

cat > "$TMP/f1.txt" << 'EOF'
apple pie
Banana split
cherry cake
APPLE juice
grape jelly
banana bread
EOF

cat > "$TMP/f2.txt" << 'EOF'
strawberry
APPLE smoothie
mango lassi
EOF

cat > "$TMP/patterns.txt" << 'EOF'
apple
banana
EOF

> "$TMP/empty.txt"

run_test() {
  local desc="$1"
  shift
  # grep exit code 1 = "moslik topilmadi" — bu ham normal holat
  local expected actual
  expected=$(grep "$@" 2>/dev/null; true)
  actual=$("$GREP" "$@" 2>/dev/null; true)
  if [ "$expected" = "$actual" ]; then
    echo "  [OK]   $desc"
    PASS=$((PASS + 1))
  else
    echo "  [XATO] $desc"
    echo "  --- kutilgan ---"; echo "$expected" | head -5
    echo "  --- olingan ---"; echo "$actual" | head -5
    FAIL=$((FAIL + 1))
  fi
}

echo "== s21_grep testlari =="
run_test "oddiy"                    apple "$TMP/f1.txt"
run_test "-i"                    -i apple "$TMP/f1.txt"
run_test "-v"                    -v apple "$TMP/f1.txt"
run_test "-c"                    -c apple "$TMP/f1.txt"
run_test "-l (ko'p fayl)"        -l APPLE "$TMP/f1.txt" "$TMP/f2.txt"
run_test "-n"                    -n apple "$TMP/f1.txt"
run_test "-h (ko'p fayl)"        -h apple "$TMP/f1.txt" "$TMP/f2.txt"
run_test "-o -i"                 -oi apple "$TMP/f1.txt"
run_test "-iv (birikma)"         -iv apple "$TMP/f1.txt"
run_test "-in (birikma)"         -in apple "$TMP/f1.txt"
run_test "-cn (birikma)"         -cn apple "$TMP/f1.txt"
run_test "-e ko'p shablon"       -e apple -e banana "$TMP/f1.txt"
run_test "-f shablon fayli"      -f "$TMP/patterns.txt" "$TMP/f1.txt"
run_test "-if"                   -if "$TMP/patterns.txt" "$TMP/f1.txt"
run_test "ko'p fayl"             apple "$TMP/f1.txt" "$TMP/f2.txt"
run_test "bo'sh fayl"            apple "$TMP/empty.txt"
run_test "regex .*"              "ban.*"  "$TMP/f1.txt"
run_test "regex satr boshi ^"    "^a"     "$TMP/f1.txt"
run_test "regex satr oxiri \$"   "y\$"    "$TMP/f1.txt"

rm -rf "$TMP"
echo
echo "Natija: $PASS muvaffaqiyatli, $FAIL muvaffaqiyatsiz"
[ "$FAIL" -eq 0 ]
