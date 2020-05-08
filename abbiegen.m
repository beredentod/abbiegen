
//ein "naechster" Pfad des Yen-Algorithmus wird 
//hier bestimmt
Path* generatePath()
{
	//An dieser Stelle wurde bereits der kuerzeste Pfad im Graphen gefunden
	//und wurde in die Menge B (Kandidatenliste) eingefuegt

	//der erste Pfad aus der Kandidatenliste wird betrachtet [Z.53]
	Path* currPath = *(B.begin());
	int currPathLen = currPath->getLength();

	//der aktuelle Pfad wird in die Ergebnissenlisten einfeguegt
	A.pb(currPath);
	
	//der spurNode (Abzweigungsknoten) gilt als Knoten, ab dem 
	//wir den Pfad veraendern
	Node* spurNode = pathsToSpurNodes.find(currPath)->second; 

	//der Teilpfad bis zum spurNode
	vector<Node*> rootPath;
	currPath->subPath(rootPath, spurNode);

	//wir beginnen, entsprechende Knoten und Kanten zu entfernen [Z.18-22]
	for (int i=0; i<A.size()-1; ++i)
	{
		//jeder fertige Pfad (ausser dem, der wir gerade einfuegten)
		//aus der Liste A wird untersucht, ob er den Teilpfad beinhaltet
		Path* currAPath = A[i];
		vector<Node*> currSubPathRootPath;
		
		//wir bestimmen den Teilpfad
		//wenn es nicht geht, setzen wir mit dem naechsten fertigen Pfad fort
		if (!currAPath->subPath(currSubPathRootPath, spurNode)) continue;

		//wenn die Laengen der Teilpfade nicht uebereinstimmen, 
		//setzen wir mit dem naechsten fertigen Pfad fort
		if (rootPath.size() != currSubPathRootPath.size()) continue;
		
		//hier wird geprueft, 
		//ob die Pfade dieselbe Knoten an allen Stellen beinhalten
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
		//um die Kante, die ihn und den spurNode verbindet
		//danach zu entfernen
		Node* nextNode = currAPath->getNode(rootPath.size()+1);
		G->removeEdge(mp(spurNode->getID(), nextNode->getID()));
	}

	//wir entfernen Kanten und Knoten im aktuellen Pfad [Z. 24f.]
	for(int i=0; i<currPathLen-1; ++i)
	{
		G->removeNode(currPath->getNode(i)->getID());
		G->removeEdge(mp(currPath->getNode(i)->getID(), currPath->getNode(i+1)->getID()));
	}

	//wir lassen den Dijkstra-Algorithmus uaf
	//dem Graphen mit entfernten Kanten und Knoten laufen
	//Es es entsteht der Baum
	//[Start (s. Ende unten) Z. 28] 
	Dijkstra rDijkstra(G); //der Baum
	rDijkstra.getReversedTraversal(endNode);

	//wir koennen nun die entfernten Kanten und Knoten
	//auf dem aktuellen Pfad zurueckstellen
	bool done = false;
	for(int i = currPathLen-2; i >= 0 && !done; i--)
	{
		//Zurueckstellung jedes Knotens im aktuellen Pfad
		Node* currNode = currPath->getNode(i);
		G->restoreNode(currNode->getID());

		//wir sollen ueberpruefen, ob wir in der naechten Iteration
		//aufhoeren sollen, die Kanten und Knoten zurueckzustellen
		if (currNode->getID() == spurNode->getID())
			done = true;

		//die Gewichte an den Kanten aus der Menge der outNodes des
		//aktuellen Knotens werden aktualisiert
		//und es entsteht ein Teilpfad vom aktuellen Knoten
		//bis zum Knoten der bereits einen Vorgaenger hat [Z.28]
		Path* newPath = rDijkstra.updateOutDistance(currNode);

		//wenn ein solcher Pfad entsteht
		if (newPath != NULL)
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
					totalWeight += 
						G->getRemovedEdgeWeight(currPath->getNode(j), 
							currPath->getNode(j+1));

					spurPath.pb(n);
				}
				else
					break;
			}

			//newPath wird am Ende von spurPath eingefuegt
			for (int j = 0; j < newPath->getLength(); ++j)
				spurPath.pb(newPath->getNode(j));

			//Erstellung eines neuen Kandidaten [Ende (s. Start oben) Z.28]
			totalPath = new Path(spurPath, totalWeight+newPath->getWeight());

			//wir stellen sicher, ob es sich genau so 
			//einen Pfad noch nicht in der Kandidatenliste gibt [Z.33f.]
			if (pathsToSpurNodes.find(totalPath) == pathsToSpurNodes.end())
			{
				B.insert(totalPath);
				pathsToSpurNodes[totalPath] = currNode;
			}
		}

		//Zurueckstellung jeder Kante im aktuellen Pfad [Z.39]
		Node* nextNode = currPath->getNode(i+1);
		G->restoreEdge(mp(currNode->getID(), nextNode->getID()));

		//es kann sein, dass das Gewicht an der Kante sich aenderte,
		//muessen wir es entprechend aktualisieren
		double edgeWeight = G->getEdgeWeight(currNode, nextNode) 
			+ rDijkstra.getStartDistance(nextNode);

		//Wenn das Gewicht sich veraenderte, muessen
		//wir die Entfernung, den Vorgaenger und die 
		//inNodes vom aktuellen Knoten im Baum aktualisieren
		if (rDijkstra.getStartDistance(currNode) > edgeWeight)
		{
			rDijkstra.setStartDistance(currNode, edgeWeight);
			rDijkstra.setPreviousNode(currNode, nextNode);
			rDijkstra.updateInDistance(currNode);
		}
	}

	//wir stellen sicher, dass keine 
	//Kanten und Knoten in den Vektoren
	//uebrig blieben [Z.38f]
	G->purgeRemovedEdges();
	G->purgeRemovedNodes();

	//der erste Pfad in der Kandidatenliste wird entfernt [Z. 56]
	B.erase(B.begin());

	return currPath;
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
 
 	//wir suchen nach der Entfernung vom Ursprung des aktuellen Knotens 
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
 
 		//die aktuelle Entfernung ist um das Gewicht an der Kante
 		//zwischen dem aktuellen Nachbarn und dem akuellen Knoten vergroesst
		currDistance += G->getEdgeWeight(n, *it2);
	
		//wir aktualisieren die Entfernung vom Ursprung von n,
		//falls sie kleiner ist
 		if (it->second > currDistance)
 		{
			startToDistance[n] = currDistance;
			previousNode[n] = it3->first;
 			weight = currDistance;
 		}
 	}

 	//wenn die Entfernung von n aktualisiert wurde,
 	//erstellen wir einen neuen Pfad
	Path* p = NULL;
	if (weight < DBL_MAX) 
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