#include "parser.h"

#include <stdlib.h>
#include <string.h>

static set_t *parse_brackets(map_t *map, list_iter_t *query_iter);

static set_t *parse_AND(set_t *prev_set, map_t *map, list_iter_t *query_iter);

static set_t *parse_sender(set_t *prev_set, map_t *map, list_iter_t *query_iter);


static set_t *parse_brackets(map_t *map, list_iter_t *query_iter){
	char *word;
	set_t *r_set;
	
	word = list_next(query_iter);
	printf("\ninside Brackets -- %s\n", word);
	r_set = NULL;
	if (map_haskey(map, word))
		r_set = map_get(map, word);

	// sending back to search parser to continue evaluation
	r_set = parse_sender(r_set, map, query_iter);

	return parse_sender(r_set, map, query_iter);
}


static set_t *parse_AND(set_t *prev_set, map_t *map, list_iter_t *query_iter){
	char *word;
	set_t *r_set, *next_set;

	word = list_next(query_iter);

	printf("\ninside AND -- %s\n", word);
	next_set = NULL;
	r_set = NULL;

	if (strcmp(word, "(") == 0){
		next_set = parse_brackets(map, query_iter);
	}
	else if (map_haskey(map, word))
		next_set = map_get(map, word);

	if (!(prev_set == NULL || next_set == NULL))
		r_set = set_intersection(prev_set, next_set);
	

	return parse_sender(r_set, map, query_iter);
}


set_t *parse_OR(set_t *prev_set, map_t *map, list_iter_t *query_iter){
	char *word;
	set_t *r_set, *next_set;

	word = list_next(query_iter);

	r_set = NULL;
	if (strcmp(word, "(") == 0){
		next_set = parse_brackets(map, query_iter);
	}
	else if (map_haskey(map, word))
		next_set = map_get(map, word);


	if (prev_set == NULL)
		r_set = next_set;
	else if (next_set == NULL)
		r_set = prev_set;
	else
		r_set = set_union(prev_set, next_set);

	return parse_sender(r_set, map, query_iter);
}


set_t *parse_ANDNOT(set_t *prev_set, map_t *map, list_iter_t *query_iter){
	char *word;
	set_t *r_set, *next_set;

	word = list_next(query_iter);

	printf("\ninside ANDNOT -- %s\n", word);
	next_set = NULL;
	r_set = NULL;

	if (strcmp(word, "(") == 0){
		next_set = parse_brackets(map, query_iter);
	}
	else if (map_haskey(map, word))
		next_set = map_get(map, word);

	if (!(prev_set == NULL || next_set == NULL))
		r_set = set_difference(prev_set, next_set);

	return parse_sender(r_set, map, query_iter);

}


static set_t *parse_sender(set_t *prev_set, map_t *map, list_iter_t *query_iter){
	char *word;
	set_t *r_set;
	
	// word is operator

	if (!list_hasnext(query_iter))
		return prev_set;

	word = list_next(query_iter);
	r_set = NULL;
	if (strcmp(word, "AND") == 0){
		r_set = parse_AND(prev_set, map, query_iter);
	
	}
	else if (strcmp(word, "OR") == 0){
		r_set = parse_OR(prev_set, map, query_iter);
	}
	if (strcmp(word, "ANDNOT") == 0){
		r_set = parse_ANDNOT(prev_set, map, query_iter);
	
	}
	else if (strcmp(word, ")") == 0){
		printf("\nend of )\n");
		return prev_set;
	}

	else if (strcmp(word, "(") == 0){
		r_set = parse_brackets(map, query_iter);
	
	}


	return r_set;
}


set_t *search_parser(map_t *map, list_iter_t *query_iter){
	char *word;
	set_t *r_set; 
	if (!list_hasnext(query_iter))
		return NULL;
	
	word = list_next (query_iter);
	printf("\n\nstarting parser -- %s", word);

	if (strcmp(word, "(") == 0){
		r_set = parse_brackets(map, query_iter);
		return r_set;
	}


	r_set = NULL;
	if (map_haskey(map, word))
		r_set = map_get(map, word);

	r_set = parse_sender(r_set, map, query_iter);


	printf("\nparse complete\n--------------\n\n");
	return r_set;


}