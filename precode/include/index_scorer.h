#ifndef INDEX_SCORER_H
#define INDEX_SCORER_H

#include "list.h"
#include "set.h"
#include "common.h"


double calculate_score(char *path, set_iter_t *query_iter, int n_documents);

set_t *get_clean_query(list_t *query);

#endif
