#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"graph.h"

#define MAX_NAMES 200
#define MAX_LINE_SIZE 20000

typedef struct pair{
    char* p1;
    char* p2;
}pair;

void trim(char *str);
void loadGraph(Graph* graph, char* filename);
int prefers(Graph* graph, char* person, char* partner, char* newPartner);
void loadPreferences(Graph* graph, edgeVector* prefs, int person);
int getIndex(Graph* graph, char* person);
char* getVertex(Graph* graph, int index);
void saveMatching(pair *matching, int numVertices, char* filename);
void stableRoommates(Graph* graph);

int main(){
    Graph* graph = init(0, 0);
    loadGraph(graph, "students_with_preferences.csv");
    printf("Graph Loaded Successfully\n");
    printf("Number of Edges = %d\nNumber of Vertices = %d\n", graph->numEdges, graph->numVertices);
    stableRoommates(graph);
    printf("Matching Saved Successfully\n");
    freeGraph(graph);
    return 0;
}

void trim(char *str) {
    int start = 0, end = strlen(str) - 1;
    while (isspace((unsigned char)str[start])) start++;
    while (end > start && isspace((unsigned char)str[end])) end--;

    for (int i = start; i <= end; i++) str[i - start] = str[i];
    str[end - start + 1] = '\0';
}

void loadGraph(Graph* graph, char* filename){
    FILE* fptr = fopen(filename, "r");
    char line[MAX_LINE_SIZE];
    while(fgets(line, sizeof(line), fptr)){
        line[strcspn(line, "\n")] = 0;
        int count = 0;
        char* token = strtok(line, ",");
        char* name = strdup(token);
        while(token != NULL && count < MAX_NAMES){
            char* p = strdup(token);
            trim(p);
            if(count != 0) addEdge(graph, name, p, count);
            count++;
            token = strtok(NULL, ",");      
        }
        addEdge(graph, name, name, count);
    }
    fclose(fptr);
}


int prefers(Graph* graph, char* person, char* partner, char* newPartner){
    int partnerPref, newPartnerPref;
    partnerPref = newPartnerPref = 0;
    for(int i = 0; i < graph->numEdges; i++){
        if(strcmp(at(graph->edgelist, i).v1, person) == 0 && strcmp(at(graph->edgelist, i).v2, partner) == 0) partnerPref = at(graph->edgelist, i).pref;
        if(strcmp(at(graph->edgelist, i).v1, person) == 0 && strcmp(at(graph->edgelist, i).v2, newPartner) == 0) newPartnerPref = at(graph->edgelist, i).pref;
        if(!(partnerPref == 0 || newPartnerPref == 0)) break;
    }
    return newPartnerPref < partnerPref ? 1 : 0;
}

void loadPreferences(Graph* graph, edgeVector* prefs, int person){
    int startIndex = 0, x = 0;
    for(int i = 0; i < graph->numEdges; i++){
        if(i != 0) if(strcmp(at(graph->edgelist, i-1).v1, at(graph->edgelist, i).v1) != 0) x++;
        if(x == person){
            startIndex = i;
            break;
        }
    }
    for(int i = startIndex; i < graph->numEdges; i++){
        push_back(prefs, at(graph->edgelist, i));
        if(i != graph->numEdges-1) if(strcmp(at(graph->edgelist, i).v1, at(graph->edgelist, i+1).v1) != 0) break;
    }
}

int getIndex(Graph* graph, char* person){
    int x = 0;
    for(int i = 0; i < graph->numEdges; i++){
        if(i != 0) if(strcmp(at(graph->edgelist, i-1).v1, at(graph->edgelist, i).v1) != 0) x++;
        if(strcmp(at(graph->edgelist, i).v1, person) == 0) break;
    }
    return x;
}

char* getVertex(Graph* graph, int index){
    int x = 0;
    for(int i = 0; i < graph->numEdges; i++){
        if(i != 0) if(strcmp(at(graph->edgelist, i-1).v1, at(graph->edgelist, i).v1) != 0) x++;
        if(x == index) return at(graph->edgelist, i).v1;
    }
    return NULL;
}

void saveMatching(pair *matching, int numVertices, char* filename){
    FILE* fptr = fopen(filename, "w");
    fprintf(fptr, "Parner 1,Partner 2\n");
    for(int i = 0; i < numVertices; i++){
        if(matching[i].p1 == NULL || matching[i].p2 == NULL) break;
        fprintf(fptr, "%s,%s\n", matching[i].p1, matching[i].p2);
    }
}

void stableRoommates(Graph* graph){
    pair* matching = (pair*) malloc(graph->numVertices * sizeof(pair));
    int* free = (int*) malloc(graph->numVertices * sizeof(int));
    for(int i = 0; i < graph->numVertices; i++){
        matching[i].p1 = NULL;
        matching[i].p2 = NULL;
        free[i] = 1;
    }

    int freeCount = graph->numVertices;
    while(freeCount > 0){
        int freePerson;
        for(freePerson = 0; freePerson < graph->numVertices; freePerson++) if(free[freePerson]) break;
        edgeVector* prefs;
        initVector(&prefs);
        loadPreferences(graph, prefs, freePerson);
        for(int i = 0; i < prefs->size; i++){
            char* partner;
            if(free[getIndex(graph, at(prefs, i).v2)]) partner = strdup(at(prefs, i).v2);
            else continue;
            if(matching[freePerson].p1 == NULL || prefers(graph, prefs->edges[0].v1, partner, matching[freePerson].p1)){
                if(matching[freePerson].p1 != NULL){
                    free[getIndex(graph, matching[freePerson].p1)] = 1;
                    free[getIndex(graph, matching[freePerson].p2)] = 1;
                    freeCount += 2;
                }
                matching[freePerson].p1 = strdup(prefs->edges[0].v1);
                matching[freePerson].p2 = strdup(partner);
                free[freePerson] = 0;
                free[getIndex(graph, partner)] = 0;
                freeCount -= 2;
                break;
            }
        }
    }
    saveMatching(matching, graph->numVertices, "matching.csv");
}