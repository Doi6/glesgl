CL=gcc -lEGL -fPIC -shared -lGLESv1_CM
C=gcc -Wall -c
ITEMS = gsg gsgx gsgg
SO=libGL.so.1

build: $(SO)

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
