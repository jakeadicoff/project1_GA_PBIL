#ifndef __PBIL_h
#define __PBIL_h

#include <iostream>
#include <vector>
#include "Individual.h"
#include <random>

using namespace std;

class PBIL{
 public:
  PBIL(int populationSize, double positiveLearningRate,
       double negativeLearningRate, double mutationProbability,
       double mutationShift, int numGenerations,
       vector< vector <int> > allClauses, int numVariables);
  void runPBIL();

 private:
  vector<double> probability_vector;
  vector <vector <int> > clauses;
  int population_size, generations, num_clauses, num_variables;
  double positive_learning_rate, negative_learning_rate,
    mutation_probability, mutation_shift, start_time, end_time;
  individual best_individual, worst_individual;
  int fitness(individual indiv);
  void generate_probability_vector();
  void generate_samples(int gen);
  void update_probability_vector();
  void mutate_probability_vector();
  individual round_probability_vector();
};

#endif
