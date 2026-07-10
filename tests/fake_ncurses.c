/*
 * fake_ncurses.c — Stub ncurses untuk test headless (tanpa terminal).
 *
 * Tujuannya: agar infrastructure_tui_adapter.c dan surfaces_*_command.c
 * bisa di-compile & dijalankan di test (CI lokal) tanpa butuh terminal nyata.
 *
 * - Tidak #include <ncurses.h> (hindari konflik makro clear/refresh/getch).
 * - Tipe ncurses di-forward-declare secara minimal.
 * - Semua fungsi ncurses jadi no-op (return 0 / NULL).
 * - Antrian input global menyediakan data untuk getnstr/getch/confirm,
 *   sehingga alur interaktif surfaces bisa disimulasikan dari test.
 *
 * File ini HANYA di-link ke binary test (bukan ke app), dan test build
 * TIDAK menggunakan -lncurses (di-override via linker).
 */
#include <string.h>
#include <stdlib.h>

/* Forward declaration tipe ncurses yang dipakai adapter. */
typedef struct _win_st WINDOW;
typedef unsigned long chtype;
typedef unsigned int  attr_t;

/* ── Antrian input (dikendalikan dari test) ── */
static int   *g_keys = NULL;
static int    g_keys_len = 0;
static int    g_keys_cap = 0;
static char  *g_strs = NULL;   /* satu string per slot, dipakai getnstr/confirm */
static int    g_strs_len = 0;
static int    g_strs_cap = 0;

void fake_tui_push_key(int k) {
    if (g_keys_len >= g_keys_cap) {
        g_keys_cap = g_keys_cap ? g_keys_cap * 2 : 16;
        g_keys = realloc(g_keys, (size_t)g_keys_cap * sizeof(int));
    }
    g_keys[g_keys_len++] = k;
}

void fake_tui_push_str(const char *s) {
    if (g_strs_len >= g_strs_cap) {
        g_strs_cap = g_strs_cap ? g_strs_cap * 2 : 16;
        g_strs = realloc(g_strs, (size_t)g_strs_cap * 64);
    }
    strncpy(g_strs + (size_t)g_strs_len * 64, s, 63);
    g_strs[(size_t)g_strs_len * 64 + 63] = '\0';
    g_strs_len++;
}

void fake_tui_reset_input(void) {
    g_keys_len = 0;
    g_strs_len = 0;
}

/* ── Variabel global ncurses ── */
int LINES = 30;
int COLS  = 80;
WINDOW *stdscr = NULL;
chtype acs_map[256];

/* ── Fungsi ncurses (no-op) ── */
WINDOW *initscr(void) { return NULL; }
int cbreak(void) { return 0; }
int noecho(void) { return 0; }
int keypad(WINDOW *w, int b) { (void)w; (void)b; return 0; }
int curs_set(int v) { (void)v; return 0; }
int has_colors(void) { return 0; }
int start_color(void) { return 0; }
int init_pair(short p, short f, short b) { (void)p;(void)f;(void)b; return 0; }
int bkgd(chtype ch) { (void)ch; return 0; }
int endwin(void) { return 0; }
int clear(void) { return 0; }
int refresh(void) { return 0; }
int echo(void) { return 0; }
int napms(int ms) { (void)ms; return 0; }

int mvprintw(int y, int x, const char *fmt, ...) { (void)y;(void)x;(void)fmt; return 0; }
int printw(const char *fmt, ...) { (void)fmt; return 0; }
int mvaddch(int y, int x, chtype ch) { (void)y;(void)x;(void)ch; return 0; }
int addstr(const char *s) { (void)s; return 0; }
int attron(attr_t a) { (void)a; return 0; }
int attroff(attr_t a) { (void)a; return 0; }

/* ── w* stubs (ncursesw macro targets) ── */
/* attron(a) -> wattr_on(stdscr,...), attroff(a) -> wattr_off(stdscr,...) */
int wattr_on(WINDOW *w, attr_t a, void *opts) { (void)w;(void)a;(void)opts; return 0; }
int wattr_off(WINDOW *w, attr_t a, void *opts) { (void)w;(void)a;(void)opts; return 0; }
int wmove(WINDOW *w, int y, int x) { (void)w;(void)y;(void)x; return 0; }
int waddch(WINDOW *w, chtype ch) { (void)w;(void)ch; return 0; }
int waddnstr(WINDOW *w, const char *s, int n) { (void)w;(void)s;(void)n; return 0; }
int wprintw(WINDOW *w, const char *fmt, ...) { (void)w;(void)fmt; return 0; }
int wrefresh(WINDOW *w) { (void)w; return 0; }
int wclear(WINDOW *w) { (void)w; return 0; }
int wbkgd(WINDOW *w, chtype ch) { (void)w;(void)ch; return 0; }
int wgetch(WINDOW *w) { (void)w; return getch(); }
int wgetnstr(WINDOW *w, char *buf, int n) { (void)w; return getnstr(buf, n); }

/* ── Input ── */
int getch(void) {
    if (g_keys_len > 0) {
        int k = g_keys[0];
        memmove(g_keys, g_keys + 1, (size_t)(--g_keys_len) * sizeof(int));
        return k;
    }
    return 0;
}

static void pop_str(char *buf, int n) {
    if (g_strs_len > 0) {
        const char *s = g_strs;  /* ambil index 0 (FIFO) */
        size_t slen = strlen(s);
        size_t to_copy = (slen < (size_t)n) ? slen : (size_t)n;
        memcpy(buf, s, to_copy);
        buf[to_copy] = '\0';
        /* geser sisa string ke depan */
        int i;
        for (i = 1; i < g_strs_len; i++)
            memcpy(g_strs + (size_t)(i - 1) * 64, g_strs + (size_t)i * 64, 64);
        g_strs_len--;
    } else {
        buf[0] = '\0';
    }
}

int getnstr(char *buf, int n) {
    if (n <= 0 || buf == NULL) return 0;
    pop_str(buf, n);
    return 0;
}

int mvgetnstr(int y, int x, char *buf, int n) {
    (void)y; (void)x;
    return getnstr(buf, n);
}

int confirm(const char *prompt) { (void)prompt; return 1; }

/* ── Stub fungsi ncurses yang berawalan 'w' (di-rename oleh makro ncurses.h) ── */
int wmove(WINDOW *win, int y, int x) { (void)win;(void)y;(void)x; return 0; }
int waddch(WINDOW *win, chtype ch) { (void)win;(void)ch; return 0; }
int wattr_on(WINDOW *win, attr_t attr, void *opts) { (void)win;(void)attr;(void)opts; return 0; }
int wattr_off(WINDOW *win, attr_t attr, void *opts) { (void)win;(void)attr;(void)opts; return 0; }
int wgetnstr(WINDOW *win, char *str, int n) { (void)win; return getnstr(str, n); }
int wbkgd(WINDOW *win, chtype ch) { (void)win;(void)ch; return 0; }
int wclear(WINDOW *win) { (void)win; return 0; }
int wgetch(WINDOW *win) { (void)win; return getch(); }
int wrefresh(WINDOW *win) { (void)win; return 0; }
int waddnstr(WINDOW *win, const char *str, int n) { (void)win;(void)str;(void)n; return 0; }
int wattron(WINDOW *win, int attrs) { (void)win;(void)attrs; return 0; }
int wattroff(WINDOW *win, int attrs) { (void)win;(void)attrs; return 0; }
int mvwprintw(WINDOW *win, int y, int x, const char *fmt, ...) { (void)win;(void)y;(void)x;(void)fmt; return 0; }
int waddstr(WINDOW *win, const char *str) { (void)win;(void)str; return 0; }
