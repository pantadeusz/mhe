#CFLAGS=-O3 -std=c++17
CFLAGS=-ggdb -std=c++17

all:  a.out

a.out: tsp1.o tsp_problem.o  
	g++ *.o

tsp_problem.o: tsp_problem.cpp
	g++ ${CFLAGS} -c tsp_problem.cpp

tsp1.o: tsp1.cpp
	g++ ${CFLAGS} -c tsp1.cpp

clean:
	rm *.o a.out
