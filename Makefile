CC = g++ -O3
CFLAGS = -std=c++11 -Wall

GAPBIL : algs.o pbil.o ga.o
	$(CC) -o $@ algs.o pbil.o ga.o

algs.o : algs.cpp algs.h
	$(CC) -c $(CFLAGS) algs.cpp -o $@

pbil.o : PBIL.cpp PBIL.h Individual.h
	$(CC) -c $(CFLAGS) PBIL.cpp -o $@

ga.o : GA.cpp GA.h Individual.h 
	$(CC) -c $(CFLAGS) GA.cpp -o $@

clean:
	rm GAPBIL
	rm *.o
