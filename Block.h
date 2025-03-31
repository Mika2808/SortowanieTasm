#pragma once

#include<fstream>
#include<string>
#include<vector>
#include <sstream>
#include "const.h"

using namespace std;

class Block
{
	fstream file;
	string filename;

	char buffer[SIZE_BUFFER];

	int read_block;
	int written_block;
	

	int read_records;
	bool end_of_file;
	bool txt;

	void load_block();
	void write_block();

public:
	Block();
	Block(string filename);
	~Block();

	vector<int> get_next_record();
	void write_record(vector<int>& record);
	void close_file();
	void clear_file();
	int index_record;
	int read_write();
};