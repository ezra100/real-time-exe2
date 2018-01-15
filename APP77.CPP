/********************************************************************/
/* `B` Small Real Time System for the Real-Time laboratory            */
/* built by: A.Teitelbaum on an idea of H.G.Mendelbaum              */
/* Jerusalem College of Technology, 5759-64 (1999-03)               */
/* update  Tishrey   5777                                           */
/* APP77.CPP, an application to demonstrate SMARTS77  functioning   */
/********************************************************************/
#include "smarts77.h"
const int emptyLoop = 6000;
const int printLoopSize = 15;
const int ex2 = 1;
// user Functions

FILE *myOutput;

Mutex outputMutex;
Mutex acMutex;
Event event;
Event bEvent;

void printLoop(char name)
{

	for (int j = 0; j < printLoopSize; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		if (j == 0)
		{
			fprintf(myOutput, "\n\n");
		}
		fprintf(myOutput, "%c", name);
		outputMutex.Release();
	}
}

void a()
{
	outputMutex.Acquire();
	fprintf(myOutput, "\n\n## `A` Start ");
	outputMutex.Release();
	printLoop('A');
	outputMutex.Acquire();
	fprintf(myOutput, "\n\n#### `A` acquireing 'AC mutex' ");
	outputMutex.Release();
	acMutex.Acquire();
	printLoop('A');
	fprintf(myOutput, "\n\n##### `A` sends event to `B` and `C` ");
	void *data = new int[0];
	event.send('C', data, true);
	bEvent.send('B', data, true);

	printLoop('A');

	outputMutex.Acquire();
	fprintf(myOutput, "\n\n#### `A` releaseing `AC mutex` ");
	outputMutex.Release();
	acMutex.Release();
	printLoop('A');
	outputMutex.Acquire();
	fprintf(myOutput, "\n\n##   `A` Finish ");
	outputMutex.Release();
}

void b()
{
	outputMutex.Acquire();
	fprintf(myOutput, "\n\n##   `B` Start    ");
	outputMutex.Release();
	printLoop('B');

	outputMutex.Acquire();
	fprintf(myOutput, "\n\n#### `B` waiting for event ");
	outputMutex.Release();
	char dummy;
	bEvent.wait(dummy);
	printLoop('B');
	outputMutex.Acquire();
	fprintf(myOutput, "\n\n##   `B` Finish ");
	outputMutex.Release();
}

void c()
{
	outputMutex.Acquire();
	fprintf(myOutput, "\n\n##   `C` Start   ");
	outputMutex.Release();
	printLoop('C');

	outputMutex.Acquire();
	fprintf(myOutput, "\n\nC waiting for event from `B` ");
	outputMutex.Release();
	char dummy;
	event.wait(dummy);
	printLoop('C');
	outputMutex.Acquire();
	fprintf(myOutput, "\n\n `C` acquiring `ac mutex` ");
	outputMutex.Release();
	acMutex.Acquire();

	outputMutex.Acquire();
	fprintf(myOutput, "\n\n Time: `%d` ", SMARTS.getTimePassed());
	outputMutex.Release();

	printLoop('C');

	outputMutex.Acquire();
	fprintf(myOutput, "\n\n `C` releasing `ac mutex` ");
	outputMutex.Release();
	acMutex.Release();
	outputMutex.Acquire();
	fprintf(myOutput, "\n\n##   `C` Finish  ");
	outputMutex.Release();
}

void main()
{
	clrscr();
//event = Event();
#ifdef priority_inheritance
	myOutput = fopen("with inheritance.md", "w");
#else
	myOutput = fopen("no inheritance.mdf", "w");
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
