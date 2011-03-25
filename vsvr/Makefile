CC = gcc
INCLUDE = /usr/include
LDLIBS = -lglut -lGL -lGLU -lX11 -lGLEW -ltiff -lm -L/usr/lib
TARGETS =  vsvr
OBJS =  vsvr.o main.o tiff_io.o
all: $(TARGETS)

$(TARGETS): $(OBJS)
	$(CC) $^ -I$(INCLUDE) $(LDLIBS) -o $@
.cpp.o:
	$(CC) -c $< -I$(INCLUDE) $(LDLIBS) -o $@

clean:
	rm $(TARGETS)
	rm *.o 
