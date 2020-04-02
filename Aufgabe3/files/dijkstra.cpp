#include <bits/stdc++.h>
#include "setup.h"
#include "graph.h"
#include "dijkstra.h"

//die Funktion loescht alle bisherige Daten in allen Variablen
void Dijkstra::purge()
{
	visNodes.clear();
	previousNode.clear();
	startToDistance.clear();
	pQ.clear();
}

//hier wird die Entfernung zwischen start und end bestimmt
//und dazu die Enfernungen zwischen allen 
//Knoten auf dem Weg
void Dijkstra::determineConnections(Node* start, Node* end, bool direction)
{
	//wir stellen sicher, dass alles geloechst ist
	purge();

	//In Bezug auf die Richtung des Graphen
	//werden die Start- und Zielknoten
	//entsprechend eingestellt
	Node* startNode = direction ? start : end;
	Node* endNode = direction ? end : start;

	//Die Entfernung zum Startknoten ist natuerlich 0
	startToDistance[startNode] = 0;
	startNode->setWeight(0);

	//der erste Knoten wird zur Vorrangwarteschlange eingefuegt
	pQ.insert(startNode);

	while (!pQ.empty())
	{
		//das erste Element aus der Vorrangwarteschlange
		//ist der aktuelle Knoten
		Node* currNode = *(pQ.begin());

		//wir entfernen den aktuellen Knoten
		pQ.erase(pQ.begin());
		
		//wenn wirden Zielknoten erreicht haben
		if (currNode == endNode) break;

		//kennzeichne den aktuellen Knoten als besucht
		visNodes.insert(currNode->getID());

		//In Bezug auf die Richtung des Graphen
		//wird neighborNodes entsprechend
		//die Menge outNodes oder inNodes zugeordnet
		set<Node*>* neighborNodes = new set<Node*>();	
		if (direction)
			G->getOutNeighbors(currNode, *neighborNodes);
		else
			G->getInNeighbors(currNode, *neighborNodes);

		//wir iterieren durch die Menge von den Nachbarn des aktuellen Knotens
		for (set<Node*>::const_iterator it = neighborNodes->begin(); it != neighborNodes->end(); ++it)
		{
			//wenn der Knoten bereits besucht wurde
			if (visNodes.find((*it)->getID()) != visNodes.end())
				continue;

			//wir suchen nach dem aktuellen Knoten und ordnern distance sein Gewicht zu
			//wenn er nicht gefunden wird, ordnem wir distance den maximalen Wert von double
			map<Node*, double>::const_iterator it2 = startToDistance.find(currNode);
			double distance =  it2 != startToDistance.end() ? it2->second : DBL_MAX;

			//In Bezug auf die Richtung wird distance um
			//das Gewicht an der entsprechenden Kante erweitert
			distance += direction ? G->getEdgeWeight(currNode, *it) : G->getEdgeWeight(*it, currNode);

			//wir suchen nach dem aktuellen Nachbarn in der map Knoten -> Entfernung
			it2 = startToDistance.find(*it);

			//wenn der aktuelle Nachbar nicht gefunden wird,
			//muessen wir die maps aktualisieren
			if (it2 == startToDistance.end() || it2->second > distance)
			{
				//Wir fuegen den Nahchbarn mit seinem Geiwicht (distance)
				//in die beiden maps ein
				startToDistance[*it] = distance;
				previousNode[*it] = currNode;
				
				//wir stellen die Entfernung fuer den aktuellen Nachbarn ein
				(*it)->setWeight(distance);

				//wir suchen nach dem aktuellen Nachbarn in der V-Schlange
				multiset<Node*, Weight<Node>>::const_iterator it3 = pQ.begin();
				for (; it3 != pQ.end(); ++it3)
					if ((*it3)->getID() == (*it)->getID()) break;

				//wenn der aktuelle Nachbar
				//in der V-Schlange ist, entfernen wird ihn
				if (it3 != pQ.end())
					pQ.erase(it3);

				//wir fuegen den Nachbarn in die V-Schlange ein
				pQ.insert(*it);
			}
		}
	}
}

//die Funktion findet den kuerzesten Weg von start zu end
Path* Dijkstra::calculateShortestPath(Node* start, Node* end)
{
	//die Entfernungen zwischen allen Knoten auf dem Weg werden bestimmt
	determineConnections(start, end, true);

	//Vektor fuer den neuen Weg
	vector<Node*> v;

	//wir suchen nach dem Zielknoten und ordnern weight sein Gewicht zu
	map<Node*, double>::const_iterator it = startToDistance.find(end);
	double weight = it != startToDistance.end() ? it->second : DBL_MAX;

	if (weight < DBL_MAX) //wenn der Zielknoten gefunden wird/existiert
	{
		Node* currNode = end;
		do 
		{
			//der aktuelle Knoten wird am Anfang des Pfades eingefuegt
			v.insert(v.begin(), currNode);

			//wir suchen nach dem Vorgaenger des aktuellen Knoten
			map<Node*, Node*>::const_iterator it2 = previousNode.find(currNode);

			//wenn der Knoten nicht gefunden wird
			if (it2 == previousNode.end()) break;

			//der neue aktuelle Knoten ist der gefundene Vorgaenger
			currNode = it2->second;

		//wir setzen fort, bis der aktuelle Knoten der Startknoten ist
		} while (currNode != start); 

		//wir fuegen den Starknoten am Anfang des Pfades ein
		v.insert(v.begin(), start);
	}

	//ein gefundener Pfad wird mit dem entsprechenden Gewicht ausgegeben
	return new Path(v, weight);
}

//Die Funktion aktualisiert die Gewichte (Entfernungen)
//der Knoten, die aus dem Knoten n fuehren
Path* Dijkstra::updateOutDistance(Node* n)
{
	//wir stellen das Geiwcht als den maximalen Wert von double ein
	//Indikator dafuer, dass mindestens eine Entfernung veraendert wurde
	double weight = DBL_MAX;

 	//wir rufen die outNodes des aktuellen Knotens ab 
	set<Node*>* outNeighbors = new set<Node*>();
	G->getOutNeighbors(n, *outNeighbors);
 
 	//wir suchen nach der Entfernung des aktuellen Knotens 
	map<Node*, double>::iterator it = startToDistance.find(n);
	//wenn es ihn nicht gibt, wird er mit dem maximalen Wert
	//von double als Entfernung eingefuegt
	if (it == startToDistance.end()) 
		it = (startToDistance.insert(mp(n, DBL_MAX))).first;

 	//wir iterieren durch alle Nachbarn, die aus dem aktuellen Knoten fuehren
 	for (set<Node*>::const_iterator it2 = outNeighbors->begin(); it2 != outNeighbors->end(); ++it2)
 	{
 		//wir stellen die Entfernung des Nachbarns ein
		//wenn es ihn nicht gibt, wird der maximale Wert von double zugeordnet 
		map<Node*, double>::const_iterator it3 = startToDistance.find(*it2);
		double currDistance = it3 == startToDistance.end() ? DBL_MAX : it3->second;
 
 		//die aktuelle Entfernung ist das Gewicht an der Kante
 		//zwischen dem aktuellen Nachbarn und dem akuellen Knoten
		currDistance += G->getEdgeWeight(n, *it2);
	
		//wir aktualisieren die Entfernung vom Ursprung von n,
		//falls sie kleiner ist (falls sie nicht DBL_MAX ist)
 		if (it->second > currDistance)
 		{
			startToDistance[n] = currDistance;
			previousNode[n] = it3->first;
 			weight = currDistance;
 		}
 	}

 	//wenn die Entfernung aktualisiert wurde,
 	//erstellen wir einen neuen Pfad
	Path* p = NULL;
	if(weight < DBL_MAX) 
 	{
		vector<Node*> v;

		//wir fangen mit dem aktuellen Knoten an
		v.pb(n);

		map<Node*, Node*>::const_iterator it = previousNode.find(n);
		
		//alle Knoten, die einen Vorgaenger haben, werden 
		//in den neuen Pfad eingefuegt
		while(it != previousNode.end())
		{
			v.pb(it->second);
			it = previousNode.find(it->second);
		}

		//weight gilt hier als die Gesamtentfernung des Pfades
		p = new Path(v, weight);
 	}
 	return p;
}

//Die Funktion aktualisiert die Gewichte (Entfernungen)
//der Knoten, die zum Knoten n fuehren
void Dijkstra::updateInDistance(Node* n)
{
	vector<Node*> v;
	v.pb(n);

	while(!v.empty())
 	{
		Node* currNode = *(v.begin());
		v.erase(v.begin());

		//die aktuelle Entfernung ist die Entfernung des aktuellen Knotens
 		double currDistance = startToDistance[currNode];

 		//wir rufen die inNodes des aktuellen Knotens ab 
		set<Node*> inNeighbors;
		G->getInNeighbors(currNode, inNeighbors);

		//wir iterieren durch alle Nachbarn, die zum aktuellen Knoten fuehren
		for (set<Node*>::const_iterator it = inNeighbors.begin(); it != inNeighbors.end(); ++it)
		{
			//wir stellen die Entfernung des Nachbarns ein
			//wenn es ihn nicht gibt, wird der maximale Wert von double zugeordnet 
			map<Node*, double>::const_iterator it1 = startToDistance.find(*it);
			double neighborDistance = startToDistance.end() == it1 ? DBL_MAX : it1->second;

			//wir aktualisieren die Gesamtentfernung, falls sie kleiner ist
			//als die Entfernung des Nachbarns
			double updatedDistance = currDistance + G->getEdgeWeight(*it, currNode);
			if (neighborDistance > updatedDistance)
			{
				startToDistance[*it] = updatedDistance;
				previousNode[*it] = currNode;

				//wir fuegen den Nachbarn in den Vektor ein,
				//um auf ihn danach Veraenderungen durchzufuehren
				v.pb(*it);
			}
		}
	}
}
