#pragma once

#define mp make_pair
#define pb push_back

using namespace std;

//***************************************************
//* Die folgende Implementierung basiert 		    *
//* grundsaetzlich auf das Pseudocode auf dem	    *
//* Wikipedia-Artikel ueber den Yen-Algorithmus	    *
//*												    *
//* https://en.wikipedia.org/wiki/Yen%27s_algorithm *
//***************************************************

class Yen
{
	Graph* G;	//der Graph, auf dem gearbeitet wird

	vector<Path*> A;				 //Vektor mit Ergebnissen
	multiset<Path*, Weight<Path>> B; //Menge mit Kanditaten

	//Map, die einem Pfad seinen Anfangsknoten zuordnet
	map<Path*, Node*> pathsToSpurNodes;		
	
	Node* startNode;	//Startknoten
	Node* endNode;		//Zielknoten

private:

	void preprocess()
	{
		if (startNode != NULL && endNode != NULL)
		{
			//zuerst finden wir den kuerzesten Weg von allen
			Dijkstra dijkstra(G);
			Path* shortestPath = dijkstra.calculateShortestPath(startNode, endNode);
			if (shortestPath != NULL && shortestPath->getLength() > 1)
			{
				//wir fuegen den kuerzesten Weg als ersten Kandidaten ein
				B.insert(shortestPath);

				//als Abzweigunsknoten gilt natuerlich der erste Knoten
				pathsToSpurNodes[shortestPath] = startNode;
			}
		}
	}

public:

	Yen(const Graph &graph, Node* start, Node* end): startNode(start), endNode(end)
	{
		G = new Graph(graph);
		preprocess(); //Vorbereitung des Graphen auf den Yen-Algorithmus 
	}
	~Yen(void){}

	//ein "naechster" Pfad des Yen-Algorithmus wird 
	//hier bestimmt
	Path* generatePath()
	{
		//der erste Pfad aus der Kandidatenliste wird betrachtet
		Path* currPath = *(B.begin());
		int currPathLen = currPath->getLength();
		
		//der erste Pfad in der Kandidatenliste wird entfernt
		B.erase(B.begin());

		//der aktuelle Pfad wird in die Ergebnissenlisten einfeguegt
		A.pb(currPath);
		
		//der spurNode (Abzweigungsknoten) gilt als Knoten, ab dem wir 
		//wir den Pfad veraendern
		Node* spurNode = pathsToSpurNodes.find(currPath)->second; 

		//der Teilpfad bis zum spurNode
		vector<Node*> rootPath;
		currPath->subPath(rootPath, spurNode);

		//wir beginnen, entsprechende Knoten und Kanten zu entfernen
		for (int i=0; i<A.size()-1; ++i)
		{
			//jeder fertige Pfad (ausser dem, der wir gerade einfuegten)
			//aus der Liste A wird untesucht, ob er den Teilpfad beinhaltet
			Path* currAPath = A[i];
			vector<Node*> currSubPathRootPath;
			
			//wir bestimmen den Teilpfad
			//wenn es nicht geht, setzen wir mit dem naechsten fertigen Pfad fort
			if (!currAPath->subPath(currSubPathRootPath, spurNode)) continue;

			//wenn die Laengen der Teilpfade nicht uebereinstimmen, 
			//setzen wir mit dem naechsten fertigen Pfad fort
			if (rootPath.size() != currSubPathRootPath.size()) continue;
			
			//hier wird geprueft, 
			//ob die Pfade dieselben Knoten an allen Stellen beinhalten
			bool same = true;
			for (int i=0; i<rootPath.size(); ++i)
			{
				if (rootPath[i] != currSubPathRootPath[i])
				{
					same = false;
					break;
				}
			}
			if (!same) continue;

			//wir suchen nach dem folgenden Knoten im aktuellen Pfad,
			//um die Kante, die ihn und den Abzweigunsknoten verbindet
			//danach zu entfernen
			Node* nextNode = currAPath->getNode(rootPath.size()+1);
			G->removeEdge(mp(spurNode->getID(), nextNode->getID()));
		}

		//wir entfernen Kanten und Knoten im aktuellen Pfad
		for(int i=0; i<currPathLen-1; ++i)
		{
			G->removeNode(currPath->getNode(i)->getID());
			G->removeEdge(mp(currPath->getNode(i)->getID(), currPath->getNode(i+1)->getID()));
		}

		//wir aktualisieren stetig
		//die Punkte vom Zeilknoten zum spurNode 
		//[Start (s. Ende unten) Z. 28] 
		Dijkstra rDijkstra(G);
		rDijkstra.getReversedTraversal(endNode);

		//wir koennen jetzt die entfernten Kanten und Knoten
		//auf dem aktuellen Pfad zurueckstellen
		bool done = false;
		for(int i = currPathLen-2; i >= 0 && !done; i--)
		{
			//Zurueckstellung jedes Knoten im aktuellen Pfad
			Node* currNode = currPath->getNode(i);
			G->restoreNode(currNode->getID());

			//wir sollen ueberpruefen, ob wir in der naechten Iteration
			//aufhoeren sollen, die Kanten und Knoten zurueckzustellen
			if (currNode->getID() == spurNode->getID())
				done = true;

			//die Gewichte an den Kanten aus der Menge der inNodes des
			//aktuellen Knotens werden aktualisiert
			//und es entsteht ein Teilpfad vom aktuellen Knoten
			//bis zum Knoten der bereits einen Vorgaenger hat
			Path* totalPath = rDijkstra.updateOutDistance(currNode);

			if (totalPath != NULL)
			{
				//die gesamte Anzahl an Abbiegungen im aktuellen Pfad
				double totalWeight = 0;
				//die inNodes werden um die entfernten Kanten und Knoten aktualisiert
				rDijkstra.updateInDistance(currNode);

				//der aktuelle Pfad (currPfad) wird zu spurPath kopiert
				//bis auf den aktuellen Knoten, den wir entfernten
				vector<Node*> spurPath;
				for (int j=0; j<currPathLen; ++j)
				{
					Node* n = currPath->getNode(j);
					if (n->getID() != currNode->getID())
					{
						totalWeight += G->getRemovedEdgeWeight(currPath->getNode(j), currPath->getNode(j+1));
						spurPath.pb(n);
					}
					else
						break;
				}

				//totalPath wird am Ende von prePath eingefuegt
				for (int j = 0; j < totalPath->getLength(); ++j)
					spurPath.pb(totalPath->getNode(j));

				//Erstellung eines neuen Kandidaten
				totalPath = new Path(spurPath, totalWeight+totalPath->getWeight());

				//wir stellen sicher, ob es sich genau so 
				//einen Pfad noch nicht in der Kandidatenliste gibt  
				if (pathsToSpurNodes.find(totalPath) == pathsToSpurNodes.end())
				{
					B.insert(totalPath);
					pathsToSpurNodes[totalPath] = currNode;
				}
			}

			//Zurueckstellung jeder Kante im aktuellen Pfad
			Node* nextNode = currPath->getNode(i+1);
			G->restoreEdge(mp(currNode->getID(), nextNode->getID()));

			//es kann sein, dass das Gewicht an der Kante sich aenderte,
			//muessen wir es entprechend aktulisieren
			double edgeWeight = G->getEdgeWeight(currNode, nextNode) + rDijkstra.getStartDistance(nextNode);

			//Wenn das Gewicht sich veraenderte, muessen
			//wir die Entfernung, den Vorgaenger und die 
			//inNodes vom aktuellen Knoten aktualisieren
			if (rDijkstra.getStartDistance(currNode) > edgeWeight)
			{
				rDijkstra.setStartDistance(currNode, edgeWeight);
				rDijkstra.setPreviousNode(currNode, nextNode);
				rDijkstra.updateInDistance(currNode);
			}
		}

		//wir stellen sicher, dass keine 
		//Kanten und Knoten in den Vektoren
		//uebrig blieben
		G->purgeRemovedEdges();
		G->purgeRemovedNodes();

		return currPath;
	}
};
