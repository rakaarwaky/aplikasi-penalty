#include "recap/module.recap.h"
#include "ranking/module.ranking.h"   /* pinjam implementasi ranking untuk wiring kontrak */

RecapAggregate root_recap_build(void) {
    static RecapProtocol protocol;
    protocol.prepare_details = capabilities_recap_prepare_details;

    static RankingProtocol ranking;   /* kontrak ranking, di-wire ke impl ranking */
    ranking.compute_ranking = capabilities_ranking_compute;

    RecapAggregate a;
    a.protocol = &protocol;
    a.ranking = &ranking;
    return a;
}
