#ifndef S21_GREP_H
#define S21_GREP_H

#define _GNU_SOURCE

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Bayroqlar va konfiguratsiya */
typedef struct {
  int e; /* -e PATTERN: shablon argument sifatida berilgan */
  int i; /* -i: katta-kichik harfni e'tiborga olmaslik */
  int v; /* -v: mos kelmagan satrlarni chiqarish */
  int c; /* -c: mos satrlar sonini chiqarish */
  int l; /* -l: faqat fayl nomlarini chiqarish */
  int n; /* -n: satrning fayldagi raqamini ko'rsatish */
  int h; /* -h: fayl nomini chiqarmaslik */
  int s; /* -s: xato xabarlarini bostirish */
  int f; /* -f FILE: shablonlarni fayldan olish */
  int o; /* -o: faqat mos kelgan qismni chiqarish */

  char** patterns;  /* shablonlar massivi */
  int n_patterns;   /* shablonlar soni */
  int patterns_cap; /* massivning hajmi (sig'imi) */

  char** files;       /* fayllar ro'yxati */
  int n_files;        /* fayllar soni */
  int show_filenames; /* har satr oldida fayl nomini ko'rsatishmi */
} Options;

#endif /* S21_GREP_H */
