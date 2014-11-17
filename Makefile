CXXFLAGS = -std=c++11 -Wall -msse

TESTS = 1

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

release :
	rm -f main.gcda
	make clean
	make OPTIMAL=1 PROF_GEN=1
	make test
	make performance_test
	make performance_test FROM=2000 TO=2002
	make clean
	make OPTIMAL=1

star : main.exe
	qrun.sh 12c 1 1slots_per_host queue00.sh
	qrun.sh 12c 1 1slots_per_host queue01.sh
	qrun.sh 12c 1 1slots_per_host queue02.sh
	qrun.sh 12c 1 1slots_per_host queue03.sh
	qrun.sh 12c 1 1slots_per_host queue04.sh
	qrun.sh 12c 1 1slots_per_host queue05.sh
	qrun.sh 12c 1 1slots_per_host queue06.sh
	qrun.sh 12c 1 1slots_per_host queue07.sh
	qrun.sh 12c 1 1slots_per_host queue08.sh
	qrun.sh 12c 1 1slots_per_host queue09.sh
	qrun.sh 12c 1 1slots_per_host queue10.sh
	
test : main.exe
	./main.exe $(TESTS) 1 test/symetric/t* | grep total
	./main.exe $(TESTS) 1 test/t* | grep total

performance_test : FROM = 500
performance_test : TO = 600
performance_test : main.exe
	./main.exe $(TESTS) 0 $(FROM) $(TO)

cache_test : FROM = 2000
cache_test : TO = 2001
cache_test : main.exe
	valgrind --tool=cachegrind --I1=$(shell echo 128*1024 | bc),2,64 --D1=$(shell echo 128*1024 | bc),2,64 --LL=$(shell echo 6*1024*1024| bc),48,64 ./main.exe $(TESTS) 0 $(FROM) $(TO)
	
main.s : src/main.cpp src/*.h
	$(CXX) $^ $(CXXFLAGS) -S
	
main.exe : src/main.cpp src/*.h
	$(CXX) -o $@ $^ $(CXXFLAGS)