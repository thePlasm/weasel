/*
 * A weasel program by Rasmus Jepsen 2016
 * Based off of Richard Dawkins' thought experiment.
 * This is a very simple genetic algorithm with only the mutation and selection operators.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#define TARGET_STRING "METHINKS IT IS LIKE A WEASEL"    //the string that the candidates will evolve towards
#define MUTATION_RATE 0.05  //the probability for each character to be swapped with a random one each generation
#define POPULATION_SIZE 100 //the number of candidates in the population array every generation
#define ERR 1   //the error code returned by functions
#define OK 0    //the ok status code returned by functions

size_t sizeLog2(size_t num) {   //a utility function for finding the second logarithm of a size_t type
    size_t result = 0;
    while (num > 1) {
        num /= 2;
        result++;
    }
    return result;
}

int allocPopulation(char ***population) {   //a function to allocate memory for the population array
    *population = malloc(POPULATION_SIZE * sizeof(char *));
    if (*population == NULL) return ERR;
    int i;
    for (i = 0; i < POPULATION_SIZE; i++) {
        (*population)[i] = malloc(sizeof(char) * (strlen(TARGET_STRING) + 1));
        if ((*population)[i] == NULL) return ERR;
    }
    return OK;
}

char genRandomChar() {  //a function to generate a random char for the creation of random candidates and for candidate mutation
    return (char) (rand() >> (sizeLog2(RAND_MAX + 1) - CHAR_BIT));
}

char *genRandomCandidate() {    //a function to create the initial random candidate
    char *randomCandidate = malloc(sizeof(char) * (strlen(TARGET_STRING) + 1));
    if (randomCandidate == NULL) return NULL;
    size_t i;
    for (i = 0; i < strlen(TARGET_STRING); i++) {
        randomCandidate[i] = genRandomChar();
    }
    randomCandidate[i] = '\0';
    return randomCandidate;
}

void freePopulation(char ***population) {   //a function to free the memory allocated for the population array
    int i;
    for (i = 0; i < POPULATION_SIZE; i++) {
        free((*population)[i]);
    }
    free(*population);
}

void reproduce(char **population, char *candidate) {    //a function to fill the population with a candidate's offspring
    int i;
    for (i = 0; i < POPULATION_SIZE; i++) {
        memcpy(population[i], candidate, strlen(TARGET_STRING) + 1);
    }
}

void mutatePopulation(char **population) {  //a function to mutate the candidates in the population
    int i;
    for (i = 0; i < POPULATION_SIZE; i++) {
        size_t j;
        for (j = 0; j < strlen(TARGET_STRING); j++) {
            if (((double) rand()) < ((double) RAND_MAX) * MUTATION_RATE)
                population[i][j] = genRandomChar();
        }
    }
}

void calculateFitnesses(size_t *fitnesses, char **population) { //a function to calculate the fitnesses of all of the candidates in the population
    int i;
    for (i = 0; i < POPULATION_SIZE; i++) {
        fitnesses[i] = 0;
        size_t j;
        for (j = 0; j < strlen(TARGET_STRING); j++) {
            fitnesses[i] += population[i][j] == TARGET_STRING[j] ? 1 : 0;
        }
    }
}

size_t maxSizes(size_t *array, size_t length) { //a utility function to find the index of the maximum value in a size_t array
    size_t i;
    size_t index = 0;
    size_t max = array[0];
    for (i = 1; i < length; i++) {
        if (array[i] > max) {
            index = i;
            max = array[i];
        }
    }
    return index;
}

int main(int argc, char **argv) {
    char **population;  //the population array - stores all of the candidates as strings
    if (allocPopulation(&population) == ERR) {  //allocate memory to population and check for errors
        fprintf(stderr, "Something went wrong in the allocation of the population!\n");
        return ERR;
    }
    time_t randSeed = time(NULL);   //get random number seed from time
    srand(randSeed);    //seed pseudo-random number generator
    char *fittestCandidate = genRandomCandidate();  //initialise the first candidate as a random candidate
    if (fittestCandidate == NULL) { //check for allocation errors
        fprintf(stderr, "Something went wrong in the allocation of the initial candidate!\n");
        return ERR;
    }
    printf("Starting weasel program now with the following parameters:\n"); //output parameters for verbosity
    printf("Population size: %d\n", POPULATION_SIZE);
    printf("Mutation rate: %f\n", MUTATION_RATE);
    printf("Random seed: %d\n", randSeed);
    printf("Target string: %s\n", TARGET_STRING);
    reproduce(population, fittestCandidate);    //initialise the population with fittestCandidate
    int generation = 0; //initialise generation counter
    size_t fittestIndex = 0;    //stores the index of fittestCandidate in the population (currently all of the population)
    size_t *fitnesses = malloc(POPULATION_SIZE * sizeof(size_t));   //stores the fitnesses of the candidates
    calculateFitnesses(fitnesses, population);  //calculates the fitnesses of all of the candidates initially
    printf("Generation %d:\n\tFittest candidate: %s\n\tFitness: %d\n", generation, fittestCandidate, fitnesses[fittestIndex]);
    while (fitnesses[fittestIndex] < strlen(TARGET_STRING)) {   //check if the fittest fitness is acceptable
        generation++;   //increment generation counter
        mutatePopulation(population);   //mutate the population
        calculateFitnesses(fitnesses, population);  //calculate the fitnesses
        fittestIndex = maxSizes(fitnesses, POPULATION_SIZE);    //find the fittest candidate
        fittestCandidate = population[fittestIndex];    //get the fittest candidate
        reproduce(population, fittestCandidate);    //fill the population with the fittest candidate's offspring
        printf("Generation %d:\n\tFittest candidate: %s\n\tFitness: %d\n", generation, fittestCandidate, fitnesses[fittestIndex]);
    }
    //free everything
    free(fitnesses);
    free(fittestCandidate);
    freePopulation(&population);
    return OK;
}
