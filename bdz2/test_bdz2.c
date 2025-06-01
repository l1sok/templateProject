#include "bdz2.h"
#include "unity.h"
#include <stdlib.h>
#include <string.h>

Map map;

int int_compare(const void *a, const void *b) {
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    return (ia > ib) - (ia < ib);
}

void string_free(void *str) {
    free(str);
}

int string_compare(const void *a, const void *b) {
    return strcmp((const char *)a, (const char *)b);
}

void setUp(void) {
    map_init(&map, int_compare, NULL, NULL);
}

void tearDown(void) {
    map_free(&map);
}

void test_map_insert_and_find(void) {
    int key1 = 10, value1 = 100;
    int key2 = 20, value2 = 200;
    int key3 = 30, value3 = 300;
    
    TEST_ASSERT_TRUE(map_insert(&map, &key1, &value1));
    TEST_ASSERT_TRUE(map_insert(&map, &key2, &value2));
    TEST_ASSERT_TRUE(map_insert(&map, &key3, &value3));
    
    TEST_ASSERT_EQUAL_PTR(&value1, map_find(&map, &key1));
    TEST_ASSERT_EQUAL_PTR(&value2, map_find(&map, &key2));
    TEST_ASSERT_EQUAL_PTR(&value3, map_find(&map, &key3));
    TEST_ASSERT_NULL(map_find(&map, &(int){40}));
    
    TEST_ASSERT_EQUAL(3, map_size(&map));
}

void test_map_insert_duplicate(void) {
    int key = 10, value1 = 100, value2 = 200;
    
    TEST_ASSERT_TRUE(map_insert(&map, &key, &value1));
    TEST_ASSERT_FALSE(map_insert(&map, &key, &value2));
    
    TEST_ASSERT_EQUAL_PTR(&value1, map_find(&map, &key));
    TEST_ASSERT_EQUAL(1, map_size(&map));
}

void test_map_erase(void) {
    int key1 = 10, value1 = 100;
    int key2 = 20, value2 = 200;
    int key3 = 30, value3 = 300;
    
    map_insert(&map, &key1, &value1);
    map_insert(&map, &key2, &value2);
    map_insert(&map, &key3, &value3);
    
    TEST_ASSERT_TRUE(map_erase(&map, &key2));
    TEST_ASSERT_NULL(map_find(&map, &key2));
    TEST_ASSERT_EQUAL(2, map_size(&map));
    
    TEST_ASSERT_TRUE(map_erase(&map, &key1));
    TEST_ASSERT_NULL(map_find(&map, &key1));
    TEST_ASSERT_EQUAL(1, map_size(&map));
    
    TEST_ASSERT_TRUE(map_erase(&map, &key3));
    TEST_ASSERT_NULL(map_find(&map, &key3));
    TEST_ASSERT_TRUE(map_empty(&map));
}

void test_map_erase_nonexistent(void) {
    int key = 10, value = 100;
    
    map_insert(&map, &key, &value);
    TEST_ASSERT_FALSE(map_erase(&map, &(int){20}));
    TEST_ASSERT_EQUAL(1, map_size(&map));
}

void test_map_with_string_keys(void) {
    Map str_map;
    map_init(&str_map, string_compare, string_free, NULL);
    
    char *key1 = strdup("apple");
    char *key2 = strdup("banana");
    char *key3 = strdup("cherry");
    
    int value1 = 1, value2 = 2, value3 = 3;
    
    TEST_ASSERT_TRUE(map_insert(&str_map, key1, &value1));
    TEST_ASSERT_TRUE(map_insert(&str_map, key2, &value2));
    TEST_ASSERT_TRUE(map_insert(&str_map, key3, &value3));
    
    TEST_ASSERT_EQUAL_PTR(&value1, map_find(&str_map, "apple"));
    TEST_ASSERT_EQUAL_PTR(&value2, map_find(&str_map, "banana"));
    TEST_ASSERT_EQUAL_PTR(&value3, map_find(&str_map, "cherry"));
    TEST_ASSERT_NULL(map_find(&str_map, "date"));
    
    TEST_ASSERT_EQUAL(3, map_size(&str_map));
    
    map_free(&str_map);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_map_insert_and_find);
    RUN_TEST(test_map_insert_duplicate);
    RUN_TEST(test_map_erase);
    RUN_TEST(test_map_erase_nonexistent);
    RUN_TEST(test_map_with_string_keys);
    
    return UNITY_END();
}