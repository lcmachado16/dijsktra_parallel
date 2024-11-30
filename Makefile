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

clean:
	$(RM) $(EXEC).o $(EXEC)
