CXX = g++
INC := -I$(PROJ_ROOT) -I$(CURDIR) -I$(INCDIR)
COMPONENT_NAME := gtests
CXXFLAGS = -Wall -Winline -Werror -pipe -std=c++11
SRCS = TsParser_Tests.cc TsStatistics_Tests.cc
BUILDDIR = $(PROJ_ROOT)/build


.PHONY: all clean

all: gtests

## Note this command must be run inside a docker container with gtest/gmock unless you have it installed natively
gtests:
	$(CXX) $(CXXFLAGS) $(SRCS) $(INC) -lgtest -lpthread -lts -L$(PROJ_ROOT)/build -o $(BUILDDIR)/$(COMPONENT_NAME)

unit-tests:
	$(BUILDDIR)/$(COMPONENT_NAME)


clean:
	rm -f $(BUILDDIR)/$(COMPONENT_NAME)
