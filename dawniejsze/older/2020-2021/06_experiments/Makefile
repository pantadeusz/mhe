
SOURCES    := ${wildcard *.cpp}
OBJECTS    := ${SOURCES:.cpp=.o}


all: mhe

mhe: $(OBJECTS)
	g++ -O3 -std=c++14 $^ -o $@

%.o: %.cpp
	gcc -O3 -std=c++14 -c $< -o $@

clean:
	rm -f *.o mhe