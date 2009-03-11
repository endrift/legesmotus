BASEDIR = .
include Makefile.in

all: legesmotus

legesmotus: common client

common:
	$(MAKE) -C common

server: common
	$(MAKE) -C server

client: common
	$(MAKE) -C client

clean:
	$(MAKE) -C common clean
	$(MAKE) -C server clean
	$(MAKE) -C client clean

.PHONY: clean common server client
