#
# This file is generated by Ray 2009.1.6
#
CC	= g++ 
CFLAGS = -O2 -Wall
INCDIR = -I../include/athena -I/usr/include/libxml2
LIBDIR = -L../lib
LIBS = -lrt -lpthread -lutility -lxml2
DEBUG = -g -D_DEBUG  
DEFINE += $(DEBUG)

############################################
TESTRLOG = ../testRLog
TESTRBasicApp = ../testRBasicApp
TESTRTHREAD = ../testRThread 
TESTRSEMAPHORE = ../testRSemaphore
TESTRNETADDRESS = ../testRNetAddress
TESTRSTREAM = ../testRStream
TESTRSTRING = ../testRString
TESTRMSGQUEUE = ../testRMsgQueue
TESTREXCEPTION = ../testRException
TESTRMUTEX = ../testRMutex
TESTRXML = ../testRXml
TESTRTcpSocket = ../testRTcpSocket
PROGS = $(TESTRTHREAD) $(TESTRSEMAPHORE) $(TESTRNETADDRESS) $(TESTRSTREAM) $(TESTRSTRING) \
	$(TESTRMSGQUEUE) $(TESTREXCEPTION) $(TESTRMUTEX) $(TESTRXML) $(TESTRTcpSocket) ${TESTRBasicApp} ${TESTRLOG}

all : $(PROGS)

${TESTRLOG}: testRLog.cxx
	$(CC) $(CFLAGS) $(DEFINE) -o $@ $? $(INCDIR) $(LIBDIR) $(LIBS) 

$(TESTREXCEPTION):  testRException.cxx
	$(CC) $(CFLAGS) $(DEFINE) -o $@ $? $(INCDIR) $(LIBDIR) $(LIBS) 

$(TESTRMSGQUEUE):  testRMsgQueue.cxx
	$(CC) $(CFLAGS) $(DEFINE) -o $@ $? $(INCDIR) $(LIBDIR) $(LIBS) 

$(TESTRTHREAD): testRThread.cxx
	$(CC) $(CFLAGS) $(DEFINE) -o $@ $? $(INCDIR) $(LIBDIR) $(LIBS) 

$(TESTRSEMAPHORE): testRSemaphore.cxx
	$(CC) $(CFLAGS) $(DEFINE) -o $@ $? $(INCDIR) $(LIBDIR) $(LIBS) 

$(TESTRNETADDRESS): testRNetAddress.cxx
	$(CC) $(CFLAGS) $(DEFINE) -o $@ $? $(INCDIR) $(LIBDIR) $(LIBS) 

$(TESTRSTREAM): testRStream.cxx
	$(CC) $(CFLAGS) $(DEFINE) -o $@ $? $(INCDIR) $(LIBDIR) $(LIBS) 

$(TESTRSTRING): testRString.cxx
	$(CC) $(CFLAGS) $(DEFINE) -o $@ $? $(INCDIR) $(LIBDIR) $(LIBS) 

$(TESTRMUTEX): testRMutex.cxx
	$(CC) $(CFLAGS) $(DEFINE) -o $@ $? $(INCDIR) $(LIBDIR) $(LIBS) 

$(TESTRXML):  testRXml.cxx
	$(CC) $(CFLAGS) $(DEFINE) -o $@ $? $(INCDIR) $(LIBDIR) $(LIBS) 

$(TESTRTcpSocket): testRTcpSocket.cxx
	$(CC) $(CFLAGS) $(DEFINE) -o $@ $? $(INCDIR) $(LIBDIR) $(LIBS) 

$(TESTRBasicApp): testRBasicApp.cxx
	$(CC) $(CFLAGS) $(DEFINE) -o $@ $? $(INCDIR) $(LIBDIR) $(LIBS) 

clean:
	rm -f ${PROGS}

