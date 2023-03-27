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



/*
 * Compare set up in reverse to allow for 
*/ 
static int compare_matches(void *a, void *b){
	query_result_t *a_q, *b_q;
	a_q = (query_result_t * ) a;
	b_q = (query_result_t * ) b;


	if (a_q -> score < b_q -> score){
      return 1;
    }
  else if (a_q -> score > b_q -> score){
      return -1;
    }
  else{
      return 0;
    }
}



static list_t* get_word_document_occurencies(set_iter_t *query_iter, index_t *index){
	list_t *word_document_occurencies;
	int *n_occurences;

	word_document_occurencies = list_create(compare_pointers);

	while (set_hasnext(query_iter)){
		n_occurences = malloc(sizeof(int));
		*n_occurences = set_size(map_get(index->set_map, set_next(query_iter)));
		list_addlast(word_document_occurencies, (void *) n_occurences);
	}

	return word_document_occurencies;
}


list_t *index_query(index_t *index, list_t *query, char **errmsg){
	list_iter_t *l_iter, *word_doc_occ_iter;
	list_t *match_list, *word_doc_occ;
	void *word;
	set_t *query_set, *clean_query;
	set_iter_t *s_iter, *clean_query_iter;

	*errmsg = NULL;
	l_iter = list_createiter(query);
	query_set = search_parser(index -> set_map, l_iter, errmsg);
	list_destroyiter(l_iter);	
 	
 	if (*errmsg != NULL){
 		return NULL;
 	}

	match_list = list_create(compare_matches);
	if (query_set == NULL)
		return match_list;
	

	clean_query = get_clean_query(query);
	clean_query_iter = set_createiter(clean_query);

	word_doc_occ = get_word_document_occurencies(clean_query_iter, index);
	word_doc_occ_iter = list_createiter(word_doc_occ);
	set_reset_iter(clean_query_iter);
	

 	s_iter = set_createiter(query_set);

 	double tf;
 	char *path;
 	while (set_hasnext(s_iter)){
 		path = (char *) set_next(s_iter);
 		tf = calculate_score(path, clean_query_iter, word_doc_occ_iter, index->size);
		query_result_t *res = query_result_create(path, tf);
		list_addfirst(match_list, (void *) res);

		set_reset_iter(clean_query_iter);
		list_reset_iter(word_doc_occ_iter);
	}
	list_sort(match_list);

	set_destroy(clean_query);
	set_destroyiter(clean_query_iter);
	set_destroyiter(s_iter);


	while (list_hasnext(word_doc_occ_iter))
		free(list_next(word_doc_occ_iter));

	list_destroy(word_doc_occ);
	list_destroyiter(word_doc_occ_iter);
	printf("\n-------------\n");
	return match_list;
}