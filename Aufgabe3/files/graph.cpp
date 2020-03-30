#include <bits/stdc++.h>
#include "setup.h"
#include "graph.h"

//Die Funktion misst den euklidischen Abstand zwei Punkte 
double Graph::measureDistance(dPair x, dPair y)
{
	int a = abs(x.first - y.first);
	int b = abs(x.second - y.second);

	return sqrt(a*a + b*b);
}

//Die Funktion prueft, ob die drei Punkte auf einer Geraden
//liegen; 0 -- ja, 1 -- nein
//Anhand deren wird bestimmt, ob man abbiegt oder nicht
//Die Formel basiert auf: 
//https://www.geeksforgeeks.org/program-check-three-points-collinear/
bool Graph::checkTurn(dPair a, dPair b, dPair c)
{
	int d = a.first * (b.second - c.second) +  
	        b.first * (c.second - a.second) +  
	        c.first * (a.second - b.second); 
	  
	    if (d == 0) 
	        return 0;
	    else
	        return 1;
}

//Die Funktion wandelt die Eingabe des jeweiligen Punktes um
//und gibt die Punkte in ueblicher Form aus
dPair Graph::convertInput(string data)
{
	string data2;
	int i = 1;
	while (data[i] != ',')
	{
		data2.push_back(data[i]);
		i++;
	}
	double coor1 = stod(data2);
	i++;
	data2 = "";
	while (data[i] != ')')
	{
		data2.push_back(data[i]);
		i++;
	}
	double coor2 = stod(data2);
	return make_pair(coor1, coor2);
}

//Hier werden die beiden in der Beschreibung erwaehnten
//Pivotpunkte erstellt (gilt nur fuer den Graphen Typ 2)
void Graph::createPivots()
{
	startPointPivot = mp(-1, 0);
	endPointPivot = mp(-2, 0);
	points.insert(startPointPivot);
	points.insert(endPointPivot);
	streetsList.insert(mp(startPointPivot, startPoint));
	streetsList.insert(mp(endPoint, endPointPivot));
}

//Vorbereitung der Punkte und Strassen fuer den Bau des Graphen
//(gilt nur fuer den Graphen Typ 1)
void Graph::assignIndicesOriginal()
{
	pointsNum = points.size();

	//jeder Punkt bekommt einen ID und
	//alle Punkte werden in zwei maps geschpeichert,
	//um die Suche nach den Punkten und ihren Indices zu erleichtern
	int id = 0;
	for (auto x: points)
	{
		pointToID.insert(mp(x, id));
		IDToPoint.insert(mp(id, x));
		set<pair<int, double>> s;
		pointIDToNeighbors.insert(mp(id, s));
		id++;
	}

	//die Indices der Anfangs- und Zielpunkte werden bestimmt
	setStartPointID(pointToID.find(startPoint)->second);
	setEndPointID(pointToID.find(endPoint)->second);

	//Anhand der Liste der Strassen werden die benachbarten 
	//Punkte jeweiliges Punktes bestimmt
	for (auto x: streetsList)
	{
		dPair a = mp(x.first.first, x.first.second);
		dPair b = mp(x.second.first, x.second.second);
		int idA = pointToID.find(a)->second;
		int idB = pointToID.find(b)->second;
		double distA_B = measureDistance(a,b); //Messung der Entfernung

		pointIDToNeighbors.find(idA)->second.insert(mp(idB, distA_B));
		pointIDToNeighbors.find(idB)->second.insert(mp(idA, distA_B));
	}

}

//Vorbereitung der Punkte und Strassen fuer den Bau des Graphen
//(gilt nur fuer den Graphen Typ 2)
void Graph::assignIndices()
{
	pointsNum = points.size();

	//jeder Punkt bekommt einen ID und
	//alle Punkte werden in zwei maps geschpeichert,
	//um die Suche nach den Punkten und ihren Indices zu erleichtern
	int id = 0;
	for (auto x: points)
	{
		pointToID.insert(mp(x, id));
		IDToPoint.insert(mp(id, x));
		set<pair<int, double>> s;
		pointIDToNeighbors.insert(mp(id, s));
		id++;
	}

	
	id = 0;
	for (auto x: streetsList)
	{
		//Anhand der Liste der Strassen werden die benachbarten 
		//Punkte jeweiliges Punktes bestimmt
		dPair a = mp(x.first.first, x.first.second);
		dPair b = mp(x.second.first, x.second.second);
		int idA = pointToID.find(a)->second;
		int idB = pointToID.find(b)->second;

		//Die Entfernung zwischen den Punkte bei dem Graph-Typ 2
		//ist uns nicht wichtig, deswegen eine 0
		pointIDToNeighbors.find(idA)->second.insert(mp(idB, 0));
		pointIDToNeighbors.find(idB)->second.insert(mp(idA, 0));

		//jede Strasse bekommt einen ID und ihre Infomationen werden
		//in zwei maps gespeichert
		streetToID.insert(mp(x, id));
		streetToID.insert(mp(mp(x.second, x.first), id));
		streetToID.insert(mp(mp(x.first, x.second), id));
		IDToStreet.insert(mp(id, x));
		IDToStreet.insert(mp(id, mp(x.second, x.first)));

		//Strassen, die mit Pivotpunkten verbunden sind,
		//kriegen das Geiwcht von 0, weil sie kein Teil der
		//Aufgabe sind
		if (x.first == startPointPivot)
			streetIDToWeight.insert(mp(id, 0));
		else if (x.second == endPointPivot)
			streetIDToWeight.insert(mp(id, 0));
		else
			//die Entfernung zwischen den beiden Punkten gilt als das Gewicht
			//an der Strasse (spaeter Knoten)
			streetIDToWeight.insert(mp(id, measureDistance(x.first, x.second)));

		id++;
	}

	//die Indices der Anfangs- und ZielStrassen werden bestimmt
	setStartStreetID(streetToID.find(mp(startPointPivot, startPoint))->second);
	setEndStreetID(streetToID.find(mp(endPoint, endPointPivot))->second);

}

//Hier werden die Abbiegungen zwischen Strassen bestimmt
void Graph::determineTurns()
{
	//jeder Strasse werden alle ihre Nachbarn
	//mit der entsprechenden Abbiegungszahl (0;1) zugeordnet
	int id = 0;
	for (auto x: streetsList)
	{
		set<pair<int, int>> s;
		streetIDToNeighbors.insert(mp(id, s));
		id++;
	}

	for (auto x: streetsList)
	{
		//jede Strasse besteht aus zwei Punkten -- hier: A und B
		int currStreetID = streetToID.find(x)->second; //ID aktueller Strasse
		int idA = pointToID.find(x.first)->second;	//ID des Punktes A
		int idB = pointToID.find(x.second)->second;	//ID des Punktes B

		//Es werden alle Nachbarn von jeweils A un B betrachtet
		set<pair<int, double>> sA = pointIDToNeighbors.find(idA)->second;
		set<pair<int, double>> sB = pointIDToNeighbors.find(idB)->second;

		for (auto y: sA)
		{
			//der aktuelle Nachbar des Punktes A in der aktuellen Strasse x
			dPair matePoint = IDToPoint.find(y.first)->second;

			//der ID der Strasse der Punte matePoint und A
			int currMateStreetID = streetToID.find(mp(x.first, matePoint))->second;

			//Es wird ueberprueft, ob alle Punkte (B, A, matePoint) auf einer Geraden liegen
			bool currTurn = checkTurn(x.second, x.first, matePoint);

			//Wenn die Strasse zu sich selbt zeigen wuerde oder wenn ein Punkt der Strasse
			//ein Pivotpunkt ist, der in die falsche Richtung zeigt (z.B. Start ist am Ende),
			//fuegen wir diese Strasse nicht als einen Nachbarn ein
			if (currStreetID == currMateStreetID || matePoint == startPointPivot 
				|| x.first == endPointPivot || x.second == endPointPivot)
				continue;

			//Wenn ein der Punkte der Strasse ein Pivotpunt ist, so wird die Strasse
			//als Nachbar eingefuegt, aber stets mit Abbiegung 0
			if (x.first == startPointPivot || matePoint == endPointPivot
				|| x.second == startPointPivot)
				streetIDToNeighbors.find(currStreetID)->second.insert(mp(currMateStreetID, 0));

			//Ansonsten wird jedem Paar von Strassen die Abbiegung
			//anhand des Ergebnises von currTurn zugeordnet
			else
			{
				if (x.second.first <= matePoint.first)
					streetIDToNeighbors.find(currStreetID)->second.insert(mp(currMateStreetID, currTurn));
				if (x.second.first >= matePoint.first)
					streetIDToNeighbors.find(currMateStreetID)->second.insert(mp(currStreetID, currTurn));
			}
		}

		for (auto y: sB)
		{
			dPair matePoint = IDToPoint.find(y.first)->second;
			int currMateStreetID = streetToID.find(mp(x.second, matePoint))->second;

			bool currTurn = checkTurn(x.first, x.second, matePoint);

			//Wenn die Strasse zu sich selbt zeigen wuerde oder wenn ein Punkt der Strasse
			//ein Pivotpunkt ist, der in die falsche Richtung zeigt (z.B. Start ist am Ende),
			//fuegen wir diese Strasse nicht als einen Nachbarn ein
			if (currStreetID == currMateStreetID || matePoint == startPointPivot 
				|| x.first == endPointPivot || x.second == endPointPivot)
				continue;

			//Wenn ein der Punkte der Strasse ein Pivotpunt ist, so wird die Strasse
			//als Nachbar eingefuegt, aber stets mit Abbiegung 0
			if (x.first == startPointPivot || matePoint == endPointPivot
				|| x.second == startPointPivot)
				streetIDToNeighbors.find(currStreetID)->second.insert(mp(currMateStreetID, 0));

			//Ansonsten wird jedem Paar von Strassen die Abbiegung
			//anhand des Ergebnises von currTurn zugeordnet
			else
			{
				if (x.first.first <= matePoint.first)
					streetIDToNeighbors.find(currStreetID)->second.insert(mp(currMateStreetID, currTurn));
				if (x.first.first >= matePoint.first)
					streetIDToNeighbors.find(currMateStreetID)->second.insert(mp(currStreetID, currTurn));
			}
		}
	}

}

//Mit Hilfe dieser Funktion werden Informationen ueber des Graphen
//aus der Datei eingelesen, um einen Graphen-Typ 1 zu bauen
void Graph::readFileOriginal(string fileName, set<pair<dPair, dPair>> &streetsList, set<dPair> &points)
{
	int streets; 		//Anzahl der Strassen
	dPair start, end;	//Startpunkt und Zielpunkt

	ifstream file(fileName.c_str());
	if (!file) //falls die Datei nicht gefunden wird
	{
		cerr<<"Die gegebene Datei '"<<fileName.c_str()<<"' wurde nicht gefunden.\n";
		exit(1);
	}
	purge(); //wir stellen sicher, dass alles leer ist

	file>>streets;

	string data;
	file>>data;
	start = convertInput(data);
	file>>data;
	end = convertInput(data);

	setStartPoint(start);
	setEndPoint(end);

	for (int i=0;i<streets;i++)
	{
		file>>data;
		dPair p1 = convertInput(data);
		file>>data;
		dPair p2 = convertInput(data);
		streetsList.insert(mp(p1, p2));

		if (p1 != start || p1 != end)
			points.insert(p1);
		if (p2 != start || p2 != end)
			points.insert(p2);
	}

	file.close();

 	//wir bereiten die Punkte und Strassen auf den Bau des Graphen vor
	assignIndicesOriginal();

	//die Anzahl aller Knoten im Graphen entspricht der Anzahl der Punkte
	nodeNum = pointIDToNeighbors.size();

	int startNode, endNode;
	double edgeWeight;

	map<int, set<pair<int, double>>>::iterator it = pointIDToNeighbors.begin();

	for(; it != pointIDToNeighbors.end(); it++)
	{
		int currID = it->first;
		for (auto x: it->second)
		{
			int currMateID = x.first;

			//Punkte werden als Knoten gespeichert
			Node* n1 = getNode(currID);
			Node* n2 = getNode(currMateID);

			//Gewicht an den Kanten ist die Entfernung 
			//zwischen den beiden Punkten
			edgeToWeight[getEdgeID(n1, n2)] = x.second;

			//jedem Knoten werden die Knoten gespiechert,
			//die zu einem Knoten und aus einem Knoten fuehren
			getNodeSet(n2, nodeToInNodes)->insert(n1); 
			getNodeSet(n1, nodeToOutNodes)->insert(n2); 
		}
	}	

	//falls die Knoten nicht korrekt eingelesen/gespeichert werden
	if(nodeNum != nodes.size())
	{
		cerr<<"Die Anzahl der Knoten stimmt nicht ueberein! ("<<nodes.size()<< " : "<<nodeNum<<")\n";
		exit(1);
	}

	nodeNum = nodes.size();
	edgeNum = edgeToWeight.size();
}

//Mit Hilfe dieser Funktion werden Informationen ueber des Graphen
//aus der Datei eingelesen, um einen Graphen-Typ 2 zu bauen
void Graph::readFile(string fileName, set<pair<dPair, dPair>> &streetsList, set<dPair> &points, dPair interP)
{
	int streets; 		//Anzahl der Strassen
	dPair start, end;	//Startpunkt und Zielpunkt

	ifstream file(fileName.c_str()); 
	if (!file) //falls die Datei nicht gefunden wird
	{
		cerr<<"Die gegebene Datei '"<<fileName.c_str()<<"' wurde nicht gefunden.\n";
		exit(1);
	}
	purge(); //wir stellen sicher, dass alles leer ist

	file>>streets;

	string data;
	file>>data;
	start = convertInput(data);
	file>>data;
	end = convertInput(data);

	setStartPoint(start);
	setEndPoint(end);

	for (int i=0;i<streets;i++)
	{
		file>>data;
		dPair p1 = convertInput(data);
		file>>data;
		dPair p2 = convertInput(data);
		streetsList.insert(mp(p1, p2));

		if (p1 != start || p1 != end)
			points.insert(p1);
		if (p2 != start || p2 != end)
			points.insert(p2);
	}

	file.close();

	//die Pivotpunkte werden gestellt
	createPivots();
	//wir bereiten die Punkte und Strassen auf den Bau des Graphen vor
	assignIndices();
	//wir bestimmen die Abbiegungen zwischen den benachbarten Strassen
	determineTurns();

	//die Anzahl aller Knoten im Graphen entspricht der Anzahl der Strassen
	nodeNum = streetIDToNeighbors.size();

	int startNode, endNode;
	double edgeWeight;

	map<int, set<pair<int, int>>>::iterator it = streetIDToNeighbors.begin();

	for(; it != streetIDToNeighbors.end(); it++)
	{
		int currID = it->first;
		for (auto x: it->second)
		{
			int currMateID = x.first;

			//Strassen werden als Knoten gespeichert
			Node* n1 = getNode(currID);
			Node* n2 = getNode(currMateID);

			//Gewicht an den Kanten ist die Abbiegung
			//zwischen den beiden Strassen
			edgeToWeight[getEdgeID(n1, n2)] = x.second;

			//jedem Knoten werden die Knoten gespiechert,
			//die zu einem Knoten und aus einem Knoten fuehren
			getNodeSet(n2, nodeToInNodes)->insert(n1); 
			getNodeSet(n1, nodeToOutNodes)->insert(n2); 
		}
	}	

	//falls die Knoten nicht korrekt eingelesen/gespeichert werden
	if(nodeNum != nodes.size())
	{
		cerr<<"Die Anzahl der Knoten stimmt nicht ueberein! ("<<nodes.size()<< " : "<<nodeNum<<")\n";
		exit(1);
	}

	nodeNum = nodes.size();
	edgeNum = edgeToWeight.size();
}

//Die Funktion loescht alle bisherige Daten in allen Variablen
void Graph::purge()
{
	startPoint = mp(0,0);
	endPoint = mp(0,0);
	startPointPivot = mp(-1,-1);
	endPointPivot = mp(-1,-1);
	startPointID = 0;
	endPointID = 0;
	startPointID = 0;
	endPointID = 0;
	startStreetID = 0;
	endStreetID = 0;
	pointsNum = 0;
	edgeNum = 0;
	nodeNum = 0;

	for(map<Node*, set<Node*>*>::const_iterator it=nodeToInNodes.begin();it!=nodeToInNodes.end(); ++it)
		delete it->second;
	nodeToInNodes.clear();

	for(map<Node*, set<Node*>*>::const_iterator it=nodeToOutNodes.begin();it!=nodeToOutNodes.end(); ++it)
		delete it->second;
	nodeToOutNodes.clear();

	for (vector<Node*>::iterator it=nodes.begin(); it != nodes.end(); ++it)
		delete *it;

	streetsList.clear();
	points.clear();
	edgeToWeight.clear();
	pointToID.clear(); 	
	IDToPoint.clear(); 
	streetToID.clear();
	IDToStreet.clear();
	streetIDToWeight.clear(); 
	pointIDToNeighbors.clear();
	streetIDToNeighbors.clear();
	nodes.clear();
	IDToNode.clear();

	removedNodes.clear();
	removedEdges.clear();
}

//Die Funktion sucht anhand des ID nach dem Knoten im Graphen
//Wenn es keinen noch gibt, wird ein erstellt
Node* Graph::getNode(int nodeID)
{
	//falls der Knoten nicht entfernt wurde
	if (removedNodes.find(nodeID) == removedNodes.end()) 
	{
		Node* n = NULL;
		const map<int, Node*>::iterator it = IDToNode.find(nodeID);
		if (it == IDToNode.end())
		{
			n = new Node(); 	//ein Knoten wird erstellt
			n->setID(nodeID);	//dem neuen Knoten wird der ID zugeordnet
			IDToNode[nodeID] = n;
			nodes.pb(n);
		}
		else
			n = it->second;

		return n;	
	}
	else
		return NULL;
}

//Die Funktion gibt die Menge der Nachbarn eines Knoten n
//(entweder aus der map nodeToInNodes oder nodeToOutNodes) aus
//Wenn sie noch nicht existiert, wird sie erstellt
set<Node*>* Graph::getNodeSet(Node* n, map<Node*, set<Node*>*> &nodeToSet)
{
	map<Node*, set<Node*>*>::iterator it = nodeToSet.find(n);
	if (it == nodeToSet.end())
	{
		set<Node*>* s = new set<Node*>(); //erstellt eine neue Menge
		pair<map<Node*, set<Node*>*>::iterator, bool> it2 = nodeToSet.insert(mp(n, s));
		it = it2.first;
	}
	return it->second;
}

//Diese Fuktion wird nur fuer die Kanten verwendet, die
//nicht entfernt wurden und deren Knoten nicht entfernt wurden
double Graph::getEdgeWeight(const Node* n1, const Node* n2)
{
	int n1ID = n1->getID();
	int n2ID = n2->getID();

	//falls ein der Knoten oder die ganze Kanten entfernt wurden,
	//wenn ja, dann wuerden wir die naechste Funktion benutzen
	if (removedNodes.find(n1ID) != removedNodes.end()
		|| removedNodes.find(n2ID) != removedNodes.end()
		|| removedEdges.find(mp(n1ID, n2ID)) != removedEdges.end())
	{
		return DBL_MAX; //um einen Semention Fault zu verhindern
	}
	else
	{
		map<int, double>::const_iterator it = edgeToWeight.find(getEdgeID(n1, n2));

		//Pruefe, ob die Kante gefunden werden kann
		if (it != edgeToWeight.end())
			return it->second;
		else
			return DBL_MAX; //um einen Semention Fault zu verhindern
	}
}

//Diese Fuktion wird nur fuer die Kanten verwendet, die
//entfernt wurden oder deren Knoten entfernt wurden
double Graph::getRemovedEdgeWeight(const Node* n1, const Node* n2)
{
	map<int, double>::const_iterator it =  edgeToWeight.find(getEdgeID(n1, n2));

	//Pruefe, ob die Kante gefunden werden kann
	if (it != edgeToWeight.end())
		return it->second;
	else
		return DBL_MAX; //um einen Semention Fault zu verhindern
}

//Die Funktion gibt alle benachbarten Knoten des Knoten n,
//die aus n fuehren, als nodeSet aus
void Graph::getOutNeighbors(Node* n, set<Node*> &nodeSet)
{
	int nID = n->getID();

	//falls der Knoten nicht entfernt wurde
	if (removedNodes.find(nID) == removedNodes.end()) 
	{
		//Die Menge wird einfach aus nodeToInNodes kopiert
		set<Node*>* s = getNodeSet(n, nodeToOutNodes);
		set<Node*>::const_iterator it=(*s).begin();
		for(; it != (*s).end(); ++it)
		{
			int currNode = (*it)->getID();

			//Wenn der aktuelle Knoten/eine mit dem Knoten
			//verbundene Kante entfent wurde
			if (removedNodes.find(currNode) != removedNodes.end()
				|| removedEdges.find(mp(nID, currNode)) != removedEdges.end())
				continue;

			nodeSet.insert(*it);
		}
	}
}

//Die Funktion gibt alle benachbarten Knoten des Knoten n,
//die zu n fuehren, als nodeSet aus
void Graph::getInNeighbors(Node* n, set<Node*> &nodeSet)
{
	//falls der Knoten nicht entfernt wurde
	if (removedNodes.find(n->getID()) == removedNodes.end())
	{
		//Die Menge wird einfach aus nodeToInNodes kopiert
		set<Node*> *s = getNodeSet(n, nodeToInNodes);
		set<Node*>::const_iterator it=(*s).begin();
		for(; it != (*s).end(); ++it)
		{
			int currNode = (*it)->getID();

			//Wenn der aktuelle Knoten/eine mit dem Knoten
			//verbundene Kante entfent wurde
			if (removedNodes.find(currNode) != removedNodes.end()
				|| removedEdges.find(mp(currNode, n->getID())) != removedEdges.end())
				continue;
			
			nodeSet.insert(*it);
		}
	}
}

