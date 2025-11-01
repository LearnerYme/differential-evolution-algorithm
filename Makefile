all: demo

demo: demo.cxx FunctionTest.h DECore.h
	g++ -o demo demo.cxx `root-config --cflags --libs` -std=c++17