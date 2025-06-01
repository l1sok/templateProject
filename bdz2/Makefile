CC = gcc
CFLAGS = -Wall -fPIC
LDFLAGS = -shared
LIB_NAME = libbdz2.so
TEST_EXEC = test_bdz2

all: $(LIB_NAME) $(TEST_EXEC)

$(LIB_NAME): bdz2.o
	$(CC) $(LDFLAGS) -o $@ $^

bdz2.o: bdz2.c bdz2.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_EXEC): test_bdz2.o unity.o
	$(CC) -o $@ $^ -L. -lbdz2 -Wl,-rpath=.

test_bdz2.o: test_bdz2.c bdz2.h unity.h
	$(CC) $(CFLAGS) -c $< -o $@

unity.o: unity.c unity.h
	$(CC) $(CFLAGS) -c $< -o $@

run-test: $(TEST_EXEC)
	./$(TEST_EXEC)

clean:
	rm -f *.o $(LIB_NAME) $(TEST_EXEC)
	touch clean