/********************************************************************/
/* A Small Real Time System for the Real-Time laboratory            */
/* built by: A.Teitelbaum on an idea of H.G.Mendelbaum              */
/* Jerusalem College of Technology, 5759-64 (1999-03)               */
/* update  Tishrey   5777                                           */
/* APP77.CPP, an application to demonstrate SMARTS77  functioning   */
/********************************************************************/
#include "smarts77.h"
const int emptyLoop = 6000;
const int printLoop = 15;
const int ex2 = 1;
// user Functions

FILE *myOutput;

Mutex outputMutex;
Mutex acMutex;
Event event;
Event bEvent;

void a()
{
	outputMutex.Acquire();
	fprintf(myOutput, "\n *************   A Start    *********************");
	outputMutex.Release();
	int j;
	for (j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		fprintf(myOutput, "A");
		outputMutex.Release();
	}
	outputMutex.Acquire();
	fprintf(myOutput, " a acquire ");
	outputMutex.Release();
	acMutex.Acquire();
	for (j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		fprintf(myOutput, "A");
		outputMutex.Release();
	}
	fprintf(myOutput, " a sends event to b and c ");
	void *data = new int[0];
	event.send('C', data, true);
	bEvent.send('B', data, true);

	for (j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		fprintf(myOutput, "A");
		outputMutex.Release();
	}
	outputMutex.Acquire();
	fprintf(myOutput, " A release ");
	outputMutex.Release();
	acMutex.Release();
	for (j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		fprintf(myOutput, "A");
		outputMutex.Release();
	}
	outputMutex.Acquire();
	fprintf(myOutput, "\n *************   A Finish   *********************");
	outputMutex.Release();
}

void b()
{
	outputMutex.Acquire();
	fprintf(myOutput, "\n *************   B Start    *********************");
	outputMutex.Release();
	int j;
	for (j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 600000; i++)
			;
		outputMutex.Acquire();
		fprintf(myOutput, "B");
		outputMutex.Release();
	}
	outputMutex.Acquire();
	fprintf(myOutput, " B waiting for event ");
	outputMutex.Release();
	char dummy;
	bEvent.wait(dummy);

	for (j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 600000; i++)
			;
		outputMutex.Acquire();
		fprintf(myOutput, "B");
		outputMutex.Release();
	}
	outputMutex.Acquire();
	fprintf(myOutput, "\n *************   B Finish   *********************");
	outputMutex.Release();
}

void c()
{
	outputMutex.Acquire();
	fprintf(myOutput, "\n *************   C Start    *********************");
	outputMutex.Release();
	int j;
	for (j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		fprintf(myOutput, "C");
		outputMutex.Release();
	}
	outputMutex.Acquire();
	fprintf(myOutput, "C waiting for event from A ");
	outputMutex.Release();
	char dummy;
	event.wait(dummy);
	for (j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		fprintf(myOutput, "C");
		outputMutex.Release();
	}
	outputMutex.Acquire();
	fprintf(myOutput, "C acquiring ac mutex ");
	outputMutex.Release();
	acMutex.Acquire();

	outputMutex.Acquire();
	fprintf(myOutput, " Time: %d ", SMARTS.getTimePassed());
	outputMutex.Release();

	for (j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		fprintf(myOutput, "C");
		outputMutex.Release();
	}
	outputMutex.Acquire();
	fprintf(myOutput, " C releasing ac mutex ");
	outputMutex.Release();
	acMutex.Release();
	outputMutex.Acquire();
	fprintf(myOutput, "\n *************   C Finish   *********************");
	outputMutex.Release();
}

void main()
{
	clrscr();
//event = Event();
#ifdef priority_inheritance
	myOutput = fopen("with inheritance.txt", "w");
#else
	myOutput = fopen("no inheritance.txt", "w");
#endif
	SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, RMS); //@@ex2
	SMARTS.declareTask(a, 'A', 402, 1);											//402=not important. 400=most important
	SMARTS.declareTask(b, 'B', 401, 1);
	SMARTS.declareTask(c, 'C', 400, 1);

	SMARTS.runTheTasks(); //
	fclose(myOutput);
	printf("Finished");
	int x;
	cin >> x;
}
