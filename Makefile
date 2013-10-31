OBJS = as3.o three_d_vector.o bez_surface.o bez_curve.o
CC = g++
INCLUDE = -I ./
FLAGS = -O2

ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -I./include/ -I/usr/X11/include -DOSX
	LDFLAGS = -framework GLUT -framework OpenGL \
    	-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    	-lGL -lGLU -lm -lstdc++
else
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -Iglut-3.7.6-bin
	LDFLAGS = -lglut -lGLU
endif
	
RM = /bin/rm -f 
all: main 
main: $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $(FLAGS) -o as3 $(OBJS) $(LDFLAGS) 
as3.o: as3.cpp three_d_vector.h bez_surface.h
	$(CC) $(CFLAGS) $(INCLUDE) $(FLAGS) -c as3.cpp -o as3.o
three_d_vector.o: three_d_vector.h three_d_vector.cpp
	$(CC) $(CFLAGS) $(INCLUDE) $(FLAGS) -c three_d_vector.cpp -o three_d_vector.o
bez_surface.o: bez_surface.h bez_surface.cpp three_d_vector.h
	$(CC) $(CFLAGS) $(INCLUDE) $(FLAGS) -c bez_surface.cpp -o bez_surface.o
bez_curve.o: bez_curve.h bez_curve.cpp three_d_vector.h
	$(CC) $(CFLAGS) $(INCLUDE) $(FLAGS) -c bez_curve.cpp -o bez_curve.o
clean: 
	$(RM) *.o as1
 


