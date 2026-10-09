#include "s21_cat.h"

/* GNU uslubidagi uzun bayroqlar. Masalan: --number ni -n ga aylantiradi. */
static struct option long_opts[] = {{"number-nonblank", no_argument, 0, 'b'},
                                    {"number", no_argument, 0, 'n'},
                                    {"squeeze-blank", no_argument, 0, 's'},
                                    {0, 0, 0, 0}};

void parse_flags(int argc, char** argv, Flags* f) {
  int opt;
  memset(f, 0, sizeof(*f));
  /* getopt_long — argumentlarni tahlil qiladi.
   * "beEnstTv" — qabul qilinadigan qisqa bayroqlar ro'yxati. */
  while ((opt = getopt_long(argc, argv, "beEnstTv", long_opts, NULL)) != -1) {
    switch (opt) {
      case 'b':
        f->b = 1;
        break;
      case 'e':
        f->e = 1;
        f->v = 1; /* GNU cat: -e o'z ichiga -v ni oladi */
        break;
      case 'E':
        f->E = 1;
        break;
      case 'n':
        f->n = 1;
        break;
      case 's':
        f->s = 1;
        break;
      case 't':
        f->t = 1;
        f->v = 1; /* GNU cat: -t o'z ichiga -v ni oladi */
        break;
      case 'T':
        f->T = 1;
        break;
      case 'v':
        f->v = 1;
        break;
      default:
        fprintf(stderr, "usage: s21_cat [-beEnstTv] [file...]\n");
        exit(1);
    }
  }
  /* -b -n bilan birga kelsa, -b ustunlik qiladi (haqiqiy cat shunday qiladi) */
  if (f->b) f->n = 0;
}

/* Bitta baytni cat qoidalariga muvofiq chiqaradi.
 * Tab va chop etib bo'lmaydigan belgilarni alohida ko'rinishga keltiradi. */
void print_byte(unsigned char c, Flags* f) {
  if (c == '\t') {
    if (f->t || f->T) {
      putchar('^');
      putchar('I');
    } else {
      putchar('\t');
    }
    return;
  }
  if (f->v) {
    /* ASCII boshqaruv belgilari (0..31), \n dan tashqari */
    if (c < 32 && c != '\n') {
      putchar('^');
      putchar(c + 64); /* masalan: 1 -> ^A, 2 -> ^B */
    } else if (c == 127) {
      putchar('^');
      putchar('?'); /* DEL belgisi */
    } else if (c >= 128 && c < 160) {
      /* Yuqori bitli boshqaruv belgilari */
      putchar('M');
      putchar('-');
      putchar('^');
      putchar((c - 128) + 64);
    } else if (c >= 160 && c < 255) {
      putchar('M');
      putchar('-');
      putchar(c - 128);
    } else if (c == 255) {
      putchar('M');
      putchar('-');
      putchar('^');
      putchar('?');
    } else {
      putchar(c);
    }
    return;
  }
  putchar(c);
}

/* Bitta faylni satrma-satr o'qib chiqaradi.
 * line_no — global satr raqami (-n va -b uchun).
 * blank_run — oldingi satr bo'sh bo'lganmi (-s uchun). */
void process_file(FILE* fp, Flags* f, int* line_no, int* blank_run) {
  char* line = NULL;
  size_t cap = 0;
  ssize_t len;

  while ((len = getline(&line, &cap, fp)) != -1) {
    int has_newline = (len > 0 && line[len - 1] == '\n');
    ssize_t content_len = has_newline ? len - 1 : len;
    int is_blank = (content_len == 0);

    /* -s: ketma-ket bo'sh satrlarni siqish */
    if (f->s && is_blank && *blank_run) {
      continue;
    }
    *blank_run = is_blank ? 1 : 0;

    /* Raqamlash */
    if (f->b) {
      if (!is_blank) {
        (*line_no)++;
        printf("%6d\t", *line_no);
      }
    } else if (f->n) {
      (*line_no)++;
      printf("%6d\t", *line_no);
    }

    /* Satr mazmunini chiqarish */
    for (ssize_t i = 0; i < content_len; i++) {
      print_byte((unsigned char)line[i], f);
    }

    /* Satr oxirini chiqarish */
    if (has_newline) {
      if (f->e || f->E) putchar('$');
      putchar('\n');
    }
  }
  free(line);
}

int main(int argc, char** argv) {
  Flags f;
  parse_flags(argc, argv, &f);

  int line_no = 0;
  int blank_run = 0;
  int had_error = 0;

  if (optind >= argc) {
    /* Fayllar berilmagan — stdin dan o'qish (ixtiyoriy, lekin foydali) */
    process_file(stdin, &f, &line_no, &blank_run);
  } else {
    for (int i = optind; i < argc; i++) {
      FILE* fp = fopen(argv[i], "r");
      if (!fp) {
        fprintf(stderr, "s21_cat: %s: No such file or directory\n", argv[i]);
        had_error = 1;
        continue;
      }
      process_file(fp, &f, &line_no, &blank_run);
      fclose(fp);
    }
  }
  return had_error ? 1 : 0;
}
