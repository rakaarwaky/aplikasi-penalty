/* EXPORT — Deklarasi Modul */
#ifndef MODULE_EXPORT_H
#define MODULE_EXPORT_H

#include "shared/taxonomy_export_error.h"
#include "shared/contract_export_protocol.h"

/* INFRASTRUCTURE — implementasi file I/O (mengimplementasikan ExportProtocol) */
ExportProtocol export_adapter_create(void);

/* AGENT — orkestrasi melalui protocol */
typedef struct {
    const ExportProtocol *protocol;
} ExportAggregate;

ExportError agent_export_ranking(const ExportAggregate *agg,
                                 const char *filename,
                                 const CompetitionState *state,
                                 const RankingEntryVO *entries,
                                 int count);

/* ROOT — container builder (wiring only) */
ExportAggregate root_export_build(void);

#endif /* MODULE_EXPORT_H */
