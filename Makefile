.PHONY: gitstatus.txt help clean
BITS=
#BITS=-m32
#BITS=-m64

all: onmi

help:
	echo check the makefile

clean:
	-rm tags *.o onmi

tags:
	ctags *.[ch]pp


					#-Wclobbered   -Wempty-body   \ -Wignored-qualifiers  -Woverride-init   \ -Wtype-limits   -Wunused-but-set-parameter 
# I'm including most of the -Wextra flags, but I want rid of the enum-in-conditional warning from boost
CXXFLAGS:=       \
          -Wmissing-field-initializers   \
          -Wsign-compare   \
          -Wuninitialized   \
          -Wunused-parameter    \
          -Wunused             \
          -Wall -Wformat #-Werror


#CXXFLAGS= ${BITS}     -g
LDFLAGS+= -lrt -lm
CC=g++
#CXXFLAGS= ${BITS} -O3 -p -pg ${CXXFLAGS} # -DNDEBUG
CXXFLAGS:= ${BITS} -O3        ${CXXFLAGS} # -DNDEBUG
#CXXFLAGS=              -O2                 

onmi: Range.o onmi.o cmdline.o

cmdline.c.FORCE:
	# remake cmdline.c . But it's OK unless you change the .ggo file. You'll need gengetopt(1) to be able to run this.
	gengetopt  --unamed-opts < onmi_opts.ggo
