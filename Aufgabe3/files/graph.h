#pragma once

#define mp make_pair
#define pb push_back
#define iPair pair<int,int>
#define dPair pair<double,double>

using namespace std;             

class Graph
{

protected:
	//Start- und Zielpunkte aus der Textdatei
	dPair startPoint, endPoint, interPoint;
	//Pivotpunke des jeweiligen Start/Zielpunktes
	dPair startPointPivot, endPointPivot, interPointPivot;

	//Indizes der o.g. Punkte
	int startPointID, endPointID, interPointID, startPointPivotID, endPointPivotID, interPointPivotID;
	//Indizes der Start- und Zielstrasse (im Graphen als Knoten)
	int startStreetID, endStreetID, interStreetID;

	//Anzahl aller Punkte aus der Textdatei
	int pointsNum;
	//Die Mengen von allen Strassen und Punkten aus der Textdatei
	set<pair<dPair, dPair>> streetsList;
	set<dPair> points;

	map<dPair, int> pointToID; 	//map: Punkt -> ID
	map<int, dPair> IDToPoint; //map: ID -> Punkt
	map<pair<dPair,dPair>, int> streetToID; //map: strasse -> ID
	map<int, pair<dPair,dPair>> IDToStreet; //map: ID -> Strasse
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
	set<iPair> removedEdges; 
	
private:
	//wandelt die Eingabe des jeweiligen Punktes um
	dPair convertInput(string data); 
	//misst den euklidischen Abstand zwischen zwei Punkten 
	double measureDistance(dPair a, dPair b);
	//prueft, ob die drei Punkte auf einer Geraden liegen
	bool checkTurn(dPair a, dPair b, dPair c);

	//stellt den Startpunkt ein
	void setStartPoint(dPair p) { startPoint = p; }
	//stellt den Zielpunkt ein
	void setEndPoint(dPair p) { endPoint = p; }
	//stellt den Zwischenpunkt ein
	void setInterPoint(dPair p) { interPoint = p; }
	//stellt den StartpunktID ein
	void setStartPointID(int id) { startPointID = id; }
	//stellt den ZielpunktID ein
	void setEndPointID(int id) { endPointID = id; }
	//stellt den ZwischenpunktID ein
	void setInterPointID(int id) { interPointID = id; }
	//stellt den StartstrasseID ein
	void setStartStreetID(int id) { startStreetID = id; }
	//stellt den ZielstrasseID ein
	void setEndStreetID(int id) { endStreetID = id; }
	//stellt den ZwischenstrasseID ein
	void setInterStreetID(int id) { interStreetID = id; }

	//erstellt Pivotpunkte
	void createPivots();
	//Indices werden Punkten zugeordent (Graph-Typ 1)
	void assignIndicesOriginal();
	//Indices werden Punkten und Strassen zugeordent (Graph-Typ 2)
	void assignIndices();
	//Die Abbiegungen zwischen Strassen werden bestimmt
	void determineTurns();
	//feugt ins Programm einen Zwischenpunkt ein
	void addInterPoint();

	//die Textdatei wird eingelsen und ein Graph-Typ 1 wird erstellt
	void readFileOriginal(string fileName, set<pair<dPair, dPair>> &streetsList, set<dPair> &points);
	//die Textdatei wird eingelsen und ein Graph-Typ 2 wird erstellt
	void readFile(string fileName, set<pair<dPair, dPair>> &streetsList, set<dPair> &points, dPair interP = mp(-3,-3));

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
	void removeEdge(const iPair edge) { removedEdges.insert(edge); }
	//entfernt einen Knoten (bei Yen-Algorithmus)
	void removeNode(const int nodeID) { removedNodes.insert(nodeID); }
	
	//entfernt eine entfernte Kante aus der Liste der entferneten Kanten
	void restoreEdge(const iPair edge) { removedEdges.erase(removedEdges.find(edge)); }
	//entfernt einen entfernten Knoten aus der Liste der entferneten Knoten
	void restoreNode(int nodeID) { removedNodes.erase(removedNodes.find(nodeID)); }
	
	//entfernt alle Kanten aus der Liste der entferneten Kanten
	void purgeRemovedEdges() { removedEdges.clear(); }
	//entfernt alle Knoten aus der Liste der entferneten Knoten
	void purgeRemovedNodes() { removedNodes.clear(); }

	//Folgende Funktionen lassen die Variablen aus der Klasse abrufen
	//und ermoeglichen die Variablen, sie spaeter in main.cpp wiederzunutzen
	int retrieveEdgeNum() { return edgeNum; }
	map<int, double> retrieveStreetWeights() { return streetIDToWeight; }
	map<int, pair<dPair, dPair>> retrieveStreets() { return IDToStreet; }
	map<pair<dPair, dPair>, int> retrieveStreetsToID() { return streetToID; }
	map<int, set<pair<int, int>>> retrieveTurns() { return streetIDToNeighbors; }
	map<int, dPair> retrieveIDToPoint() { return IDToPoint; }
	set<dPair> retrieveAllPoints() { return points; }
	set<pair<dPair,dPair>> retrieveAllStreets() { return streetsList; }
	int retrieveStartStreet() { return startStreetID; }
	int retrieveEndStreet() { return endStreetID; }
	int retrieveStartPointID() { return startPointID; }
	int retrieveEndPointID() { return endPointID; }
	dPair retrieveStartPoint() { return startPoint; }
	dPair retrieveStartPointPivot() { return startPointPivot; }
	dPair retrieveEndPoint() { return endPoint; }
	dPair retrieveEndPointPivot() { return endPointPivot; }
};



