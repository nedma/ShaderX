.SUFFIXES: .cpp .o

INCLUDE		= -I.
COMPILE         = gcc $(INCLUDE) -O3
LINK            = gcc 
OBJECTS = main.o extgl.o ltga.o

LIB = -lm -lglut -lGL -lGLU -lstdc++

all: parallaxmap

parallaxmap: $(OBJECTS)
	$(LINK) -o ../bin/$@ $(OBJECTS) $(LIB)

clean:
	rm *.o
	rm ../bin/parallaxmap

.cpp.o:
	$(COMPILE) -o $@ -c $<
