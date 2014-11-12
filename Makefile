CXXFLAGS = -std=c++11 -Wall -msse

TESTS = 1

FROM_C = 2000
TO_C = 2000
FROM_P = 500
TO_P = 600

ifeq ($(DEBUG), 1)
	CXXFLAGS += -g
endif
	
ifeq ($(OPTIMAL), 1)
	CXXFLAGS += -O3 -march=opteron-sse3 -fprofile-use
else
	CXXFLAGS += -O0
endif

ifeq ($(PROF_GEN), 1)
	CXXFLAGS += -fprofile-generate
endif

all : main.exe

clean : 
	rm -f main.exe
	
star : main.exe
	qrun.sh 12c 1 1slots_per_host queue0.sh
	qrun.sh 12c 1 1slots_per_host queue1.sh
	qrun.sh 12c 1 1slots_per_host queue2.sh
	qrun.sh 12c 1 1slots_per_host queue3.sh
	
test : main.exe
	./main.exe $(TESTS) 1 test/symetric/t* | grep total
	./main.exe $(TESTS) 1 test/t* | grep total

performance_test : main.exe
	./main.exe $(TESTS) 0 $(FROM_P) $(TO_P)

cache_test : main.exe
	valgrind --tool=cachegrind --I1=$(shell echo 128*1024 | bc),2,64 --D1=$(shell echo 128*1024 | bc),2,64 --LL=$(shell echo 6*1024*1024| bc),48,64 ./main.exe $(TESTS) 0 $(FROM_C) $(TO_C)
	
main.s : src/main.cpp src/*.h
	$(CXX) $^ $(CXXFLAGS) -S
	
main.exe : src/main.cpp src/*.h
	$(CXX) -o $@ $^ $(CXXFLAGS)