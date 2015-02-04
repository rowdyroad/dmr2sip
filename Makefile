sip_linux: 
	g++ src/linphone.cpp -llinphone -lmediastreamer_base -o build/linphone

dmr_osx:
	g++ src/main.cpp src/xnl_connection.cpp -stdlib=libc++ -lc++  -std=c++11 -o build/dmr2sip

dmr_linux:
	g++ src/main.cpp src/xnl_connection.cpp  -std=c++11 -lpthread -o build/dmr2sip