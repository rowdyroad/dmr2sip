CC=g++
CFLAGS=-std=c++11 -g
LIBS=-lpthread
SIP_LIBS=-llinphone -lmediastreamer_base -lconfig++ -I/usr/include/mysql/ -lmysqlpp -L/usr/local/lib
BUILD_DIR=build

TARGET=commutator

SOURCES=$(wildcard src/include/**/*.cpp)
all: $(TARGET) 

$(TARGET):
	$(CC) src/commutator.cpp $(SOURCES) $(CFLAGS) $(LIBS) $(SIP_LIBS) -o $(BUILD_DIR)/$(TARGET)

