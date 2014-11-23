CXXFLAGS = -std=c++11 -Wall -pedantic -msse

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
	rm -f *.gcda
	make clean
	make OPTIMAL=1 PROF_GEN=1
	make test
	make performance_test
	make performance_test FROM=2000 TO=2002
	make clean
	make OPTIMAL=1

star : main.exe
	qrun.sh 12c 1 1slots_per_host queue/queue00.sh
	qrun.sh 12c 1 1slots_per_host queue/queue01.sh
	qrun.sh 12c 1 1slots_per_host queue/queue02.sh
	qrun.sh 12c 1 1slots_per_host queue/queue03.sh
	qrun.sh 12c 1 1slots_per_host queue/queue04.sh
	qrun.sh 12c 1 1slots_per_host queue/queue05.sh
	qrun.sh 12c 1 1slots_per_host queue/queue06.sh
	qrun.sh 12c 1 1slots_per_host queue/queue07.sh
	qrun.sh 12c 1 1slots_per_host queue/queue08.sh
	qrun.sh 12c 1 1slots_per_host queue/queue09.sh
	qrun.sh 12c 1 1slots_per_host queue/queue10.sh
	
gnuplot : main.exe
	cat out.log | awk '{\
	n = $$1;\
	n1 = n*n*n/3 /($$2/(1000*1000*1000));\
	n2 = n*n*n/3 /($$3/(1000*1000*1000));\
	n3 = n*n*n/3 /($$4/(1000*1000*1000));\
	n4 = n*n*n/3 /($$5/(1000*1000*1000));\
	n5 = n*n*n/3 /($$6/(1000*1000*1000));\
	n6 = n*n*n/3 /($$7/(1000*1000*1000));\
	n7 = n*n*n/3 /($$8/(1000*1000*1000));\
	print n, n1, n2, n3, n4, n5, n6, n7;\
	}' > tmp.log
	
	echo 'set term pdf;\
	set output "graph";\
	plot "tmp.log" using 1:2 with lines title "GaussTrivial",\
	"tmp.log" using 1:3 with lines title "LUTrivial",\
	"tmp.log" using 1:4 with lines title "CholeskyTrivial",\
	"tmp.log" using 1:5 with lines title "GaussOptimizedSimple",\
	"tmp.log" using 1:6 with lines title "GaussOptimized",\
	"tmp.log" using 1:7 with lines title "LUOptimizedSimple",\
	"tmp.log" using 1:8 with lines title "CholeskyOptimizedSimple"' | gnuplot
	
test : main.exe
	./main.exe $(TESTS) 1 test/symetric/t* | grep total
	./main.exe $(TESTS) 1 test/t* | grep total

performance_test : FROM = 500
performance_test : TO = 600
performance_test : main.exe
	./main.exe $(TESTS) 0 $(FROM) $(TO) > out.log

cache_test : FROM = 2000
cache_test : TO = 2001
cache_test : main.exe
	valgrind --tool=cachegrind --I1=$(shell echo 128*1024 | bc),2,64 --D1=$(shell echo 128*1024 | bc),2,64 --LL=$(shell echo 6*1024*1024| bc),48,64 ./main.exe $(TESTS) 0 $(FROM) $(TO)
	
main.s : src/*.cpp src/*.h
	$(CXX) $^ $(CXXFLAGS) -S
	
main.exe : src/*.cpp src/*.h
	$(CXX) -o $@ $^ $(CXXFLAGS)
