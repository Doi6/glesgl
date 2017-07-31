GLESGL

An attempt to provide an opengl library having an opengl es library

The code compiled as a shared object (.so) acts as an OpenGL library. 
Aside from implementing a few OpenGL calls, it simply forwards most of OpenGL calls
to an already existing OpenGL ES library. The ES library name can be given as an environment
variable called GSG_GLESLIB

example invocation on an ubuntnu execution on an ubuntu 12.04 having kernel 3.2 generic with cedarview apckages installed:

GSG_GLESLIB=/usr/lib/intel-cdv/ LD_LIBRARY_PATH=. neverball

should start neverball with hardware acceleration.

At the moment glesgl is far from being complete and has many glitches. Most opengl apps will probably end
with an error message or a segmentation fault.

I am not an OpenGL expert (yet:)). Purpose was to have some kind of hardware acceleration under linux 
which at the moment is only availble under windows 7 with the cedarview hardware. 

Library might (or might not) work with other hardware (having openGL ES support).

Anyway contribution and constructive comments are welcome.


