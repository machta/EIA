CXXFLAGS += -std=c++11 -Wall

ifeq ($(DEBUG), 1)
	CXXFLAGS += -g
else
	CXXFLAGS += -O3
endif

all : main.exe

clean : 
	rm main.exe
	
test : main.exe
	./main.exe 1   1 test/symetric/t* | grep total
	./main.exe 110 1 test/t* | grep total

main.exe : src/main.cpp src/*.h
	$(CXX) -o $@ $^ $(CXXFLAGS)