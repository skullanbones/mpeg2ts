CXX = g++
INC := -I$(CURDIR) $(INCLUDE_DIRS)
COMPONENT_NAME := gtests
CXXFLAGS = -Wall -Winline -Werror -pipe -std=c++11 -O0
LDFLAGS = -lgtest -lgmock -lpthread -lgcov --coverage

BUILDDIR = $(PROJ_ROOT)/build
STATIC = libmpeg2ts.a
TESTDIR = $(PROJ_ROOT)/tests

## Tests
SRCS = 	TsParser_Tests.cc \
		TsStatistics_Tests.cc \
		GetBits_Tests.cc \
		PsiTables_Tests.cc \
		TsDemuxer_Tests.cc \
		Mpeg2VideoParser_Tests.cc \
		TsUtilities_Tests.cc \
		H264Parser_Tests.cc \
		gtest_main.cc

OBJS = $(patsubst %.cc,$(BUILDDIR)/%.o,$(SRCS))

.PHONY: all clean

all: gtests

## Note this command must be run inside a docker container with gtest/gmock unless you have it installed natively
gtests: $(BUILDDIR)/$(COMPONENT_NAME)

$(BUILDDIR)/$(COMPONENT_NAME): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS) -L$(BUILDDIR) -l:$(STATIC)

$(OBJS): $(BUILDDIR)/%.o : $(TESTDIR)/%.cc
	@echo [Compile] $<
	@$(CXX) $(INC) -c $(CXXFLAGS) $< -o $@

run-unit-tests:
	$(BUILDDIR)/$(COMPONENT_NAME)

coverage: $(BUILDDIR)/coverage.info
	lcov --remove $(BUILDDIR)/coverage.info "/usr*" -o $(BUILDDIR)/coverage.info
	lcov --remove $(BUILDDIR)/coverage.info "plog/*" -o $(BUILDDIR)/coverage.info
	genhtml $(BUILDDIR)/coverage.info -o $(BUILDDIR)/coverage

$(BUILDDIR)/coverage.info: $(BUILDDIR)/coverage-all.info
	lcov --remove $< "/usr*" -o $@

$(BUILDDIR)/coverage-all.info: gtests
	$(BUILDDIR)/$(COMPONENT_NAME)
	lcov -c -d $(BUILDDIR) -o $@

clean:
	rm -f $(OBJS)
	rm -f $(BUILDDIR)/$(COMPONENT_NAME)
