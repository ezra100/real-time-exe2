/********************************************************************/
/* A Small Real Time System for the Real-Time laboratory            */
/* built by: A.Teitelbaum on an idea of H.G.Mendelbaum              */
/* Jerusalem College of Technology, 5759-64 (1999-03)               */
/* update  Tishrey   5777                                           */
/* APP77.CPP, an application to demonstrate SMARTS77  functioning   */
/********************************************************************/
#include "smarts77.h"
const int emptyLoop=6000;
const int printLoop=15;
const int ex2=1;
// user Functions
void a( )
{
	if(ex2) SMARTS.mutex.Acquire(); else SMARTS.contextSwitchOff();
	cout << "\n *************   A Start    *********************";
	if(ex2) SMARTS.mutex.Release(); else SMARTS.contextSwitchOn();
	for (int j=0; j <printLoop; j++)
	{
		for (long i=0; i <60000; i++);
		if(ex2) SMARTS.mutex.Acquire(); else SMARTS.contextSwitchOff();
		cout << "A";
		if(ex2) SMARTS.mutex.Release(); else SMARTS.contextSwitchOn();
	}
	if(ex2) SMARTS.mutex.Acquire(); else SMARTS.contextSwitchOff();
	cout << "\n *************   A Finish   *********************";
	if(ex2) SMARTS.mutex.Release(); else SMARTS.contextSwitchOn();
}

void b( )
{
	if(ex2) SMARTS.mutex.Acquire(); else SMARTS.contextSwitchOff();
	cout << "\n *************   B Start    *********************";
	if(ex2) SMARTS.mutex.Release(); else SMARTS.contextSwitchOn();
	for (int j=0; j <printLoop; j++)
	{
		for (long i=0; i < 60000; i++);
		if(ex2) SMARTS.mutex.Acquire(); else SMARTS.contextSwitchOff();
		cout << "B";
		if(ex2) SMARTS.mutex.Release(); else SMARTS.contextSwitchOn();
	}
	if(ex2) SMARTS.mutex.Acquire(); else SMARTS.contextSwitchOff();
	cout << "\n *************   B Finish   *********************";
	if(ex2) SMARTS.mutex.Release(); else SMARTS.contextSwitchOn();
}

void c( )
{
	if(ex2) SMARTS.mutex.Acquire(); else SMARTS.contextSwitchOff();
	cout << "\n *************   C Start    *********************";
	if(ex2) SMARTS.mutex.Release(); else SMARTS.contextSwitchOn();
	for (int j=0; j <printLoop; j++)
	{
		for (long i=0; i < 60000; i++);
		if(ex2) SMARTS.mutex.Acquire(); else SMARTS.contextSwitchOff();
		cout << "C";
		if(ex2) SMARTS.mutex.Release(); else SMARTS.contextSwitchOn();
	}
	if(ex2) SMARTS.mutex.Acquire(); else SMARTS.contextSwitchOff();
	cout << "\n *************   C Finish   *********************";
	if(ex2) SMARTS.mutex.Release(); else SMARTS.contextSwitchOn();
}

void main( )
{
	clrscr();
	/*SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, EDF);
	SMARTS.declareTask(a, 'A', 15, 4);
	SMARTS.declareTask(b, 'B', 50, 3);
	SMARTS.declareTask(c, 'C', 25, 4);*/

	SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, RMS); //@@ex2
	SMARTS.declareTask(a, 'A', 402, 4); //402=not important. 400=most important
	SMARTS.declareTask(b, 'B', 401, 5);
	SMARTS.declareTask(c, 'C', 400, 3);

	SMARTS.runTheTasks();
	int x;
	cin>>x;
}



