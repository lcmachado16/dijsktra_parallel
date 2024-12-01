#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <sys/time.h>
#include <omp.h>

static unsigned long int next = 1;

int my_rand(void) {
	return ((next = next * 1103515245 + 12345) % ((u_long) RAND_MAX + 1));
}

void my_srand(unsigned int seed) {
	next = seed;
}

struct Graph {
	int nNodes;
	int *nEdges;
	int **edges;
	int **w;
};

struct Graph *createRandomGraph(int nNodes, int nEdges, int seed) {
	my_srand(seed);

	struct Graph *graph = (struct Graph *) malloc(sizeof(struct Graph));
	graph->nNodes = nNodes;
	graph->nEdges = (int *) malloc(sizeof(int) * nNodes);
	graph->edges = (int **) malloc(sizeof(int *) * nNodes);
	graph->w = (int **) malloc(sizeof(int *) * nNodes);

	int k, v;
	for (v = 0; v < nNodes; v++) {
		graph->edges[v] = (int *) malloc(sizeof(int) * nNodes);
		graph->w[v] = (int *) malloc(sizeof(int) * nNodes);
		graph->nEdges[v] = 0;
	}

	int source = 0;
	for (source = 0; source < nNodes; source++) {
		int nArestasVertice = (double) nEdges / nNodes
				* (0.5 + my_rand() / (double) RAND_MAX);
		for (k = nArestasVertice; k >= 0; k--) {
			int dest = my_rand() % nNodes;
			int w = 1 + (my_rand() % 10);
			graph->edges[source][graph->nEdges[source]] = dest;
			graph->w[source][graph->nEdges[source]++] = w;
		}
	}

	return graph;
}

int *dijkstra(struct Graph *graph, int source) {
    int nNodes = graph->nNodes;
    int *visited = (int *) malloc(sizeof(int) * nNodes);
    int *distances = (int *) malloc(sizeof(int) * nNodes);
    int v, k;

    for (v = 0; v < nNodes; v++) {
        distances[v] = INT_MAX;
        visited[v] = 0;
    }
    distances[source] = 0;
    visited[source] = 1;

    for (k = 0; k < graph->nEdges[source]; k++)
        distances[graph->edges[source][k]] = graph->w[source][k];

    for (v = 1; v < nNodes; v++) {

        int min = -1;
        int minValue = INT_MAX;

        #pragma omp parallel
        {
            int localMin = -1; /* local variable for each thread*/
            int localMinValue = INT_MAX;

            #pragma omp for nowait
            for (k = 0; k < nNodes; k++) {
                if (visited[k] == 0 && distances[k] < localMinValue) {
                    localMinValue = distances[k];
                    localMin = k;
                }
            }

            #pragma omp critical
            {
                if (localMinValue < minValue) {
                    minValue = localMinValue;
                    min = localMin;
                }
            }
        }

        if (min == -1) break;

        visited[min] = 1;

        #pragma omp parallel
        {
            #pragma omp single
            for (k = 0; k < graph->nEdges[min]; k++) {
                int dest = graph->edges[min][k];
                int newDist = distances[min] + graph->w[min][k];
                if (newDist < distances[dest]) {
                    #pragma omp task firstprivate(dest, newDist)
                    {
                        if (newDist < distances[dest])
                            distances[dest] = newDist;
                    }
                }
            }
        }
    }

    free(visited);
    return distances;
}

/* ********************************************************************************** 
 *  Get Time Function
 *  Source: https://bitbucket.org/jruschel/parallel-cholesky/src/master/lib/hpcelo.c
 ************************************************************************************/
double hpcelo_t1, hpcelo_t2;

double hpcelo_gettime (void)
{
  struct timeval tr;
  gettimeofday(&tr, NULL);
  return (double)tr.tv_sec+(double)tr.tv_usec/1000000;
}

// Macros para iniciar, terminar e obter o tempo
#define HPCELO_START_TIMER  hpcelo_t1 = hpcelo_gettime();
#define HPCELO_END_TIMER    hpcelo_t2 = hpcelo_gettime();
#define HPCELO_GET_TIMER    (hpcelo_t2 - hpcelo_t1)

int main(int argc, char ** argv) {

	int nNodes;
	int nEdges;
	int seed;

	if (argc == 4) {
		nNodes = atoi(argv[1]);
		nEdges = atoi(argv[2]);
		seed = atoi(argv[3]);
	} else {
		fscanf(stdin, "%d %d %d", &nNodes, &nEdges, &seed);
	}

	nEdges = nNodes * nEdges;

	struct Graph *graph = createRandomGraph(nNodes, nEdges, seed);

	/*---------------------------------------------------------------------------------*/	
	// clock_t start, end;
	// start = clock();
	HPCELO_START_TIMER
	/*---------------------------------------------------------------------------------*/
	int *dist = dijkstra(graph, 0);
	/*---------------------------------------------------------------------------------*/
	HPCELO_END_TIMER
	// end = clock();

	FILE *file_result = fopen("resultados.txt", "a"); 
  	fprintf(file_result, "{ nNodes: %d :: nEdges: %d  :: seed: %d  :: ", nNodes, nEdges, seed);
	// fprintf(file_result, " Tempo de execução: %f\t :: (mean/nodes): ", (double)(end - start) / CLOCKS_PER_SEC);
	fprintf(file_result, " Tempo de execução: %f\t :: (mean/nodes): ", HPCELO_GET_TIMER);
	fclose(file_result);
	/*---------------------------------------------------------------------------------*/



	double mean = 0;
	int v;

	for (v = 0; v < graph->nNodes; v++)
		mean += dist[v];

	fprintf(stdout, "%.2f}\n ", mean / nNodes);

	return 0;
}
