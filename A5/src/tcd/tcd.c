#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>
#include "../../include/common/Vector.h"
#include "../../include/common/Utils.h"

#define MIN_CREDIT_BALANCE 100

typedef struct {
	int inPostings;
	int outPostings;
	int creditBalance;
} CollectorData;

typedef struct
{
	Vector taxCollectors;
	pthread_mutex_t lock;
} SimulationContext;

typedef struct TaxCollector_s
{
	SimulationContext * context;
	CollectorData data;
	unsigned int seed;
	struct TaxCollector_s * waitTaxCollector;
	pthread_cond_t minCreditBalanceCondition;
} TaxCollector;

static void StartSimulation(double duration, int collectors, int funds);

static void SimulationContext_Init(SimulationContext * self);

static void TaxCollector_Init(TaxCollector * self, SimulationContext * context, int initialCreditBalance);

static void * TaxCollector_Run(void * arg);

static bool CheckDependencies(TaxCollector * from, TaxCollector * to);

static unsigned int GenerateSeed();

static unsigned long Random(unsigned int * seed, unsigned long max);

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

static void StartSimulation(double duration, int collectors, int funds)
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

	pthread_mutex_lock(&simulationContext.lock);
	TaxCollector * taxCollector;
	Vector_ForeachBegin(&taxCollectors, taxCollector, i)
		pthread_t thread;
		if (pthread_create(&thread, NULL, TaxCollector_Run, taxCollector) != 0)
			terminate();
		Vector_Append(&threads, &thread);
	Vector_ForeachEnd
	pthread_mutex_unlock(&simulationContext.lock);

	usleep((useconds_t) (duration * 1000000.0));

	pthread_t * thread;
	Vector_ForeachBegin(&threads, thread, j)
		void * ret;
		pthread_cancel(*thread);
		pthread_join(*thread, &ret);
    Vector_ForeachEnd

	Vector_Destroy(&taxCollectors);
	Vector_Destroy(&threads);
}

static void SimulationContext_Init(SimulationContext * self)
{
	Vector_Init(&self->taxCollectors, sizeof(TaxCollector), NULL);
	pthread_mutex_init(&self->lock, NULL);
}

static void TaxCollector_Init(TaxCollector * self, SimulationContext * context, int initialCreditBalance)
{
	self->context = context;
	self->data.inPostings = 0;
	self->data.outPostings = 0;
	self->data.creditBalance = initialCreditBalance;
	self->waitTaxCollector = NULL;
	pthread_cond_init(&self->minCreditBalanceCondition, NULL);
}

static void * TaxCollector_Run(void * arg)
{
	TaxCollector * me = arg;
	Vector * collectors = &me->context->taxCollectors;
	SimulationContext * context = me->context;
	int oldState;   // for pthread_setcancelstate

	me->seed = GenerateSeed();

	for (;;)
	{
		size_t randomIndex = Random(&me->seed, (unsigned long) Vector_Size(collectors));
		TaxCollector * otherCollector = Vector_At(collectors, randomIndex);
		// retry when unlucky
		if (otherCollector == me)
			continue;

		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldState);
		pthread_mutex_lock(&context->lock);
		bool result = CheckDependencies(me, otherCollector);
		if (result)
		{
			if (otherCollector->data.creditBalance < MIN_CREDIT_BALANCE)
				pthread_cond_wait(&otherCollector->minCreditBalanceCondition, &context->lock);

			int transferSize = otherCollector->data.creditBalance / 2;
			otherCollector->data.creditBalance -= transferSize;
			otherCollector->data.outPostings += transferSize;

			me->data.creditBalance += transferSize;
			me->data.inPostings += transferSize;

			if (me->data.creditBalance >= MIN_CREDIT_BALANCE)
				pthread_cond_signal(&me->minCreditBalanceCondition);
		}
		pthread_mutex_unlock(&context->lock);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldState);
	}
}

static unsigned int GenerateSeed()
{
	return (unsigned int) time(NULL) ^ getpid() ^ (unsigned int) pthread_self();
}

static bool CheckDependencies(TaxCollector * from, TaxCollector * to)
{
	if (to->waitTaxCollector == NULL)
		return true;
	else if (to->waitTaxCollector == from)
		return false;
	else
		return CheckDependencies(from, to->waitTaxCollector);
}

static unsigned long Random(unsigned int * seed, unsigned long max)
{
	return (unsigned long) (rand_r(seed) / (RAND_MAX + 1.0) * max);
}