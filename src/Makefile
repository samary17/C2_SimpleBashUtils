.PHONY: all s21_cat s21_grep clean rebuild test

all: s21_cat s21_grep

s21_cat:
	$(MAKE) -C cat

s21_grep:
	$(MAKE) -C grep

clean:
	$(MAKE) -C cat clean
	$(MAKE) -C grep clean

rebuild: clean all

test:
	bash tests/test_cat.sh
	bash tests/test_grep.sh
