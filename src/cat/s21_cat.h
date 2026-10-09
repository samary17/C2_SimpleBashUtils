#ifndef S21_CAT_H
#define S21_CAT_H

#define _GNU_SOURCE

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Bayroqlar (flags) holatini saqlaydigan struktura.
 * Har bir maydon — bitta bayroq (1 = yoqilgan, 0 = o'chiq). */
typedef struct {
  int b; /* -b: faqat bo'sh bo'lmagan satrlarni raqamlash */
  int e; /* -e: satr oxirini $ bilan ko'rsatish, -v ni ham yoqadi */
  int E; /* -E: faqat satr oxirini $ bilan ko'rsatish */
  int n; /* -n: barcha satrlarni raqamlash */
  int s; /* -s: ketma-ket bo'sh satrlarni bittaga siqish */
  int t; /* -t: tab belgisini ^I sifatida ko'rsatish, -v ni ham yoqadi */
  int T; /* -T: faqat tab belgisini ^I sifatida ko'rsatish */
  int v; /* -v: chop etib bo'lmaydigan belgilarni ko'rinarli qilish */
} Flags;

void parse_flags(int argc, char** argv, Flags* f);
void process_file(FILE* fp, Flags* f, int* line_no, int* blank_run);
void print_byte(unsigned char c, Flags* f);

#endif /* S21_CAT_H */
