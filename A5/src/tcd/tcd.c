#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include "../../include/common/Vector.h"
#include "../../include/common/Utils.h"

#define MIN_CREDIT_BALANCE 100

typedef struct
{
	int inPostings;
	int outPostings;
	int creditBalance;
} CollectorData;

typedef struct
{
	Vector taxCollectors;
	pthread_mutex_t lock;
	bool running;
} SimulationContext;

typedef struct TaxCollector_s
{
	SimulationContext * context;
	CollectorData data;
	unsigned int seed;
	struct TaxCollector_s * other;
	// for debugging / TODO: remove this!
	int id;
	// for finding cycles;
	bool visited;
} TaxCollector;

static void StartSimulation(double duration, int collectors, int funds);

static void SimulationContext_Init(SimulationContext * self);

static void SimulationContext_Destroy(SimulationContext * self);

static void TaxCollector_Init(TaxCollector * self, SimulationContext * context, int initialCreditBalance);

static void TaxCollector_Destroy(TaxCollector * self);

static void TaxCollectorItemDestroyer(VectorItem item);

static void * TaxCollector_Run(void * arg);

static void * DependencyChecker_Run(void * arg);

static unsigned int GenerateSeed();

static unsigned long Random(unsigned int * seed, unsigned long max);

static void printResults(Vector * taxCollectors);

static void printCurrState(SimulationContext * context);

static void PrepareCycleDetection(Vector * taxCollectors);

static TaxCollector * DetectCycle(TaxCollector * collector);

static TaxCollector * FindSuitableTaxCollector(SimulationContext * context, TaxCollector * self);

int main(int argc, const char * argv[])
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

	StartSimulation(duration, collectors, funds);

	return 0;
}

static void StartSimulation(double duration, int collectors, int funds)
{
	Vector threads;
	Vector_Init(&threads, sizeof(pthread_t), NULL);

	SimulationContext simulationContext;
	SimulationContext_Init(&simulationContext);

	for (int i = 0; i < collectors; ++i)
	{
		TaxCollector taxCollector;
		TaxCollector_Init(&taxCollector, &simulationContext, funds);
		// TODO: remove this!
		taxCollector.id = i;
		Vector_Append(&simulationContext.taxCollectors, &taxCollector);
	}

	pthread_mutex_lock(&simulationContext.lock);
	TaxCollector * taxCollector;
	Vector_ForeachBegin(&simulationContext.taxCollectors, taxCollector, i)
		pthread_t thread;
		if (pthread_create(&thread, NULL, TaxCollector_Run, taxCollector) != 0)
			terminate();
		Vector_Append(&threads, &thread);
	Vector_ForeachEnd
	pthread_mutex_unlock(&simulationContext.lock);

	pthread_t dependencyCheckerThread;
	if (pthread_create(&dependencyCheckerThread, NULL, DependencyChecker_Run, &simulationContext) != 0)
		terminate();
	Vector_Append(&threads, &dependencyCheckerThread);

	usleep((useconds_t) (duration * 1000000.0));

	printf("\nStopping simulation!\n");

	simulationContext.running = false;

	pthread_t * thread;
	Vector_ForeachBegin(&threads, thread, j)
		void * ret;
		pthread_join(*thread, &ret);
	Vector_ForeachEnd

	printResults(&simulationContext.taxCollectors);

	SimulationContext_Destroy(&simulationContext);
	Vector_Destroy(&threads);
}

static void SimulationContext_Init(SimulationContext * self)
{
	Vector_Init(&self->taxCollectors, sizeof(TaxCollector), TaxCollectorItemDestroyer);
	pthread_mutex_init(&self->lock, NULL);
	self->running = true;
}

static void SimulationContext_Destroy(SimulationContext * self)
{
	Vector_Destroy(&self->taxCollectors);
	pthread_mutex_destroy(&self->lock);
}

static void TaxCollector_Init(TaxCollector * self, SimulationContext * context, int initialCreditBalance)
{
	self->context = context;
	self->data.inPostings = 0;
	self->data.outPostings = 0;
	self->data.creditBalance = initialCreditBalance;
	self->other = NULL;
	self->visited = false;
}

static void TaxCollector_Destroy(TaxCollector * self)
{

}

static void TaxCollectorItemDestroyer(VectorItem item)
{
	TaxCollector_Destroy((TaxCollector *) item);
}

static void * TaxCollector_Run(void * arg)
{
	TaxCollector * me = arg;
	Vector * collectors = &me->context->taxCollectors;
	SimulationContext * context = me->context;

	me->seed = GenerateSeed();

	while (context->running)
	{
		size_t randomIndex = Random(&me->seed, (unsigned long) Vector_Size(collectors));
		TaxCollector * other = Vector_At(collectors, randomIndex);
		// retry when unlucky
		if (other == me)
			continue;

		pthread_mutex_lock(&context->lock);
		me->other = other;
		pthread_mutex_unlock(&context->lock);

		while (context->running)
		{
			pthread_mutex_lock(&context->lock);
			if (me->other->data.creditBalance >= MIN_CREDIT_BALANCE)
			{
				int transferSize = me->other->data.creditBalance / 2;
				me->other->data.creditBalance -= transferSize;
				me->other->data.outPostings += transferSize;

				me->data.creditBalance += transferSize;
				me->data.inPostings += transferSize;

				me->other = NULL;

				pthread_mutex_unlock(&context->lock);
				break;
			};
			pthread_mutex_unlock(&context->lock);
		}
	}

	return NULL;
}

static void * DependencyChecker_Run(void * arg)
{
	SimulationContext * context = arg;

	while (context->running)
	{
		pthread_mutex_lock(&context->lock);

		TaxCollector * taxCollector = Vector_At(&context->taxCollectors, 0);
		PrepareCycleDetection(&context->taxCollectors);
		if ((taxCollector = DetectCycle(taxCollector)) != NULL)
		{
			printf("Cycle detected!\n");
			printCurrState(context);
			TaxCollector * suitableCollector = FindSuitableTaxCollector(context, taxCollector);
			printf("%d is redirected to %d\n\n", taxCollector->id, suitableCollector->id);
			taxCollector->other = suitableCollector;
		}

		pthread_mutex_unlock(&context->lock);
	}

	return NULL;
}

static unsigned int GenerateSeed()
{
	return (unsigned int) time(NULL) ^ getpid() ^ (unsigned int) pthread_self();
}

static unsigned long Random(unsigned int * seed, unsigned long max)
{
	return (unsigned long) (rand_r(seed) / (RAND_MAX + 1.0) * max);
}

static void printResults(Vector * taxCollectors)
{
	int totalInPostings = 0;
	int totalOutPostings = 0;
	int totalFunding = 0;

	TaxCollector * taxCollector;
	Vector_ForeachBegin(taxCollectors, taxCollector, i)
		totalInPostings += taxCollector->data.inPostings;
		totalOutPostings += taxCollector->data.outPostings;
		totalFunding += taxCollector->data.creditBalance;
    Vector_ForeachEnd

	printf("Total in postings:  %d\n"
		       "Total out postings: %d EUR\n"
		       "Total funding:      %d s\n",
	       totalInPostings, totalOutPostings, totalFunding
	);
}

static void printCurrState(SimulationContext * context)
{
	int total = 0;
	TaxCollector * taxCollector;
	Vector_ForeachBegin(&context->taxCollectors, taxCollector, i)
		printf("%d:\n\tcredit balance = %d\n", taxCollector->id, taxCollector->data.creditBalance);
		if (taxCollector->other)
			printf("\tother = %d", taxCollector->other->id);
		else
			printf("\tother = NULL");
		printf("\n");
		total += taxCollector->data.creditBalance;
	Vector_ForeachEnd
	printf("total credit balance: %d\n", total);
}

static void PrepareCycleDetection(Vector * taxCollectors)
{
	TaxCollector * taxCollector;
	Vector_ForeachBegin(taxCollectors, taxCollector, i)
		taxCollector->visited = false;
	Vector_ForeachEnd
}

static TaxCollector * DetectCycle(TaxCollector * collector)
{
	if (collector->other == NULL || collector->other->data.creditBalance >= MIN_CREDIT_BALANCE)
		return NULL;

	if (collector->visited)
		return collector;

	collector->visited = true;

	return DetectCycle(collector->other);
}

static TaxCollector * FindSuitableTaxCollector(SimulationContext * context, TaxCollector * self)
{
	TaxCollector * taxCollector;
	Vector_ForeachBegin(&context->taxCollectors, taxCollector, i)
		if (taxCollector == self)
			continue;
		if (taxCollector->data.creditBalance >= MIN_CREDIT_BALANCE)
			return taxCollector;
	Vector_ForeachEnd
	assert(0);  // this should never happen!
	return NULL;
}