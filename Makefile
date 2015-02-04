CC=g++
CFLAGS=-std=c++11
LIBS=-lpthread
SIP_LIBS=-llinphone -lmediastreamer_base
BUILD_DIR=build

SIP_TARGET=sip
DMR_TARGET=dmr

all: $(SIP_TARGET) 

$(SIP_TARGET):
	$(CC) src/linphone.cpp src/xnl_connection.cpp $(CFLAGS) $(LIBS) $(SIP_LIBS) -o $(BUILD_DIR)/$(SIP_TARGET)

$(DMR_TARGET):
	$(CC) src/main.cpp src/xnl_connection.cpp  $(LIBS) $(CFLAGS) -o $(BUILD_DIR)/$(DMR_TARGET)