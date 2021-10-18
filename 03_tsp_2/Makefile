all:  a.out

a.out: tsp1.o tsp_problem.o  
	g++ *.o

tsp_problem.o: tsp_problem.cpp
	g++ -c tsp_problem.cpp

tsp1.o: tsp1.cpp
	g++ -c tsp1.cpp

clean:
	rm *.o a.out
