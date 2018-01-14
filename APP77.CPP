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

FILE *myOutput = fopen("test.txt",”w”);

Mutex outputMutex;
Mutex acMutex;
Event event;
Event bEvent;
void a()
{
	acMutex.Acquire();
	outputMutex.Acquire();
	cout << "\n *************   A Start    *********************";
	outputMutex.Release();

	for (int j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		cout << "A";
		outputMutex.Release();
	}
	outputMutex.Acquire();
	cout << " a acquire ";
	outputMutex.Release();
	SMARTS.mutexa.Acquire;
	for (int j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		cout << "A";
		outputMutex.Release();
	}
	cout << " a sends event to b and c ";
	void *data = new int[0];
	event.send('C', data, true);
	bEvent.send('B', data, true);

	for (int j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		cout << "A";
		outputMutex.Release();
	}
	outputMutex.Acquire();
	cout << " A release";
	outputMutex.Release();
	acMutex.Release();
	for (int j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		cout << "A";
		outputMutex.Release();
	}
	outputMutex.Acquire();
	cout << "\n *************   A Finish   *********************";
	outputMutex.Release();
}

void b()
{
	outputMutex.Acquire();
	cout << "\n *************   B Start    *********************";
	outputMutex.Release();
	for (int j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 600000; i++)
			;
		outputMutex.Acquire();
		cout << "B";
		outputMutex.Release();
	}
	outputMutex.Acquire();
	cout << " B waiting for event ";
	outputMutex.Release();
	bEvent.wait(' ');

	for (int j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 600000; i++)
			;
		outputMutex.Acquire();
		cout << "B";
		outputMutex.Release();
	}
	outputMutex.Acquire();
	cout << "\n *************   B Finish   *********************";
	outputMutex.Release();
}

void c()
{
	outputMutex.Acquire();
	cout << "\n *************   C Start    *********************";
	outputMutex.Release();
	for (int j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		cout << "C";
		outputMutex.Release();
	}
	outputMutex.Acquire();
	cout << "C waiting for event from A ";
	outputMutex.Release();
	char dummy;
	event.wait(dummy);

	outputMutex.Acquire();
	cout << "C acquiring ac mutex ";
	outputMutex.Release();
	acMutex.Acquire();

	outputMutex.Acquire();
	SMARTS.printTimePassed();
	outputMutex.Release();

	for (int j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		outputMutex.Acquire();
		cout << "C";
		outputMutex.Release();
	}
	outputMutex.Acquire();
	cout << "C releasing ac mutex ";
	outputMutex.Release();
	acMutex.Release();
	outputMutex.Acquire();
	cout << "\n *************   C Finish   *********************";
	outputMutex.Release();
}

void main()
{
	clrscr();
	//event = Event();

	SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, RMS); //@@ex2
	SMARTS.declareTask(a, 'A', 402, 1);											//402=not important. 400=most important
	SMARTS.declareTask(b, 'B', 401, 1);
	SMARTS.declareTask(c, 'C', 400, 1);

	SMARTS.runTheTasks(); //
	int x;
	cin >> x;
}
