SHELL = /bin/sh

CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wextra -g
LFLAGS = -lcheck -lsubunit -lrt -lpthread -lm -lncurses
GFLAGS = -fprofile-arcs -ftest-coverage
VFLAGS = valgrind --tool=memcheck --leak-check=yes

EXE_NAME = tetris
TEST_NAME = tetris_test
LIB_NAME = tetris.a
GCOV_NAME = gcov_tests.info

BUILD_DIR = build
SRC_DIR = .
BACKEND_DIR = brick_game/tetris/backend
FRONTEND_DIR = gui/cli
TEST_DIR = test

LIB_SRC = $(wildcard $(BACKEND_DIR)/*.c)
CORE_SRC = $(BACKEND_DIR)/../tetris.c
FRONT_SRC = $(wildcard $(FRONTEND_DIR)/*.c)
TEST_SRC = $(wildcard $(TEST_DIR)/*.c)

LIB_O = $(LIB_SRC:.c=.o)
TEST_O = $(TEST_SRC:.c=.o)

.PHONY: all clean install uninstall play test gcov_report dvi dist clang cppcheck leaks

all: install play

install: $(LIB_NAME)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(FRONT_SRC)
	$(CC) $(CFLAGS) $(CORE_SRC) *.o -o $(BUILD_DIR)/$(EXE_NAME) $(LFLAGS) -L. -l:$(LIB_NAME)
	@touch $(BUILD_DIR)/high_score.txt
	@rm -f *.o

uninstall:
	rm -rf $(BUILD_DIR)

clean:
	rm -f *.o *.a *.gcno *.gcda *.info
	rm -rf report $(TEST_NAME) dvi Brickgame_v1.0.tgz

$(LIB_NAME): $(LIB_O)
	ar rc $@ $^
	ranlib $@
	rm -f $(LIB_O)

play: install
	./$(BUILD_DIR)/$(EXE_NAME)

test: $(TEST_O) $(LIB_NAME) install
	$(CC) $(CFLAGS) $< -o $(TEST_NAME) -L. -l:$(LIB_NAME) $(LFLAGS)
	./$(TEST_NAME)
	rm -f $(TEST_O) $(TEST_NAME)

gcov_report: install
	$(CC) $(CFLAGS) $(LIB_SRC) $(TEST_SRC) -o $(TEST_NAME) $(LFLAGS) $(GFLAGS)
	./$(TEST_NAME)
	lcov -t "gcov_tests" -o $(GCOV_NAME) -c -d .
	genhtml -o report $(GCOV_NAME)
	rm -f *.gcno *.gcda $(GCOV_NAME) $(TEST_NAME)

dvi:
	doxygen Doxyfile

dist: clean
	mkdir -p Brickgame_v1.0
	cp -r brick_game gui test Makefile diagram_FSM.png Brickgame_v1.0/
	tar cvzf Brickgame_v1.0.tgz Brickgame_v1.0/
	rm -rf Brickgame_v1.0/

clang:
	@cp ../materials/linters/.clang-format . 
	@clang-format -n \
		$(BACKEND_DIR)/*.c \
		$(BACKEND_DIR)/*.h \
		$(FRONTEND_DIR)/*.c \
		$(FRONTEND_DIR)/*.h \
		$(TEST_DIR)/*.c \
		$(TEST_DIR)/*.h
	@rm -f .clang-format

cppcheck:
	@cppcheck --enable=all --std=c11 --check-level=exhaustive \
		--disable=information --suppress=missingIncludeSystem \
		--suppress=missingInclude --suppress=checkersReport \
		$(BACKEND_DIR)/ \
		$(BACKEND_DIR)/../ \
		$(FRONTEND_DIR)/ \
		$(TEST_DIR)/ \
		-I $(BACKEND_DIR)/ \
		-I $(FRONTEND_DIR)/ \
		-I $(TEST_DIR)/

leaks: $(TEST_O) $(LIB_NAME) install
	$(CC) $(CFLAGS) $(TEST_SRC) -o $(TEST_NAME) -L. -l:$(LIB_NAME) $(LFLAGS)
	$(VFLAGS) ./$(TEST_NAME)
	rm -f $(TEST_O) $(TEST_NAME)