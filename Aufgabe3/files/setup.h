#ifndef SETUP_H
#define SETUP_H

#pragma once

#include <string>
#include <iostream>

using namespace std;

template<class T> //Operatoren fuer multiset von Klassen
class Weight
{
public:
	//fuer statische Klassen
	bool operator()(const T &a, const T &b) const
	{
		return a.getWeight() < b.getWeight();
	}

	//fuer Zeiger von Klassen
	bool operator()(const T *a, const T *b) const
	{
		return a->getWeight() < b->getWeight();
	}
};

class Node 			//Knoten
{
	int	id;			//ID von einem Knoten
	double w; 		//Gewicht an einem Knoten

public:

	int getID() const { return id; }		//liefert id
	void setID(int ID) { id = ID; }			//setzt id
	double getWeight() const { return w; }	//liefert Gewicht
	void setWeight(double val) { w = val; } //setzt Gewicht
};


class Path 	//Pfad
{
protected:

	int len;	//Laenge des Pfades
	double w;	//Gewicht des Pfades
	vector<Node*> nodes;	//Pfad als Adjazenzliste

public:
	Path(const vector<Node*> &nodeList, double weight): w(weight)
	{
		//ein Pfad wird "eingelesen"
		nodes.assign(nodeList.begin(), nodeList.end());
		len = nodes.size();
	}
	~Path(void){}

	double getWeight() const { return w; } 	//liefert Gewicht
	int getLength() { return len; }			//liefert Laenge
	Node* getNode(int i) { return nodes[i]; } //liefert das Element i in der Liste

	//erstellt einen Teilpfad vom Ursprung des Graphen bis zum endNode 
	bool subPath(std::vector<Node*> &sPath, Node* endNode) 
	{
		for (vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			if (*it != endNode)
				sPath.push_back(*it);
			else
				return true;
		}

		return false;		//Wenn ein Teilpfad nicht exisitiert/nicht erstellt werden kann
	}

	pair<double, vector<int>> retrievePath() const //fuer die Ausgabe eines gefundenen Pfades
	{
		vector<int> v;
		//double sumDistance = 0;
		for(vector<Node*>::const_iterator it=nodes.begin(); it!=nodes.end(); ++it)
		{	
			//sumDistance += (*it)->getWeight();
			v.push_back((*it)->getID());
		}

		//in Form: max. Anzahl an Abbiegungen, [Pfad]
		return make_pair(w, v); 
	}
};

#endif