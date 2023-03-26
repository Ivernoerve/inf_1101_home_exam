#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "printing.h"


static set_t *parse_brackets(map_t *map, list_iter_t *query_iter, char **errmsg);

static set_t *parse_sender(set_t *prev_set, map_t *map, list_iter_t *query_iter, char **errmsg);

static set_t *parse_AND(set_t *prev_set, set_t *next_set, char **errmsg);

static set_t *parse_ANDNOT(set_t *prev_set, set_t *next_set, char **errmsg);

static set_t *parse_OR(set_t *prev_set, set_t *next_set, char **errmsg);

static set_t *parse_XOR(set_t *prev_set, set_t *next_set, char **errmsg);


static set_t *parse_brackets(map_t *map, list_iter_t *query_iter, char **errmsg){
    char *word;
    set_t *result_set;
    *errmsg = "Opening bracket not closed";
    if (list_hasnext(query_iter))
        word = list_next(query_iter);
    else{
        return NULL;
    }

    if (strcmp(word, ")") == 0){
        *errmsg = "empty pair of brackets in search.";
        return NULL;
    }


    result_set = NULL;
    if (map_haskey(map, word))
        result_set = map_get(map, word);


    result_set = parse_sender(result_set, map, query_iter, errmsg);

    if (strcmp(*errmsg, "Closing bracket not opened") == 0)
        *errmsg = NULL;
    return result_set;
}


static set_t *parse_AND(set_t *prev_set, set_t *next_set, char **errmsg){
    
    if (!(prev_set == NULL || next_set == NULL))
        return set_intersection(prev_set, next_set);
    return NULL;
}


static set_t *parse_ANDNOT(set_t *prev_set, set_t *next_set, char **errmsg){
    if (!(prev_set == NULL || next_set == NULL))
        return set_difference(prev_set, next_set);
    return NULL;
}


static set_t *parse_OR(set_t *prev_set, set_t *next_set, char **errmsg){

    if (prev_set == NULL)
        return next_set;
    else if (next_set == NULL)
        return prev_set;
    
    return set_union(prev_set, next_set);
}


static set_t *parse_XOR(set_t *prev_set, set_t *next_set, char **errmsg){

    if (prev_set == NULL)
        return next_set;
    else if (next_set == NULL)
        return prev_set;
    
    return set_union(set_difference(prev_set, next_set), set_difference(next_set, prev_set));
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

    operator = (char*)list_next(query_iter);

    // catches end brackets
    if (strcmp(operator, ")") == 0){
        *errmsg = "Closing bracket not opened";
        return prev_set;
    }
    // catches search token
    else if (!list_hasnext(query_iter)){
        *errmsg = concatenate_strings(2, "Expected search token after the operator: ", operator);
        return NULL;
    }

    word = (char*)list_next(query_iter);


    next_set = result_set = NULL;
    if (map_haskey(map, word)){
        next_set = map_get(map, word);
        next_set = parse_sender(next_set, map, query_iter, errmsg);
    }
    else if (strcmp(word, "(") == 0){
        next_set = parse_brackets(map, query_iter, errmsg);
    }
    else if (strcmp(word, ")") == 0){
        *errmsg = concatenate_strings(2, "Closing bracket used as search token after the operator: ", operator);
    } 
        
    if (strcmp(operator, "AND") == 0)
        result_set = parse_AND(prev_set, next_set, errmsg);
    
    else if (strcmp(operator, "ANDNOT") == 0)
        result_set = parse_ANDNOT(prev_set, next_set, errmsg);
    
    else if (strcmp(operator, "OR") == 0){
        result_set = parse_OR(prev_set, next_set, errmsg);
    }
    else if (strcmp(operator, "XOR") == 0){
        result_set = parse_XOR(prev_set, next_set, errmsg);
    }
    else{
        *errmsg = concatenate_strings(2, "Operator not recognized: ", operator);
        return NULL;
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