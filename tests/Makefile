CXX = g++
INC := -I$(PROJ_ROOT) -I$(CURDIR) -I$(INCDIR)
COMPONENT_NAME := gtests
CXXFLAGS = -Wall -Winline -Werror -pipe -std=c++11

.PHONY: all clean

all:
	$(CXX) $(CXXFLAGS) TestTsParser.cc $(INC) -lgtest -L$(PROJ_ROOT) -lts -o $(COMPONENT_NAME)
	./$(COMPONENT_NAME)

clean:
	rm -f $(COMPONENT_NAME)
