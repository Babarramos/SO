heatSim: main.o matrix2d.o mplib3.o leQueue.o
	gcc  -pthread -o heatSim *.o

main.o: main.c matrix2d.h
	gcc -g -Wall -pedantic -c main.c

matrix2d.o: matrix2d.c matrix2d.h
	gcc -g -Wall -pedantic -c matrix2d.c

mplib3.o: mplib3.c mplib3.h leQueue.h
	gcc -g -Wall -pedantic -c mplib3.c

leQueue.o: leQueue.c leQueue.h
	gcc -g -Wall -pedantic -c leQueue.c

clean:
	rm -f *.o heatSim

run:
	./heatSim 6 10 10 20 20 8 2 2

