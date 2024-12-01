FLAGS=-O2 -fopenmp

CC=gcc

RM=rm -f

EXEC=dijkstra

all: $(EXEC)  

$(EXEC): clean
	$(CC) $(FLAGS) $(EXEC).c -c -o $(EXEC).o
	$(CC) $(FLAGS) $(EXEC).o -o $(EXEC)

run:
	./$(EXEC)

entrega:
	tar -cvzf entrega.tar.gz $(EXEC).c Makefile *.pdf *.pptx *.txt run_tests.sh

clean:
	$(RM) $(EXEC).o $(EXEC)
