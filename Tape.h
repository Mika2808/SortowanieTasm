#pragma once
#include "Block.h"

class Tape
{
	string filename;


	bool empty;

public:
	int series;
	int dummy_series;
	Block block;

	Tape();
	Tape(string name);
	~Tape();

	vector<int> get_next_record();
	void write_record(vector<int>& record);
	void close_tape();
	string get_name();
	void clear_file();
	int read_write();
};

