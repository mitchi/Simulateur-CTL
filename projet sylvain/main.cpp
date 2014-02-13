//EDMOND LA CHANCE UQAC 2014

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

/* NOTES

La plupart des opérateurs de CTL sont des fonctions récursives qui sont dynamisables, c'est a dire qu'on risque 
de repasser sur des solutions déja calculées alors on fait la bonne chose et on les enregistre a mesure.
Il est utile de voir des exemples avec des graphes et non des arbres pour s'en rendre compte.
Les arbres peuvent être parcourus sans répétition contrairement aux graphes

*/



/*
Todo : En ce moment ce programme est limité a une seule FSM car la FSM est une variable globale du module main.obj.
2 options sont possibles si c'est un problème :
1.Copier coller tout le code et faire un objet avec. Si simple!
2.Rajouter un paramètre supplémentaire FSM a toutes les fonctions... Bof
Ou 3. Laisser ça comme ça :)
*/

//Shortcut utile sur VS2008 CTRL+M+O (collapse all)
/*

Fonction de recherche

#1. On cherche et tag tous les états qui satisfont les variables de base.

#2. On implémente le premier niveau de CTL

#3. 2ème niveau de CTL.


Résultat = un vecteur d'états.




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

//Retourne un vecteur avec tous les états matchés par la formule en logique propositionnelle (simple vérification de variables, comme dans l'exemple).
vector<int> matchVariables(vector<int> recherche )
{

	vector<int>resultats;
	//resultats.resize(8);
	//for (int i=0; i < resultats.size(); i++) resultats[i]=0; //Initialisation (peut etre non nécessaire?

	//On parcoure tous les états au moins une fois
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

		//Non presente, on continue au prochain état
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
		if (answer == false) return false; //on sort de cette branche immédiatement si le parcours récursif trouve une réponse fausse

	}

	//Si tous les chemins ont fini par être valide, on se retrouve ici
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

		//Les states deja calculés, add aux resultats
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


//Cette fonction recursive prend en parametre un etat initial et trouve si les transitions partant de cet état
//satisfont toutes (x).

//Peut utiliser le même tableau global pour faire la fonction a mémoire ou alors on passe le tableau en paramètre a la fonction
//Comme ça aucun side effects...



bool all_paths_satisfy(int stateId, vector<int> & memory)
{

	memory[stateId] = 2;

	//Pour toutes les transitions de cet état
	for (int i=0; i< fsm[stateId].transitions.size(); i++)
	{
		int nextState = fsm[stateId].transitions[i].destinationid;

		//Si le prochain etat est deja calcule valide, on return true
		if (memory[nextState] == COMPLETE_VALID)
		{
			continue;
		}

		//Si le prochain état est un etat invalide OU faux d'avance, return false
		if (memory[nextState] == INVALID || memory[nextState] == COMPLETE_INVALID)
		{
			return false;
		}

		//Si le prochain état est un état déja visité, on continue a la prochaine transition
		if (memory[nextState] == VISITED)
		{
			continue;
		}

		//Si le prochain état est valide et non visité, on fait un appel récursif
		if (memory[nextState] == VALID)
		{
			bool answer = all_paths_satisfy(nextState, memory);
			if (answer == true) continue;
			if (answer == false) return false;
		}
	}

	//Si on a parcouru toutes les transitions de l'état sans atterir dans un état invalide, on peut retourner true
	memory[stateId] = COMPLETE_VALID;
	return true;
}


/*Cette fonction fait le travail "All paths, globally x"
//Prend en paramètre les états acceptants X. La fonction retourne en résultats les états qui valident la propriété
//All paths, globally x.*/
vector<int> AG(vector<int> states)
{

	vector<int> results;
	vector<int> memory;
	memory.resize(fsm.size() );

	//On crée le vecteur memory
	for (int i = 0; i < states.size(); i++)
	{
		int state = states[i];
		memory[state] = VALID;
	}

	//On itère a travers tous les états qui acceptent x.
	for (int i =0; i<states.size(); i++)
	{

		int currentState = states[i];
		bool answer = all_paths_satisfy(currentState,memory);
		
		if (answer == true) {
			results.push_back(currentState);
			memory[currentState] = COMPLETE_VALID; //3 == Complètement calculé, true
		}

		else {
			memory[currentState] = COMPLETE_INVALID; //4 == Complètement calculé, false
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

A (o) – All: o has to hold on all paths starting from the current state.
X (o) – Next: o has to hold at the next state (this operator is sometimes noted N instead of X).

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

	//Condition de sortie : on a atteint un état qui satisfait la seconde condition ou un etat qui accept deja
	if (memory[stateId] > AU_FIRST )
		return true;

	//Le reste des cas
	if (memory[stateId] != AU_FIRST)
		return false;

	
	//Pour tous les transitions de cet état. Chaque transition doit retourner true pour que l'état soit validé
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


enum EX_ENUM
{

	EX_VALID = 1,
};

//Find the states that have the property : Some paths lead to o formula
//Input : states that accept the rightside formula (o)
//Output : a vector of states that satisfy the whole formula (EX o)
//This function does not need a helper function since it's not recursive
vector<int> EX(vector<int> states)
{

	//Let's build the memory structure
	vector<int> memory;
	memory.resize(fsm.size() );
	vector<int> results;

	//We input the accepting states into the memory structure
	for (int i = 0; i < states.size(); i++)
	{
		int state = states[i];
		memory[state] = EX_VALID;
	}


	//For all the states
	for (int i = 0; i < fsm.size() ; i++)
	{
		int state = i;

		//For all the transitions, find if there's a transition to a valid state
		for (int j = 0; j < fsm[state].transitions.size(); j++)
		{
			int destination = fsm[state].transitions[j].destinationid;
			if (memory[destination] == EX_VALID) {
				results.push_back(state);
				break; //we don't want to do the others after
			}

		}

	}

	return results;

}






enum AW_enum
{

	AW_FIRST = 1, //The state satisfies the first condition
	AW_SECOND = 2, //the state satisfies the second condition
	AW_ACCEPT = 3, //Not used since using AW_SECOND works as well
};



bool AW_helper(int stateId, vector<int > & memory)
{

	//Si on atteint le second, on return true tout de suite
	if (memory[stateId] == AW_SECOND)
		return true;

	//Condition de sortie : Si on atteint autre chose que FIRST OU SECOND, return false
	if (memory[stateId] != AW_FIRST  && memory[stateId] != AW_SECOND )
		return false;

	//Pour tous les transitions de cet état. Chaque transition doit retourner true pour que l'état soit validé
	for (int i = 0; i < fsm[stateId].transitions.size(); i++)
	{
		int destination = fsm[stateId].transitions[i].destinationid;
		bool answer = AW_helper(destination, memory);
		if (answer == false) return false;
	}

	//Use the memory to save this calculation
	memory[stateId] = AW_ACCEPT;
	return true;

}


/*
Input : set of states for the first condition, set of states for the second condition
output : the states that satisfy these constraints
*/

vector<int> AW(vector<int> first, vector<int> second)
{

	vector<int> results;
	vector<int> memory;
	memory.resize(fsm.size());

	//Init the memory data structure
	for (int i = 0; i < first.size(); i++)
	{
		int state = first[i];
		memory[state] = AW_FIRST;
	}

	for (int i = 0; i < second.size(); i++)
	{
		int state = second[i];
		memory[state] = AW_SECOND;
	}

	//The memory is now filled with states that satisfy either first or second condition
	//We can now iterate through all the states that satisfy the first condition and decide 
	//whether or not they hold until the second condition
	//Weak until difference : they don't have to hold until a second condition

	for (int i = 0; i < first.size(); i++)
	{
		int state = first[i];

		//These states have already been calculated with the recursivity
		if (memory[state] == AW_ACCEPT) {
			results.push_back(state);
			continue;
		}

		bool answer = AW_helper(state, memory);
		if (answer == true)
			results.push_back(state);
	}



	return results;

}






enum EU_enum
{

	EU_FIRST = 1, //The state satisfies the first condition
	EU_SECOND = 2, //the state satisfies the second condition
	EU_ACCEPT = 3, //Not used since using AU_SECOND works as well
};



bool EU_helper(int stateId, vector<int > & memory)
{

	//Condition de sortie : on a atteint un état qui satisfait la seconde condition ou un etat qui accept deja
	if (memory[stateId] > EU_FIRST )
		return true;

	//Le reste des cas
	if (memory[stateId] != EU_FIRST)
		return false;

	//Donc si c'est EU_FIRST ON CONTINUE ICI
	
	//Pour tous les transitions de cet état. Une seule! transition doit retourner true pour que l'état soit validé
	for (int i = 0; i < fsm[stateId].transitions.size(); i++)
	{
		int destination = fsm[stateId].transitions[i].destinationid;
		bool answer = EU_helper(destination, memory);
		if (answer == true) 
		{
			//On sauve la reponse dans la memoire
			memory[stateId] = EU_ACCEPT;
			return true;
		}
	}

	//Si après avoir fait toutes les transitions on n'a pas eu un vrai, on retourne faux
	return false;

}


/*
Input : set of states for the first condition, set of states for the second condition
output : the states that satisfy these constraints
*/

vector<int> EU(vector<int> first, vector<int> second)
{

	vector<int> results;
	vector<int> memory;
	memory.resize(fsm.size());

	//Init the memory data structure
	for (int i = 0; i < first.size(); i++)
	{
		int state = first[i];
		memory[state] = EU_FIRST;
	}

	for (int i = 0; i < second.size(); i++)
	{
		int state = second[i];
		memory[state] = EU_SECOND;
	}

	//The memory is now filled with states that satisfy either first or second condition (or other)
	//We can now iterate through all the states that satisfy the first condition and decide 
	//whether or not they hold until the second condition

	for (int i = 0; i < first.size(); i++)
	{
		int state = first[i];

		//These states have already been calculated with the recursivity
		if (memory[state] == EU_ACCEPT) {
			results.push_back(state);
			continue;
		}

		bool answer = EU_helper(state, memory);
		if (answer == true)
			results.push_back(state);
	}



	return results;

}





enum EW_enum
{

	EW_FIRST = 1, //The state satisfies the first condition
	EW_SECOND = 2, //the state satisfies the second condition
	EW_ACCEPT = 3, //Not used since using AU_SECOND works as well
};


//TODO : reflechir a la condition de sortie avec le noir

bool EW_helper(int stateId, vector<int > & memory)
{

	//Condition de sortie : on a atteint un état qui satisfait la seconde condition ou un etat qui accept deja
	if (memory[stateId] > EW_FIRST )
		return true;

	//Le reste des cas
	if (memory[stateId] != EW_FIRST)
		return false;

	//Cas rare : a tester plus
	if (memory[stateId] == EW_FIRST && fsm[stateId].transitions.size() == 0)
		return true;

	//Donc si c'est EW_FIRST ON CONTINUE ICI
	
	//Pour tous les transitions de cet état. Une seule transition doit retourner true pour que l'état soit validé
	for (int i = 0; i < fsm[stateId].transitions.size(); i++)
	{
		int destination = fsm[stateId].transitions[i].destinationid;
		bool answer = EW_helper(destination, memory);
		if (answer == true) 
		{
			//On sauve la reponse dans la memoire
			memory[stateId] = EW_ACCEPT;
			return true;
		}
	}

	//Si après avoir fait toutes les transitions on n'a pas eu un vrai, on retourne faux
	return false;

}


/*
Input : set of states for the first condition, set of states for the second condition
output : the states that satisfy these constraints
*/

vector<int> EW(vector<int> first, vector<int> second)
{

	vector<int> results;
	vector<int> memory;
	memory.resize(fsm.size());

	//Init the memory data structure
	for (int i = 0; i < first.size(); i++)
	{
		int state = first[i];
		memory[state] = EW_FIRST;
	}

	for (int i = 0; i < second.size(); i++)
	{
		int state = second[i];
		memory[state] = EW_SECOND;
	}

	//The memory is now filled with states that satisfy either first or second condition (or other)
	//We can now iterate through all the states that satisfy the first condition and decide 
	//whether or not they hold until the second condition

	for (int i = 0; i < first.size(); i++)
	{
		int state = first[i];

		//These states have already been calculated with the recursivity
		if (memory[state] == EW_ACCEPT) {
			results.push_back(state);
			continue;
		}

		bool answer = EW_helper(state, memory);
		if (answer == true)
			results.push_back(state);
	}



	return results;

}




void batterie_tests1(void)
{

	//AX AG AF AU AW
	//EX EG EF EU EW

	//TODO
	// EU EW (just a couple more!!)

	
	readStates("states.txt");

	readTransitions("transitions.txt");
 
	//On veut vérifier la propriété AF AG x sur la FSM
	
	//#1 Matcher les états en fonction de la valuation des variables. Ici on cherche x.
	//Si n états et m variables, prend un temps O(n*m)
	vector<int> recherche;
	recherche.resize(3);

	recherche[0] = 1;
	recherche[1] = 0;
	recherche[2] = 0;

	vector<int> res = matchVariables(recherche);

	vector<int> result = AG(res);

	vector<int> result2 = AF(result);

	//Pour l'instant les résultats sont bons, encore quelques incertitudes par rapport a AF


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

	//Let's test EX
	vector<int> inputex;
	inputex.push_back(5);
	vector<int> resex = EX(inputex);

	//EX MARCHE (easy)



}

void batterie_tests2(void)
{


	readStates("states_rougenoir.txt");

	readTransitions("transitions_rougenoir.txt");

	//Maintenant faisons un test sur rouge noir

	//On veut trouver tous les états qui sont NOIR
	//Need initializer list pls
	vector<int> noir; noir.resize(2); noir[0] = 1; noir[1] = 0;
	vector<int> rouge; rouge.resize(2); rouge[0] = 0; rouge[1] = 1;


	vector<int> resultats_noir = matchVariables(noir);
	vector<int> resultats_rouge = matchVariables(rouge);

	//Testons maintenant AU
	vector<int> resultats_AU = AU(resultats_noir, resultats_rouge);

	//Testons maintenant AW, meme resultats normalement
	vector<int> resultats_AW = AW(resultats_noir, resultats_rouge);


	//Modifions maintenant légèrement l'exemple pour AU. Nous allons mettre le noeud 4 en blanc
	//et normalement cela fait en sorte que le noeud 1 ne peut plus accepter.

	fsm[4].variables[0] = 0;
	fsm[4].variables[1] = 0;

	resultats_noir = matchVariables(noir);
	resultats_rouge = matchVariables(rouge);

	//Testons maintenant AU
	resultats_AU = AU(resultats_noir, resultats_rouge);

	//Marche comme prevu, 1 n'est plus accepté

	//Testons AW en coloriant l'arbre de gauche (4-5-9-10-14) en noir
	fsm[4].variables[0] = 1; fsm[4].variables[1] = 0;
	fsm[5].variables[0] = 1; fsm[5].variables[1] = 0;
	fsm[9].variables[0] = 1; fsm[9].variables[1] = 0;
	fsm[10].variables[0] = 1; fsm[10].variables[1] = 0;
	fsm[14].variables[0] = 1; fsm[14].variables[1] = 0;

	resultats_noir = matchVariables(noir);
	resultats_rouge = matchVariables(rouge);

	//Testons maintenant AW (bcp de noeuds peuvent accepter maintenant)

	resultats_AW = AW(resultats_noir, resultats_rouge);

	//essai EU

	//Modifions les noeuds 1 et 2 pour que leurs chemins soient invalides
	fsm[1].variables[0] = 0; fsm[1].variables[1] = 0;
	fsm[2].variables[0] = 0; fsm[2].variables[1] = 0;

	resultats_noir = matchVariables(noir);
	resultats_rouge = matchVariables(rouge);

	//On teste
	vector<int> resultats_EU = EU(resultats_noir, resultats_rouge);

	//marche bien!
	

	int u = 2+2; //pause

}




//BATTERIE TESTS1
int main(void)
{

	batterie_tests2();


	//Let's test weak until now (copy paste of until, just remove one exit condition)

	return 0;

}


