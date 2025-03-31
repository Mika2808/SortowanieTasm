#include "Tape.h"

Tape::Tape() :block(""), series(0), dummy_series(0), empty(0) {

}

Tape::Tape(string name): block(name), filename(name), series(0), dummy_series(0), empty(0){
	ofstream file(filename, ios::binary | ios::trunc);
	file.close();
}

Tape::~Tape() {

}

vector<int> Tape::get_next_record() {
	return block.get_next_record();
}

void Tape::write_record(vector<int>& record) {
	block.write_record(record);
}

void Tape::close_tape() {
	block.close_file();
}

void Tape::clear_file() {
	block.clear_file();
}

string Tape::get_name() {
	return filename;
}

int Tape::read_write() {
	return block.read_write();
}