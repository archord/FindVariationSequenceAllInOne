#
# Makefile for CrossMatchLibrary library:
#       libcfits.a

# CROSSMATCH version numbers:
CROSSMATCH_MAJOR	= 0
CROSSMATCH_MINOR	= 0
CROSSMATCH_SONAME	= 1

prefix		= /home/xy/software/FindVariationSequence/CrossMatchLibrary
exec_prefix	= ${prefix}
DESTDIR		= 
CROSSMATCH_LIB	= ${DESTDIR}${exec_prefix}/lib
CROSSMATCH_INCLUDE	= ${DESTDIR}${prefix}/include
INSTALL_DIRS	= ${prefix} ${CROSSMATCH_INCLUDE} ${CROSSMATCH_LIB}


SHELL =		/bin/sh
ARCHIVE =	ar rv
RANLIB =	ranlib
CC =		g++
CFLAGS =	-g -O2 -fPIC
SSE_FLAGS =	
FC =		gfortran
LDFLAGS =	$(CFLAGS)
DEFS =		-DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MATH_H=1 -DHAVE_LIMITS_H=1 -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -DHAVE_FTRUNCATE=1 -DHAVE_LONGLONG=1 -DHAVE_SHMEM_SERVICES=1 -DHAVE_NET_SERVICES=1
LIBS =		
FLEX =		flex
BISON =		bison

SHLIB_LD =	g++ -shared -Wl,-soname,libcrossmatch${SHLIB_SUFFIX}.${CROSSMATCH_SONAME}
SHLIB_SUFFIX =	.so
CROSSMATCH_SHLIB =	libcrossmatch${SHLIB_SUFFIX}.${CROSSMATCH_SONAME}.${CROSSMATCH_MAJOR}.${CROSSMATCH_MINOR}
CROSSMATCH_SHLIB_SONAME = libcrossmatch${SHLIB_SUFFIX}.${CROSSMATCH_SONAME}

CORE_SOURCES = CrossMatch.cpp Partition.cpp cmutils.cpp

SOURCES = ${CORE_SOURCES}

OBJECTS = 	${SOURCES:.cpp=.o}

CORE_OBJECTS = 	${CORE_SOURCES:.cpp=.o}


all:
		${MAKE} stand_alone;

stand_alone:	libcrossmatch.a

libcrossmatch.a:	${OBJECTS}
		${ARCHIVE} libcrossmatch.a ${OBJECTS}; \
		${RANLIB} libcrossmatch.a;

shared: libcrossmatch${SHLIB_SUFFIX}

libcrossmatch${SHLIB_SUFFIX}: ${OBJECTS}
		${SHLIB_LD} ${LDFLAGS} -o ${CROSSMATCH_SHLIB} ${OBJECTS} -lm ${LIBS}
		@if [ "x${CROSSMATCH_SHLIB_SONAME}" != x ]; then \
			ln -sf ${CROSSMATCH_SHLIB} ${CROSSMATCH_SHLIB_SONAME}; \
			ln -sf ${CROSSMATCH_SHLIB_SONAME} $@; \
		fi

install:	libcrossmatch.a $(INSTALL_DIRS)
		/bin/cp -a libcrossmatch* ${CROSSMATCH_LIB}
		/bin/cp CrossMatch.h Partition.h cmhead.h ${CROSSMATCH_INCLUDE}

.c.o:
		$(CC) -c $(CFLAGS) $(DEFS) $<


clean:
	-	/bin/rm -f *.o libcrossmatch*

# Make target which outputs the list of the .o contained in the crossmatch lib
# usefull to build a single big shared library containing Tcl/Tk and other
# extensions.  used for the Tcl Plugin. 

crossmatchLibObjs:
	@echo ${CORE_OBJECTS}

crossmatchLibSrcs:
	@echo ${SOURCES}

# This target actually builds the objects needed for the lib in the above
# case
objs: ${CORE_OBJECTS}

$(INSTALL_DIRS):
	@if [ ! -d $@ ]; then mkdir -p $@; fi


