/**
 * @file infrastructure_tui_adapter.c
 * @brief Pembungkus ncurses untuk menggambar layar & membaca tombol.
 */

#include "tui/infrastructure_tui_adapter.h"

#ifdef _WIN32
#include <curses.h>   /* PDCurses di Windows */
#else
#include <ncurses.h>  /* ncurses di Linux */
#endif
#include <string.h>

/* Hidupkan moda ncurses & daftarkan pasang warna. */
void tui_init(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        /* Paksa seluruh layar background hitam agar konsisten di semua terminal.
         * Pasangan warna 0 dicadangkan oleh ncurses dan tidak bisa diubah dengan init_pair.
         * Kita gunakan pasangan warna 15 sebagai default background hitam. */
        init_pair(15, COLOR_WHITE, COLOR_BLACK);
        bkgd(COLOR_PAIR(15));
        init_pair(COLOR_TITLE,     COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_MENU,      COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_HIGHLIGHT, COLOR_BLACK,   COLOR_WHITE);
        init_pair(COLOR_SUCCESS,   COLOR_GREEN,   COLOR_BLACK);
        init_pair(COLOR_ERROR,     COLOR_RED,     COLOR_BLACK);
        init_pair(COLOR_BORDER,    COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_DIM,       COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_GOLD,      COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_SILVER,    COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_BRONZE,    COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_WARNING,   COLOR_YELLOW,  COLOR_BLACK);
        init_pair(COLOR_INFO,      COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_HEADER,    COLOR_WHITE,   COLOR_BLACK);
    }
}

/* Matikan moda ncurses (wajib sebelum keluar). */
void tui_end(void) {
    endwin();
}

/* Bersihkan layar. */
void tui_clear(void) {
    clear();
}

/* Tulis teks di posisi (baris, kolom). */
void tui_print(int row, int col, const char *text) {
    if (text == NULL) return;
    mvprintw(row, col, "%s", text);
}

/* Tulis teks di tengah baris. */
void tui_print_centered(int row, const char *text) {
    if (text == NULL) return;
    int len = (int)strlen(text);
    int col = (COLS - len) / 2;
    if (col < 0) col = 0;
    mvprintw(row, col, "%s", text);
}

/* Gambar kotak berbingkai di posisi & ukuran tertentu (Unicode Box Drawing). */
void tui_box(int row, int col, int width, int height) {
    if (width < 2 || height < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_BORDER));

    /* Bingkai atas: ╔ ═ ╗ */
    mvprintw(row, col, "\xe2\x95\x94");                         /* ╔ */
    for (i = 1; i < width - 1; i++)
        mvprintw(row, col + i, "\xe2\x95\x90");                /* ═ */
    mvprintw(row, col + width - 1, "\xe2\x95\x97");            /* ╗ */

    /* Bingkai sisi: ║ */
    for (i = 1; i < height - 1; i++) {
        mvprintw(row + i, col, "\xe2\x95\x91");                /* ║ */
        mvprintw(row + i, col + width - 1, "\xe2\x95\x91");    /* ║ */
    }

    /* Bingkai bawah: ╚ ═ ╝ */
    mvprintw(row + height - 1, col, "\xe2\x95\x9a");           /* ╚ */
    for (i = 1; i < width - 1; i++)
        mvprintw(row + height - 1, col + i, "\xe2\x95\x90");  /* ═ */
    mvprintw(row + height - 1, col + width - 1, "\xe2\x95\x9d"); /* ╝ */

    attroff(COLOR_PAIR(COLOR_BORDER));
}

/* Baris terpilih: latar terang + teks + bold untuk kontras lebih tinggi (A4). */
void tui_highlight_row(int row, int col, int width, const char *text) {
    if (text == NULL || width < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_HIGHLIGHT) | A_BOLD);
    for (i = 0; i < width - 2; i++) mvaddch(row, col + 1 + i, ' ');
    mvprintw(row, col + 2, "%s", text);
    attroff(COLOR_PAIR(COLOR_HIGHLIGHT) | A_BOLD);
}

/* Baris biasa: latar default + teks. */
void tui_normal_row(int row, int col, int width, const char *text) {
    if (text == NULL || width < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_MENU));
    for (i = 0; i < width - 2; i++) mvaddch(row, col + 1 + i, ' ');
    mvprintw(row, col + 2, "%s", text);
    attroff(COLOR_PAIR(COLOR_MENU));
}

/* Baca satu tombol dari pengguna. */
int tui_getch(void) {
    return getch();
}

/* Gambar progress bar: [#####-----] 50% di (row,col). persen 0..100. */
void tui_progress_bar(int row, int col, int width, int percent, int color) {
    if (width < 4) return;
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    int inner = width - 2;
    int filled = (inner * percent) / 100;
    int empty  = inner - filled;

    attron(COLOR_PAIR(COLOR_BORDER));
    mvaddch(row, col, '[');
    attroff(COLOR_PAIR(COLOR_BORDER));

    attron(COLOR_PAIR(color));
    int i;
    for (i = 0; i < filled; i++) mvaddch(row, col + 1 + i, '#');
    attroff(COLOR_PAIR(color));

    attron(COLOR_PAIR(COLOR_DIM));
    for (i = 0; i < empty; i++) mvaddch(row, col + 1 + filled + i, '-');
    attroff(COLOR_PAIR(COLOR_DIM));

    attron(COLOR_PAIR(COLOR_BORDER));
    mvaddch(row, col + width - 1, ']');
    attroff(COLOR_PAIR(COLOR_BORDER));

    attron(COLOR_PAIR(COLOR_MENU));
    mvprintw(row, col + width + 1, "%3d%%", percent);
    attroff(COLOR_PAIR(COLOR_MENU));
}

/* Garis pemisah horizontal (Unicode Box Drawing: ─). */
void tui_separator(int row, int col, int width) {
    if (width < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_BORDER));
    mvprintw(row, col, "\xe2\x94\x9c");                         /* ├ */
    for (i = 1; i < width - 1; i++)
        mvprintw(row, col + i, "\xe2\x94\x80");                /* ─ */
    mvprintw(row, col + width - 1, "\xe2\x94\xa4");            /* ┤ */
    attroff(COLOR_PAIR(COLOR_BORDER));
}

/* Garis pemisah tebal (Unicode Box Drawing: ━). */
void tui_separator_thick(int row, int col, int width) {
    if (width < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_BORDER) | A_BOLD);
    mvprintw(row, col, "\xe2\x94\x9c");                         /* ├ */
    for (i = 1; i < width - 1; i++)
        mvprintw(row, col + i, "\xe2\x94\x81");                /* ━ */
    mvprintw(row, col + width - 1, "\xe2\x94\xa4");            /* ┤ */
    attroff(COLOR_PAIR(COLOR_BORDER) | A_BOLD);
}

/* Kotak dengan garis ganda (Unicode Box Drawing Double). */
void tui_box_double(int row, int col, int width, int height) {
    if (width < 2 || height < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    /* Sudut atas: ╔ ═ ╗ */
    mvprintw(row, col, "\xe2\x95\x94");                         /* ╔ */
    for (i = 1; i < width - 1; i++)
        mvprintw(row, col + i, "\xe2\x95\x90");                /* ═ */
    mvprintw(row, col + width - 1, "\xe2\x95\x97");            /* ╗ */

    /* Sisi: ║ */
    for (i = 1; i < height - 1; i++) {
        mvprintw(row + i, col, "\xe2\x95\x91");                /* ║ */
        mvprintw(row + i, col + width - 1, "\xe2\x95\x91");    /* ║ */
    }

    /* Sudut bawah: ╚ ═ ╝ */
    mvprintw(row + height - 1, col, "\xe2\x95\x9a");           /* ╚ */
    for (i = 1; i < width - 1; i++)
        mvprintw(row + height - 1, col + i, "\xe2\x95\x90");  /* ═ */
    mvprintw(row + height - 1, col + width - 1, "\xe2\x95\x9d"); /* ╝ */

    attroff(COLOR_PAIR(COLOR_TITLE) | A_BOLD);
}

/* Teks dengan warna spesifik. */
void tui_print_colored(int row, int col, const char *text, int color_pair) {
    if (text == NULL) return;
    attron(COLOR_PAIR(color_pair));
    mvprintw(row, col, "%s", text);
    attroff(COLOR_PAIR(color_pair));
}

/* Teks tengah dengan warna spesifik. */
void tui_print_centered_colored(int row, const char *text, int color_pair, int bold) {
    if (text == NULL) return;
    int len = (int)strlen(text);
    int col = (COLS - len) / 2;
    if (col < 0) col = 0;
    int attrs = COLOR_PAIR(color_pair);
    if (bold) attrs |= A_BOLD;
    attron(attrs);
    mvprintw(row, col, "%s", text);
    attroff(attrs);
}

/* Footer bantuan konsisten di baris paling bawah layar. */
void tui_footer(const char *help) {
    if (help == NULL) return;
    int row = LINES - 2;
    /* Gambar separator dengan COLOR_DIM (bukan COLOR_BORDER). */
    attron(COLOR_PAIR(COLOR_DIM));
    int sep_row = row - 1;
    int sep_col = 2;
    int sep_w = COLS - 4;
    int j;
    mvprintw(sep_row, sep_col, "\xe2\x94\x9c");                /* ├ */
    for (j = 1; j < sep_w - 1; j++)
        mvprintw(sep_row, sep_col + j, "\xe2\x94\x80");       /* ─ */
    mvprintw(sep_row, sep_col + sep_w - 1, "\xe2\x94\xa4");   /* ┤ */
    tui_print_centered(row, help);
    attroff(COLOR_PAIR(COLOR_DIM));
}

/* Kembalikan medali/tanda untuk peringkat 1,2,3. rank lain: "". */
const char *tui_medal(int rank) {
    switch (rank) {
        case 1: return " *1* ";
        case 2: return " *2* ";
        case 3: return " *3* ";
        default: return "     ";
    }
}

/* ── Terminal info ── */
int tui_get_lines(void) { return LINES; }
int tui_get_cols(void)  { return COLS;  }

/* Konfirmasi ya/tidak. Tampilkan prompt, balik 1 bila user tekan y/Y. */
int tui_confirm(const char *prompt) {
    if (prompt == NULL) return 0;
    attron(COLOR_PAIR(COLOR_WARNING));
    mvprintw(LINES - 4, 4, "%s (y/n): ", prompt);
    attroff(COLOR_PAIR(COLOR_WARNING));
    refresh();
    echo();
    curs_set(1);
    char buf[8] = {0};
    getnstr(buf, 1);
    curs_set(0);
    noecho();
    return (buf[0] == 'y' || buf[0] == 'Y');
}

/* Tampilkan splash screen selama delay_ms milidetik (animasi sederhana). */
/* ── Input String (dipanggil surfaces untuk baca teks dari pengguna) ── */

/**
 * Baca string dari pengguna di posisi layar tertentu.
 * row=-1, col=-1 = gunakan posisi kursor saat ini.
 */
void tui_input_string(int row, int col, char *buf, int maxlen) {
    if (buf == NULL || maxlen <= 0) return;
    echo();
    curs_set(1);
    memset(buf, 0, (size_t)maxlen + 1);
    if (row >= 0 && col >= 0)
        mvgetnstr(row, col, buf, maxlen);
    else
        getnstr(buf, maxlen);
    curs_set(0);
    noecho();
}

/* ── Primitif Draw (implementasi untuk DisplayPort) ── */

/* Tulis teks polos di posisi tertentu. */
static void tui_draw_at(int row, int col, const char *text) {
    if (text == NULL) return;
    mvprintw(row, col, "%s", text);
}

/* Tulis teks berwarna di posisi tertentu. */
static void tui_draw_colored(int row, int col, int color, int bold,
                             const char *text) {
    if (text == NULL) return;
    int attrs = COLOR_PAIR(color);
    if (bold) attrs |= A_BOLD;
    attron(attrs);
    mvprintw(row, col, "%s", text);
    attroff(attrs);
}

/* Lanjutkan tulis teks dari posisi kursor saat ini. */
static void tui_draw_append(const char *text) {
    if (text == NULL) return;
    addstr(text);
}

/* Lanjutkan tulis teks berwarna dari posisi kursor saat ini. */
static void tui_draw_append_colored(int color, int bold, const char *text) {
    if (text == NULL) return;
    int attrs = COLOR_PAIR(color);
    if (bold) attrs |= A_BOLD;
    attron(attrs);
    addstr(text);
    attroff(attrs);
}

/* Kirim buffer ke terminal. */
static void tui_screen_refresh(void) {
    refresh();
}

/* ── Terminal size ── */
static int tui_dp_get_lines(void) { return LINES; }
static int tui_dp_get_cols(void)  { return COLS;  }

/**
 * Buat DisplayPort yang sudah terisi semua function pointer.
 * Dipanggil oleh root_display_container untuk wiring.
 */
DisplayPort tui_display_port_create(void) {
    DisplayPort dp;

    dp.cls                = tui_clear;
    dp.screen_refresh       = tui_screen_refresh;
    dp.draw_at              = tui_draw_at;
    dp.draw_colored         = tui_draw_colored;
    dp.draw_append          = tui_draw_append;
    dp.draw_append_colored  = tui_draw_append_colored;
    dp.print_centered_colored = tui_print_centered_colored;
    dp.print_colored        = tui_print_colored;
    dp.box                  = tui_box;
    dp.box_double           = tui_box_double;
    dp.separator            = tui_separator;
    dp.separator_thick      = tui_separator_thick;
    dp.highlight_row        = tui_highlight_row;
    dp.normal_row           = tui_normal_row;
    dp.footer               = tui_footer;
    dp.progress_bar         = tui_progress_bar;
    dp.medal                = tui_medal;
    dp.readkey              = tui_getch;
    dp.confirm              = tui_confirm;
    dp.input_string         = tui_input_string;
    dp.get_lines            = tui_dp_get_lines;
    dp.get_cols             = tui_dp_get_cols;

    return dp;
}
