
noinst_PROGRAMS = decurs 

# bezsel

decurs_SOURCES =	\
	decurs.cc	\
	decurs.hh	\
	bdf.cc		\
	bdf.hh		\
	cppstring.cc	\
	cppstring.hh	\
	picture.cc	\
	picture.hh	\
	filesel.cc	\
	filesel.hh	\
	base.h		\
	nlist.hh	\
	idefs.h

#DEFS = @DEFS@ \
#-DG_DISABLE_DEPRECATED \
#-DGDK_DISABLE_DEPRECATED \
#-DGDK_PIXBUF_DISABLE_DEPRECATED \
#-DGTK_DISABLE_DEPRECATED

#CXXFLAGS = @CXXFLAGS@ \
#-Wall

INCLUDES = -I$(top_builddir) -I$(top_srcdir) $(GTKMM_CFLAGS)               
LIBS     = $(GTKMM_LIBS) -rdynamic



EXTRA_DIST = cursor.bdf



