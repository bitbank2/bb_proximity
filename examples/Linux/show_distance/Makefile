CFLAGS= -D__LINUX__ -c -Wall -O2
LIBS = -lm -lbb_proximity -lpthread

all: show_distance

show_distance: main.o
	g++ main.o $(LIBS) -o show_distance 

main.o: main.cpp
	g++ $(CFLAGS) main.cpp

clean:
	rm *.o show_distance
