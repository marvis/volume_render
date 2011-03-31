##############################################################################
#
# Makefile for Very Simple Volume Rendering
#
##############################################################################


###################### default targets ############################

dbg :
	make -k vsvrd

rel :
	make -k vsvr


########################### Programs #################################

CC        = g++ -Wall

########################### Flags #################################

FLAGS = -I.
LIBS  = -L/usr/X11/lib -lm -lz -lGL -lGLU -lglut -lglui -ltiff
ifeq ($(shell uname -o),Cygwin)
  LIBS += -lglew32 -lopengl32 -lglu32 -lglut32
else
  LIBS += -lGLEW
endif


ifeq ($(MAKECMDGOALS),vsvrd)
  OBJDIR  = debug
  CFLAGS  = -pg -g -ggdb $(FLAGS) -D_DEBUG
endif
ifeq ($(MAKECMDGOALS),vsvr)
  OBJDIR  = release
  CFLAGS  = -O3 $(FLAGS)
endif
ifeq ($(MAKECMDGOALS),iso_change_res)
  CFLAGS  = -O3 $(FLAGS)
endif

$(OBJDIR) :
	mkdir -p $(OBJDIR)


############################  vsvr  #################################

OBJECTS = \
  $(OBJDIR)/fparser.o \
  $(OBJDIR)/vsvr.o \
  $(OBJDIR)/glui_controls.o \
  $(OBJDIR)/glui_draws.o \
  $(OBJDIR)/glui_main.o \
  $(OBJDIR)/glui_mouse.o \
  $(OBJDIR)/glui_tex_gen.o \
  $(OBJDIR)/glui_tf_gen.o \
  $(OBJDIR)/tiff_io.o


vsvrd : $(OBJDIR) $(OBJECTS)
	$(CC) -pg -g -ggdb -o $@ $(OBJECTS) $(LIBS)

vsvr  : $(OBJDIR) $(OBJECTS)
	$(CC) -O3 -o $@ $(OBJECTS) $(LIBS)

iso_change_res : iso_change_res.c
	$(CC) -O3 -o $@ $<


########################### Commands #################################

$(OBJDIR)/%.o : %.cpp
	$(CC) -c $<	-o $@	$(CFLAGS)

$(OBJDIR)/%.o : %.c
	$(CC) -c $<	-o $@	$(CFLAGS)

########################### Tar file #################################

DIRNAME = $(notdir ${PWD})
TARNAME = ../$(shell echo $(DIRNAME) | tr [:upper:] [:lower:]).tgz

$(TARNAME) : README Makefile *.h *.c *.cpp *.csg *.sln *.vcproj *.xcodeproj Doxyfile
	tar -C.. -c -v -z -f $@ $(foreach file,$^,$(DIRNAME)/$(file)) > /dev/null


########################### Documentation #################################

html/index.html : Doxyfile *.h
	rm -Rf html
	doxygen >& /dev/null


########################### Directives #################################

.PHONY: all clean depend doc tar dbg rel cvs

tar : $(TARNAME)

cvs :
	cvs ci -m "backup $(shell date -u '+%A, %d %b %Y, %H:%M')"

depend:
	makedepend -p"\$$(OBJDIR)/" -Y -I. $(wildcard *.cpp) $(wildcard *.c) >& /dev/null
	touch -r Makefile.bak Makefile
	rm -f Makefile.bak

clean:
	rm -f */*.o core *~ gmon.out vsvrd vsvr

doc  :  html/index.html

all :
	make -k depend
	make -k dbg
	make -k rel
	make -k iso_change_res
	make tar
	make doc


########################### Dependencies #################################

# DO NOT DELETE

$(OBJDIR)/fparser.o: fparser.h
$(OBJDIR)/glui_controls.o: glui_defs.h vsvr.h
$(OBJDIR)/glui_draws.o: glui_defs.h vsvr.h
$(OBJDIR)/glui_main.o: glui_defs.h vsvr.h
$(OBJDIR)/glui_mouse.o: glui_defs.h vsvr.h
$(OBJDIR)/glui_tex_gen.o: csg.h fparser.h glui_defs.h vsvr.h
$(OBJDIR)/glui_tf_gen.o: colormaps.h fparser.h glui_defs.h vsvr.h
$(OBJDIR)/vsvr.o: vsvr.h
