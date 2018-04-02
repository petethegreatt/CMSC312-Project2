#
# File          : Makefile
# Description   : Build file for CMSC312 project 2


# Environment Setup
LIBDIRS=-L. 
INCLUDES=-I.
CC=gcc 
CFLAGS=-c $(INCLUDES) -g -Wall
LINK=gcc -g
LDFLAGS=$(LIBDIRS)
AR=ar rc
RANLIB=ranlib

# Suffix rules
.c.o :
	${CC} ${CFLAGS} $< -o $@

#
# Setup builds

PT-TARGETS=cmsc312-p2
CMSC312LIB=
CMSC312LIBOBJS=

# proj lib
LIBS=

#
# Project Protections

p3 : $(PT-TARGETS)

cmsc312-p2 : cmsc312-p2.o cmsc312-p2-mfu.o cmsc312-p2-second.o cmsc312-p2-lfu.o
	$(LINK) $(LDFLAGS) cmsc312-p2.o cmsc312-p2-mfu.o cmsc312-p2-second.o cmsc312-p2-lfu.o -o $@

lib$(CMSC312LIB).a : $(CMSC312LIBOBJS)
	$(AR) $@ $(CMSC312LIBOBJS)
	$(RANLIB) $@

clean:
	rm -f *.o *~ $(TARGETS) $(LIBOBJS) lib$(CMSC312LIB).a 
