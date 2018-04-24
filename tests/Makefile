CXX = g++
INC := -I$(PROJ_ROOT) -I$(CURDIR) -I$(INCDIR)
COMPONENT_NAME := gtests
CXXFLAGS = -Wall -Winline -Werror -pipe -std=c++11
SRCS = TsParser_Tests.cc

.PHONY: all clean

all:
	$(CXX) $(CXXFLAGS) $(SRCS) $(INC) -lgtest -lpthread -L$(PROJ_ROOT) -lts -o $(COMPONENT_NAME)
	./$(COMPONENT_NAME)

clean:
	rm -f $(COMPONENT_NAME)
