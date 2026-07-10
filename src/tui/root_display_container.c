/**
 * @file root_display_container.c
 * @brief Wiring: buat DisplayPort dari infrastructure TUI adapter.
 */

#include "shared/contract_display_port.h"
#include "tui/infrastructure_tui_adapter.h"

DisplayPort root_display_build(void) {
    return tui_display_port_create();
}
