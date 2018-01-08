all:
	g++ -std=c++11 -DUSE_BOOST ipc.cpp -o ipc `pkg-config --cflags --libs opencv` -pthread -lboost_regex
	#g++ -std=c++11 -DUSE_BOOST ipc.cpp -o ipc -lopencv_core -lopencv_imgproc -lopencv_highgui -pthread -lboost_regex
