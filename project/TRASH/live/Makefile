# Makefile
# 2/10/2011
LIVE_HOME = $(HOME)/opt/live
CXXFLAGS = -O2 \
		   -I$(LIVE_HOME)/liveMedia/include \
		   -I$(LIVE_HOME)/groupsock/include \
		   -I$(LIVE_HOME)/UsageEnvironment/include \
		   -I$(LIVE_HOME)/BasicUsageEnvironment/include \
		   -I/opt/local/include 
LDFLAGS	= -L$(LIVE_HOME)/liveMedia -lliveMedia \
		  -L$(LIVE_HOME)/groupsock -lgroupsock \
		  -L$(LIVE_HOME)/UsageEnvironment -lUsageEnvironment \
		  -L$(LIVE_HOME)/BasicUsageEnvironment -lBasicUsageEnvironment
QTINCL	= -I/opt/local/include/Qt
QTLIBS	= -F/opt/local/lib -framework QtCore

CXX = g++

all: main
main: main.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(QTINCL) $(QTLIBE) -o $@ $^
test: test.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^
