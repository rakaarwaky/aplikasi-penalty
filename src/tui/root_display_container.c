/**
 * @file root_display_container.c
 * @brief Rakit DisplayPort dari implementasi TUI konkret (ncurses).
 */

#include "tui/infrastructure_tui_adapter.h"

/**
 * Bangun DisplayPort: sambungkan seluruh fungsi gambar layar
 * agar siap dipakai oleh surfaces melalui contract.
 *
 * @return DisplayPort yang sudah terisi.
 */
DisplayPort root_display_build(void) {
    return tui_display_port_create();
}
