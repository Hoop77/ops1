#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>
#include "../../include/common/Vector.h"
#include "../../include/common/Utils.h"

typedef struct {
	int inPostings;
	int outPostings;
	int creditBalance;
} CollectorData;

typedef struct
{
	Vector taxCollectors;
	// mutexes...
} SimulationContext;

typedef struct TaxCollector_s
{
	SimulationContext * context;
	CollectorData data;
	struct TaxCollector_s * waitTaxCollector;
} TaxCollector;

static void StartSimulation(double duration, int collectors, int funds);

static void SimulationContext_Init(SimulationContext * self);

static void TaxCollector_Init(TaxCollector * self, SimulationContext * context, int initialCreditBalance);

static void * TaxCollector_Run(void * arg);

int main(int argc, const char* argv[])
{
	double duration = 2; // default duration in seconds
	int collectors = 5;  // default number of tax collectors
	int funds = 300;     // default funding per collector in Euro
	
	// allow overriding the defaults by the command line arguments
	switch (argc)
	{
	case 4:
		duration = atof(argv[3]);
		/* fall through */
	case 3:
		funds = atoi(argv[2]);
		/* fall through */
	case 2:
		collectors = atoi(argv[1]);
		/* fall through */
	case 1:
		printf(
			"Tax Collectors:  %d\n"
			"Initial funding: %d EUR\n"
			"Duration:        %g s\n",
			collectors, funds, duration
		);
		break;
		
	default:
		printf("Usage: %s [collectors [funds [duration]]]\n", argv[0]);
		return -1;
	}
	
	// TODO: implement the scenario
    //
    // TODO:
    // Creating as many threads as "collectors" available
    // Giving every collector the Thread-List to grant access for each other
    // Run the szenario and wait for "duration"
    // Join/cancel all threads, sum up thier results and print (?) them
    //

	StartSimulation(duration, collectors, funds);

    int totalInPostings     = 0;
    int totalOutPostings    = 0;
    int totalFunding        = 0;
	
	printf(
		"Total in postings:  %d\n"
		"Total out postings: %d EUR\n"
		"Total funding:      %d s\n",
		totalInPostings, totalOutPostings, totalFunding
	);
	return 0;
}

void StartSimulation(double duration, int collectors, int funds)
{
	Vector threads;
	Vector_Init(&threads, sizeof(pthread_t), NULL);

	Vector taxCollectors;
	Vector_Init(&taxCollectors, sizeof(TaxCollector), NULL);

	SimulationContext simulationContext;
	SimulationContext_Init(&simulationContext);

	for (int i = 0; i < collectors; ++i)
	{
		TaxCollector taxCollector;
		TaxCollector_Init(&taxCollector, &simulationContext, funds);
		Vector_Append(&simulationContext.taxCollectors, &taxCollector);
	}

	for (int i = 0; i < collectors; ++i)
	{
		pthread_t thread;
		if (pthread_create(&thread, NULL, TaxCollector_Run, &simulationContext) != 0)
			terminate();
		Vector_Append(&threads, &thread);
	}

	for (int i = 0; i < collectors; ++i)
	{
		pthread_t * thread = Vector_At(&threads, (size_t) i);
		void * ret;
		pthread_join(*thread, &ret);
	}

	Vector_Destroy(&threads);
}

void SimulationContext_Init(SimulationContext * self)
{
	Vector_Init(&self->taxCollectors, sizeof(TaxCollector), NULL);
	// TODO: initialize mutexes
}

void TaxCollector_Init(TaxCollector * self, SimulationContext * context, int initialCreditBalance)
{
	self->context = context;
	self->data.inPostings = 0;
	self->data.outPostings = 0;
	self->data.creditBalance = initialCreditBalance;
	self->waitTaxCollector = NULL;
}

void * TaxCollector_Run(void * arg)
{

}