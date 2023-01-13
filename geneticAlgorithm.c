#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define POPULATION 200
#define PHRASE_LEN 19


struct DNA {
    char * genes;
    float  fitness;
};

struct population {
    struct DNA ** mating_pool;
    struct DNA ** population;
    char        * target;
    float         mutation_rate;
    int           max_population;
    int           generations;
    bool          finished;
    int           mating_pool_len;
    struct DNA *  best;
};

char 
get_new_char()
{
    int new_char_val = (rand() % 63) + 59;

    return (char)new_char_val;
}


float 
calc_indiv_fitness(char * target, char * genes)
{

    float ret_val = -1;

    if ((NULL == target) || (NULL == genes))
    {
        goto EXIT;
    }

    int score = 0;

    for (int i = 0; i < PHRASE_LEN; i++)
    {
        if (target[i] == genes[i])
        {
            score++;
        }
    }

    ret_val = (float)score / PHRASE_LEN;

    EXIT:
        return ret_val;
}

struct DNA *
crossover(struct DNA * parent_one, struct DNA * parent_two) 
{
    struct DNA * p_new_dna = NULL;

    if ((NULL == parent_one) || (NULL == parent_two))
    {
        goto EXIT;
    }

    p_new_dna = malloc(sizeof(struct DNA));

    if (NULL == p_new_dna)
    {
        goto EXIT;
    }

    p_new_dna->genes = malloc(sizeof(char)*PHRASE_LEN);

    if (NULL == p_new_dna->genes)
    {
        free(p_new_dna);
        p_new_dna = NULL;
        goto EXIT;
    }
    
    int mid_point = (rand() % PHRASE_LEN);

    for (int i = 0; i < PHRASE_LEN; i++)
    {
        if (i < mid_point)
        {
            p_new_dna->genes[i] = parent_one->genes[i];
        }
        else
        {
            p_new_dna->genes[i] = parent_two->genes[i];
        }
    }

    EXIT:
        return p_new_dna;
}

void
calc_fitness(struct population * p_pop, char * target)
{
    
    if (NULL == p_pop)
    {
        goto EXIT;
    }

    //Update fitness values for the population
    for (int count = 0; count < p_pop->max_population; count++)
    {
        char * genes = p_pop->population[count]->genes;
        calc_indiv_fitness(target, genes);
    }
    

    EXIT:
        ;
}

struct population * 
create_new_population(char * target, float mutation_rate, int max_population) {

    struct population * p_new_pop = NULL;
    
    if (NULL == target)
    {
        goto EXIT;
    }

    p_new_pop = malloc(sizeof(struct population));

    if (NULL == p_new_pop)
    {
        goto EXIT;
    }

    p_new_pop->target          = target;
    p_new_pop->mutation_rate   = mutation_rate;
    p_new_pop->max_population  = max_population;
    p_new_pop->generations     = 0;
    p_new_pop->finished        = false;
    p_new_pop->mating_pool_len = 0;
    p_new_pop->best            = NULL;
    p_new_pop->mating_pool     = NULL;
    

    for (int count = 0; count < max_population; count++)
    {
        struct DNA * p_new_dna = malloc(sizeof(struct DNA));

        if (NULL == p_new_dna)
        {
            free(p_new_pop);
            p_new_pop = NULL;
            goto EXIT;
        }

        p_new_dna->genes = malloc(sizeof(char)*PHRASE_LEN);

        //NULL CHECK

        for (int char_num = 0; char_num < PHRASE_LEN; char_num++)
        {
            p_new_dna->genes[char_num] = get_new_char();
        }

        p_new_pop->population[count] = p_new_dna;

    }

    calc_fitness(p_new_pop, target);

    EXIT:
        return p_new_pop;
}

void
clear_mating_pool(struct population * p_pop)
{
    if (NULL == p_pop)
    {
        goto EXIT;
    }

    if (NULL == p_pop->mating_pool)
    {
        goto EXIT;
    }

    for (int count = 0; count < p_pop->mating_pool_len; count++)
    {
        free(p_pop->mating_pool[count]);
    }

    free(p_pop->mating_pool);

    p_pop->mating_pool     = NULL;
    p_pop->mating_pool_len = 0;

    EXIT:
        ;
}

void
push_dna(struct population * p_pop, struct DNA * current)
{
    struct DNA ** mating_pool = p_pop->mating_pool;
    int mating_pool_len = p_pop->mating_pool_len;

    if (NULL == current)
    {
        goto EXIT;
    }

    *mating_pool = realloc(*mating_pool, sizeof(struct DNA)*(mating_pool_len+1));

    mating_pool[mating_pool_len] = current;

    p_pop->mating_pool_len += 1;

    EXIT:
        ;
}


void 
generate(struct population * p_pop)
{

    if (NULL == p_pop)
    {
        goto EXIT;
    }

    for (int count = 0; count < p_pop->max_population; count++)
    {
        int a_parent = (rand() % p_pop->mating_pool_len);
        int b_parent = (rand() % p_pop->mating_pool_len);

        struct DNA * parent_one = p_pop->mating_pool[a_parent];
        struct DNA * parent_two = p_pop->mating_pool[b_parent];

        struct DNA * child = crossover(parent_one, parent_two);

        //MUTATE

        //FREE CURRENT DNA


        p_pop->population[count] = child;
    }

    p_pop->generations++;

    EXIT:
        ;
    
}

void
evaluate(struct population * p_pop, char * target)
{
    
    if ((NULL == p_pop) || (NULL == target))
    {
        goto EXIT;
    }

    float best_fitness = 0;
    int best_index     = 0;

    for (int count = 0; count < p_pop->max_population; count++)
    {
        float current_fitness = p_pop->population[count]->fitness;

        if (current_fitness > best_fitness)
        {
            best_fitness = current_fitness;
            best_index   = count;
        }
    }

    p_pop->best = p_pop->population[best_index];

    if (0 == strncmp(p_pop->best->genes, target, PHRASE_LEN))
    {
        p_pop->finished = true;
    }

    EXIT:
        ;
}


void
natural_selection(struct population * p_pop, char * target)
{
    
    if (NULL == p_pop)
    {
        goto EXIT;
    }

    //Clear the mating pool
    if (NULL != p_pop->mating_pool)
    {
        clear_mating_pool(p_pop);
    }
    

    //Get the max fitness val in the population
    float max_fitness = 0;

    for (int count = 0; count < p_pop->max_population; count++)
    {
        struct DNA * current = p_pop->population[count];

        if (current->fitness > max_fitness)
        {
            max_fitness = current->fitness;
        }
    }


    //Based on fitness, add members of the population to the
    //mating pool a certain number of times

    for (int count = 0; count < p_pop->max_population; count++)
    {
        struct DNA * current = p_pop->population[count];
        float fitness = current->fitness;

        float add_val = floor((fitness/max_fitness)*100);

        for (int add_count = 0; add_count < add_val; add_count++)
        {
            push_dna(p_pop, current);
        }
    }



    EXIT:
        ;
}

int 
main(int argc, char *argv[]) {

    srand(time(NULL));

    //Initialize population

    char * target = "To be or not to be";

    //Create new population
    //Fill the population with new DNA and calculate initial fitness
    struct population * p_pop = create_new_population(target, 0.01, 200);

    printf("Population created");


    while (true)
    {
        //Natural selection
        //Clear the mating pool
        //Get the max fitness val in the population
        //Based on fitness, add members of the population to the
        //mating pool a certain number of times
        natural_selection(p_pop, target);

        printf("Natural selection done");

        printf("Length of mating pool: %d", p_pop->mating_pool_len);

        //Generate
        //Refill the population with children from the mating pool
        //using crossover
        //Increment generations
        generate(p_pop);

        printf("Generation done");

        //Calc fitness
        //Update the fitness of each element in the population
        calc_fitness(p_pop, target);

        printf("Fitness calculated");

        //Evaluate
        //Compute the current "most fit" member of the population
        //Set finished if the most fit member's genes equal the target
        evaluate(p_pop, target);

        printf("Best fitness from current generation: %s", p_pop->best->genes);

        break;

        if (p_pop->finished)
        {
            break;
        }
    }

    printf("Finished after %d generations", p_pop->generations);
    
    
}