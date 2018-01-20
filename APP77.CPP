/********************************************************************/
/* B Small Real Time System for the Real-Time laboratory            */
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
Mutex outputMutex, acMutex, dfMutex, abMutex, bcMutex, cdMutex, deMutex;
Event cEvent, bEvent, eEvent, fEvent, dEvent;
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
	printf(msg);
	fprintf(myOutput, msg);
	lastMdTask = currentTask;
	outputMutex.Release();
}
void initMdLog()
{
	//outputMutex.Acquire();
	fprintf(myOutput, "TIME |");
	for (int i = 0; i < SMARTS.getTotalTasks(); i++)
	{
		fprintf(myOutput, " %c |", SMARTS.getName(i));
	}
	fprintf(myOutput, "\n:--- |");
	for (i = 0; i < SMARTS.getTotalTasks(); i++)
	{
		fprintf(myOutput, " :---: |");
	}
	//outputMutex.Release();
}
void printLoop(char name)
{
	char _FAR *buffer = new char[5];
	sprintf(buffer, "%c", name);
	for (int j = 0; j < printLoopSize; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		mdLog(buffer, false);
	}
	delete buffer;
}
#pragma region tasks
void a()
{
	mdLog("A Start ");
	printLoop('A');
	mdLog("A acquireing 'AC mutex' ");
	acMutex.Acquire();
	printLoop('A');
	mdLog("A sends Event to D ");
	void *data = new int[0];
	dEvent.send('D', data, true);
	printLoop('A');
	mdLog("A releaseing AC mutex ");
	acMutex.Release();
	printLoop('A');
	mdLog("A Finish ");
}
void b()
{
	mdLog("B Start    ");
	printLoop('B');
	mdLog("B waiting for Event");
	char dummy;
	bEvent.wait(dummy);
	printLoop('B');
	mdLog("B Finish ");
}
void c()
{
	mdLog("C Start   ");
	printLoop('C');
	mdLog("C waiting for Event");
	char dummy;
	cEvent.wait(dummy);
	printLoop('C');
	mdLog("Sending event to F and B");
	void *data;
	bEvent.send('B', data, true);
	fEvent.send('F', data, true);
	printLoop('C');
	mdLog(" C acquiring ac mutex ");
	acMutex.Acquire();
	printLoop('C');
	mdLog(" C releasing ac mutex ");
	acMutex.Release();
	mdLog("C Finish  ");
}
void d()
{
	mdLog("D Start ");
	printLoop('D');
	mdLog("D waiting for event ");
	dEvent.wait(' ');
	printLoop('D');

	mdLog("D acquireing 'df mutex' ");
	dfMutex.Acquire();
	printLoop('D');
	mdLog("sending event to C");
	void *data;
	cEvent.send('C', data, true);
	printLoop('D');
	mdLog("D releaseing df mutex ");
	dfMutex.Release();
	printLoop('D');
	mdLog("D Finish ");
}
/// A-D, D-C, C-F, F-E, C-B
void e()
{
	mdLog("E Start    ");
	printLoop('E');
	mdLog("E waiting for event ");
	char dummy;
	eEvent.wait(dummy);
	printLoop('E');
	mdLog("E Finish ");
}
void f()
{
	mdLog("F Start   ");
	printLoop('F');
	mdLog(" F waiting for Event");
	char dummy;
	fEvent.wait(dummy);
	printLoop('F');
	mdLog("Sending event to E");
	int *data;
	eEvent.send('E', data, true);
	printLoop('F');
	mdLog(" F acquiring df mutex ");
	dfMutex.Acquire();
	printLoop('F');
	mdLog(" F releasing df mutex ");
	dfMutex.Release();
	mdLog("F Finish  ");
}
#pragma endregion
#pragma region recursiveTasks
void ra()
{
	mdLog("A Start ");
	printLoop('A');
	mdLog("A acquireing 'AB mutex' ");
	abMutex.Acquire();
	printLoop('A');
	mdLog("A sends Event to B ");
	void *data;
	bEvent.send('B', data, true);
	printLoop('A');
	mdLog("A releaseing AB mutex ");
	abMutex.Release();
	printLoop('A');
	mdLog("A Finish ");
}
void rb()
{
	mdLog("B Start ");
	printLoop('B');
	mdLog("B waiting for Event from A ");
	char dummy;
	bEvent.wait(dummy);
	mdLog("B acquireing 'BC mutex' ");
	bcMutex.Acquire();
	printLoop('B');
	mdLog("B sends Event to C ");
	void *data;
	cEvent.send('C', data, true);
	printLoop('B');
	mdLog("B acquireing 'AB mutex' ");
	abMutex.Acquire();
	printLoop('B');
	mdLog("B releaseing AB mutex ");
	abMutex.Release();
	printLoop('B');
	mdLog("B releaseing BC mutex ");
	bcMutex.Release();
	printLoop('B');
	mdLog("B Finish ");
}
void rc()
{
	mdLog("C Start ");
	printLoop('C');
	mdLog("C waiting for Event from B ");
	char dummy;
	cEvent.wait(dummy);
	mdLog("C acquireing 'CD mutex' ");
	cdMutex.Acquire();
	printLoop('C');
	mdLog("C sends Event to C ");
	void *data;
	dEvent.send('D', data, true);
	printLoop('C');
	mdLog("C acquireing 'BC mutex' ");
	bcMutex.Acquire();
	printLoop('C');
	mdLog("C releaseing BC mutex ");
	bcMutex.Release();
	printLoop('C');
	mdLog("C releaseing CD mutex ");
	cdMutex.Release();
	printLoop('C');
	mdLog("C Finish ");
}
void rd()
{
	mdLog("D Start ");
	printLoop('D');
	mdLog("D waiting for Event from C ");
	char dummy;
	dEvent.wait(dummy);
	mdLog("D acquireing 'DE mutex' ");
	deMutex.Acquire();
	printLoop('D');
	mdLog("D sends Event to E ");
	void *data;
	eEvent.send('E', data, true);
	printLoop('D');
	mdLog("D acquireing 'CD mutex' ");
	cdMutex.Acquire();
	printLoop('D');
	mdLog("D releaseing CD mutex ");
	cdMutex.Release();
	printLoop('D');
	mdLog("D releaseing DE mutex ");
	deMutex.Release();
	printLoop('D');
	mdLog("D Finish ");
}
void re()
{
	mdLog("E Start ");
	printLoop('E');
	mdLog("E waiting for Event from D ");
	char dummy;
	eEvent.wait(dummy);
	printLoop('E');
	mdLog("E acquireing 'DE mutex' ");
	deMutex.Acquire();
	printLoop('E');
	mdLog("E releaseing DE mutex ");
	deMutex.Release();
	printLoop('E');
	mdLog("E Finish ");
}
#pragma endregion

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
	SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, RMS); //@@ex2
	SMARTS.declareTask(ra, 'A', 206, 5);
	SMARTS.declareTask(rb, 'B', 205, 5);
	SMARTS.declareTask(rc, 'C', 204, 5);
	SMARTS.declareTask(rd, 'D', 203, 5);
	SMARTS.declareTask(re, 'E', 202, 5);

	// SMARTS.declareTask(a, 'A', 206, 1);
	// SMARTS.declareTask(b, 'B', 204, 1);
	// SMARTS.declareTask(c, 'C', 202, 1);
	// SMARTS.declareTask(d, 'D', 205, 1);
	// SMARTS.declareTask(e, 'E', 203, 1);
	// SMARTS.declareTask(f, 'F', 201, 1);

	initMdLog();
	SMARTS.runTheTasks(); //
	fclose(myOutput);
	//printf("Finished");
	int x;
	cin >> x;
}
