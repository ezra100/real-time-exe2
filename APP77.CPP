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
	if(ex2) SMARTS.mutex.Acquire(); 
	cout << "\n *************   A Start    *********************";
	if(ex2) SMARTS.mutex.Release();
	if(ex2) SMARTS.mutex.Acquire(); 
	//if(ex2) SMARTS.mutex.Acquire(); 

	for (int j=0; j <printLoop; j++)
	{
		for (long i=0; i <200000; i++);
		cout << "A";
	}
	if(ex2) SMARTS.mutex.Release();

	if(ex2) SMARTS.mutex.Acquire(); 
	cout << "\n *************   A Finish   *********************";
	if(ex2) SMARTS.mutex.Release();
}

void b( )
{
	if(ex2) SMARTS.mutex.Acquire(); 
	cout << "\n *************   B Start    *********************";
	if(ex2) SMARTS.mutex.Release();
	for (int j=0; j <printLoop; j++)
	{
		for (long i=0; i < 60000; i++);
		if(ex2) SMARTS.mutex.Acquire(); 
		cout << "B";
		if(ex2) SMARTS.mutex.Release();
	}
	if(ex2) SMARTS.mutex.Acquire(); 
	cout << "\n *************   B Finish   *********************";
	if(ex2) SMARTS.mutex.Release();
}

void c( )
{
	if(ex2) SMARTS.mutex.Acquire(); 
	cout << "\n *************   C Start    *********************";
	if(ex2) SMARTS.mutex.Release();
	for (int j=0; j <printLoop; j++)
	{
		for (long i=0; i < 60000; i++);
		if(ex2) SMARTS.mutex.Acquire(); 
		cout << "C";
		if(ex2) SMARTS.mutex.Release();
	}
	if(ex2) SMARTS.mutex.Acquire(); 
	cout << "\n *************   C Finish   *********************";
	if(ex2) SMARTS.mutex.Release();
}

void main( )
{
	clrscr();
	/*SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, EDF);
	SMARTS.declareTask(a, 'A', 15, 4);
	SMARTS.declareTask(b, 'B', 50, 3);
	SMARTS.declareTask(c, 'C', 25, 4);*/

	SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, RMS); //@@ex2
	SMARTS.declareTask(a, 'A', 50, 4); //402=not important. 400=most important
	SMARTS.declareTask(b, 'B', 27, 5);
//	SMARTS.declareTask(c, 'C', 40, 3);

	SMARTS.runTheTasks(); //
	int x;
	cin>>x;
}



