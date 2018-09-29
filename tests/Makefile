CXX = g++
INC := -I$(CURDIR) $(INCLUDE_DIRS)
COMPONENT_NAME := gtests
CXXFLAGS = -Wall -Winline -Werror -pipe -std=c++11
BUILDDIR = $(PROJ_ROOT)/build

## Tests
SRCS = 	TsParser_Tests.cc \
		TsStatistics_Tests.cc \
		GetBits_Tests.cc \
		PsiTables_Tests.cc \
		TsDemuxer_Tests.cc \
		Mpeg2VideoParser_Tests.cc \
		main.cc



.PHONY: all clean

all: gtests

## Note this command must be run inside a docker container with gtest/gmock unless you have it installed natively
gtests:
	$(CXX) $(CXXFLAGS) $(SRCS) $(INC) -lgtest -lgmock -lpthread -lmpeg2ts -L$(PROJ_ROOT)/build -o $(BUILDDIR)/$(COMPONENT_NAME)

unit-tests:
	$(BUILDDIR)/$(COMPONENT_NAME)


clean:
	rm -f $(BUILDDIR)/$(COMPONENT_NAME)
