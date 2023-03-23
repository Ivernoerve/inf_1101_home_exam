#ifndef PARSER_H
#define PARSER_H

#include "list.h"
#include "set.h"
#include "map.h"

// mk1 
//set_t *search_parser(map_t *map, list_iter_t *query_iter);

set_t *search_parser(map_t *map, list_iter_t *query_iter, char **errmsg);

#endif
