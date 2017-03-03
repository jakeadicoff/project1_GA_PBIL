#include "GA.h"

#include <ctype.h>
#include <algorithm>
#include <iostream>
#include <random>
#include <cmath>
#include <time.h>

using namespace std;

GA::GA(int populationSize, string selectionType, string crossoverType, double crossoverProbability, double mutationProbability, int generationNumber, vector <vector <int> > allClauses, int numVariables) {
  this->population_size = populationSize;
  this->selection_type = selectionType;
  this->crossover_type = crossoverType;
  this->crossover_probability = crossoverProbability;
  this->mutation_probability = mutationProbability;
  this->generations = generationNumber;
  this->clauses = allClauses;
  this->num_clauses = clauses.size();
  this->num_variables = numVariables;
  this->population = generate_initial_population();
  this->best_generation = 0;
}

//add file name in main
//format print answer
//add best_individual to class. This throws abort trap 6. ew

void GA::runGA() {
  start_time = clock();
  individual best_overall_individual = population[0];
  individual best_after_update;
  for(int i = 0; i < generations; i++) {
    fitness();
    best_after_update = get_best();
    if(best_after_update.number_satisfied > best_overall_individual.number_satisfied) {
      best_overall_individual = best_after_update;
      best_generation = i+1;
    }
    if(selection_type == "ts" ) {
      tournament_selection();
    } else if(selection_type == "bs") {
      boltzmann_selection();
    } else if(selection_type == "rs") {
      rank_selection();
    }

    for(int j = 0; j < population_size-1; j++) { //size of breeding population
      individual ind;

      if(crossover_type == "uc") {
	ind = uniform_crossover(breeding_population[2*j],
				breeding_population[2*j+1]);
      }
      else if(crossover_type == "1c") {
	ind = one_point_crossover(breeding_population[2*j],
				  breeding_population[2*j+1]);
      }

      population[j] = ind;
    }//for pop
    mutation();
  }//for gen
  end_time = clock();
  extract_and_print_answer(best_overall_individual);
}

void GA::debug_print() {
  cout << endl << endl;
  fitness();
  for(int i = 0; i < population_size; i++) {
    for(int j = 0; j < num_variables+1; j++) {
      cout << population[i].assignments[j];
    }
    cout << "   " << population[i].number_satisfied << endl;
  }
}

void GA::extract_and_print_answer(individual best_individual) {
   fitness();//get final fitness update
   individual ind = get_best();
   if(ind.number_satisfied > best_individual.number_satisfied) {
     best_individual = ind;
     best_generation = generations;
   }
   cout << " Number of variables: " << num_variables << endl;
   cout << " Number of clauses: " << num_clauses << endl;
   cout << " Number of clauses satisfied: " << best_individual.number_satisfied << endl;
   cout << " Percentage of clauses satisfied: " << 100.0*best_individual.number_satisfied/(num_clauses*1.0)<< "%" << endl;
   cout << " Generation where best assignment was found: " << best_generation << endl;
   cout << " Time to solve: " << (end_time-start_time)/CLOCKS_PER_SEC << endl;
   cout << endl;

}

individual GA::get_best() {
  int index_of_fittest = 0;
  for(int i = 0; i < population_size; i++) {
    if(population[index_of_fittest].number_satisfied < population[i].number_satisfied) index_of_fittest = i;
  }
  return population[index_of_fittest];
}

vector <individual> GA::generate_initial_population() {
  vector <individual> population; // to return
  srand(time(NULL));
  for(int i = 0; i < population_size; i++) {
    vector <bool> assignment;
    for(int j = 0; j < num_variables+1; j++) { // first bit is trash for easy indexing
      int random;
      random = rand() % 2;
      if(random == 0) assignment.push_back(false);
      if(random == 1) assignment.push_back(true);
    }
    individual ind;
    ind.number_satisfied = -1;
    ind.assignments = assignment;
    population.push_back(ind);
  }
  return population;
}

void GA::fitness() {
  // for every individual in the population
  for(int i = 0; i < population_size; i++) {
    population[i].number_satisfied = 0;
    // for every clause
    for(int j = 0; j < num_clauses; j++) {
      int satisfied_literals = 0;
      // for every literal in the clause
      for(int k = 0; k < clauses[j].size(); k++) {
	// if clause unsatisfied, break
	if((clauses[j][k] > 0 && population[i].assignments[abs(clauses[j][k])]) ||
	   (clauses[j][k] < 0 && !population[i].assignments[abs(clauses[j][k])])) {
	  satisfied_literals++;

	}
      }//for literals
      // MAY BE BAD
      //population[i].number_satisfied += clause_satisfied;
      if(satisfied_literals > 0) {
	population[i].number_satisfied +=1;
      }//if sat
    }//for num_clause
  }//for pop
}//func


void GA::boltzmann_selection() {
  breeding_population.clear();
  vector<long double> boltzmann_weights;  //declare size equal to popultion size
  long double boltzmann_sum = 0;
  srand(time(NULL));

  for (int i = 0; i < population_size; ++i) {
    long double k = expl(population[i].number_satisfied);
    boltzmann_weights.push_back(k);
    boltzmann_sum += k;
  }
  for(int i = 0; i < population_size * 2; ++i) {
    long double random_weight;
    random_weight = boltzmann_sum*double(rand())/RAND_MAX;//CHECK THIS, NEED TO BE IN RANGE 0 TO B_SUM
    int boltzmann_index = 0;
    //cout << random_weight << endl;
    while(random_weight > 0) {
      random_weight -= boltzmann_weights[boltzmann_index];
      boltzmann_index++;

    }
    boltzmann_index--;  //because we went one past
    breeding_population.push_back(population[boltzmann_index]);
  }
}


void GA::tournament_selection() {
  breeding_population.clear();
  srand(time(NULL));
  int first_random_index;
  int second_random_index;

  for(int i = 0; i < population_size*2; ++i) {

    // get two random indices
    first_random_index = rand() % (population_size-1);
    second_random_index = rand() % (population_size-1);
    if(population[first_random_index].number_satisfied > population[second_random_index].number_satisfied) {
      breeding_population.push_back(population[first_random_index]);
    }
    else {
      breeding_population.push_back(population[second_random_index]);
    }
  }
}


// comparison overload fucntion for individuals
bool compare_individual_satisfication(individual a, individual b){
    return a.number_satisfied < b.number_satisfied;
}


void GA::rank_selection() {
  //cout << "in rs " << endl;
  breeding_population.clear();
  srand(time(NULL));
  // sorts the population in ascending order
  sort(population.begin(), population.end(), compare_individual_satisfication);
  int gaussian_sum = population_size * (population_size + 1) / 2;

  // weight probabilities by index from least
  for(int i = 0; i < population_size * 2; ++i) {
    int random_number;
    random_number = 1+rand() % (gaussian_sum); //CHECK THE +1 OH GOD
    // GOOD AREA FOR BUGS
    int index = ceil((-1 + sqrt(1 + 8 * random_number)) / 2);
    breeding_population.push_back(population[index-1]);
  }
}

void GA::mutation() {
  srand(time(NULL));
  for(int i = 0; i < population_size; i++) { //all individuals
    for(int j = 0; j < num_variables+1; j++) { //first bit is trash
      double random; // = distribution(generator);
      random = double(rand())/RAND_MAX;
      if(random <= mutation_probability) { // if rand is less than prob[0,1]
	population[i].assignments[j] = !population[i].assignments[j]; // flip bit
      }
    }
  }
}

individual GA::one_point_crossover(individual parent_a, individual parent_b) {
  srand(time(NULL));
  // for crossover probability
  double random_number;
  random_number = double(rand()) / RAND_MAX;

  // initialize new individual
  individual new_individual;
  new_individual.number_satisfied = -1;

  // crossover
  if(random_number < crossover_probability) {
    int random_index;
    random_index = rand() % num_variables;
    for(int i = 0; i < random_index; ++i) {
      new_individual.assignments.push_back(parent_a.assignments[i]);
    }
    for(int j = random_index; j < num_variables; ++j) {
      new_individual.assignments.push_back(parent_b.assignments[j]);
    }
  } else { // don't crossover
    // MAY PASS BY REFERENCE?
    new_individual.assignments = parent_a.assignments;
  }

  return new_individual;
}

individual GA::uniform_crossover(individual parent_a, individual parent_b) {
  //cout << "in uc " << endl;
  srand(time(NULL));
  // initialize new individual
  individual new_individual;
  new_individual.number_satisfied = -1;
  double random1;
  random1 = double(rand())/RAND_MAX;
  // crossover
  if(random1< crossover_probability) {
    for(int i = 0; i < parent_a.assignments.size(); ++i) {// for parent selection
      // pick parent
      int random2;
      random2 = rand() % 2;
      if(random2 == 0) {
	new_individual.assignments.push_back(parent_a.assignments[i]);
      } else {
	new_individual.assignments.push_back(parent_b.assignments[i]);
      }
    }
  } else { // don't crossover
    new_individual.assignments = parent_a.assignments;
  }
  return new_individual;
}
