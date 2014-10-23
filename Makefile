BUILD_OPTIONS += -std=c++11 -Wall

ifeq ($(DEBUG), 1)
	BUILD_OPTIONS += -g
else
	BUILD_OPTIONS += -O3
endif

all : main.exe

clean : 
	rm main.exe

main.exe : src/main.cpp src/*.h
	g++ -o $@ $^ $(BUILD_OPTIONS)