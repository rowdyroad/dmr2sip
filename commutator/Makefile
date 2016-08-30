CC=g++
CFLAGS=-std=c++11 -g
LIBS=-lpthread -lasound
SIP_LIBS=-llinphone -lmediastreamer_base -I/usr/include/mysql/ -lmysqlpp -L/usr/local/lib -Isrc/include -Isrc/vendor/include -Lsrc/vendor/lib -ljson
BUILD_DIR=build

SOURCES=$(wildcard src/include/**/*.cpp)

all: commutator dmr

commutator:
	$(CC) src/commutator.cpp $(SOURCES) $(CFLAGS)  $(LIBS) $(SIP_LIBS) -o $(BUILD_DIR)/commutator

dmr:
	$(CC) src/dmr.cpp $(SOURCES) $(CFLAGS) $(LIBS) $(SIP_LIBS)  -o $(BUILD_DIR)/dmr
