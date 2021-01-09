// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
/* Uncomment the following to detect memery leaks */
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#include "main.h"
#include "myparser.h"
#include <fstream>
#include <sstream>

int main_generate_basis(int argc, char** argv)
{
	/*Database db;
	db.init(R"(C:\Users\lwnpk\Documents\MyProgramData\Math_AlgTop\database\E4t.db)");
	std::string table_prefix = "HA";
	int t_max = 200;
	generate_basis(db, table_prefix, t_max, true);
	return 0;*/
	
	if (argc == 6) {
		const char* db_path = argv[2];
		const char* table_prefix = argv[3];
		int t_max = std::stoi(argv[4]);
		bool drop_existing = argv[5] == "0" ? false : true;

		Database db(db_path);
		generate_basis(db, table_prefix, t_max, drop_existing);
		return 0;
	}
	else {
		std::cout << "usage ss basis <db_path> <table_name> <t_max> <drop_existing : 1 | 0>\n";
		return 0;
	}
}

int main_generate_diff(int argc, char** argv)
{
	if (argc == 5) {
		const char* db_path = argv[2];
		const char* table_prefix = argv[3];
		int r = std::stoi(argv[4]);

		Database db(db_path);
		generate_mon_diffs(db, table_prefix, r);
		return 0;
	}
	else {
		std::cout << "usage ss diff <db_path> <table_name> <r>\n";
		return 0;
	}
}

int main_generate_ss(int argc, char** argv)
{
	if (argc == 5) {
		const char* db_path = argv[2];
		std::string table_prefix = argv[3];
		int r = std::stoi(argv[4]);

		Database db(db_path);
		generate_ss(db, table_prefix + "_basis", table_prefix + "_ss", r);
		return 0;
	}
	else {
		std::cout << "usage ss ss <db_path> <table_name> <r>\n";
		return 0;
	}
}

int main_generate_next_page(int argc, char** argv)
{
	if (argc == 6) {
		const char* db_path = argv[2];
		std::string table_prefix = argv[3];
		std::string table_H_prefix = argv[4];
		int r = std::stoi(argv[5]);

		Database db(db_path);
		generate_next_page(db, table_prefix, table_H_prefix, r);
		return 0;
	}
	else {
		std::cout << "usage ss next_page <db_path> <table_name> <tables_name_H> <r>\n";
		return 0;
	}
}

int main_generate_ss_test()
{
	char argv0[100] = "";
	char argv1[100] = "";
	char argv2[100] = "C:\\Users\\lwnpk\\Documents\\MyProgramData\\Math_AlgTop\\database\\tmp.db";
	char argv3[100] = "E4t";
	char argv4[100] = "4";
	char* argv[] = { argv0, argv1, argv2, argv3, argv4 };
	main_generate_ss(5, argv);

	return 0;
}

void DeduceDiffs(const Database& db, const std::string& table_prefix, int t_max);

int main_deduce_diffs()
{
	Database db("C:\\Users\\lwnpk\\Documents\\MyProgramData\\Math_AlgTop\\database\\tmp.db");
	std::string table_prefix = "E4t";
	int t_max = 30;
	DeduceDiffs(db, table_prefix, t_max);
	return 0;
}

int main(int argc, char** argv)
{
	return main_deduce_diffs();////

	if (argc > 1) {
		if (strcmp(argv[1], "basis") == 0)
			return main_generate_basis(argc, argv);
		else if (strcmp(argv[1], "diff") == 0)
			return main_generate_diff(argc, argv);
		else if (strcmp(argv[1], "ss") == 0)
			return main_generate_ss(argc, argv);
		else if (strcmp(argv[1], "next_page") == 0)
			return main_generate_next_page(argc, argv);
	}
	std::cout << "usage ss <basis, diff, ss, next_page> [<args>]\n";
	return 0;

	/* Uncomment the following to detect memery leaks */
	//_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	//_CrtDumpMemoryLeaks();
}