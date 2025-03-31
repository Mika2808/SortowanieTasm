#include "Block.h"

Block::Block(string filename) : filename(filename), read_block(0), written_block(0),read_records(0), index_record(0), end_of_file(false) {
	int dot_position = filename.find_last_of(".");
	filename.substr(dot_position + 1) == "txt" ? txt = true : txt = false;

	txt ? file.open(filename, ios::in | ios::out | ios::binary) : file.open(filename, ios::in | ios::out);
}

Block::~Block() {
	if (index_record) {
		write_block();
	}

	if (file.is_open()) {
		file.close();
	}
}

void Block::load_block() {

	if (!txt) {
		file.read(buffer, SIZE_BUFFER);
		
		read_records = file.gcount() / (SIZE_INT * SIZE_RECORD);
		read_records < SIZE_BLOCK ? end_of_file = true : end_of_file = false;

	}
	else {
		vector<int> records_buffer;

		string block;
		
		getline(file, block);
		
		int integer;
		istringstream stream(block);

		while (stream >> integer) {
			records_buffer.push_back(integer);
		}

		if (records_buffer.size() < SIZE_RECORD * SIZE_BLOCK) { 
			end_of_file = true; 
		}

		read_records = records_buffer.size() / SIZE_RECORD;

		for (int i = 0; i < records_buffer.size(); ++i) {

			std::memcpy(buffer + i * SIZE_INT, &records_buffer[i], SIZE_INT);
		}

	}

	index_record = 0;
	read_block++;
}

void Block::write_block() {
	if (!txt) {
		if (index_record) {
			file.write(buffer, index_record * SIZE_RECORD * SIZE_INT);
		}		
	}
	else {
		vector<int> records_buffer;

		for (int i = 0; i < index_record * SIZE_RECORD; i++) {
			int integer;
			std::memcpy(&integer, &buffer[i * SIZE_INT], SIZE_INT);

			records_buffer.push_back(integer);
		}

		stringstream stream;

		for (int i = 0; i < records_buffer.size(); ++i) {
			stream << records_buffer[i];
			if (i != records_buffer.size() - 1) {
				stream << " ";
			}
		}

		file << stream.str() << endl;
	}

	index_record = 0;
	written_block++;
}

vector<int> Block::get_next_record() {
	
	if (index_record == read_records && !end_of_file) {
		load_block();
	}

	if (index_record < read_records || !end_of_file) {

		vector<int> record(SIZE_RECORD);

		for (int i = 0; i < SIZE_RECORD; i++) {
			memcpy(&record[i], buffer + (index_record * SIZE_RECORD + i) * SIZE_INT, SIZE_INT);
		}

		index_record++;

		return record;
	}
	else {

		return {};
	}
}

void Block::write_record(vector<int>& record) {
	memcpy(buffer + index_record * SIZE_RECORD * SIZE_INT, record.data(), record.size() * sizeof(int));

	index_record++;
	if (index_record == SIZE_BLOCK) {
		write_block();
	}
}

void Block::close_file() {
	if (index_record) {
		write_block();
	}
	
	file.close();
	txt ? file.open(filename, ios::in | ios::out | ios::binary) : file.open(filename, ios::in | ios::out);
}

void Block::clear_file() {
	file.close();
	ofstream file(filename, ios::binary | ios::trunc);
	file.close();
	end_of_file = false;
	index_record = 0;
	read_records = 0;
	txt ? file.open(filename, ios::in | ios::out | ios::binary) : file.open(filename, ios::in | ios::out);
}

int Block::read_write() {
	return (read_block + written_block);
}