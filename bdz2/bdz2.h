#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>

typedef enum { RED, BLACK } Color;

typedef struct Node {
    void *key;
    void *value;
    Color color;
    struct Node *left, *right, *parent;
} Node;

typedef struct {
    Node *root;
    int (*compare)(const void *, const void *);
    void (*free_key)(void *);
    void (*free_value)(void *);
    size_t size;
} Map;

void map_init(Map *map, 
              int (*compare)(const void *, const void *),
              void (*free_key)(void *),
              void (*free_value)(void *));

void map_free(Map *map);

bool map_insert(Map *map, void *key, void *value);

void *map_find(Map *map, const void *key);

bool map_erase(Map *map, const void *key);

bool map_empty(const Map *map);

size_t map_size(const Map *map);

#ifdef __cplusplus
}
#endif