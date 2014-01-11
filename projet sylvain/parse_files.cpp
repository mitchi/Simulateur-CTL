//Ce fichier contient les fonctions qui parse les fichiers d'entrée, construisent la FSM.

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include "includes.h"

//The FSM, defined elsewhere
extern vector<state> fsm;

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