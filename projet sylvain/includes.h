#ifndef INCLUDES_H
#define INCLUDES_H

enum state_flags
{
	INVALID = 0, COMPLETE_INVALID = 4,
	COMPLETE_VALID = 3,
	VISITED = 2,
	VALID = 1,
};


struct transition
{
	string name; //nom de la transition (insert coin)
	int sourceid, destinationid;
	//vector <modification> changes; //This vector is used to change the state's variables
};

struct state
{
	int id; //id du state
	string name; //nom de l'état (S1)
	vector <int> variables; //AP, propositions atomiques avec la valuation des variables
	vector <transition> transitions; //transitions partant de ce vecteur

};



//Tuple pour mettre a jour la valeur d'une variable avec la transition
//struct modification
//{
//	int variableid;
//	bool value;
//};




#endif