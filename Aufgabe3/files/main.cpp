#include <bits/stdc++.h>
#include "setup.h"
#include "graph.h"
#include "dijkstra.h"
#include "yen.h"
#include "simple_svg.hpp"

using namespace std;
using namespace svg;

//Angaben zur Hersttelung SVG-Grafiken
#define WIN_WIDTH 2000
#define WIN_HEIGHT 1000
#define ZOOM 120
#define MOVE 50
#define CIRC_DIAM 20


//Erstellung der SVG-Datei
void MakeSVG (string $OUTPUT_FILE, dPair s, dPair e, dPair sp, dPair ep, set<dPair> pts, 
				set<pair<dPair, dPair>> strL, set<pair<dPair, dPair>> path)
{
	//Pivotkanten und -knoten werden entfernt
	pts.erase(sp);
	pts.erase(ep);
	strL.erase(mp(sp, s));
	strL.erase(mp(e, ep));
	path.erase(mp(sp, s));
	path.erase(mp(e, ep));

	//Groesse des Fenster wird bestimmt
	Dimensions dimensions(WIN_WIDTH, WIN_HEIGHT);
    Document doc($OUTPUT_FILE, Layout(dimensions, Layout::BottomLeft));

    //alle Strassen werden eingefuegt
    for (auto x: strL)
    {
    	Polyline polyline (Stroke(3, Color::Black));
    	polyline << Point(MOVE+x.first.first*ZOOM, MOVE+x.first.second*ZOOM) 
    		<< Point(MOVE+x.second.first*ZOOM, MOVE+x.second.second*ZOOM);
    	doc << polyline;
    }

    //der Pfad wird gekennzeichnet
    for (auto x: path)
    {
    	if (x.first.first >= 0 && x.first.second >= 0)
    	{
	    	Polyline polyline (Stroke(3, Color::Red));
	    	polyline << Point(MOVE+x.first.first*ZOOM, MOVE+x.first.second*ZOOM) 
	    		<< Point(MOVE+x.second.first*ZOOM, MOVE+x.second.second*ZOOM);
	    	doc << polyline;
  		}
    }

    //alle Punkte werden eingefuegt
    for (auto x: pts)
    	doc << Circle(Point(MOVE+x.first*ZOOM, MOVE+x.second*ZOOM), CIRC_DIAM, 
    		Fill(Color(0, 0, 0)), Stroke(1, Color(0, 0, 0)));

    //Start- und Zielpunkte werden gekennzeichnet
    doc << Circle(Point(MOVE+(s.first*ZOOM), MOVE+(s.second*ZOOM)), CIRC_DIAM, 
    	Fill(Color(0, 0, 256)), Stroke(1, Color(0, 0, 256)));
    doc << Circle(Point(MOVE+e.first*ZOOM, MOVE+e.second*ZOOM), CIRC_DIAM, 
    	Fill(Color(0, 256, 0)), Stroke(1, Color(0, 256, 0)));

    doc.save();
    cout << "SVG-Datei \"" << $OUTPUT_FILE << "\" wurde hergestellt.\n";
}

int main()
{
	//Nummer des Beispiels
	int num;
	cout << "Geben Sie bitte die Nummer des Beispiels ein: ";
	cin >> num;

	//Ein- und Ausgabe
	string $FILE = "../input/abbiegen"+to_string(num)+".txt";
	string $OUTPUT = "../output/abbiegen"+to_string(num)+".svg";
	string $OUTPUT_LEASTTURNS = "../output/abbiegen"+to_string(num)+"leastTurns.svg";
	string $OUTPUT_SHORTEST = "../output/abbiegen"+to_string(num)+"shortest.svg";

	//der kuerzeste Weg im Graphen (Entfernung)
	double scale;

	//der kuerzeste Weg
	pair<double, vector<int>> shortestPath;
	//der Weg mit der niedrigsten Anzahl an Abbiegungen
	tuple<double, double, vector<int>> bestPath;
	//Punkte der Strassen des besten Wegs
	vector<dPair> bestPoints;

	//eine Menge mit allen generierten Strassen
	set<tuple<double, double, vector<int>>> allPaths;
	//eine Menge mit Punkte, fuer die graphische Ausgabe
	set<pair<dPair, dPair>> vis_path;

	//map: die Laengen der Strassen (aus dem Graphen abgerufen)
	map<int, double> streetWeights;
	//map: die Koordinaten der Punkte der Strassen (aus dem Graphen abgerufen)
	map<int, pair<dPair, dPair>> IDToStreet;
	
	//der erste Graph -- Muster (Graph G)
	Graph G1($FILE, 1);
	Dijkstra dijkstra(&G1);
	//der kuerzeste Weg von allen wird gefunden
	shortestPath = dijkstra.calculateShortestPath(G1.getNode(G1.retrieveStartPointID()), 
		G1.getNode(G1.retrieveEndPointID()))->retrievePath();

	//die kuerzeste Entfernung vom Startknoten bis zum Zielknoten
	scale = shortestPath.first;
	
	//der zweite Graph -- mit Strassen als Knoten (Graph G')
	Graph G($FILE, 2);

	streetWeights = G.retrieveStreetWeights();
	IDToStreet = G.retrieveStreets();
	int edgeNum = G.retrieveEdgeNum();

	//Der Yen-Algorithmus laeuft vom Startknoten bis zum Endknoten
	Yen yen(G, G.getNode(G.retrieveStartStreet()), G.getNode(G.retrieveEndStreet()));

	//Die maximale, prozentuale Verlaengerung des Pfades
	double k;
	cout << "Geben Sie bitte die maximale Verlängerung als einen prozentualen Wert ein (z.B. 20): ";
	cin >> k;
	double maxProcent = 100 + k;

	//Wahl der Genauigkeit
	bool exact;
	string response;
	cout << "Möchten Sie eine approximierte Suche durchführen (j/n): ";
	cin >> response;

	if (response == "j" || response == "J") {exact = 0; cout<<"Approximierte Suche.\n";}
	else if (response == "n" || response == "N") {exact = 1; cout<<"Genaue Suche.\n";}
	else {cout<<"Angabe nicht erkannt. Genaue Suche.\n"; exact = 1;}

	//die maximale Laenge des Pfades wird bestimmt
	double maxDist = (double)(1 + ((double)k/100))*scale;

	int i = 0;
	double currDist;
	double currDistProcent;
	tuple<double, double, vector<int>> currPath;

	//Generierung der Pfade
	generate:
	do 
	{
		pair<double, vector<int>> p;
		map<dPair, int> vis;
		//der naechste beste Pfad wird gefunden 
		p = yen.generatePath()->retrievePath();

		double dist = 0;
		for (auto x: p.second)
		{
			//Pfade, in denen dreimal ein Punkt besucht wird,
			//werden nicht betrachtet
			pair<dPair, dPair> str = IDToStreet.find(x)->second;

			if (vis.find(str.first) == vis.end())
				vis[str.first] = 1;
			else
			{
				if (vis.find(str.first)->second == 2)
					goto generate;
				else 
					vis.find(str.first)->second++;
			}
			if (vis.find(str.second) == vis.end())
				vis[str.second] = 1;
			else
			{
				if (vis.find(str.second)->second == 2)
					goto generate;
				else 
					vis.find(str.second)->second++;
			}

			//die Gewichte an den Knoten (Strassen)
			//werden zugeordnet
			dist += streetWeights.find(x)->second;
		}

		++i;
		currDist = dist;
		currDistProcent = round(currDist/scale*100);

		//ein neur, fertige Pfad wird in der Menge allPaths gespeichert
		currPath = make_tuple(p.first, dist, p.second);
		allPaths.insert(currPath);

	} while (exact && currDist > maxDist || !exact && currDistProcent > maxProcent);

	bestPath = currPath;

	//Der Pfad mit der niedrigsten Anzahl von Abbiegungen
	tuple<double, double, vector<int>> leastTurns = *(allPaths.begin());

	cout << "\nFolgender Pfad wurde gefunden.\nDie Anzahl von Abbiegungen:\t" << get<0>(bestPath) << "\n";

	if (exact)
		cout << "Die ganze Entfernung:\t\t" <<get<1>(bestPath)<< " (" << get<1> (bestPath)/scale*100 << "%)\n";
	else
		cout << "Die ganze Entfernung:\t\t" <<get<1>(bestPath)<< " (" << round(get<1> (bestPath)/scale*100) << "%)\n";

	cout << "Alle Punkte: ";

	//Formatierung der Punkte
	//Strassen besitzen je zwei Punkte
	//Wiederholungen muessen entfernt werden
	dPair prevA = mp(-1,-1), prevB = mp(-1,-1);
	for (auto y: get<2>(bestPath))
	{
		pair<dPair, dPair> p = IDToStreet.find(y)->second;

		if (prevA.second == -1 && prevB.second == -1)
		{
			bestPoints.pb(p.first);
			bestPoints.pb(p.second);
		}
		else
		{
			if (prevA != p.first && prevB != p.first)
				bestPoints.pb(p.first);
			if (prevA != p.second && prevB != p.second)
				bestPoints.pb(p.second);
		}
		prevA = p.first;
		prevB = p.second;
	}
	bestPoints.erase(bestPoints.begin());
	bestPoints.erase(bestPoints.end());

	//Ausgabe aller Punkte der besten Strasse
	//mit der niedrigsten Anzahl an Abbiegungen
	for (auto x: bestPoints)
		cout << "(" << x.first << ", " << x.second << ") ";
	cout << "\n\n";

	//Vorbereitung auf die graphische Ausgabe
	for (auto x: get<2>(bestPath))
		vis_path.insert(IDToStreet.find(x)->second);


	//der kuerzeste Pfad
	tuple<double, double, vector<int>> shortestTurns;

	map<pair<dPair,dPair>, int> streetToID = G.retrieveStreetsToID();
	map<int, dPair> IDToPoint = G1.retrieveIDToPoint();
	map<int, set<pair<int, int>>> streetIDToNeighbors = G.retrieveTurns();

	vector<int> shortestStreets;
	vector<dPair> shortestPoints;
	int shortestTurnsNum = 0;

	//Umwandlung Indizes -> Punkte
	for (auto x: shortestPath.second)
	{
		dPair a = IDToPoint.find(x)->second;
		shortestPoints.pb(a);
	}

	//Umwandlung Punkte -> Strassen
	for (int i=0; i < shortestPoints.size()-1; ++i)
	{
		dPair a = shortestPoints[i];
		dPair b = shortestPoints[i+1];

		int id = streetToID.find(mp(a,b))->second;
		shortestStreets.pb(id);
	}

	//Umwandlung Strassen -> Abbiegungen
	for (int i=0; i < shortestStreets.size()-1; ++i)
	{
		int strA = shortestStreets[i];
		int strB = shortestStreets[i+1];

		bool found = 0;

		for (auto x: streetIDToNeighbors.find(strA)->second)
		{
			if (x.first == strB)
			{
				shortestTurnsNum += x.second;
				found = 1;
				break;
			}
		}
		if (!found)
		{
			for (auto x: streetIDToNeighbors.find(strB)->second)
			{
				if (x.first == strA)
				{
					shortestTurnsNum += x.second;
					break;
				}
			}			
		}
	}

	//der kuerzeste Pfad
	shortestTurns = make_tuple(shortestTurnsNum, scale, shortestStreets);

	cout << "\nZum Vergleich: der Pfad mit der niedrigsten anzahl von Abbiegungen.\n"<<
	"Die Anzahl von Abbiegungen:\t" << get<0>(leastTurns) << "\n";

	if (exact)
		cout << "Die ganze Entfernung:\t\t" <<get<1>(leastTurns)<< " (" << get<1> (leastTurns)/scale*100 << "%)\n\n";
	else
		cout << "Die ganze Entfernung:\t\t" <<get<1>(leastTurns)<< " (" << round(get<1> (leastTurns)/scale*100) << "%)\n\n";

	cout << "\nZum Vergleich: der kürzeste Pfad.\nDie Anzahl von Abbiegungen:\t" << get<0>(shortestTurns) << "\n";
	
	if (exact) 
		cout << "Die ganze Entfernung:\t\t" <<get<1>(shortestTurns)<< " (" << get<1> (shortestTurns)/scale*100 << "%)\n\n\n";

	//eine SVG-Datei mit dem gefunden Pfad wird erstellt
	cout << "Der gefundene Pfad:\n";
	MakeSVG($OUTPUT, G.retrieveStartPoint(), G.retrieveEndPoint(), G.retrieveStartPointPivot(),
		G.retrieveEndPointPivot(), G.retrieveAllPoints(), G.retrieveAllStreets(), vis_path);

	set<pair<dPair, dPair>> vis_path_least_turns;

	for (auto x: get<2>(leastTurns))
		vis_path_least_turns.insert(IDToStreet.find(x)->second);

	//eine SVG-Datei mit dem Pfad mit der niedrigsten Anzahl von Abbieg. wird erstellt
	cout << "\nDer Pfad mit der niedrigsten Anzahl von Abbiegungen:\n";
	MakeSVG($OUTPUT_LEASTTURNS, G.retrieveStartPoint(), G.retrieveEndPoint(), G.retrieveStartPointPivot(),
		G.retrieveEndPointPivot(), G.retrieveAllPoints(), G.retrieveAllStreets(), vis_path_least_turns);

	set<pair<dPair, dPair>> vis_path_shortest_turns;

	for (auto x: get<2>(shortestTurns))
		vis_path_shortest_turns.insert(IDToStreet.find(x)->second);

	//eine SVG-Datei mit dem kuerzesten Pfad wird erstellt
	cout << "\nDer kürzeste Pfad:\n";
	MakeSVG($OUTPUT_SHORTEST, G.retrieveStartPoint(), G.retrieveEndPoint(), G.retrieveStartPointPivot(),
		G.retrieveEndPointPivot(), G.retrieveAllPoints(), G.retrieveAllStreets(), vis_path_shortest_turns);
}
