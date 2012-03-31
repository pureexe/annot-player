# Makefile
# 11/30/2011

CXXFLAGS = -g -O2
LDFLAGS = -lssh2 -L/usr/local/lib

all: x

x: main.cc
	g++ $< -o $@ $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f *.{o,core,stackdump}

# EOF
