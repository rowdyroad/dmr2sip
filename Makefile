osx:
	g++ src/main.cpp src/xnl_connection.cpp -stdlib=libc++ -lc++  -std=c++11 -o build/dmr2sip

linux:
	g++ src/main.cpp src/xnl_connection.cpp  -std=c++11 -lpthread -o build/dmr2sip