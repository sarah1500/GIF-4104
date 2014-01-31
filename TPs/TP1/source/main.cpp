#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Chrono.hpp"

#define NUM_THREADS     4

void * worker(void * _pn);

class PrimeNumers
{
public:
	PrimeNumers(unsigned long _lMax, unsigned long _nb_workers);
	~PrimeNumers();
	unsigned long getK();
	void compute();
	void Show();

	//Variable
	bool *lFlags;
	bool done;
	unsigned long lMax;
	unsigned long startIndex;
	unsigned long currentIndex;
	unsigned long endIndex;
	int tmpThreadID = 0;
	
	//Pthread Variable
	pthread_t * workers;
	pthread_mutex_t mK;
	unsigned long nb_workers;

};

PrimeNumers::PrimeNumers(unsigned long _lMax, unsigned long _nb_workers)
{
	//Init pthread variables
	workers = (pthread_t *)calloc(nb_workers, sizeof(*workers));
	mK = PTHREAD_MUTEX_INITIALIZER;

	lMax = _lMax;
	currentIndex = 0;
	endIndex = (_lMax / 2) - 1;
	done = false;
	nb_workers = _nb_workers;

	// Allouer le tableau des drapeaux (flags) d'invalidation
	lFlags = (bool*)calloc(endIndex, sizeof(*lFlags));
	//assert(lFlags != 0);

	
}

unsigned long PrimeNumers::getK()
{
	unsigned long tmp = currentIndex;
	while (true)
	{
		if (currentIndex + 1 == endIndex)
		{
			done = true;
			return currentIndex;
		}

		currentIndex++;
		
		if (lFlags[currentIndex] == false)
		{
			return tmp;
		}
		//Not the end, increment the index;
		
	}
	
}

PrimeNumers::~PrimeNumers()
{
}

void PrimeNumers::compute()
{
	
	//create workers
	for (unsigned long t = 0; t < nb_workers; t++)
	{
		//printf("Create Worker %ld\n", t);
		pthread_create(&workers[t], NULL, worker, (void *) this);
	}
}

void PrimeNumers::Show()
{
	int count = 1;
	printf("2 ");
	for (unsigned long p = 0; p < endIndex; p++) {
		if (lFlags[p] == false)
		{
			//Alignement
			if (count == 10)
			{
				printf("\n");
				count = 0;
			}
			count++;
			printf("%ld ", p * 2 + 3);
		}
	}
	printf("\n");
}

void * worker(void * _pn)
{
	PrimeNumers * pn = (PrimeNumers *)_pn;
	unsigned long k;
	unsigned long n;
	int id = pn->tmpThreadID;
	pn->tmpThreadID++;

	while (!pn->done)
	{
		pthread_mutex_lock(&(pn->mK));
		k = pn->getK();
		if (pn->done)
		{
			pthread_mutex_unlock(&(pn->mK));
			break;
		}
		pthread_mutex_unlock(&(pn->mK));
		n = 2 * k + 3;

		for (unsigned long i = 3; (n * i) < (pn->lMax); i = i + 2)
		{
			pn->lFlags[((n * i) - 3) / 2] = true;
			
		}
	}

	pthread_exit(NULL);
}

PrimeNumers * pn;

int main(int argc, char *argv[])
{
	unsigned long max = 10000;
	unsigned long thread = 4;


	if (argc >= 2) {
		max = atol(argv[1]);
	}

	if (argc >= 3) {
		thread = atol(argv[2]);
	}
	pn = new PrimeNumers(max, thread);
    // Démarrer le chronomètre
    Chrono lChrono(true);
	
	
	pn->compute();

	

    // Arrêter le chronomètre
    lChrono.pause();

	for (unsigned long i = 0; i<pn->nb_workers; i++) {
		pthread_join(pn->workers[i], NULL);
	}

	pn->Show();

	printf("Temps d'execution = %f sec\n", lChrono.get());

	// Afficher les nombres trouvés à la console
	/*
	for (unsigned long p = 2; p<lMax; p++) {
		if (lFlags[p] == 0) printf("%ld ", p);
	}
	*/
	/* Last thing that main() should do */
	pthread_exit(NULL);
	return 0;
}