CC=g++
CFLAGS=-std=c++11 -g
LIBS=-lpthread
SIP_LIBS=-llinphone -lmediastreamer_base -lconfig++ -lsqlite -lvsqlitepp
BUILD_DIR=build

TARGET=commutator

SOURCES=$(wildcard src/include/**/*.cpp)
all: $(TARGET) 

$(TARGET):
	$(CC) src/linphone.cpp $(SOURCES) $(CFLAGS) $(LIBS) $(SIP_LIBS) -o $(BUILD_DIR)/$(TARGET)

