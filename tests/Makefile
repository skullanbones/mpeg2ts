CC = g++
INC := -I$(PROJ_ROOT) -I$(CURDIR) -I$(INCDIR)
TESTS_NAME := gtests

.PHONY: all clean

all:
	$(CC) TestTsParser.cc $(INC) -lgtest -L$(PROJ_ROOT) -lts -o $(TESTS_NAME)
	./$(TESTS_NAME)

clean:
	rm $(TESTS_NAME)
