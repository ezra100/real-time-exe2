/********************************************************************/
/* B Small Real Time System for the Real-Time laboratory            */
/* built by: A.Teitelbaum on an idea of H.G.Mendelbaum              */
/* Jerusalem College of Technology, 5759-64 (1999-03)               */
/* update  Tishrey   5777                                           */
/* APP77.CPP, an application to demonstrate SMARTS77  functioning   */
/********************************************************************/

// Modified as part of an execise in Real Time Systems by:
// Ezra Stinmetz עזרא שטיינמעץ
// Rotem Haimov רותם חיימוב
// תשע"ח 2017-8

#include "smarts77.h"
#include "string.h"
#include "STDARG.H"
const int emptyLoop = 6000;
const int printLoopSize = 15;
const int ex2 = 1;
// user Functions
FILE *myOutput;
Mutex outputMutex, acMutex, dfMutex, abMutex, bcMutex, cdMutex, deMutex;
Event cEvent, bEvent, eEvent, fEvent, dEvent, event1, event2, event3;
int lastMdTask = -1;
void mdLog(const char _FAR *msg, int newLine = true)
{
	//SMARTS.contextSwitchOff();
	outputMutex.Acquire();
	int currentTask = SMARTS.getCurrentTask();
	if (newLine || lastMdTask != currentTask)
	{
		fprintf(myOutput, "\nnote over %c: %s", SMARTS.getCurrentName(), msg);
	}
	lastMdTask = currentTask;
	//SMARTS.contextSwitchOn();
	outputMutex.Release();
}
void mdLogMsg(const char _FAR *msg, char *dest)
{
	//SMARTS.contextSwitchOff();

	outputMutex.Acquire();
	fprintf(myOutput, "\n%c->%s:%s", SMARTS.getCurrentName(), dest, msg);
	lastMdTask = -1;
	//SMARTS.contextSwitchOn();
	outputMutex.Release();
}

mdLogAcquire(char *target)
{
	char buffer[15];
	sprintf(buffer, "Acquiring %s", target);
	mdLog(buffer);
}
mdLogRelease(char *target)
{
	char buffer[15];
	sprintf(buffer, "Releasing %s", target);
	mdLog(buffer);
}

void initMdLog()
{
	//outputMutex.Acquire();
	fprintf(myOutput, "<!--this is a markdown file, in VSCode use Ctrl + Shift + V to view it, or search google for other options-->\n");
#ifdef priority_inheritance
	fprintf(myOutput, "## **WITH** priority inheritance\n");
#else
	fprintf(myOutput, "## **WITHOUT** priority inheritance\n");
#endif
	fprintf(myOutput, "```mermaid\nsequenceDiagram\n");
	for (int i = 0; i < SMARTS.getTotalTasks(); i++)
	{
		fprintf(myOutput, "participant %c\n", SMARTS.getName(i));
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
	mdLogAcquire("AC");
	acMutex.Acquire();
	printLoop('A');
	mdLogMsg("event ", "D");
	void *data = new int[0];
	dEvent.send('D', data, false);
	printLoop('A');
	mdLog("A releasing AC mutex ");
	acMutex.Release();
	printLoop('A');
	mdLog("A Finish ");
}
void b()
{
	mdLog("B Start");
	printLoop('B');
	mdLog("B waiting for Event");
	char dummy;
	bEvent.wait(dummy);
	printLoop('B');
	mdLog("B Finish ");
}
void c()
{
	mdLog("C Start");
	printLoop('C');
	mdLog("C waiting for Event");
	char dummy;
	cEvent.wait(dummy);
	printLoop('C');
	mdLogMsg("event", "B");
	void *data;
	bEvent.send('B', data, false);
	mdLogMsg("event", "F");
	fEvent.send('F', data, false);
	printLoop('C');
	mdLogAcquire("AC");
	acMutex.Acquire();
	printLoop('C');
	mdLogRelease("AC");
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

	mdLogAcquire("DF");
	dfMutex.Acquire();
	printLoop('D');
	mdLogMsg("event", "C");
	void *data;
	cEvent.send('C', data, false);
	printLoop('D');
	mdLogRelease("DF");
	dfMutex.Release();
	printLoop('D');
	mdLog("D Finish ");
}
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
	mdLogMsg("event", "E");
	int *data;
	eEvent.send('E', data, false);
	printLoop('F');
	mdLogMsg("acquire", "DF Mutex");
	dfMutex.Acquire();
	printLoop('F');
	mdLogRelease("DF");
	dfMutex.Release();
	mdLog("F Finish  ");
}
#pragma endregion
#pragma region recursiveTasks
void ra()
{
	mdLog("A Start ");
	printLoop('A');
	mdLogAcquire("AB");
	abMutex.Acquire();
	printLoop('A');
	mdLogMsg("event", "B");
	void *data;
	bEvent.send('B', data, false);
	printLoop('A');
	mdLogMsg("acquire", "AB Mutex");
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
	mdLogAcquire("BC");
	bcMutex.Acquire();
	printLoop('B');
	mdLogMsg("event", "C");
	void *data;
	cEvent.send('C', data, false);
	printLoop('B');
	mdLogAcquire("AB");
	abMutex.Acquire();
	printLoop('B');
	mdLogRelease("AB");

	abMutex.Release();
	printLoop('B');
	mdLogRelease("BC");
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
	mdLogAcquire("CD");
	cdMutex.Acquire();
	printLoop('C');
	mdLogMsg("event", "D");
	void *data;
	dEvent.send('D', data, false);
	printLoop('C');
	mdLogAcquire("BC");
	bcMutex.Acquire();
	printLoop('C');
	mdLogRelease("BC");
	bcMutex.Release();
	printLoop('C');
	mdLogRelease("CD");
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
	mdLogAcquire("DE");
	deMutex.Acquire();
	printLoop('D');
	mdLogMsg("event", "E");
	void *data;
	eEvent.send('E', data, false);
	printLoop('D');
	mdLogAcquire("CD");
	cdMutex.Acquire();
	printLoop('D');
	mdLogRelease("CD");
	cdMutex.Release();
	printLoop('D');
	mdLogRelease("DE");
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

	mdLogMsg("event", "1");
	void *data;
	event1.send('1', data, false);
	mdLogMsg("event", "2");
	event2.send('2', data, false);
	mdLogMsg("event", "3");
	event3.send('3', data, false);
	mdLogAcquire("DE");
	deMutex.Acquire();
	printLoop('E');
	mdLogRelease("DE");
	deMutex.Release();
	printLoop('E');
	mdLog("E Finish ");
}
void r1()
{
	mdLog("`1` Start ");
	mdLog("`1` waiting for Event from `E` ");
	char dummy;
	event1.wait(dummy);
	printLoop('1');
	printLoop('1');
	/*printLoop('1');
	printLoop('1');
	printLoop('1');
	printLoop('1');*/
	mdLog("`1` Finish ");
}
void r2()
{
	mdLog("`2` Start ");
	mdLog("`2` waiting for Event from `E` ");
	char dummy;
	event2.wait(dummy);
	printLoop('2');
	printLoop('2');
	/*printLoop('2');
	printLoop('2');
	printLoop('2');
	printLoop('2');*/
	mdLog("`2` Finish ");
}
void r3()
{
	mdLog("`3` Start ");
	mdLog("`3` waiting for Event from `E` ");
	char dummy;
	event3.wait(dummy);
	printLoop('3');
	printLoop('3');
	/*printLoop('3');
	printLoop('3');
	printLoop('3');
	printLoop('3');*/
	mdLog("`3` Finish ");
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

	int zero = 200;
	int times = 3;
	SMARTS.declareTask(ra, 'A', zero + 9, times);
	SMARTS.declareTask(r1, '1', zero + 8, times); //not using mutexes
	SMARTS.declareTask(rb, 'B', zero + 7, times);
	SMARTS.declareTask(r2, '2', zero + 6, times); //not using mutexes
	SMARTS.declareTask(rc, 'C', zero + 5, times);
	SMARTS.declareTask(r3, '3', zero + 4, times); //not using mutexes
	SMARTS.declareTask(rd, 'D', zero + 3, times);
	SMARTS.declareTask(re, 'E', zero + 2, times);

	// SMARTS.declareTask(a, 'A', 206, 3);
	// SMARTS.declareTask(b, 'B', 204, 3);
	// SMARTS.declareTask(c, 'C', 202, 3);
	// SMARTS.declareTask(d, 'D', 205, 3);
	// SMARTS.declareTask(e, 'E', 203, 3);
	// SMARTS.declareTask(f, 'F', 201, 3);

	initMdLog();
	SMARTS.runTheTasks(); //
	fprintf(myOutput, "\n```");
	fclose(myOutput);
	//printf("Finished");

	int x;
	cin >> x;
}
