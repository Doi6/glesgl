CL=gcc -lX11 -lEGL -fPIC -shared -Wl,-init,gsgInit
C=gcc -Wall -c
ITEMS = gsg gsgx gsgg gsgl
SO=libGL.so.1

build: $(SO) Makefile

$(SO): $(ITEMS:=.o)
	$(CL) -o $@ $^

test: build
	$(MAKE) -C test/

%.o: %.c %.h gsga.h Makefile
	$(C) -o $@ $<

gsgx.c: gsg.h

head:
	$(MAKE) build 2>&1 | head

clean:
	rm -f $(ITEMS:=.o)  $(SO)

.PHONY: test build
