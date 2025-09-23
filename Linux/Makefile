CFLAGS=-c -Wall -O2 -D__LINUX__ -I../src
LIBS = -lm -lpthread

all: libbb_proximity.a

libbb_proximity.a: bb_proximity.o
	ar -rc libbb_proximity.a bb_proximity.o ;\
	sudo cp libbb_proximity.a /usr/local/lib ;\
	sudo cp ../src/bb_proximity.h /usr/local/include

bb_proximity.o: ../src/bb_proximity.cpp ../src/bb_proximity.h
	$(CXX) $(CFLAGS) ../src/bb_proximity.cpp

clean:
	rm *.o libbb_proximity.a
