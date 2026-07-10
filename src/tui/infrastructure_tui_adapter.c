/**
 * @file infrastructure_tui_adapter.c
 * @brief Pembungkus ncurses untuk menggambar layar & membaca tombol.
 */

#include "infrastructure_tui_adapter.h"

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
        /* Paksa seluruh layar背景 hitam agar konsisten di semua terminal. */
        init_pair(0, COLOR_WHITE, COLOR_BLACK);
        bkgd(COLOR_PAIR(0));
        init_pair(COLOR_TITLE,     COLOR_CYAN,    COLOR_BLACK);
        init_pair(COLOR_MENU,      COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_HIGHLIGHT, COLOR_BLACK,   COLOR_CYAN);
        init_pair(COLOR_SUCCESS,   COLOR_GREEN,   COLOR_BLACK);
        init_pair(COLOR_ERROR,     COLOR_RED,     COLOR_BLACK);
        init_pair(COLOR_BORDER,    COLOR_CYAN,    COLOR_BLACK);
        init_pair(COLOR_DIM,       COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_GOLD,      COLOR_YELLOW,  COLOR_BLACK);
        init_pair(COLOR_SILVER,    COLOR_WHITE,   COLOR_BLACK);
        init_pair(COLOR_BRONZE,    COLOR_YELLOW,  COLOR_BLACK);
        init_pair(COLOR_WARNING,   COLOR_YELLOW,  COLOR_BLACK);
        init_pair(COLOR_INFO,      COLOR_BLUE,    COLOR_BLACK);
        init_pair(COLOR_HEADER,    COLOR_BLACK,   COLOR_CYAN);
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

/* Gambar kotak berbingkai di posisi & ukuran tertentu. */
void tui_box(int row, int col, int width, int height) {
    if (width < 2 || height < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_BORDER));

    /* Bingkai atas. */
    mvaddch(row, col, ACS_ULCORNER);
    for (i = 1; i < width - 1; i++) mvaddch(row, col + i, ACS_HLINE);
    mvaddch(row, col + width - 1, ACS_URCORNER);

    /* Bingkai sisi. */
    for (i = 1; i < height - 1; i++) {
        mvaddch(row + i, col, ACS_VLINE);
        mvaddch(row + i, col + width - 1, ACS_VLINE);
    }

    /* Bingkai bawah. */
    mvaddch(row + height - 1, col, ACS_LLCORNER);
    for (i = 1; i < width - 1; i++)
        mvaddch(row + height - 1, col + i, ACS_HLINE);
    mvaddch(row + height - 1, col + width - 1, ACS_LRCORNER);

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

/* Garis pemisah horizontal. */
void tui_separator(int row, int col, int width) {
    if (width < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_BORDER));
    mvaddch(row, col, ACS_LTEE);
    for (i = 1; i < width - 1; i++) mvaddch(row, col + i, ACS_HLINE);
    mvaddch(row, col + width - 1, ACS_RTEE);
    attroff(COLOR_PAIR(COLOR_BORDER));
}

/* Garis pemisah tebal. */
void tui_separator_thick(int row, int col, int width) {
    if (width < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_BORDER) | A_BOLD);
    mvaddch(row, col, ACS_LTEE);
    for (i = 1; i < width - 1; i++) mvaddch(row, col + i, ACS_HLINE);
    mvaddch(row, col + width - 1, ACS_RTEE);
    attroff(COLOR_PAIR(COLOR_BORDER) | A_BOLD);
}

/* Kotak dengan garis ganda. */
void tui_box_double(int row, int col, int width, int height) {
    if (width < 2 || height < 2) return;
    int i;
    attron(COLOR_PAIR(COLOR_TITLE) | A_BOLD);

    /* Sudut atas. */
    mvaddch(row, col, ACS_ULCORNER);
    for (i = 1; i < width - 1; i++) mvaddch(row, col + i, ACS_HLINE);
    mvaddch(row, col + width - 1, ACS_URCORNER);

    /* Sisi. */
    for (i = 1; i < height - 1; i++) {
        mvaddch(row + i, col, ACS_VLINE);
        mvaddch(row + i, col + width - 1, ACS_VLINE);
    }

    /* Sudut bawah. */
    mvaddch(row + height - 1, col, ACS_LLCORNER);
    for (i = 1; i < width - 1; i++) mvaddch(row + height - 1, col + i, ACS_HLINE);
    mvaddch(row + height - 1, col + width - 1, ACS_LRCORNER);

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
    mvaddch(sep_row, sep_col, ACS_LTEE);
    for (j = 1; j < sep_w - 1; j++) mvaddch(sep_row, sep_col + j, ACS_HLINE);
    mvaddch(sep_row, sep_col + sep_w - 1, ACS_RTEE);
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
void tui_splash(int delay_ms) {
    tui_clear();

    int center_row = LINES / 2;

    /* ASCII art title */
    const char *art[] = {
        " _____ ____  _   _ _____ _____ ____  __  __ ___ _   _    _    _     ",
        "|  ___|  _ \\| \\ | |_   _| ____|  _ \\|  \\/  |_ _| \\ | |  / \\  | |    ",
        "| |_  | |_) |  \\| | | | |  _| | |_) | |\\/| || ||  \\| | / _ \\ | |    ",
        "|  _| |  __/| |\\  | | | | |___|  _ <| |  | || || |\\  |/ ___ \\| |___ ",
        "|_|   |_|   |_| \\_| |_| |_____|_| \\_\\_|  |_|___|_| \\_/_/   \\_\\_____|",
        NULL
    };

    /* Animasi: tampilkan baris per baris */
    int i;
    for (i = 0; art[i] != NULL; i++) {
        tui_print_centered_colored(center_row - 3 + i, art[i], COLOR_TITLE, 1);
        refresh();
        napms(delay_ms);
    }

    /* Subtitle */
    tui_print_centered_colored(center_row + 2, "LOMBA TENDANGAN PENALTI", COLOR_GOLD, 1);
    refresh();
    napms(delay_ms);

    /* Progress bar loading */
    tui_print_centered(center_row + 4, "Memuat aplikasi...");
    int bar_width = 30;
    int bar_col = (COLS - bar_width) / 2;
    int step;
    for (step = 0; step <= 100; step += 5) {
        tui_progress_bar(center_row + 5, bar_col, bar_width, step, COLOR_SUCCESS);
        refresh();
        napms(delay_ms / 3);
    }

    /* Tampilkan versi */
    tui_print_centered_colored(center_row + 8, "Versi 1.0.0", COLOR_DIM, 0);
    refresh();

    /* Splash otomatis lanjut ke menu (tanpa menunggu Enter). */
}

