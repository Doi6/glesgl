PTH=..
AT=glxgears
ITEMS=test cube glut_example
GLESLIB=/usr/lib/intel-cdv/libGLES.so

run: 
	GSG_GLESLIB=$(GLESLIB) LD_LIBRARY_PATH=$(PTH) $(AT)

build: $(ITEMS) 

cube: cube.c
	gcc -lGL -lEGL -lglut -lGLU -o $@ $^ 

test: test.c
	gcc -g -lGL -lEGL -lX11 -o $@ $^

glut_example: glut_example.c texture.c
	gcc -g -lGL -lglut -lGLU -o $@ $^

clean:
	rm -f $(ITEMS)
