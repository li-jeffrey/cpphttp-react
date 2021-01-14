DEPENDENCY_TARGETS=uSockets spdlog jsoncpp googletest

STD=-Wpedantic -std=c++17 -g
WARN=-Wall
OPT=-O2

CXXFLAGS=$(STD) $(WARN) $(OPT)
LDFLAGS=-L../deps/jsoncpp/build/lib -L../deps/spdlog/build -lpthread -lz -ljsoncpp -lspdlog -lstdc++fs ../deps/uWebSockets/uSockets/uSockets.a
TEST_LDFLAGS=-L../deps/googletest/build/lib/ -L../deps/jsoncpp/build/lib -L../deps/spdlog/build -lpthread -lgtest_main -lgtest -lgmock -ljsoncpp -lspdlog -lstdc++fs ../deps/uWebSockets/uSockets/uSockets.a

CXXFLAGS+=-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG

# deps
CXXFLAGS+=-I../deps/uWebSockets/src -I../deps/uWebSockets/uSockets/src -I../deps/spdlog/include/ -I../deps/jsoncpp/include/

ifeq ($(MAKECMDGOALS),test)
CXXFLAGS+=-I../deps/googletest/googletest/include/ -I../deps/googletest/googlemock/include/
endif

APP_NAME=cpphttp-react
APP_OBJ=server.o products.o

TEST_APP_NAME=cpphttp-react-test
TEST_APP_OBJ=http_test.o products_test.o products.o

all: $(APP_NAME)

.PHONY: all

test: $(TEST_APP_NAME)

.PHONY: test

.make-prereqs:
	@touch $@
	$(MAKE) -C ../deps $(DEPENDENCY_TARGETS)
	
clean:
	rm -f *.o $(APP_NAME) $(TEST_APP_NAME)
	rm -f $(APP_OBJ:%.o=%.d) $(TEST_APP_OBJ:%.o=%.d)

.PHONY: clean

distclean: clean
	-($(MAKE) -C ../deps distclean)
	-(rm -f .make-*)

.PHONY: distclean

$(APP_NAME): $(APP_OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(TEST_APP_NAME): $(TEST_APP_OBJ)
	$(CXX) -o $@ $^ $(TEST_LDFLAGS)

-include $(APP_OBJ:%.o=%.d) $(TEST_APP_OBJ:%.o=%.d)
	
%.o: %.cc .make-prereqs
	$(CXX) $(CXXFLAGS) -MMD -o $@ -c $<
