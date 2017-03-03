#include "PBIL.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

int BIG_INT = 0x0FFFFFFF;
individual global_best;
int generation_found = 0;

double get_random_percent() {
  return rand()/((double) RAND_MAX);
}

//@TODO: Rename generationNumber to numIterations in algs
PBIL::PBIL(int populationSize, double positiveLearningRate,
	   double negativeLearningRate, double mutationProbability,
	   double mutationShift, int numIterations,
	   vector< vector <int> > allClauses, int numVariables) {
  this->population_size = populationSize;
  this->positive_learning_rate = positiveLearningRate;
  this->negative_learning_rate = negativeLearningRate;
  this->mutation_probability = mutationProbability;
  this->mutation_shift = mutationShift;
  this->generations = numIterations;
  this->clauses = allClauses;
  this->num_clauses = clauses.size();
  this->num_variables = numVariables;
  this->start_time = clock();
  this->end_time = clock();

  global_best.number_satisfied = 0;
  // seed random number generator with time
  srand(time(0));

  //initialize the vector w/ all 0.5 probability
  generate_probability_vector();
}


// returns fitness of an individual
int PBIL::fitness(individual indiv) {
  int numsat = 0;

  // for every clause
  for(int i = 0; i < num_clauses; i++) {
    // for every literal in the clause
    for(int j = 0; j < clauses[i].size(); j++) {

      //if a SINGLE literal is satisfied, break and increment numsat
      if( (clauses[i][j] > 0 && indiv.assignments[abs(clauses[i][j])]) ||
      	  (clauses[i][j] < 0 && !indiv.assignments[abs(clauses[i][j])]) ) {
	numsat++;
      	break;
      }
    }
  }

  return numsat;
}

// generate probability vector full of 0.5
// note that probability vector is num_literals PLUS ONE to allow for
// one indexing. First value is a dummy value
void PBIL::generate_probability_vector() {
  for(int i = 0; i <= num_variables; ++i) {
    probability_vector.push_back(0.5);
  }
}

// generate samples, save the best and the worst
void PBIL::generate_samples(int gen) {
  //reset best and worst individuals
  best_individual.number_satisfied = -1;
  worst_individual.number_satisfied = BIG_INT;

  //iterate over representative population
  for(int i = 0; i < population_size; i++) {

    individual sample;
    // dummy value, we want to 1 index
    sample.assignments.push_back(-1);

    // make assignments
    for(int j = 1; j <= num_variables; j++) {

      if(get_random_percent() < probability_vector[j]) {
	sample.assignments.push_back(1);
      }
      else {
	sample.assignments.push_back(0);
      }
    }

    sample.number_satisfied = fitness(sample);

    // check if this new sample is the best or the worst
    if(sample.number_satisfied > best_individual.number_satisfied) {
      best_individual = sample;
      if(sample.number_satisfied > global_best.number_satisfied) {
	global_best = sample;
	generation_found = gen;
      }
    }
    else if(sample.number_satisfied < worst_individual.number_satisfied) {
      worst_individual = sample;
    }
  }
}

void PBIL::update_probability_vector() {
  // positive
  for(int i = 1; i <= num_variables; i++) {
    probability_vector[i] = probability_vector[i] * (1.0 - positive_learning_rate) +
      (double)best_individual.assignments[i] * positive_learning_rate;

  }

  //negative
  for(int i = 1; i <= num_variables; i++) {
    if(best_individual.assignments[i] != worst_individual.assignments[i]) {
      probability_vector[i] = probability_vector[i] * (1.0 - negative_learning_rate) +
	(double)best_individual.assignments[i] * negative_learning_rate;
    }
  }
}

void PBIL::mutate_probability_vector() {
  // for every index in probability vector
  for(int i = 1; i <= num_variables; ++i) {

    // default_random_engine generator;
    // uniform_real_distribution<double> real_distribution(0.0, 1.0);

    // with mutation probability, mutate
    if(get_random_percent() < mutation_probability) {

      int mutate_direction;

      // randomize mutation direction
      if(get_random_percent() > 0.5) {
	mutate_direction = 1;
      }
      else {
	mutate_direction = 0;
      }

      // mutate
      probability_vector[i] = probability_vector[i] * (1.0 - mutation_shift) +
	mutate_direction * mutation_shift;
    }
  }
}

individual PBIL::round_probability_vector() {
  individual rounded;
  rounded.assignments.push_back(-1);

  for(int i = 1; i <= num_variables; i++) {
    if(probability_vector[i] >= 0.5) rounded.assignments.push_back(1);
    else rounded.assignments.push_back(0);
  }

  return rounded;
}

void PBIL::runPBIL(){
  start_time = clock();
  for(int i = 0; i < generations; i++) {
    generate_samples(i);
    update_probability_vector();
    mutate_probability_vector();
  }
  end_time = clock();

  // results printout
  int best_fitness = global_best.number_satisfied;
  cout << " Number of variables: " << num_variables << endl;
  cout << " Number of clauses: " << num_clauses << endl;
  cout << " Number of clauses satisfied: " << best_fitness << endl;
  cout << " Percentage of clauses satisfied: " << 100.0*best_fitness/(1.0*num_clauses) << endl;
  cout << " Generation where best assignment was found: " << generation_found <<  endl;
  cout << " Time to solve: " << (end_time-start_time)/CLOCKS_PER_SEC << endl;
  cout << endl;
  //dont think we need this
    //cout << "Final Probability Vector values" << ": ";
  //for(int i = 1; i <= num_variables; i++) {
  //cout.precision(2);
  //cout << probability_vector[i]<< " ";
  //}
  //cout << endl;
}
