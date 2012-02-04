# 2/1/2012
CFLAGS = -O2 -I/opt/local/include
LDFLAGS = -llua -L/opt/local/lib
luascript: luascript.cc
	g++ -o $@ $(LDFLAGS) $(CFLAGS) $^
