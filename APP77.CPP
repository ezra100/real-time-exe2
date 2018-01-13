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

Event event;
Event bEvent;
void a()
{

	SMARTS.mutex.Acquire();
	cout << "\n *************   A Start    *********************";
	void *data = new int[0];
	event.send('C', data, true);
	bEvent.send('B', data, true);
	for (int j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;
		cout << "A";
	}

	cout << "\n *************   A Finish   *********************";

	SMARTS.mutex.Release();
}

void b()
{
	bEvent.wait(' ');
	cout << "\n *************   B Start    *********************";
	for (int j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 600000; i++)
			;

		cout << "B";
	}

	cout << "\n *************   B Finish   *********************";
}

void c()
{
	char dummy;
	event.wait(dummy);
	SMARTS.mutex.Acquire();
	SMARTS.printTimePassed();
	cout << "\n *************   C Start    *********************";

	for (int j = 0; j < printLoop; j++)
	{
		for (long i = 0; i < 60000; i++)
			;

		cout << "C";
	}
	cout << "\n *************   C Finish   *********************";

	SMARTS.mutex.Release();
}

void main()
{
	clrscr();
	//event = Event();

	SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, RMS); //@@ex2
	SMARTS.declareTask(a, 'A', 402, 3);											//402=not important. 400=most important
	SMARTS.declareTask(b, 'B', 401, 3);
	SMARTS.declareTask(c, 'C', 400, 3);

	SMARTS.runTheTasks(); //
	int x;
	cin >> x;
}
