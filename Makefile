CC = gcc
INCLUDE = /usr/include
LDLIBS = -lglut -lGL -lGLU -lX11 -lm -L/usr/lib -ltiff
TARGETS =  volume
OBJS =  volume.o tiff_io.o
all: $(TARGETS)

$(TARGETS): $(OBJS)
	$(CC) $^ -I$(INCLUDE) $(LDLIBS) -o $@
.c.o:
	$(CC) -c $< -I$(INCLUDE) $(LDLIBS) -o $@

clean:
	rm $(TARGETS)
	rm *.o 
