BINCOLOR="\033[37;1m"
MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

default:
	@echo "Explicit target required"

.PHONY: default

distclean:
	$(MAKE) -C uWebSockets/uSockets clean
	$(MAKE) -C spdlog/build clean
	$(MAKE) -C jsoncpp/build clean
	$(MAKE) -C googletest/build clean

.PHONY: distclean

uSockets:
	@printf '%b %b\n' $(MAKECOLOR)MAKE$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	WITH_LIBUV=0 WITH_OPENSSL=0 WITH_WOLFSSL=0 $(MAKE) -C uWebSockets/uSockets

.PHONY: uSockets

spdlog:
	@printf '%b %b\n' $(MAKECOLOR)MAKE$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	cd spdlog && cmake -B build && cd build && make -j

.PHONY: spdlog

jsoncpp:
	@printf '%b %b\n' $(MAKECOLOR)MAKE$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	cd jsoncpp && cmake -DCMAKE_BUILD_TYPE=debug -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DARCHIVE_INSTALL_DIR=build -G "Unix Makefiles" -B build && cd build && make

.PHONY: jsoncpp

googletest:
	@printf '%b %b\n' $(MAKECOLOR)MAKE$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	cd googletest && cmake -B build && cd build && make

.PHONY: googletest
