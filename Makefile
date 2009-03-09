BASEDIR = .
include Makefile.in

all: legesmotus

legesmotus: common server client

common:
	$(MAKE) -C common

server:
	$(MAKE) -C server

client:
	$(MAKE) -C client

clean:
	$(MAKE) -C common clean
	$(MAKE) -C server clean
	$(MAKE) -C client clean

.PHONY: clean common server client
