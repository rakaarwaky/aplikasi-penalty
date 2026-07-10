/* EXPORT — Agent Orchestrator */
#include "module.export.h"

ExportError agent_export_ranking(const ExportAggregate *agg,
                                 const char *filename,
                                 const RankingEntryVO *entries,
                                 int count) {
    if (agg == NULL || agg->protocol == NULL) {
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    if (agg->protocol->export_ranking == NULL) {
        return EXP_ERROR_FILE_NOT_FOUND;
    }
    return agg->protocol->export_ranking(filename, entries, count);
}
