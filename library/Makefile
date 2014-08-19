CC = gcc
CCC = g++
CXX = g++

BASICOPTS = -g -Wall
CFLAGS = $(BASICOPTS)

CCFLAGS = $(BASICOPTS) -std=c++11
CPPFLAGS += -isystem $(GTEST_DIR)/include

CXXFLAGS = $(BASICOPTS) -std=c++11
CXXFLAGS += -Wextra -pthread

ARCPP = $(AR) $(ARFLAGS) $@ 

#LWS_INCLUDE = -I/usr/local/lws/include/
#LWS_LIB = -L/usr/local/lws/lib/ -lwebsockets

REDIS_INCLUDE = -I/usr/local/hiredis/include
REDIS_LIB = -L/usr/local/hiredis/lib -lhiredis

COMM_INCLUDE = -I. $(REDIS_INCLUDE)
COMM_LIB= $(REDIS_LIB) -lcurl -lmcrypt -lcrypto -lxml2 -lstdc++ -lpthread -lm

TARGETDIR = build

all : $(TARGETDIR)/libcomm.a
OBJS = \
	$(TARGETDIR)/liboption.o \
	$(TARGETDIR)/libredisclient.o \
	$(TARGETDIR)/libprocessutil.o \
	$(TARGETDIR)/libxmlparser.o \
	$(TARGETDIR)/libstringhelper.o \
	$(TARGETDIR)/libcrypto.o \
	$(TARGETDIR)/libhttpclient.o \
	$(TARGETDIR)/libdatetime.o \
	$(TARGETDIR)/libjson.o \
	$(TARGETDIR)/liblog.o \
	$(TARGETDIR)/libqueue.o \
	$(TARGETDIR)/beanstalk.o \
	$(TARGETDIR)/json.o \
	$(TARGETDIR)/beanstalk-wrapper.o

$(TARGETDIR)/libcomm.a : $(TARGETDIR) $(OBJS)
	$(ARCPP) $(OBJS)

$(TARGETDIR)/%.o : %.cc
		$(CCC) $(CCFLAGS) $(COMM_INCLUDE)  -o $@  -c $(filter %.cc, $^)
$(TARGETDIR)/%.o : %.cpp
		$(CXX) $(CXXFLAGS) $(COMM_INCLUDE)  -o $@  -c $(filter %.cpp, $^)
$(TARGETDIR)/%.o : %.c
		$(CC) $(CFLAGS) $(COMM_INCLUDE)  -o $@  -c $(filter %.c, $^)

GTEST_DIR = ../dep_lib/gtest-1.7.0
USER_DIR = .

TESTS = comm_unittest

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

TESTDIR = test
$(TESTDIR)/gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc -o $(TESTDIR)/gtest-all.o

$(TESTDIR)/gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc -o $(TESTDIR)/gtest_main.o

$(TESTDIR)/gtest.a : $(TESTDIR)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

$(TESTDIR)/gtest_main.a : $(TESTDIR)/gtest-all.o $(TESTDIR)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

TESTOBJS = \
	$(TESTDIR)/test_libhttpclient.o \
	$(TESTDIR)/test_libjson.o \
	$(TESTDIR)/test_libqueue.o \
	$(TESTDIR)/test_libcrypto.o \
	$(TESTDIR)/test_libredisclient.o \
	$(TESTDIR)/test_libstringhelper.o \
	$(TESTDIR)/test_liboption.o \
	$(TESTDIR)/test_libxmlparser.o \
	$(TESTDIR)/test_liblog.o \
	$(TESTDIR)/test_libdatetime.o

$(TESTDIR)/%.o : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(COMM_INCLUDE) -o $@ -c $(filter %.cpp, $^)

$(TESTS) : $(TESTDIR) $(TESTOBJS) $(TARGETDIR)/libcomm.a $(TESTDIR)/gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(filter %.o %.a, $^) $(COMM_LIB) -o $@

unittest : $(TESTS)
	LD_LIBRARY_PATH=/usr/local/hiredis/lib ./$(TESTS)

clean :
	rm -f $(TARGETDIR)/libcomm.a $(TARGETDIR)/*.o
	rm -f $(TESTS) $(TESTDIR)/gtest.a $(TESTDIR)/gtest_main.a $(TESTDIR)/*.o
	rm -fr $(TARGETDIR)
	rm -fr $(TESTDIR)

$(TESTDIR) :
	mkdir -p $(TESTDIR)

$(TARGETDIR) :
	mkdir -p $(TARGETDIR)

#gcc -g -Wall -std=c++11 web_socket_server.cpp ../comm/library/libjson.cpp ../comm/library/json.c ../comm/library/libqueue.cpp ../comm/library/beanstalk.c ../comm/library/beanstalk.cc -I ../comm/library/ -I /usr/local/lws/include/ -L /usr/local/lws/lib/ -lwebsockets -lcurl -lstdc++ -lm -o wss

