CXX = g++
CPPFLAGS = -Isrc -pthread -std=c++17
BIN_DIR = bin
OBJ_DIR = bin
TEST_DIR = test
EXAMPLE_DIR = example

TEST_SRC := $(patsubst $(TEST_DIR)/%.cpp, %.cpp, $(wildcard $(TEST_DIR)/*.cpp))
TEST_OBJ := $(patsubst %.cpp, %.o, $(TEST_SRC))

# Google test settings
GTEST 	  = /usr/src/googletest/googletest
GTEST_INC = /usr/src/googletest/googletest/include
GTEST_SRC = /usr/src/googletest/googletest/src/gtest-all.cc \
			/usr/src/googletest/googletest/src/gtest_main.cc

vpath %.rl  example/
vpath %.rl  src/peanats/internal/
vpath %.cpp test/

.precious: %.cpp %.h %.cpp.rl

TARGET := all

all: parser png test example test

%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) -o $@ $^

parser:
	ragel -G2 src/peanats/internal/parser.h.rl -o src/peanats/internal/autogen/ragel_parser.h

png:
	ragel -Vp src/peanats/internal/parser.h.rl > parser.dot
	dot parser.dot -Tpng -o parser.png

example: parser
	$(CXX) $(CPPFLAGS) example/nats_pub.cpp -o nats_pub
	$(CXX) $(CPPFLAGS) example/nats_sub.cpp -o nats_sub


test: CPPFLAGS += -I$(GTEST) -I$(GTEST_INC)
test: $(TEST_OBJ)
	$(CXX) $(CPPFLAGS) $(TEST_OBJ) $(GTEST_SRC) -o testrunner
	./testrunner


.PHONY: install
install: parser
	@cp -a src/peanats /usr/local/include/
	@echo installed to "/usr/local/include/"


.PHONY: clean
clean:
	@rm -rf *.o
	@rm -rf parser.png
	@rm -rf testrunner
	@rm -rf nats_sub
	@rm -rf nats_pub
