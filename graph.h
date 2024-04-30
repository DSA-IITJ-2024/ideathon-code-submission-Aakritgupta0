#ifndef GRAPH_H
#define GRAPH_H

typedef struct Edge{
    char* v1;
    char* v2;
    int pref;
}Edge;

typedef struct edgeVector{
    Edge* edges;
    size_t size;
    size_t capacity;
}edgeVector;

typedef struct Graph{
    int numVertices;
    int numEdges;
    edgeVector* edgelist;
}Graph;

void initVector(edgeVector** edgelist);
void push_back(edgeVector* edgelist, Edge edge);
Edge at(edgeVector* edgelist, int index);
void freeVector(edgeVector* edgelist);

Graph* init(int numVertices, int numEdges);
int findVertex(Graph* graph, char* p);
int findEdge(Graph* graph, char* p1, char* p2);
void addEdge(Graph* graph, char* p1, char* p2, int pref);
// void removeEdge(Graph* graph, int v1, int v2);
// void removeVertex(Graph* graph, int v);
void freeGraph(Graph* graph);

#endif