LIBS = `pkg-config --cflags --libs opencv4`

make :
	g++ $(LIBS) -std=c++11 main.cpp -o main
clean :
	rm main
