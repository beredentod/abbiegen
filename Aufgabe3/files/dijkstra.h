#pragma once

#define mp make_pair
#define pb push_back

using namespace std;

class Dijkstra
{
private:

	//der Graph, auf dem gearbeitet wird
	Graph* G;

	//map: Knoten -> die Entfernung vom Ursprung
	map<Node*, double> startToDistance; 
	//map: Knoten -> voriger Knoten im Pfad
	map<Node*, Node*> previousNode;
	//Menge mit bereits besuchten knoten
	set<int> visNodes;
	//Vorrangwarteschlange					
	multiset<Node*, Weight<Node> > pQ; 

	//die Entfernungen zwischen allen Knoten auf dem Weg von start zu end
	//werden hier bestimmt und Vorgeanger werden zugeordnet
	void determineConnections(Node* start, Node* end, bool direction);

public:
	Dijkstra(Graph* pGraph): G(pGraph) {} //Konstruktor
	Dijkstra(){};
	~Dijkstra(void){purge();} //Destruktor

	void init (Graph* pGraph)
	{
		G = pGraph;
	}

	void purge(); //setzt alle Werte zurueck

	//stellt n2 als den Vorgaenger von n1 ein
	void setPreviousNode(Node* n1, Node* n2) { previousNode[n1] = n2; }
	//gibt die Entfernung vom Ursprung des Knotens n aus
	double getStartDistance(Node* n) { return startToDistance.find(n)->second; }
	//stellt die Entfernung weight vom Ursprung des Knotens n ein
	void setStartDistance(Node* n, double weight) { startToDistance[n] = weight; }
	//die Pfade werden vom Ursprung bis zum Start betrachtet
	void getReversedTraversal(Node* end) { determineConnections(NULL, end, 0); }

	//Funktion bestimmt den kuerzesten Weg von start zu end
	Path* calculateShortestPath(Node* start, Node* end);


	//Die folgenden Methoden werden im Yen-Algorithmus verwendet

	//Die Funktion aktualisiert die Gewichte (Entfernungen)
	//der Knoten, die aus dem Knoten n fuehren
	Path* updateOutDistance(Node* n);
	//Die Funktion aktualisiert die Gewichte (Entfernungen)
	//der Knoten, die zum Knoten n fuehren
	void updateInDistance(Node* n);

};
