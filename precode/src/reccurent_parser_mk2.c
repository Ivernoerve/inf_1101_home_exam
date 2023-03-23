#include "parser.h"

#include <stdlib.h>
#include <string.h>

static set_t *parse_brackets(map_t *map, list_iter_t *query_iter, char **errmsg);

static set_t *parse_AND(set_t *prev_set, set_t *next_set, char **errmsg);

static set_t *parse_ANDNOT(set_t *prev_set, set_t *next_set, char **errmsg);

static set_t *parse_OR(set_t *prev_set, set_t *next_set, char **errmsg);

static set_t *parse_sender(set_t *prev_set, map_t *map, list_iter_t *query_iter, char **errmsg);


static set_t *parse_brackets(map_t *map, list_iter_t *query_iter, char **errmsg){
    char *word;
    set_t *result_set;

    word = list_next(query_iter);

    result_set = NULL;
    if (map_haskey(map, word))
        result_set = map_get(map, word);

    result_set = parse_sender(result_set, map, query_iter, errmsg);

    return result_set;
}


static set_t *parse_OR(set_t *prev_set, set_t *next_set, char **errmsg){
    return NULL;
}

static set_t *parse_ANDNOT(set_t *prev_set, set_t *next_set, char **errmsg){
    return NULL;
}




static set_t *parse_AND(set_t *prev_set, set_t *next_set, char **errmsg){
    set_t *result_set;
    
    result_set = NULL;
    if (!(prev_set == NULL || next_set == NULL))
        result_set = set_intersection(prev_set, next_set);

    return result_set;
}

/*
 * Command to guide parsing to the operator tokens.
*/
static set_t *parse_sender(set_t *prev_set, map_t *map, list_iter_t *query_iter, char **errmsg){
    char *operator, *word;
    set_t *result_set, *next_set;

    // catches operator 
    if (!list_hasnext(query_iter))
        return prev_set;

    operator = list_next(query_iter);

    // catches search token
    if (!list_hasnext(query_iter)){
        return NULL;
        *errmsg = concatenate_strings(2, "Expected search token after the operator: ", operator);
    }

    word = list_next(query_iter);


    next_set = result_set = NULL;
    if (map_haskey(map, word)){
        next_set = map_get(map, word);
        next_set = parse_sender(next_set, map, query_iter, errmsg);
    }
    else if (strcmp(word, "(") == 0){
        next_set = parse_brackets(map, query_iter, errmsg);
    }

    if (strcmp(operator, "AND") == 0){
        result_set = parse_AND(prev_set, next_set, errmsg);
    }
    else if (strcmp(operator, "ANDNOT") == 0){
        result_set = parse_ANDNOT(prev_set, next_set, errmsg);
    }
    else if (strcmp(operator, "OR") == 0){
        result_set = parse_OR(prev_set, next_set, errmsg);
    }
    else{
        *errmsg = concatenate_strings(2, "Operator not recognized: ", operator);
    }

    return parse_sender(result_set, map, query_iter, errmsg);
}


set_t *search_parser(map_t *map, list_iter_t *query_iter, char **errmsg){
    char *word; 
    set_t *result_set;

    if (!list_hasnext(query_iter)){
        *errmsg = "The search bar is empty\n Please input a search."; 
        return NULL;
    }

    word = list_next(query_iter);

    result_set = NULL;
    if (strcmp(word, "(") == 0){
        result_set = parse_brackets(map, query_iter, errmsg);
        return result_set;
    }
    else if (map_haskey(map, word)){
        result_set = map_get(map, word);   
    }

    result_set = parse_sender(result_set, map, query_iter, errmsg);
        
    return result_set;
}