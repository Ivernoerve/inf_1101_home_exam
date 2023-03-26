#include "index_scorer.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>


static set_t *get_clean_query(list_t *query){
	set_t *clean_query;
	list_iter_t *l_iter;
	char *word;

	clean_query = set_create(compare_strings);
	l_iter = list_createiter(query);
	while (list_hasnext(l_iter)){
		word = (char *) list_next(l_iter);

		if (strcmp(word, "AND") == 0)
			continue;
		else if (strcmp(word, "ANDNOT") == 0){
			list_next(l_iter);
			continue;
		}
		else if (strcmp(word, "OR") == 0)
			continue;
		else if (strcmp(word, "XOR") == 0)
			continue;
		else if (strcmp(word, "(") == 0)
			continue;
		else if (strcmp(word, ")") == 0)
			continue;

		set_add(clean_query, word);
	}
	return clean_query;
}

static double calculate_tf(char *path, char *word){
	list_t *f_token_words;
	list_iter_t *l_iter;
	char *f_word;
	int n;
	f_token_words = list_create(compare_strings);
	tokenize_file(path, f_token_words);

	n = 0;
	l_iter = list_createiter(f_token_words);
	while(list_hasnext(l_iter)){
		f_word = list_next(l_iter);
		if (strcmp(word, f_word) == 0)
			n += 1;
		free(f_word);
	}
	list_destroyiter(l_iter);
	list_destroy(f_token_words);
	return n / list_size(f_token_words);
}

static double calculate_idf(set_t *query_set, int n_documents){
	return log(n_documents / (set_size(query_set) + 1));
}





double calculate_score(set_t *query_set, list_t *query, int n_documents){
	list_iter_t *l_iter;
	set_iter_t *clean_set_iter;
	set_t *clean_query;

	clean_query = get_clean_query(query);
	l_iter = list_createiter(query);
	clean_set_iter = set_createiter(clean_query);

	while(list_hasnext(l_iter)){
		printf("%s --", (char *) list_next(l_iter));
	}

	printf("\n");
	while(set_hasnext(clean_set_iter)){
		printf("%s --", (char *) set_next(clean_set_iter));
	}
	printf("\n---END---\n");
	list_destroyiter(l_iter);
	set_destroyiter(clean_set_iter);
	set_destroy(clean_query);
	return 0.0000;
}