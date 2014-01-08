#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

//Composantes fortement connexes
//Voir cet algorithme la

//Tuple pour mettre a jour la valeur d'une variable avec la transition
//struct modification
//{
//	int variableid;
//	bool value;
//};


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




vector<state> fsm;


//Essayer de le faire avec sprintf ?


int readIntegerBeforeComma(char  * &  pc)
{

	char buffer[200];
	memset(buffer, 0, 200);
	int i = 0;
		
	while (1)
	{
		//test#1, fin de la string
		if (*pc == 0) return -1;

		//test#2, la virgule
		if (*pc == ',') break;

		//skip whitespace
		if (*pc == ' ') 
		{
			pc++;	
			continue;
		}

		//On enregistre dans buffer
		buffer[i++] = *pc;
		pc++;



	}
	//On va pointer apres la virgule
	pc++;
	int value = atoi(buffer);
	return value;

}


string readStringBeforeComma(char * & pc)
{

	char buffer[200];
	memset(buffer, 0, 200);
	int i = 0;
		
	while (1)
	{
		//test#1, fin de la string
		if (*pc == 0) return 0;

		//test#2, la virgule
		if (*pc == ',') break;

		//On enregistre dans buffer
		buffer[i++] = *pc;
		pc++;
	}

	//On va pointer apres la virgule
	pc++;
	string value = buffer;
	return value;

}


int readVariables(char * & pc, state & newState)
{

	//Match opening bracket
	while (1)
	{
		//check for end of string
		if (*pc == 0) return 0;

		if (*pc == '{' ) {
			pc++;
			break;
		}

		pc++;
	}

	//Match variables, add them to the vector one by one. Stop when } is found

	while (1)
	{
		//End of string, error
		if (*pc == 0) return 0;

		else if (*pc == '1')
		{
			newState.variables.push_back(1);
			pc++;
		}

		else if (*pc == '0')
		{
			newState.variables.push_back(0);
			pc++;
		}

		//Good exit
		else if (*pc == '}' )
		{
			break;
		}

		//Normal continuation
		else pc++;

	}

	return 1;
}



void readStates(char * filename)
{

	FILE * fichier = fopen(filename, "r");
	static char line[200];

	char * pc; //iterator

	while (fgets(line, 200, fichier) != NULL) 
	{

		//Commentary, skip
		if (line[0] == '#') continue;

		//Empty line, skip
		if (line[0] == 10) continue;

		//Read the state
		state newState; //create a new empty state

		pc = line;

		int r1 = readIntegerBeforeComma(pc);
		if (r1 == -1) exit(1); //generate error here
		else newState.id = r1;

		string r2 = readStringBeforeComma(pc);
		if (r2 == "") exit(1);
		else newState.name = r2;

		int r3 = readVariables(pc, newState);
		if (r3 == -1) exit(1);

		//Push the state into the FSM
		fsm.push_back(newState);

	}



}

string readTransitionName(char * & pc)
{
	static char name[200];
	memset(name, 0, 200); //probably not necessary/fast but it removes potential future bugs!
	int i = 0;

	while (1)
	{
		//EOF?
		if (*pc == NULL) break;

		//Linefeed, CR (possibly a problem with Linux created files here,or not)
		if (*pc == 10 || *pc == 13) break;
		
		//Add it to this
		name[i++] = *pc;

		pc++;
	}

	return name;

}



//This function reads the file that contains the transitions 
void readTransitions(char * filename)
{

	FILE * fichier = fopen(filename, "r");
	static char line[200];

	char * pc; //iterator

	while (fgets(line, 200, fichier) != NULL) 
	{

		transition newTransition;

		//Commentary, skip
		if (line[0] == '#') continue;

		//Empty line, skip
		if (line[0] == 10) continue;

		pc = line;

		int source = readIntegerBeforeComma(pc);
		if (source == -1) exit(1); //generate error here

		int destination = readIntegerBeforeComma(pc);
		if (destination == -1) exit(1);

		string name = readTransitionName(pc);
		//if (name == "") exit(1);

		newTransition.sourceid = source;
		newTransition.destinationid = destination;
		newTransition.name = name;

		//Add the transition to the FSM
		fsm[source].transitions.push_back(newTransition);

		//Les transtions sont one way, donc c'est tout ce qu'on fait.
	}

}



/*

Fonction de recherche

#1. On cherche et tag tous les états qui satisfont les variables de base.

#2. On implémente le premier niveau de CTL

#3. 2ème niveau de CTL.


Résultat = un vecteur d'états.




*/

//Retourne un vecteur avec tous les états matchés (simple vérification de variables, comme dans l'exemple).
vector<int> matchVariables(vector<int> recherche )
{

	vector<int>resultats;
	resultats.resize(8);
	for (int i=0; i < resultats.size(); i++) resultats[i]=0; //Initialisation (peut etre non nécessaire?

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
		resultats[i] = 1;

		//Non presente, on continue au prochain état
		nonPresente: ;

	}


	return resultats;

}


//Cette fonction recursive prend en parametre un etat initial et trouve si les transitions partant de cet état
//satisfont toutes (x).

//Peut utiliser le même tableau global pour faire la fonction a mémoire ou alors on passe le tableau en paramètre a la fonction
//Comme ça aucun side effects...

bool all_paths_satisfy(int stateId, vector<int> & satisfy_x)
{

	//Pour toutes les transitions de cet état
	for (int i=0; i< fsm[stateId].transitions.size(); i++)
	{
		int nextState = fsm[stateId].transitions[i].destinationid;
		//Si le prochain état est un etat invalide, return false
		if (satisfy_x[nextState] == 0)
		{
			return false;
		}

		//Si le prochain état est un état déja visité, on continue a la prochaine transition
		if (satisfy_x[nextState] == 2)
		{
			continue;
		}

		//Si le prochain état est valide et non visité, on fait un appel récursif
		if (satisfy_x[nextState] == 1)
		{
			bool answer = all_paths_satisfy(nextState, satisfy_x);
			if (answer == true) return true;
			if (answer == false) return false;
		}
	}

	//Si on a parcouru toutes les transitions de l'état sans atterir dans un état invalide, on peut retourner true
	return true;
}


//Cette fonction fait le travail "All paths, globally x"
//Prend en paramètre les états acceptants X. La fonction retourne en résultats les états qui valident la propriété
//All paths, globally x.
//Input : Table de taille n. Si l'état est présent, 1, sinon 0
//Output : Table de taille n. Si l'état est présent 1, sinon 0
vector<int> AG(vector<int> startingStates)
{

	vector<int> results;
	vector<int> satisfy_x;

	satisfy_x.resize(fsm.size() );

	//On crée le vecteur satisfy_x
	for (int i = 0; i < startingStates.size(); i++)
	{
		int state = startingStates[i];
		satisfy_x[state] == 1;
	}

	//On itère a travers tous les états qui acceptent x.
	for (int i =0; i<startingStates.size(); i++)
	{

		int currentState = startingStates[i];
		bool answer = all_paths_satisfy(currentState,satisfy_x);
		
		if (answer == true)
			results.push_back(currentState);

	}

	return results;

}

int main(void)
{

	//Done
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


	return 0;


}


