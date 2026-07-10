/*
 * fake_ncurses.c — Stub ncurses untuk test headless (tanpa terminal).
 *
 * Pendekatan:
 * - Include ncurses.h untuk tipe yang benar.
 * - #undef SEMUA macro ncurses yang mengubah nama fungsi
 *   (attron→wattr_on, clear→wclear, getch→wgetch, dll).
 * - Definisikan fungsi stub sendiri sebagai no-op.
 * - Antrian input global menyediakan data untuk getnstr/getch.
 */
#ifdef _WIN32
#include <curses.h>
#else
#include <ncurses.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/* ── Hapus SEMUA macro ncurses yang mengalihkan nama fungsi ── */
#undef attron
#undef attroff
#undef clear
#undef refresh
#undef getch
#undef addstr
#undef bkgd
#undef printw
#undef getnstr
#undef mvaddch
#undef mvprintw
#undef mvgetnstr
#undef mvwprintw
#undef keypad
#undef has_colors
#undef curs_set
#undef wattron
#undef wattroff
#undef wattr_on
#undef wattr_off
#undef wmove
#undef waddch
#undef waddnstr
#undef waddstr
#undef wprintw
#undef wrefresh
#undef wclear
#undef wbkgd
#undef wgetch
#undef wgetnstr

/* ── Antrian input ── */
static int   *g_keys = NULL;
static int    g_keys_len = 0;
static int    g_keys_cap = 0;
static char  *g_strs = NULL;
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

/* ── Input primitives (harus sebelum w* stubs) ── */
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
        const char *s = g_strs;
        size_t slen = strlen(s);
        size_t to_copy = (slen < (size_t)n) ? slen : (size_t)n;
        memcpy(buf, s, to_copy);
        buf[to_copy] = '\0';
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

int confirm(const char *prompt) { (void)prompt; return 1; }

/* ── Fungsi ncurses atas-nama (no-op) ── */
WINDOW *initscr(void) { return NULL; }
int cbreak(void) { return 0; }
int noecho(void) { return 0; }
int echo(void) { return 0; }
int endwin(void) { return 0; }
int napms(int ms) { (void)ms; return 0; }
int start_color(void) { return 0; }
int init_pair(short p, short f, short b) { (void)p;(void)f;(void)b; return 0; }

int attron(int a) { (void)a; return 0; }
int attroff(int a) { (void)a; return 0; }
int clear(void) { return 0; }
int refresh(void) { return 0; }
int bkgd(chtype ch) { (void)ch; return 0; }
int addstr(const char *s) { (void)s; return 0; }
int printw(const char *fmt, ...) { (void)fmt; return 0; }
int mvprintw(int y, int x, const char *fmt, ...) { (void)y;(void)x;(void)fmt; return 0; }
int mvaddch(int y, int x, chtype ch) { (void)y;(void)x;(void)ch; return 0; }
int mvgetnstr(int y, int x, char *buf, int n) { (void)y;(void)x; return getnstr(buf, n); }

int keypad(WINDOW *w, _Bool b) { (void)w;(void)b; return 0; }
_Bool has_colors(void) { return 0; }
int curs_set(int v) { (void)v; return 0; }

/* ── w* stubs (target makro ncurses.h) ── */
int wattr_on(WINDOW *w, attr_t a, void *opts) { (void)w;(void)a;(void)opts; return 0; }
int wattr_off(WINDOW *w, attr_t a, void *opts) { (void)w;(void)a;(void)opts; return 0; }
/* wattron/wattroff adalah macro di ncurses → sudah di-#undef di atas,
   jadi stub berikut aman pakai int (bukan fungsi nyata). */
int wattron(WINDOW *w, int a) { (void)w;(void)a; return 0; }
int wattroff(WINDOW *w, int a) { (void)w;(void)a; return 0; }
int wmove(WINDOW *w, int y, int x) { (void)w;(void)y;(void)x; return 0; }
int waddch(WINDOW *w, chtype ch) { (void)w;(void)ch; return 0; }
int waddnstr(WINDOW *w, const char *s, int n) { (void)w;(void)s;(void)n; return 0; }
/* waddstr adalah macro di ncurses (→ waddnstr(w,s,-1)); jangan definisikan ulang. */
int wprintw(WINDOW *w, const char *fmt, ...) { (void)w;(void)fmt; return 0; }
int mvwprintw(WINDOW *w, int y, int x, const char *fmt, ...) { (void)w;(void)y;(void)x;(void)fmt; return 0; }
int wrefresh(WINDOW *w) { (void)w; return 0; }
int wclear(WINDOW *w) { (void)w; return 0; }
int wbkgd(WINDOW *w, chtype ch) { (void)w;(void)ch; return 0; }
int wgetch(WINDOW *w) { (void)w; return getch(); }
int wgetnstr(WINDOW *w, char *buf, int n) { (void)w; return getnstr(buf, n); }
