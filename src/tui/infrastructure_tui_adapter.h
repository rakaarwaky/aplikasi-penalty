/**
 * @file infrastructure_tui_adapter.h
 * @brief Antarmuka ncurses: inisialisasi layar, splash, dan factory DisplayPort.
 *
 * Surfaces layer TIDAK boleh include file ini secara langsung.
 * Gunakan contract_display_port.h untuk semua operasi menggambar.
 * File ini hanya digunakan oleh root layer (wiring) dan tui container.
 */

#ifndef INFRASTRUCTURE_TUI_ADAPTER_H
#define INFRASTRUCTURE_TUI_ADAPTER_H

#include "shared/taxonomy_display_constant.h"
#include "shared/contract_display_port.h"

/* Hidupkan & matikan moda ncurses. Dipanggil dari root_cli_main_entry. */
void tui_init(void);
void tui_end(void);

/* Splash screen dengan animasi sederhana. Dipanggil dari root entry. */
void tui_splash(int delay_ms);

/* Factory: buat DisplayPort yang sudah terisi semua function pointer. */
DisplayPort tui_display_port_create(void);

#endif /* INFRASTRUCTURE_TUI_ADAPTER_H */
