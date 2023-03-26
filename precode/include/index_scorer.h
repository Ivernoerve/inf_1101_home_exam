#ifndef INDEX_SCORER_H
#define INDEX_SCORER_H

#include "list.h"
#include "set.h"
#include "common.h"


double calculate_score(set_t *query_set, list_t *query, int n_documents);


#endif
