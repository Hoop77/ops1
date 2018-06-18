#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>

typedef struct {
    int inPostings = 0;
    int outPostings= 0;
    int creditBalance = 0;
} collectorData;

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
    int totalInPostings     = 0;
    int totalOutPostings    = 0;
    int totalFunding        = 0;
	
	printf(
		"Total in postings:  %d\n"
		"Total out postings: %d EUR\n"
		"Total funding:      %d s\n",
		collectors, funds, duration
	);
	return 0;
}

int taxCollector (){
    // TODO:
    //
    //
}
