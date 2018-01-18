/********************************************************************/
/* `B` Small Real Time System for the Real-Time laboratory            */
/* built by: A.Teitelbaum on an idea of H.G.Mendelbaum              */
/* Jerusalem College of Technology, 5759-64 (1999-03)               */
/* update  Tishrey   5777                                           */
/* APP77.CPP, an application to demonstrate SMARTS77  functioning   */
/********************************************************************/
#include "smarts77.h"
#include "string.h"
#include "STDARG.H"
const int emptyLoop = 6000;
const int printLoopSize = 15;
const int ex2 = 1;
// user Functions

FILE *myOutput;

Mutex outputMutex;
Mutex acMutex, dfMutex;
Event cEvent, bEvent, eEvent, fEvent;

int lastMdTask = -1;
void mdLog(const char _FAR *msg, int newLine = true)
{
	outputMutex.Acquire();
	int currentTask = SMARTS.getCurrentTask();
	if (newLine || lastMdTask != currentTask)
	{
		fprintf(myOutput, "\n%d | ", SMARTS.getTimePassed());
		for (int i = 0; i < currentTask; i++)
		{
			fprintf(myOutput, "|");
		}
	}
	fprintf(myOutput,"%s" ,msg);
	lastMdTask = currentTask;
	outputMutex.Release();
}

void printLoop(char name)
{

	char buffer[5];
	sprintf(buffer, "%c", name);
	for (int j = 0; j < printLoopSize; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		mdLog(buffer, false);
	}
}

void a()
{
	outputMutex.Acquire();
	mdLog("`A` Start ");
	outputMutex.Release();
	printLoop('A');
	outputMutex.Acquire();
	mdLog("`A` acquireing 'AC mutex' ");
	outputMutex.Release();
	acMutex.Acquire();
	printLoop('A');
	outputMutex.Acquire();
	mdLog("`A` sends Event to `B` and `C` ");
	outputMutex.Release();
	void *data = new int[0];
	cEvent.send('C', data, true);
	bEvent.send('B', data, true);

	printLoop('A');

	outputMutex.Acquire();
	mdLog("`A` releaseing `AC mutex` ");
	outputMutex.Release();
	acMutex.Release();
	printLoop('A');
	outputMutex.Acquire();
	mdLog("`A` Finish ");
	outputMutex.Release();
}

void b()
{
	outputMutex.Acquire();
	mdLog("`B` Start    ");
	outputMutex.Release();
	printLoop('B');

	outputMutex.Acquire();
	mdLog("`B` waiting for Event from `A` ");
	outputMutex.Release();
	char dummy;
	bEvent.wait(dummy);
	printLoop('B');
	outputMutex.Acquire();
	mdLog("`B` Finish ");
	outputMutex.Release();
}

void c()
{
	outputMutex.Acquire();
	mdLog("`C` Start   ");
	outputMutex.Release();
	printLoop('C');

	outputMutex.Acquire();
	mdLog("`C` waiting for Event from `A` ");
	outputMutex.Release();
	char dummy;
	cEvent.wait(dummy);
	printLoop('C');
	outputMutex.Acquire();
	mdLog(" `C` acquiring `ac mutex` ");
	outputMutex.Release();

	acMutex.Acquire();

	printLoop('C');

	outputMutex.Acquire();
	mdLog(" `C` releasing `ac mutex` ");
	outputMutex.Release();
	acMutex.Release();
	outputMutex.Acquire();
	mdLog("`C` Finish  ");
	outputMutex.Release();
}
void d()
{
	outputMutex.Acquire();
	mdLog("`D` Start ");
	outputMutex.Release();
	printLoop('D');
	outputMutex.Acquire();
	mdLog("`D` acquireing 'df mutex' ");
	outputMutex.Release();
	dfMutex.Acquire();
	printLoop('D');
	outputMutex.Acquire();
	mdLog("`D` sends Event to `E` and `F` ");
	outputMutex.Release();
	void *data = new int[0];
	fEvent.send('F', data, true);
	eEvent.send('E', data, true);

	printLoop('D');

	outputMutex.Acquire();
	mdLog("`D` releaseing `df mutex` ");
	outputMutex.Release();
	dfMutex.Release();
	printLoop('D');
	outputMutex.Acquire();
	mdLog("`D` Finish ");
	outputMutex.Release();
}

void e()
{
	outputMutex.Acquire();
	mdLog("`E` Start    ");
	outputMutex.Release();
	printLoop('E');

	outputMutex.Acquire();
	mdLog("`E` waiting for Event from `D` ");
	outputMutex.Release();
	char dummy;
	eEvent.wait(dummy);
	printLoop('E');
	outputMutex.Acquire();
	mdLog("`E` Finish ");
	outputMutex.Release();
}

void f()
{
	outputMutex.Acquire();
	mdLog("`F` Start   ");
	outputMutex.Release();
	printLoop('F');

	outputMutex.Acquire();
	mdLog(" `F` waiting for Event from `D` ");
	outputMutex.Release();
	char dummy;
	fEvent.wait(dummy);
	printLoop('F');
	outputMutex.Acquire();
	mdLog(" `F` acquiring `df mutex` ");
	outputMutex.Release();

	dfMutex.Acquire();



	printLoop('F');

	outputMutex.Acquire();
	mdLog(" `F` releasing `df mutex` ");
	outputMutex.Release();
	dfMutex.Release();
	outputMutex.Acquire();
	mdLog("`F` Finish  ");
	outputMutex.Release();
}

void main()
{
	clrscr();
#ifdef priority_inheritance
	myOutput = fopen("inheritance.md", "w");
	printf("WITH priority inheritance\n-------------------------");
#else
	myOutput = fopen("no inheritance.md", "w");
	printf("WITHOUT priority inheritance\n-------------------------");
#endif
	fprintf(myOutput, "TIME | A | B | C | D |E|F\n:--- | :---: | :---: | :---: | :---: | :---: | :---: |");
	SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, RMS); //@@ex2
	SMARTS.declareTask(a, 'A', 402, 1);											//402=not important. 400=most important
	SMARTS.declareTask(b, 'B', 401, 1);
	SMARTS.declareTask(c, 'C', 400, 1);
	SMARTS.declareTask(d, 'D', 402, 1); //402=not important. 400=most important
	SMARTS.declareTask(e, 'E', 401, 1);
	SMARTS.declareTask(f, 'F', 400, 1);

	SMARTS.runTheTasks(); //
	fclose(myOutput);
	printf("Finished");
	int x;
	cin >> x;
}
