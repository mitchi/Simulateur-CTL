#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include "includes.h"

extern void readStates(char * filename);
extern void readTransitions(char * filename);

vector<state> fsm;


/*

Fonction de recherche

#1. On cherche et tag tous les �tats qui satisfont les variables de base.

#2. On impl�mente le premier niveau de CTL

#3. 2�me niveau de CTL.


R�sultat = un vecteur d'�tats.




*/

//This utility function takes a vector of results and outputs an array for dynamic programming
//The states that accept are tagged properly according to the enum (1 = accept)
vector<int> make_dp_array_from_results(vector<int> results)
{
	vector<int> memory;
	memory.resize(fsm.size() );

	for (int i=0; i<results.size(); i++)
	{
		int state = results[i];
		memory[i] = VALID;
	}

	return memory;
}

//Retourne un vecteur avec tous les �tats match�s par la formule en logique propositionnelle (simple v�rification de variables, comme dans l'exemple).
vector<int> matchVariables(vector<int> recherche )
{

	vector<int>resultats;
	//resultats.resize(8);
	//for (int i=0; i < resultats.size(); i++) resultats[i]=0; //Initialisation (peut etre non n�cessaire?

	//On parcoure tous les �tats au moins une fois
	for (int i = 0; i < fsm.size(); i++)
	{
		//On check si ya un match avec la formule en logique propositionnelle (juste une comparaison de vecteurs).
		for (int j = 0; j < fsm[i].variables.size() ; j++)
		{
			//Si on cherche cette variable
			if (recherche[j] == 1) {
				//Est-elle non-presente dans cet etat? Si elle n'est pas presente, on change d'etat
				if (fsm[i].variables[j] != 1) {
					goto nonPresente;
				}
			}
		}

		//La variable est presente dans l'etat:
		resultats.push_back(i);

		//Non presente, on continue au prochain �tat
		nonPresente: ;

	}


	return resultats;

}

enum AF_enum
{
	AF_UNKNOWN = 0,
	AF_ACCEPT = 1,
	AF_INVALID = 2,
	AF_VISITED = 3,
};

//Recursive function
bool all_paths_finally_satisfy(int stateId, vector<int> & memory)
{
	
	if (memory[stateId] == AF_ACCEPT)
		return true;

	if (memory[stateId] == AF_INVALID)
		return false;

	if (memory[stateId] == AF_VISITED)
		return false;

	//On marque ce state comme visited, pour pas revenir 2 fois dessus avec la recursivite
	memory[stateId] = AF_VISITED;

	//Pour toutes les transitions de cet etat
	for (int i =0; i < fsm[stateId].transitions.size(); i++)
	{

		int nextState = fsm[stateId].transitions[i].destinationid;
		
		bool answer = all_paths_finally_satisfy(nextState, memory);
		if (answer == false) return false; //on sort de cette branche imm�diatement si le parcours r�cursif trouve une r�ponse fausse

	}

	//Si tous les chemins ont fini par �tre valide, on se retrouve ici
	memory[stateId] = AF_ACCEPT;
	return true;


}



vector<int> AF(vector<int> states)
{

	vector<int> memory;
	vector<int> results;
	memory.resize(fsm.size() );

	//Let's start with the array for the DP recursive function
	//We tag our accepting states in the memory
	for (int i =0; i < states.size() ; i++) 
	{
		int state = states[i];
		memory[state] = AF_ACCEPT;
	}

	for (int i = 0; i < memory.size(); i++)
	{

		//Les states deja calcul�s, add aux resultats
		if (memory[i] == AF_ACCEPT) {
			results.push_back(i);
			continue;
		}

		if (memory[i] == AF_INVALID)
			continue;

		bool answer = all_paths_finally_satisfy(i, memory);

		if (answer == false)
			memory[i] = AF_INVALID;

		else if (answer == true) {
			//Add this state to the results
			results.push_back(i);
		}
	}

	return results;
}


//Cette fonction recursive prend en parametre un etat initial et trouve si les transitions partant de cet �tat
//satisfont toutes (x).

//Peut utiliser le m�me tableau global pour faire la fonction a m�moire ou alors on passe le tableau en param�tre a la fonction
//Comme �a aucun side effects...



bool all_paths_satisfy(int stateId, vector<int> & memory)
{

	memory[stateId] = 2;

	//Pour toutes les transitions de cet �tat
	for (int i=0; i< fsm[stateId].transitions.size(); i++)
	{
		int nextState = fsm[stateId].transitions[i].destinationid;

		//Si le prochain etat est deja calcule valide, on return true
		if (memory[nextState] == COMPLETE_VALID)
		{
			continue;
		}

		//Si le prochain �tat est un etat invalide OU faux d'avance, return false
		if (memory[nextState] == INVALID || memory[nextState] == COMPLETE_INVALID)
		{
			return false;
		}

		//Si le prochain �tat est un �tat d�ja visit�, on continue a la prochaine transition
		if (memory[nextState] == VISITED)
		{
			continue;
		}

		//Si le prochain �tat est valide et non visit�, on fait un appel r�cursif
		if (memory[nextState] == VALID)
		{
			bool answer = all_paths_satisfy(nextState, memory);
			if (answer == true) continue;
			if (answer == false) return false;
		}
	}

	//Si on a parcouru toutes les transitions de l'�tat sans atterir dans un �tat invalide, on peut retourner true
	memory[stateId] = COMPLETE_VALID;
	return true;
}


/*Cette fonction fait le travail "All paths, globally x"
//Prend en param�tre les �tats acceptants X. La fonction retourne en r�sultats les �tats qui valident la propri�t�
//All paths, globally x.*/
vector<int> AG(vector<int> states)
{

	vector<int> results;
	vector<int> memory;
	memory.resize(fsm.size() );

	//On cr�e le vecteur memory
	for (int i = 0; i < states.size(); i++)
	{
		int state = states[i];
		memory[state] = VALID;
	}

	//On it�re a travers tous les �tats qui acceptent x.
	for (int i =0; i<states.size(); i++)
	{

		int currentState = states[i];
		bool answer = all_paths_satisfy(currentState,memory);
		
		if (answer == true) {
			results.push_back(currentState);
			memory[currentState] = COMPLETE_VALID; //3 == Compl�tement calcul�, true
		}

		else {
			memory[currentState] = COMPLETE_INVALID; //4 == Compl�tement calcul�, false
		}

	}

	return results;

}



enum AX_ENUM
{

	AX_INVALID = 0,
	AX_VALID = 1, //VALID means (o) is true in this state
	AX_ACCEPT = 2,// ACCEPT means AX (o) is true in this state
};

//AX recursive function. Temporary name.
bool axhelper(int stateId, vector<int> & memory)
{

	//For all the outgoing paths of this state
	for (int i = 0; i < fsm[stateId].transitions.size(); i++)
	{
		int dest = fsm[stateId].transitions[i].destinationid;

		if (memory[dest] == AX_INVALID) {
			memory[stateId] = AX_INVALID;
			return false;
		}

	}

	memory[stateId] = AX_ACCEPT;
	return true;


}

/*
This function returns the set of states that matches AX ( formula)
Input : a set of accepting states
Output : results

A (o) � All: o has to hold on all paths starting from the current state.
X (o) � Next: o has to hold at the next state (this operator is sometimes noted N instead of X).

*/

//A quicker algorithm to check for this is just to have all the nodes in a few linked lists and check them one by one. 
//When a sibling node is different than the accepting state, we 
vector<int> AX(vector<int> states)
{

	//Let's build the memory structure
	vector<int> memory;
	memory.resize(fsm.size() );

	vector<int> results;

	//We input the accepting states into the memory structure
	for (int i = 0; i < states.size(); i++)
	{
		int state = states[i];
		memory[state] = AX_VALID;
	}

	//For all the states, we find those who match
	for (int i=0; i < fsm.size(); i++)
	{

		bool answer = axhelper(i, memory);
		if (answer == true)
			results.push_back(i);

	}


return results;


}

enum AU_enum
{

	AU_FIRST = 1, //The state satisfies the first condition
	AU_SECOND = 2, //the state satisfies the second condition
	AU_ACCEPT = 3, //Not used since using AU_SECOND works as well
};



bool AU_helper(int stateId, vector<int > & memory)
{

	//Condition de sortie : on a atteint un �tat qui satisfait la seconde condition ou un etat qui accept deja
	if (memory[stateId] > AU_FIRST )
		return true;

	//Le reste des cas
	if (memory[stateId] != AU_FIRST)
		return false;

	
	//Pour tous les transitions de cet �tat. Chaque transition doit retourner true pour que l'�tat soit valid�
	for (int i = 0; i < fsm[stateId].transitions.size(); i++)
	{
		int destination = fsm[stateId].transitions[i].destinationid;
		bool answer = AU_helper(destination, memory);
		if (answer == false) return false;
	}

	//Use the memory to save this calculation
	memory[stateId] = AU_ACCEPT;
	return true;

}


/*
Input : set of states for the first condition, set of states for the second condition
output : the states that satisfy these constraints
*/

vector<int> AU(vector<int> first, vector<int> second)
{

	vector<int> results;
	vector<int> memory;
	memory.resize(fsm.size());

	//Init the memory data structure
	for (int i = 0; i < first.size(); i++)
	{
		int state = first[i];
		memory[state] = AU_FIRST;
	}

	for (int i = 0; i < second.size(); i++)
	{
		int state = second[i];
		memory[state] = AU_SECOND;
	}

	//The memory is now filled with states that satisfy either first or second condition
	//We can now iterate through all the states that satisfy the first condition and decide 
	//whether or not they hold until the second condition

	for (int i = 0; i < first.size(); i++)
	{
		int state = first[i];

		//These states have already been calculated with the recursivity
		if (memory[state] == AU_ACCEPT) {
			results.push_back(state);
			continue;
		}

		bool answer = AU_helper(state, memory);
		if (answer == true)
			results.push_back(state);
	}



	return results;

}

int main(void)
{

	//Done

	//AX AG AF AU AW
	//EX EG EF EU EW

	//TODO
	//AW EU EW EX (just a couple more!!)

	
	readStates("states.txt");

	readTransitions("transitions.txt");
 
	//On veut v�rifier la propri�t� AF AG x sur la FSM
	
	//#1 Matcher les �tats en fonction de la valuation des variables. Ici on cherche x.
	//Si n �tats et m variables, prend un temps O(n*m)
	vector<int> recherche;
	recherche.resize(3);

	recherche[0] = 1;
	recherche[1] = 0;
	recherche[2] = 0;

	vector<int> res = matchVariables(recherche);

	vector<int> result = AG(res);

	vector<int> result2 = AF(result);

	//Pour l'instant les r�sultats sont bons, encore quelques incertitudes par rapport a AF


	//Calculer AX (y)
	vector<int> input;
	input.push_back(0);
	input.push_back(4);input.push_back(2);input.push_back(1);

	vector<int> resax = AX(input);

	//Testons AU
	vector<int> firststates;
	vector<int> secondstates;

	firststates.push_back(0);firststates.push_back(2);firststates.push_back(4);firststates.push_back(1);

	//cas plus dur
	firststates.push_back(7);

	secondstates.push_back(5);

	vector<int> resau = AU(firststates, secondstates);


	return 0;

}


