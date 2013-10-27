#
# Makefile for program FindVariationSequence:
#       FindVariationSequence


prefix		= /home/xy/software/FindVariationSequence
exec_prefix	= ${prefix}
DESTDIR		= 
CROSSMATCH_DIR	= ${DESTDIR}${prefix}/CrossMatchLibrary
CFITSIO_DIR	= ${DESTDIR}${prefix}/cfitsio
INSTALL_DIRS	= ${prefix}

CC =		g++

DEFS = -std=c++0x -c -g

LIBS = -L${CFITSIO_DIR}/lib -L${CROSSMATCH_DIR}/lib -lcrossmatch -lcfitsio -lm
INCLUDES = -I${CFITSIO_DIR}/include -I${CROSSMATCH_DIR}/include

CORE_SOURCES = utils.cpp CircleList.cpp FindVariationSequence.cpp main.cpp

SOURCES = ${CORE_SOURCES}

OBJECTS = 	${SOURCES:.cpp=.o}

CORE_OBJECTS = 	${CORE_SOURCES:.cpp=.o}


all:
		${MAKE} stand_alone;

stand_alone:	FindVariationSequence

FindVariationSequence2:	${OBJECTS}
		${CC} $(LIBS) ${OBJECTS} -o FindVariationSequence;

FindVariationSequence:	${SOURCES}
		${CC} $(INCLUDES) ${SOURCES} $(LIBS) -o FindVariationSequence;

.c.o:
		$(CC) $(INCLUDES) $(LIBS) $(DEFS) $<


install:	FindVariationSequence $(INSTALL_DIRS)
		/bin/cp -a FindVariationSequence ${INSTALL_DIRS}

clean:
	-	/bin/rm -f *.o FindVariationSequence

# This target actually builds the objects needed for the lib in the above
# case

outlibs:
	@echo ${LIBS}

outincludes:
	@echo ${INCLUDES}

objs: ${CORE_OBJECTS}

$(INSTALL_DIRS):
	@if [ ! -d $@ ]; then mkdir -p $@; fi


