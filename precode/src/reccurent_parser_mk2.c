#include "parser.h"

#include <stdlib.h>
#include <string.h>

static set_t *parse_brackets(map_t *map, list_iter_t *query_iter);

static set_t *parse_AND(set_t *prev_set, map_t *map, list_iter_t *query_iter);

static set_t *parse_sender(set_t *prev_set, map_t *map, list_iter_t *query_iter);

static set_t *parse_ANDNOT(set_t *prev_set, map_t *map, list_iter_t *query_iter)


