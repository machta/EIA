CXXFLAGS = -std=c++11 -Wall -pedantic -msse -fopenmp
TESTS = 1
SHELL = bash
COMMA = ,
TMP := $(shell mktemp)

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
	./release.sh
	
release_star : 
	qrun.sh 4c 1 serial ./release.sh | awk 'NR == 2 { print $$3 }' > $(TMP)
	while [ $$(qstat | grep $$(cat $(TMP)) | wc -l) != 0 ]; do sleep 10; done;

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
	
gnuplot :
	cat out.log | awk '{\
	n = $$1;\
	a[1] = a[4] = a[5] = a[8] = -1/6*n + 3/2*n*n + 2/3*n*n*n;\
	a[2] = a[6] = a[9] = 1/3*n + 2*n*n + 2/3*n*n*n;\
	a[3] = a[7] = a[10] = 1/6*n + 5/2*n*n + 1/3*n*n*n;\
	printf "%d", n;\
	for (i = 2; i <= NF; i++)\
		if ($$i == 0) printf " 0";\
		else printf " %f", a[i - 1]/($$i/(1000*1000*1000));\
	printf "\n";\
	}' > $(TMP)
	
	echo 'set term pdf;\
	set output "graph";\
	plot\
	$(if $(shell str=$(TESTS)111111111111111111111 ; echo $${str:0:1} | tr -d 0), "$(TMP)" using 1:2 with lines title "GaussTrivial"$(COMMA))\
	$(if $(shell str=$(TESTS)111111111111111111111 ; echo $${str:1:1} | tr -d 0), "$(TMP)" using 1:3 with lines title "LUTrivial"$(COMMA))\
	$(if $(shell str=$(TESTS)111111111111111111111 ; echo $${str:2:1} | tr -d 0), "$(TMP)" using 1:4 with lines title "CholeskyTrivial"$(COMMA))\
	$(if $(shell str=$(TESTS)111111111111111111111 ; echo $${str:3:1} | tr -d 0), "$(TMP)" using 1:5 with lines title "GaussOptimizedSimple"$(COMMA))\
	$(if $(shell str=$(TESTS)111111111111111111111 ; echo $${str:4:1} | tr -d 0), "$(TMP)" using 1:6 with lines title "GaussOptimized"$(COMMA))\
	$(if $(shell str=$(TESTS)111111111111111111111 ; echo $${str:5:1} | tr -d 0), "$(TMP)" using 1:7 with lines title "LUOptimizedSimple"$(COMMA))\
	$(if $(shell str=$(TESTS)111111111111111111111 ; echo $${str:6:1} | tr -d 0), "$(TMP)" using 1:8 with lines title "CholeskyOptimizedSimple"$(COMMA))\
	' | gnuplot
	
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
	
main.s : src/*.cpp src/*.h
	$(CXX) $^ $(CXXFLAGS) -S
	
main.exe : src/*.cpp src/*.h
	$(CXX) -o $@ $^ $(CXXFLAGS)

