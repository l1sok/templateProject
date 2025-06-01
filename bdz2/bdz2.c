#include "bdz2.h"
#include <stdlib.h>
#include <stdio.h>

/* ======== Закрытые функции библиотеки ======================*/

/**
 * @brief Создает новый узел дерева и возвращает указатель на него
 * @param key  ключ узла
 * @param value  значение узла
 * @param color  цвет узла
 * @param parent  родительский узел
 * @return указатель на созданный узел или NULL при ошибке выделения памяти */
static Node *create_node(void *key, void *value, Color color, Node *parent) {
    Node *node = malloc(sizeof(Node));
    if (!node) {return NULL;}
    
    node->key = key;
    node->value = value;
    node->color = color;
    node->left = node->right = NULL;
    node->parent = parent;
    return node;
}

/**
 * @brief Освобождает память узла и его ключа/значения
 * @param map  указатель на структуру Map
 * @param node  узел для освобождения */
static void free_node(Map *map, Node *node) {
    if (!node) return;
    
    if (map->free_key)   { map->free_key(node->key);     }
    if (map->free_value) { map->free_value(node->value); }
    free(node);
}

/**
 * @brief Выполняет левый поворот вокруг узла
 * @param map  указатель на структуру Map
 * @param node  узел, вокруг которого выполняется поворот */
static void rotate_left(Map *map, Node *node) {
    Node *right_child = node->right;
    node->right = right_child->left;
    
    if (right_child->left) { right_child->left->parent = node; }
    
    right_child->parent = node->parent;
    
    if (!node->parent)                   { map->root = right_child; }
    else if (node == node->parent->left) { node->parent->left = right_child; }
    else                                 { node->parent->right = right_child; }
    
    right_child->left = node;
    node->parent = right_child;
}

/**
 * @brief Выполняет правый поворот вокруг узла
 * @param map  указатель на структуру Map
 * @param node  узел, вокруг которого выполняется поворот */
static void rotate_right(Map *map, Node *node) {
    Node *left_child = node->left;
    node->left = left_child->right;
    
    if (left_child->right) { left_child->right->parent = node; }
    
    left_child->parent = node->parent;
    
    if (!node->parent)                    { map->root = left_child; }
    else if (node == node->parent->right) { node->parent->right = left_child; }
    else                                  { node->parent->left = left_child; }
    
    left_child->right = node;
    node->parent = left_child;
}

/**
 * @brief Восстанавливает свойства красно-черного дерева после вставки
 * @param map  указатель на структуру Map
 * @param node  вставленный узел */
static void fix_insert(Map *map, Node *node) {
    while (node != map->root && node->parent->color == RED) {
        Node *uncle;
        if (node->parent == node->parent->parent->left) {
            uncle = node->parent->parent->right;
            
            if (uncle && uncle->color == RED) {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            } else {
                if (node == node->parent->right) {
                    node = node->parent;
                    rotate_left(map, node);
                }
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                rotate_right(map, node->parent->parent);
            }
        } else {
            uncle = node->parent->parent->left;
            
            if (uncle && uncle->color == RED) {
                node->parent->color = BLACK;
                uncle->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            } else {
                if (node == node->parent->left) {
                    node = node->parent;
                    rotate_right(map, node);
                }
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                rotate_left(map, node->parent->parent);
            }
        }
    }
    map->root->color = BLACK;
}

/**
 * @brief Рекурсивно освобождает поддерево узлов
 * @param map  указатель на структуру Map
 * @param node  корневой узел поддерева для удаления
 * @note Функция рекурсивно обходит все узлы поддерева и освобождает их,
 *       включая ключи и значения (если указаны соответствующие функции в Map) */
static void free_subtree(Map *map, Node *node) {
    if (!node) { return; }
    free_subtree(map, node->left);
    free_subtree(map, node->right);
    free_node(map, node);
}

/**
 * @brief Находит узел с минимальным ключом в поддереве
 * @param node  корень поддерева
 * @return узел с минимальным ключом */
static Node *find_min(Node *node) {
    while (node && node->left) { node = node->left; }
    return node;
}

/**
 * @brief Восстанавливает свойства красно-черного дерева после удаления
 * @param map  указатель на структуру Map
 * @param node  узел, с которого начинается восстановление */
static void fix_delete(Map *map, Node *node) {
    while (node != map->root && (!node || node->color == BLACK)) {
        if (node == node->parent->left) {
            Node *sibling = node->parent->right;
            
            if (sibling->color == RED) {
                sibling->color = BLACK;
                node->parent->color = RED;
                rotate_left(map, node->parent);
                sibling = node->parent->right;
            }
            
            if ((!sibling->left || sibling->left->color == BLACK) &&
                (!sibling->right || sibling->right->color == BLACK)) {
                sibling->color = RED;
                node = node->parent;
            } else {
                if (!sibling->right || sibling->right->color == BLACK) {
                    sibling->left->color = BLACK;
                    sibling->color = RED;
                    rotate_right(map, sibling);
                    sibling = node->parent->right;
                }
                
                sibling->color = node->parent->color;
                node->parent->color = BLACK;
                sibling->right->color = BLACK;
                rotate_left(map, node->parent);
                node = map->root;
            }
        } else {
            Node *sibling = node->parent->left;
            
            if (sibling->color == RED) {
                sibling->color = BLACK;
                node->parent->color = RED;
                rotate_right(map, node->parent);
                sibling = node->parent->left;
            }
            
            if ((!sibling->right || sibling->right->color == BLACK) &&
                (!sibling->left || sibling->left->color == BLACK)) {
                sibling->color = RED;
                node = node->parent;
            } else {
                if (!sibling->left || sibling->left->color == BLACK) {
                    sibling->right->color = BLACK;
                    sibling->color = RED;
                    rotate_left(map, sibling);
                    sibling = node->parent->left;
                }
                
                sibling->color = node->parent->color;
                node->parent->color = BLACK;
                sibling->left->color = BLACK;
                rotate_right(map, node->parent);
                node = map->root;
            }
        }
    }
    
    if (node) { node->color = BLACK; }
}

/* ======== Открытые функции библиотеки ==================================*/

/**
 * @brief Инициализирует структуру Map
 * @param map  указатель на структуру Map
 * @param compare  функция сравнения ключей
 * @param free_key  функция освобождения памяти ключа (может быть NULL)
 * @param free_value  функция освобождения памяти значения (может быть NULL)
 * @note Перед использованием Map должна быть инициализирована этой функцией */
void map_init(Map *map, 
              int (*compare)(const void *, const void *),
              void (*free_key)(void *),
              void (*free_value)(void *)) {
    map->root = NULL;
    map->compare = compare;
    map->free_key = free_key;
    map->free_value = free_value;
    map->size = 0;
}


/**
 * @brief Освобождает ресурсы, занятые Map
 * @param map  указатель на структуру Map
 * @note Удаляет все узлы и освобождает память ключей/значений, если указаны соответствующие функции */
void map_free(Map *map) {
    free_subtree(map, map->root);
    map->root = NULL;
    map->size = 0;
}

/**
 * @brief Вставляет пару ключ-значение в Map
 * @param map  указатель на структуру Map
 * @param key  ключ для вставки
 * @param value  значение для вставки
 * @return true - если вставка успешна, false - если ключ уже существует
 * @note Ключи сохраняются в отсортированном порядке согласно функции compare */
bool map_insert(Map *map, void *key, void *value) {
    Node *parent = NULL;
    Node **current = &map->root;
    
    while (*current) {
        parent = *current;
        int cmp = map->compare(key, parent->key);
        
        if (cmp < 0)      { current = &parent->left;  }
        else if (cmp > 0) { current = &parent->right; }
        else              { return false; }
    }
    
    *current = create_node(key, value, RED, parent);
    if (!*current) return false;
    
    map->size++;
    fix_insert(map, *current);
    return true;
}

/**
 * @brief Поиск значения по ключу
 * @param map  указатель на структуру Map
 * @param key  ключ для поиска
 * @return указатель на найденное значение или NULL, если ключ не найден */
void *map_find(Map *map, const void *key) {
    Node *current = map->root;
    
    while (current) {
        int cmp = map->compare(key, current->key);
        
        if (cmp < 0)      { current = current->left;  }
        else if (cmp > 0) { current = current->right; }
        else              { return current->value; }
    }
    
    return NULL;
}


/**
 * @brief Удаляет элемент по ключу
 * @param map  указатель на структуру Map
 * @param key  ключ для удаления
 * @return true - если удаление успешно, false - если ключ не найден
 * @note Освобождает память ключа/значения, если указаны соответствующие функции*/
bool map_erase(Map *map, const void *key) {
    Node *node = map->root;
    
    while (node) {
        int cmp = map->compare(key, node->key);
        
        if (cmp < 0)      { node = node->left;  }
        else if (cmp > 0) { node = node->right; }
        else              { break; }
    }
    
    if (!node)  { return false; }
    
    Node *to_delete = node;
    Node *child;
    Color original_color = to_delete->color;
    
    if (!node->left) {
        child = node->right;
        if (!node->parent)                   { map->root = child; }
        else if (node == node->parent->left) { node->parent->left = child;  }
        else                                 { node->parent->right = child; }
        
        if (child) { child->parent = node->parent; }
    } else if (!node->right) {
        child = node->left;
        if (!node->parent)                   { map->root = child; }
        else if (node == node->parent->left) { node->parent->left = child;  }
        else                                 { node->parent->right = child; }
        
        if (child) { child->parent = node->parent; }
    } else {
        Node *successor = find_min(node->right);
        original_color = successor->color;
        child = successor->right;
        
        if (successor->parent == node) {
            if (child) { child->parent = successor; }
        } else {
            if (successor->parent->left == successor) { successor->parent->left = child;  }
            else                                      { successor->parent->right = child; }
            
            if (child)   { child->parent = successor->parent; }
            
            successor->right = node->right;
            node->right->parent = successor;
        }
        
        if (!node->parent)                   { map->root = successor; }
        else if (node == node->parent->left) { node->parent->left = successor; }
        else                                 { node->parent->right = successor; }
        
        successor->parent = node->parent;
        successor->left = node->left;
        node->left->parent = successor;
        successor->color = node->color;
    }
    
    if (original_color == BLACK) { fix_delete(map, child ? child : (node->parent ? node->parent : NULL)); }
    
    free_node(map, node);
    map->size--;
    return true;
}

/**
 * @brief Проверяет, пуста ли Map
 * @param map  указатель на структуру Map
 * @return true - если Map пуста, false - в противном случае */
bool map_empty(const Map *map) { return map->size == 0; }

/**
 * @brief Возвращает количество элементов в Map
 * @param map  указатель на структуру Map
 * @return количество элементов в Map */
size_t map_size(const Map *map) { return map->size; }