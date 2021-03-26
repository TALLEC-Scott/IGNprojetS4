#ifndef META_VEC_H_INCLUDED
#define META_VEC_H_INCLUDED
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
/**
 * @brief The generator for the type type. It provides vector_type, push pushing, 
 * access and deletion operations. It offers construction, access, freeing and
 * iteration on those structures
 *
 * 
 */
#define VEC(type)                                                              \
    vector_##type vector_of_##type(size_t size) {                              \
        vector_##type to_return;                                               \
        to_return.element_count = 0;                                           \
        to_return.allocated = size;                                            \
        to_return.data = (type*)malloc(size * sizeof(type));                   \
        return to_return;                                                      \
    }                                                                          \
    void push_back_for_##type(vector_##type* v, type value) {                  \
        if (v->allocated == v->element_count) {                                \
            v->allocated += 5;                                                 \
            v->data =                                                          \
                (type*)realloc(v->data, (v->allocated + 5) * sizeof(type));    \
        }                                                                      \
        v->element_count++;                                                    \
        v->data[v->element_count - 1] = value;                                 \
    }                                                                          \
    
    #define delete_vec(to_free) \
    (to_free).element_count = 0; (to_free).allocated = 0; free((to_free).data)
#define VEC_header(type)                                                              \
    typedef struct {                                                           \
        size_t element_count;                                                  \
        size_t allocated;                                                      \
        type* data;                                                            \
    } vector_##type;\
    vector_##type vector_of_##type(size_t size);\
    void push_back_for_##type(vector_##type* v, type value);
#endif
