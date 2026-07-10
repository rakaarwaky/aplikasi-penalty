/* INFRASTRUCTURE — Export Layer */
#ifndef INFRASTRUCTURE_EXPORT_H
#define INFRASTRUCTURE_EXPORT_H

#include "shared/taxonomy_competition_error.h"
#include "shared/taxonomy_competition_state_vo.h"

typedef enum {
    EXPORT_CSV,
    EXPORT_JSON
} ExportFormat;

StorageError export_ranking(const CompetitionState *state,
                           const char *filename,
                           ExportFormat format);

StorageError export_recap(const CompetitionState *state,
                         const char *filename,
                         ExportFormat format);

#endif /* INFRASTRUCTURE_EXPORT_H */
