
SOURCES    := ${wildcard *.cpp}
OBJECTS    := ${SOURCES:.cpp=.o}


all: mhe

mhe: $(OBJECTS)
	g++ -std=c++14 $^ -o $@

%.o: %.cpp
	gcc -std=c++14 -c $< -o $@

clean:
	rm -f *.o mhe