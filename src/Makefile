 
DEPENDENCY_TARGETS=uSockets spdlog jsoncpp

STD=-Wpedantic -std=c++17
WARN=-Wall
OPT=-O2

CXXFLAGS=$(STD) $(WARN) $(OPT)
LDFLAGS=../deps/uWebSockets/uSockets/*.o ../deps/spdlog/build/libspdlog.a ../deps/jsoncpp/build/lib/libjsoncpp.a -lz

# deps
CXXFLAGS+=-I../deps/uWebSockets/src -I../deps/uWebSockets/uSockets/src -I../deps/spdlog/include/ -I../deps/jsoncpp/include/

APP_NAME=cpphttp-react
APP_OBJ=server.o

all: $(APP_NAME)

.PHONY: all

.make-prereqs:
	@touch $@
	$(MAKE) -C ../deps $(DEPENDENCY_TARGETS)

clean:
	rm -f *.o $(APP_NAME)
	rm -f $(APP_OBJ:%.o=%.d)

.PHONY: clean

distclean: clean
	-($(MAKE) -C ../deps distclean)
	-(rm -f .make-*)

.PHONY: distclean

$(APP_NAME): $(APP_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cc .make-prereqs
	$(CXX) $(CXXFLAGS) -MMD -o $@ -c $<
