#include<iostream>

#include "Tape.h"

using namespace std;

int function_g(vector<int> record) {
	int result = 0;

	// g(x) = a0 + a1*x + a2*x^2 + a3*x^3 + a4*x^4
	for (int i = 0; i < 5; i++) {
		result += record[i] * pow(record[SIZE_RECORD - 1], i);
	}

	return result;
}

void adding_record(const string filename, vector<int> numbers) {

	ofstream file(filename, ios::binary | ios::app);

	file.write(reinterpret_cast<const char*>(numbers.data()), numbers.size() * SIZE_INT);

	file.close();
}

void generating_records(const string filename, int records_numbers) {
	ofstream file(filename, ios::binary | ios::trunc);

	srand(time(NULL));

	for (int i = 0; i < records_numbers; i++) {
		vector<int> numbers;
		for (int ii = 0; ii < SIZE_RECORD; ii++) {
			numbers.push_back(rand() % 10);
		}
		file.write(reinterpret_cast<const char*>(numbers.data()), numbers.size() * SIZE_INT);
	}

	file.close();
}

void show_file(const string filename) {
	Block read_file(filename);

	vector<int> record = read_file.get_next_record();
	cout << "\nRecords in " << filename << ":\n";
	int inc = 1;

	while (record.size()) {
		cout << inc << ". ";

		for (int ii = 0; ii < SIZE_RECORD; ii++) {
			cout << record[ii] << " ";
		}
		cout << "G(x) = " << function_g(record) << endl;
		inc++;
		record = read_file.get_next_record();
	}
}

int next_fibonacci(int n) {
	int a = 0, b = 1;
	while (a <= n) {
		int temp = a + b;
		a = b;
		b = temp;
	}
	return a;
}

void rewrite(string filename_read, string filename_write) {
	Block read(filename_read);
	
	ofstream file(filename_write, ios::binary | ios::trunc);

	vector<int> record = read.get_next_record();
	while (record.size()) {
		file.write(reinterpret_cast<const char*>(record.data()), record.size() * SIZE_INT);
		record = read.get_next_record();
	}
}

void divide_data(const string filename, Tape* tapes[SIZE_TAPES]) {

	Block read_data(filename);

	Tape* write = tapes[0];

	int series_counter = 0, tmp_fib = 1;

	vector<int> record1 = read_data.get_next_record(), record2(0);

	if (record1.size()) {

		while (true) {
			write->write_record(record1);

			record2 = read_data.get_next_record();

			if (!record2.size()) break;

			if (function_g(record1) > function_g(record2)) {
				series_counter++;

				write->series++;

				if (write->series == tmp_fib) {
					
					write == tapes[0] ? write = tapes[1] : write = tapes[0];

					tmp_fib = next_fibonacci(tmp_fib);  
				}
			}
			record1 = record2;
		}
		write->series++;
	}
	write->dummy_series = tmp_fib - write->series;
}

void sort(const string filename, const string result_filename, bool showing_after_phase) {
	Tape tape1("Tapes/tape1.bin");
	Tape tape2("Tapes/tape2.bin");
	Tape tape3("Tapes/tape3.bin");
	
	Tape* tapes[SIZE_TAPES] = { &tape1, &tape2, &tape3};

	divide_data(filename, tapes);

	if (showing_after_phase) {
		cout << endl << "After dividing into tapes" << endl;
		for (int i = 0; i < SIZE_TAPES; i++) {
			tapes[i]->close_tape();
			cout << endl << "Series: " << tapes[i]->series;
			if (tapes[i]->dummy_series) {
				cout << "(" << tapes[i]->dummy_series << ")";
			}
			show_file(tapes[i]->get_name());
		}
	}
	int series = 0;
	for (int i = 0; i < SIZE_TAPES; i++) {
		tapes[i]->close_tape();
		series += tapes[i]->series;
	}

	Tape* merge_tape1 = &tape1;
	Tape* merge_tape2 = &tape2;
	Tape* empty_tape = &tape3;
	
	int phase_counter = 1;
	
	vector<int> record1 = merge_tape1->get_next_record(), record2 = merge_tape2->get_next_record();

	// main loop
	while ((tape1.series + tape2.series + tape3.series) != 1)
	{

		int last_value1, value1, value2, last_value2;

		// dopóki mamy jeszcze serie
		while ((merge_tape1->series + merge_tape1->dummy_series) && (merge_tape2->series + merge_tape2->dummy_series)) {

			bool end_of_series1 = false, end_of_series2 = false;

			if (record1.size()) {
				end_of_series1 = false;
				value1 = function_g(record1);
				last_value1 = value1;
			}
			else {
				end_of_series1 = true;
				merge_tape1->series ? merge_tape1->series--: merge_tape1->dummy_series--;
			}

			if (record2.size()) {
				end_of_series2 = false;
				value2 = function_g(record2);
				last_value2 = value2;
			}
			else {
				end_of_series2 = true;
				merge_tape2->series ? merge_tape2->series-- : merge_tape2->dummy_series--;
			}

			while (!end_of_series1 || !end_of_series2) {
				
				bool greater = false;

				if (end_of_series1) {
					empty_tape->write_record(record2);
					last_value2 = value2;
				}
				else if (end_of_series2) {
					empty_tape->write_record(record1);
					last_value1 = value1;
				}
				else {
					greater = true;
					if (value1 <= value2) {
						empty_tape->write_record(record1);
						last_value1 = value1;
					}
					else {
						empty_tape->write_record(record2);
						last_value2 = value2;
					}					
				}

				if (!end_of_series1 && (end_of_series2 || value1 <= value2)) {
					record1 = merge_tape1->get_next_record();
					if (record1.size()) {
						value1 = function_g(record1);
						if (last_value1 > value1) {
							end_of_series1 = true;
							merge_tape1->series--;
						}
					}
					else {
						end_of_series1 = true;
						merge_tape1->series--;
					}
				}
				else {
					record2 = merge_tape2->get_next_record();
					if (record2.size()) {
						value2 = function_g(record2);
						if (last_value2 > value2) {
							end_of_series2 = true;
							merge_tape2->series--;
						}
					}
					else {
						end_of_series2 = true;
						merge_tape2->series--;
					}
				}

			}
			empty_tape->series++;
		}
		
		empty_tape->close_tape();
	
		for (int i = 0; i < SIZE_TAPES; i++) {
			if (!tapes[i]->series) {
				tapes[i]->clear_file();
				int tmp = tapes[i]->block.index_record;
				if (!tapes[i]->series) tapes[i]->close_tape();
				tapes[i]->block.index_record = tmp;
			}
		}

		bool sorted;
		tape1.series + tape2.series + tape3.series == 1 ? sorted = true : sorted = false;
		
		if ((merge_tape1->dummy_series + merge_tape1->series) == 0) {
			Tape* tmp = merge_tape1;
			merge_tape1 = empty_tape;
			empty_tape = tmp;
			if (!sorted) record1 = merge_tape1->get_next_record();
		}
		else {
			Tape* tmp = merge_tape2;
			merge_tape2 = empty_tape;
			empty_tape = tmp;
			if (!sorted) record2 = merge_tape2->get_next_record();
		}
		


		if (showing_after_phase) {
			cout << endl << "-------------PHASE " << phase_counter << "-------------" << endl;
			for (int i = 0; i < SIZE_TAPES; i++) {
				int tmp = tapes[i]->block.index_record;
				if(!tapes[i]->series) tapes[i]->close_tape();
				tapes[i]->block.index_record = tmp;
				cout << endl << "Series: " << tapes[i]->series;
				if (tapes[i]->dummy_series) {
					cout << "(" << tapes[i]->dummy_series << ")";
				}
				show_file(tapes[i]->get_name());
			}
		}
		phase_counter++;
	}

	int read_write = 0;
	for (int i = 0; i < SIZE_TAPES; i++) {
		if (tapes[i]->series) {
			rewrite(tapes[i]->get_name(), result_filename);
		}
		read_write += tapes[i]->read_write();
	}

	cout << endl << "Records have been sorted" << endl;
	cout << "Phases: " << phase_counter << endl;
	cout << "Disk operation: " << read_write << endl;
	cout << "Series: " << series << endl;
}


void start_program(const string filename, const string result_filename) {

	while (true) {
		cout << "\nChoose: (write 1-4)" << endl;
		cout << "1. Print file" << endl;
		cout << "2. Print sorted file" << endl;
		cout << "3. Sort" << endl;
		cout << "4. Generate data" << endl;
		cout << "5. Add record" << endl;
		cout << "6. Quit\n" << endl;

		int choice;
		cin >> choice;

		if (choice == 1) {
			show_file(filename);
		}
		else if (choice == 2) {
			show_file(result_filename);
		}
		else if (choice == 3) {
			cout << "Do you want to print file in every phase of sorting(Choose: 0(No)/1(Yes)): ";
			cin >> choice;
			sort(filename, result_filename, choice);
		}
		else if (choice == 4) {
			cout << "Enter number of records: ";
			cin >> choice;
			generating_records(filename, choice);
		}
		else if (choice == 5) {
			
			while (true) {
				vector<int> new_record;
				
				cout << "Enter values for new record (6 integers after spaces): ";
				for (int i = 0; i < SIZE_RECORD; i++) {
					cin >> choice;
					new_record.push_back(choice);
				}
				
				adding_record(filename, new_record);
				cout << "Do you want to add more records? (Choose: 0(NO)/ YES)";
				cin >> choice;

				if (!choice) break;
			}
		}
		else if (choice == 6) {
			break;
		}
		else {
			cout << "Wrong option. Try again.\n\n";
		}
	}
}

int main() {
	
	string filename = "Data/data.bin", result = "Data/result.bin";
	
	start_program(filename, result);

	return 0;
}