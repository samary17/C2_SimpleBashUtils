#include "s21_grep.h"

/* Yangi shablonni dinamik massivga qo'shadi */
static void add_pattern(Options* opts, const char* p) {
  if (opts->n_patterns >= opts->patterns_cap) {
    opts->patterns_cap = opts->patterns_cap ? opts->patterns_cap * 2 : 8;
    opts->patterns =
        realloc(opts->patterns, sizeof(char*) * opts->patterns_cap);
    if (!opts->patterns) {
      fprintf(stderr, "s21_grep: memory allocation failed\n");
      exit(2);
    }
  }
  opts->patterns[opts->n_patterns++] = strdup(p);
}

/* -f bayrog'i bilan berilgan fayldan shablonlarni o'qib oladi.
 * Har bir satr — alohida shablon. */
static void load_patterns_from_file(Options* opts, const char* filename) {
  FILE* fp = fopen(filename, "r");
  if (!fp) {
    if (!opts->s)
      fprintf(stderr, "s21_grep: %s: No such file or directory\n", filename);
    exit(2);
  }
  char* line = NULL;
  size_t cap = 0;
  ssize_t len;
  while ((len = getline(&line, &cap, fp)) != -1) {
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
    add_pattern(opts, line);
  }
  free(line);
  fclose(fp);
}

/* Buyruq qatori argumentlarini tahlil qiladi */
static void parse_args(int argc, char** argv, Options* opts) {
  memset(opts, 0, sizeof(*opts));
  int opt;
  /* "e:" va "f:" — argument talab qiladigan bayroqlar */
  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1) {
    switch (opt) {
      case 'e':
        opts->e = 1;
        add_pattern(opts, optarg);
        break;
      case 'i':
        opts->i = 1;
        break;
      case 'v':
        opts->v = 1;
        break;
      case 'c':
        opts->c = 1;
        break;
      case 'l':
        opts->l = 1;
        break;
      case 'n':
        opts->n = 1;
        break;
      case 'h':
        opts->h = 1;
        break;
      case 's':
        opts->s = 1;
        break;
      case 'f':
        opts->f = 1;
        load_patterns_from_file(opts, optarg);
        break;
      case 'o':
        opts->o = 1;
        break;
      default:
        fprintf(stderr, "usage: s21_grep [-eivclnhsfo] PATTERN [FILE...]\n");
        exit(2);
    }
  }

  /* Agar -e va -f berilmagan bo'lsa, birinchi argument shablon hisoblanadi */
  if (!opts->e && !opts->f) {
    if (optind >= argc) {
      fprintf(stderr, "s21_grep: pattern required\n");
      exit(2);
    }
    add_pattern(opts, argv[optind++]);
  }

  opts->files = &argv[optind];
  opts->n_files = argc - optind;
  /* Bir nechta fayl bo'lganda fayl nomini har satr oldiga qo'shamiz,
   * lekin -h berilgan bo'lsa qo'shmaymiz */
  opts->show_filenames = (opts->n_files > 1) && !opts->h;
}

/* Barcha shablonlarni "(p1)|(p2)|(p3)" ko'rinishida bitta regex ga
 * birlashtiradi. Shu tarzda regex har qaysi shablonga mos kelgan satrni topadi.
 */
static char* build_regex(Options* opts) {
  size_t total = 1;
  for (int i = 0; i < opts->n_patterns; i++) {
    total += strlen(opts->patterns[i]) + 3; /* "()|" uchun joy */
  }
  char* result = calloc(total, 1);
  for (int i = 0; i < opts->n_patterns; i++) {
    if (i > 0) strcat(result, "|");
    strcat(result, "(");
    strcat(result, opts->patterns[i]);
    strcat(result, ")");
  }
  return result;
}

/* Bitta faylni qayta ishlaydi */
static void process_file(const char* filename, FILE* fp, Options* opts,
                         regex_t* re) {
  char* line = NULL;
  size_t cap = 0;
  ssize_t len;
  int line_no = 0;
  long match_count = 0;

  while ((len = getline(&line, &cap, fp)) != -1) {
    line_no++;
    int has_newline = (len > 0 && line[len - 1] == '\n');
    if (has_newline) line[len - 1] = '\0';

    regmatch_t pmatch[1];
    int matched = (regexec(re, line, 1, pmatch, 0) == 0);
    if (opts->v) matched = !matched;

    if (matched) {
      match_count++;

      /* -l: faqat fayl nomini chiqaramiz va keyingi faylga o'tamiz */
      if (opts->l) {
        printf("%s\n", filename);
        free(line);
        return;
      }
      /* -c: faqat hisoblaymiz, satr chiqarmaymiz */
      if (opts->c) continue;

      /* -o: faqat mos kelgan qismlarni chiqaramiz (-v bilan ma'nosiz) */
      if (opts->o && !opts->v) {
        char* p = line;
        regmatch_t m;
        while (regexec(re, p, 1, &m, 0) == 0) {
          if (opts->show_filenames) printf("%s:", filename);
          if (opts->n) printf("%d:", line_no);
          fwrite(p + m.rm_so, 1, m.rm_eo - m.rm_so, stdout);
          putchar('\n');
          if (m.rm_eo == m.rm_so)
            p += 1; /* bo'sh moslikda cheksiz tsikldan saqlanish */
          else
            p += m.rm_eo;
          if (*p == '\0') break;
        }
      } else {
        if (opts->show_filenames) printf("%s:", filename);
        if (opts->n) printf("%d:", line_no);
        printf("%s\n", line);
      }
    }
  }

  /* -c: oxirida sanagan miqdorni chiqaramiz */
  if (opts->c && !opts->l) {
    if (opts->show_filenames) printf("%s:", filename);
    printf("%ld\n", match_count);
  }

  free(line);
}

int main(int argc, char** argv) {
  Options opts;
  parse_args(argc, argv, &opts);

  /* Barcha shablonlarni bitta REG_EXTENDED regex ga birlashtiramiz */
  char* combined = build_regex(&opts);
  int flags = REG_EXTENDED;
  if (opts.i) flags |= REG_ICASE;

  regex_t re;
  if (regcomp(&re, combined, flags) != 0) {
    fprintf(stderr, "s21_grep: invalid pattern: %s\n", combined);
    free(combined);
    exit(2);
  }
  free(combined);

  if (opts.n_files == 0) {
    /* stdin dan o'qish (ixtiyoriy) */
    process_file("(standard input)", stdin, &opts, &re);
  } else {
    for (int i = 0; i < opts.n_files; i++) {
      FILE* fp = fopen(opts.files[i], "r");
      if (!fp) {
        if (!opts.s)
          fprintf(stderr, "s21_grep: %s: No such file or directory\n",
                  opts.files[i]);
        continue;
      }
      process_file(opts.files[i], fp, &opts, &re);
      fclose(fp);
    }
  }

  regfree(&re);
  for (int i = 0; i < opts.n_patterns; i++) free(opts.patterns[i]);
  free(opts.patterns);

  return 0;
}
