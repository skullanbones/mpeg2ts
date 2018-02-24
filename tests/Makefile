CC = g++
INC := $(PROJ_ROOT)
TESTS_NAME := gtests

.PHONY: all clean

all:
	$(CC) TestTsParser.cc -I$(INC) -lgtest -L$(PROJ_ROOT) -lts -o $(TESTS_NAME)
	./$(TESTS_NAME)

clean:
	rm $(TESTS_NAME)
