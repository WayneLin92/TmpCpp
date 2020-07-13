// tmp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// Uncomment the following to detect memery leaks
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

//#include "mymath.h"
#include "dababase.h"
#include "myparser.h"
#include <fstream>
#include <sstream>

int main(int argc, char** argv)
{
	int return_code = main_sqlite(argc, argv);

	//int return_code = 0;
	/*
	std::ifstream file;
	if (argc > 1) {
		char* filename = argv[1];
		file.open(filename);
	}
	else
		file.open("relations-7.txt");

	if (file.fail()) {
		std::cout << "File can not open." << std::endl;
		return 0;
	}
	std::string line;
	std::vector<std::string> gen_names;
	std::vector<int> gen_degs;
	Polys rel_gens;
	file >> "gen_names:";
	load_vector(gen_names, file, "[", ",", "]", load_py_str);
	file >> std::ws >> "gen_degs:";

	load_vector(gen_degs, file, "[", ",", "]");
	file >> std::ws >> "relations:" >> rel_gens;
	file.close();
	std::cout << "gen_names: ";
	dump_vector(gen_names, std::cout, "[", ", ", "]\n");
	std::cout << "gen_degs: " << gen_degs << '\n';
	std::cout << "relations: " << rel_gens << '\n';

	GroebnerBasis gb;
	for (Poly& rel : rel_gens) {
		sort(rel);
		std::cout << "adding " << rel << '\n';
		gb.add_rel(rel, &gen_degs);
	}
	/*for (Poly& rel : relations)
		std::cout << rel << '\n';*/
	/*
	std::cout << "rels size:" << gb.m_rels.size() << '\n';
	*/

	// Uncomment the following to detect memory leaks
	//_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	//_CrtDumpMemoryLeaks();
	return return_code;
}