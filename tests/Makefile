CXX = g++
INC := -I$(CURDIR) $(INCLUDE_DIRS)
COMPONENT_NAME := gtests
CXXFLAGS = -Wall -Winline -Werror -pipe -std=c++11 -O0 --coverage
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

coverage:
	lcov -t "result" -o $(COMPONENT_NAME).info -c -d .
	genhtml -o html $(COMPONENT_NAME).info
#	cp $(PROJ_ROOT)/tests/*.gcno $(BUILDDIR)
#	lcov -c -i -d $(PROJ_ROOT)/tests -o $(COMPONENT_NAME).coverage.base
#	lcov -c -d $(PROJ_ROOT)/tests -o $(COMPONENT_NAME).coverage.run
#	lcov -d $(PROJ_ROOT)/tests -a $(COMPONENT_NAME).coverage.base -a .coverage.run -o .coverage.total
#	genhtml --no-branch-coverage -o $(BUILDDIR) $(COMPONENT_NAME).coverage.total

clean:
	rm -f $(BUILDDIR)/$(COMPONENT_NAME)
	rm -f $(PROJ_ROOT)/tests/*.gcno
	rm -f $(PROJ_ROOT)/tests/*.gcda
	rm -f $(PROJ_ROOT)/tests/*.info
	rm -rf $(PROJ_ROOT)/tests/html
