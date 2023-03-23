#ifndef PARSER_H
#define PARSER_H

#include "list.h"
#include "set.h"
#include "map.h"

set_t *search_parser(map_t *map, list_iter_t *query_iter);

#endif
