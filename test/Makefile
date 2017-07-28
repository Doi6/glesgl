PTH=..
AT=glxgears
ITEMS=test cube
GLESLIB=/usr/lib/intel-cdv/libGLES.so

run: 
	GSG_GLESLIB=$(GLESLIB) LD_LIBRARY_PATH=$(PTH) $(AT)

build: $(ITEMS) 

cube: cube.c
	gcc -lGL -lEGL -lglut -lGLU -o $@ $^ 

test: test.c
	gcc -g -lGL -lEGL -lX11 -o $@ $^

clean:
	rm -f $(ITEMS)
