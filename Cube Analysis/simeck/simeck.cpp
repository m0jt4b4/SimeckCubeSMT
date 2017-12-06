// simeck.cpp : Defines the entry point for the console application.
// author: Mojtaba Zaheri
#include "stdafx.h"
#include <iostream>

#include "offline_search_32.h"
#include "online_attack_32.h"
#include "offline_full_search_32.h"

using namespace std;

void usage(char* binary)
{
	wcerr << "Usage of the tool:" << endl;
	wcerr << endl;
	wcerr << (char*)binary << " /online r n" << endl;
	wcerr << (char*)binary << " /offline s r\n" << endl;
	wcerr << "    /online r n: online cube attack on r-round simeck32/64 n times." << endl;
	wcerr << "    /offine s r : s==l:: offline cube search for linear equations on r-round simeck32/64 ." << endl;
	wcerr << "                  s==a:: offline r-size cube search for constant and linear equations for all rounds" << endl;
	exit(1);
}

int main(int argc, char* argv[])
{
	cout << "\n ** Simeck Cube Attack Tool **\n" << endl;
	if(argc > 1)
		if (!strcmp(argv[1], ("/online")) && argc == 4)
			online_attack_32(atoi(argv[2]), atoi(argv[3]));
		else if(!strcmp(argv[1], ("/offline")) && argc == 4)
			if(!strcmp(argv[2],"l"))
				offline_search(atoi(argv[3]));
			else if(!strcmp(argv[2],"a"))
				offline_full_search(atoi(argv[3]));
			else
				usage(argv[0]);
		else
			usage(argv[0]);
	else
		usage(argv[0]);

	return 0;
}

