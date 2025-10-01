CFLAGS= -D__LINUX__ -c -Wall -O2
LIBS = -lm -lbb_proximity -lpthread

all: led_boost 

led_boost: main.o
	g++ main.o $(LIBS) -o led_boost

main.o: main.cpp
	g++ $(CFLAGS) main.cpp

clean:
	rm *.o led_boost
