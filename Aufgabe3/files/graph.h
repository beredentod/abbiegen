#pragma once

#define mp make_pair
#define pb push_back
#define iPair pair<int,int>

using namespace std;             

class Graph
{

protected:
	//Start- und Zielpunkte aus der Textdatei
	iPair startPoint, endPoint; 
	//Pivotpunke des jeweiligen Start/Zielpunktes
	iPair startPointPivot, endPointPivot;

	//Indices der o.g. Punkte
	int startPointID, endPointID, startPointPivotID, endPointPivotID;
	//Indices der Start- und Zielstrasse (im Graphen als Knoten)
	int startStreetID, endStreetID;

	//Anzahl aller Punkte aus der Textdatei
	int pointsNum;
	//Die Mengen von allen Strassen und Punkten aus der Textdatei
	set<pair<iPair, iPair>> streetsList;
	set<iPair> points;

	map<iPair, int> pointToID; 	//map: Punkt -> ID
	map<int, iPair> IDToPoint; //map: ID -> Punkt
	map<pair<iPair,iPair>, int> streetToID; //map: strasse -> ID
	map<int, pair<iPair,iPair>> IDToStreet; //map: ID -> Strasse
	map<int, double> streetIDToWeight; //map: StrasseID -> Gewicht

	//map: ID -> alle benachbarten Punkte des Punktes ID
	map<int, set<pair<int, double>>> pointIDToNeighbors;

	//map: ID -> alle benachbarten Strassen der Strasse ID
	map<int, set<pair<int, int>>> streetIDToNeighbors;

	//map: Knoten -> Menge von benachbarten Knoten, die aus n fuehren
	map<Node*, set<Node*>*> nodeToOutNodes;
	//map: Knoten -> Menge von benachbarten Knoten, die zu n fuehren
	map<Node*, set<Node*>*> nodeToInNodes;

	//eine Map, die die Gewichte an den Kanten behaelt
	map<int, double> edgeToWeight; 	
	//eine Map, die die internen Indices der Knoten behaelt
	map<int, Node*> IDToNode;	

	vector<Node*> nodes; 		//Vektor mit allen Knoten
	int edgeNum;				//Anzahl aller Kanten
	int nodeNum;				//Anzahl aller Knoten

	//die Menge von Koten, die wir entfernen (bei Yen-Algorithmus)
	set<int> removedNodes;		
	//die Menge von Kanten, die wir entfernen (bei Yen-Algorithmus)
	set<pair<int,int>> removedEdges; 
	
private:
	//wandelt die Eingabe des jeweiligen Punktes um
	iPair convertInput(string data); 
	//misst den euklidischen Abstand zwischen zwei Punkten 
	double measureDistance(iPair a, iPair b);
	//prueft, ob die drei Punkte auf einer Geraden liegen
	bool checkTurn(iPair a, iPair b, iPair c);

	//stellt den Startpunkt ein
	void setStartPoint(iPair p) { startPoint = p; }
	//stellt den Zielpunkt ein
	void setEndPoint(iPair p) { endPoint = p; }
	//stellt den StartpunktID ein
	void setStartPointID(int id) { startPointID = id; }
	//stellt den ZielpunktID ein
	void setEndPointID(int id) { endPointID = id; }
	//stellt den StartstrasseID ein
	void setStartStreetID(int id) { startStreetID = id; }
	//stellt den ZielstrasseID ein
	void setEndStreetID(int id) { endStreetID = id; }

	//erstellt Pivotpunkte
	void createPivots();
	//Indices werden Punkten zugeordent (Graph-Typ 1)
	void assignIndicesOriginal();
	//Indices werden Punkten und Strassen zugeordent (Graph-Typ 2)
	void assignIndices();
	//Die Abbiegungen zwischen Strassen werden bestimmt
	void determineTurns();

	//die Textdatei wird eingelsen und ein Graph-Typ 1 wird erstellt
	void readFileOriginal(string fileName, set<pair<iPair, iPair>> &streetsList, set<iPair> &points);
	//die Textdatei wird eingelsen und ein Graph-Typ 2 wird erstellt
	void readFile(string fileName, set<pair<iPair, iPair>> &streetsList, set<iPair> &points);

public:

	Graph(string fileName, int type) //Konstruktor
	{
		if (type == 1) readFileOriginal(fileName, streetsList, points);
		if (type == 2) readFile(fileName, streetsList, points);
	}
	~Graph(void){purge();} //Destruktor

	void purge(); //setzt alle Werte zurueck

	//gibt den gesuchten Knoten aus oder erstellt einen neuen,
	//wenn es ihn noch nicht gibt
	Node* getNode(int nodeID);

	//gibt die gesuchten Menge von Knoten (inNodes/outNodes) aus oder 
	//erstellt eine neue, wenn es keine gibt
	set<Node*>* getNodeSet(Node* n, map<Node*, set<Node*>*> &nodeToSet);

	//gibt einen internen Index einen Kante aus
	int getEdgeID(const Node* n1, const Node* n2) const { return n1->getID() * nodeNum + n2->getID();}
	
	//gibt das Gewicht an einer Kante aus, die nicht entfernt wurde
	double getEdgeWeight(const Node* n1, const Node* n2);
	//gibt das Gewicht an einer Kante aus, die entfernt wurde
	double getRemovedEdgeWeight(const Node* n1, const Node* n2);

	//gibt alle Knoten aus, die aus n fuehren
	void getOutNeighbors(Node* n, set<Node*> &nodeSet);
	//gibt alle Knoten aus, die nach n fuehren
	void getInNeighbors(Node* n, set<Node*> &nodeSet);
	
	//entfernt eine Kante (bei Yen-Algorithmus)
	void removeEdge(const pair<int,int> edge) { removedEdges.insert(edge); }
	//entfernt einen Knoten (bei Yen-Algorithmus)
	void removeNode(const int nodeID) { removedNodes.insert(nodeID); }
	
	//entfernt eine entfernte Kante aus der Liste der entferneten Kanten
	void restoreEdge(const pair<int,int> edge) { removedEdges.erase(removedEdges.find(edge)); }
	//entfernt einen entfernten Knoten aus der Liste der entferneten Knoten
	void restoreNode(int nodeID) { removedNodes.erase(removedNodes.find(nodeID)); }
	
	//entfernt alle Kanten aus der Liste der entferneten Kanten
	void purgeRemovedEdges() { removedEdges.clear(); }
	//entfernt alle Knoten aus der Liste der entferneten Knoten
	void purgeRemovedNodes() { removedNodes.clear(); }

	double retrieveAvgNumNeighbors ()
	{
		int sum = 0;
		for (map<int, set<pair<int, int>>>::iterator it = streetIDToNeighbors.begin(); it != streetIDToNeighbors.end(); ++it)
		{
			sum += it->second.size();
		}
		return (double) sum/streetIDToNeighbors.size();
	}

	double retrieveGMeanNeighbors ()
	{
		int product = 0;
		for (map<int, set<pair<int, int>>>::iterator it = streetIDToNeighbors.begin(); it != streetIDToNeighbors.end(); ++it)
		{
			product += it->second.size();
		}
		return pow(product, (double)1/streetIDToNeighbors.size());
	}
	

	//Folgende Funktionen lassen die Variablen aus der Klasse abrufen
	//und ermoeglichen die Variablen, sie spaeter in main.cpp wiederzunutzen
	int retrieveEdgeNum() { return edgeNum; }
	map<int, double> retrieveStreetWeights() { return streetIDToWeight; }
	map<int, pair<iPair, iPair>> retrieveStreets() { return IDToStreet; }
	set<iPair> retrieveAllPoints() { return points; }
	set<pair<iPair,iPair>> retrieveAllStreets() { return streetsList; }
	int retrieveStartStreet() { return startStreetID; }
	int retrieveEndStreet() { return endStreetID; }
	int retrieveStartPointID() { return startPointID; }
	int retrieveEndPointID() { return endPointID; }
	iPair retrieveStartPoint() { return startPoint; }
	iPair retrieveStartPointPivot() { return startPointPivot; }
	iPair retrieveEndPoint() { return endPoint; }
	iPair retrieveEndPointPivot() { return endPointPivot; }
};



