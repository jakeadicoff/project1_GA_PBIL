#include "algs.h"
#include <cstdlib>
#include <fstream>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

int numVars;

int main(int argc, char** argv) {
  string problem_file_name = argv[1];
  int populationSize = stoi(argv[2]);
  string selectionType = argv[3];
  string crossoverType = argv[4];
  double crossoverProbability = stod(argv[5]);
  double mutationProbability = stod(argv[6]);
  int generationNumber = stoi(argv[7]);
  string algType = argv[8];
  //process file and get pointer to vector of clauses
  vector <vector<int> > all_clauses = readFile(problem_file_name);
  cout << "******************************************" << endl;
  cout << " File name: " << problem_file_name << endl;
  if (algType == "g") {
    GA ga_alg(populationSize, selectionType, crossoverType,
  	      crossoverProbability, mutationProbability, generationNumber,
  	      all_clauses, numVars );
    ga_alg.runGA();
  }
  else if (algType == "p") {
    PBIL pbil_alg(populationSize, stod(argv[3]), stod(argv[4]), stod(argv[5]),
		  stod(argv[6]), generationNumber, all_clauses, numVars);
    pbil_alg.runPBIL();
  }
  cout << "******************************************" << endl;
  return 0;
}

vector <vector<int> > readFile(string problem_file_name) {
    //item to return
    vector <vector<int> > vector_of_clauses;
    //file stream initialization
    ifstream problem_stream;
    string num_clauses, num_literals;
    //hold iteam that was just pulled from the file stream
    string next_item_in_stream;
    //for advancing filest ream
    int big_int = 100000;
    problem_stream.open(problem_file_name.c_str(), ios::in);
    if(!problem_stream.good()) { //open file stream
        cout << "Error: not able to open file" << endl;
    }
    // WILL NEED FIXING IN FILES WITH MORE WRITING AT BEGINING. WILL IGNORE TO FIRST 'p'

    string line;
    while(true) {
        problem_stream.ignore(big_int, 'p'); //jump to where line starts with p
        char curr_line[100];
        problem_stream.getline(curr_line, 100);
        line = curr_line;
        if(line.substr(0, 4).compare(" cnf") == 0) {
            break;
        }
    }
    istringstream iss(line);
    iss.ignore(big_int, 'f'); //jump to after 'cnf' in same line
    iss >> num_literals; //start grapping relevant info
    iss >> num_clauses;
    problem_stream >> next_item_in_stream; //to first literal in first clause
    while(problem_stream.peek()!=EOF) { //run until end of file
        vector <int> clause; //make clause vectore to hold literals
        while(next_item_in_stream != "0") { //run until end of line
            clause.push_back(stoi(next_item_in_stream));
            problem_stream >> next_item_in_stream;
        }
        vector_of_clauses.push_back(clause);
        clause.clear();
        problem_stream >> next_item_in_stream; //advance past "0"
    }
    //return address
    int nv = stoi(num_literals);

    numVars = nv;
    return vector_of_clauses;
}
