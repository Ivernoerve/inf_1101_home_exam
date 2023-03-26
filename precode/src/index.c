#include "index.h"
#include "map.h"
#include "set.h"

#include "parser.h"
#include "index_scorer.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

struct index{
	map_t *set_map;
	int size;

};


query_result_t *query_result_create(char *path, double score){
	query_result_t *query = malloc(sizeof(query_result_t));
	query -> path = path;
	query -> score = score;
	return query;
}


index_t *index_create(){
	index_t *idx = malloc(sizeof(index_t));

	idx -> set_map = map_create(compare_strings, hash_string);
	idx -> size = 0;
	return idx;
}


void index_destroy(index_t *index){

	map_destroy(index -> set_map, free, (void *) set_destroy);
	free(index);
}


static void index_map_insert(index_t *index, void *path, void *word){
	set_t *value_set;

	if (!map_haskey(index -> set_map, word)){
		value_set = set_create(compare_strings);
		set_add(value_set, path);
		map_put(index -> set_map, word, (void *) value_set);
		return;
	}

	value_set = (set_t *) map_get(index -> set_map, word);

	if (set_contains(value_set, path)){
		return;
	}

	set_add(value_set, path);
	map_put(index -> set_map, word, (void *) value_set);
}


void index_addpath(index_t *index, char *path, list_t *words){
	list_iter_t *l_iter;
	void *word;

	l_iter = list_createiter(words);

	while (list_hasnext(l_iter)){
		word = list_next(l_iter);

		index_map_insert(index, (void *) strdup(path), strdup(word));
		free(word);
	}
	free(path);
	index->size += 1;
}


list_t *index_query(index_t *index, list_t *query, char **errmsg){
	list_iter_t *l_iter;
	list_t *match_list;
	void *word;
	set_t *query_set;
	set_iter_t *s_iter;

	*errmsg = NULL;
	l_iter = list_createiter(query);
	// mk1 call	
	//query_set = search_parser(index -> set_map, l_iter);
	
	query_set = search_parser(index -> set_map, l_iter, errmsg);
	list_destroyiter(l_iter);	


 	
 	if (*errmsg != NULL){
 		return NULL;
 	}

	match_list = list_create(compare_pointers);
	if (query_set == NULL)
		return match_list;
	
	calculate_score(query_set, query, index->size);

 	s_iter = set_createiter(query_set);
 	while (set_hasnext(s_iter)){
		query_result_t *res = query_result_create((char *) set_next(s_iter), 1);
		list_addfirst(match_list, (void *) res);
	}

	
	set_destroyiter(s_iter);
	return match_list;
}