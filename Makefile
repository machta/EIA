CXXFLAGS += -std=c++11 -Wall

ifeq ($(DEBUG), 1)
	CXXFLAGS += -g
else
	CXXFLAGS += -O3
endif

all : main.exe

clean : 
	rm main.exe

main.exe : src/*.cpp src/*.h
	$(CXX) -o $@ $^ $(CXXFLAGS)