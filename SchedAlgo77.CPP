/********************************************************************/
/* A Small Real Time System for the Real-Time laboratory                 */
/* built by: A.Teitelbaum on an idea of H.G.Mendelbaum                  */
/* Jerusalem College of Technology, 5759-64 (1999)                            */
/* SchedAlgo64.CPP, short-term scheduler algorithms                */
/****************************************************************/

// Modified as part of an execise in Real Time Systems by:
// Ezra Stinmetz עזרא שטיינמעץ
// Rotem Haimov רותם חיימוב
// תשע"ח 2017-8

#include "smarts77.h"

int roundRobin()
// Round Robin short-term algorithm
{
	int count;
	int nextTask = (SMARTS.getCurrentTask() + 1) % SMARTS.getTotalTasks();
	for (count = 0;
		 SMARTS.getStatus(nextTask) != READY && count < SMARTS.getTotalTasks();
		 count++)
		nextTask = ++nextTask % SMARTS.getTotalTasks();
	if (count == SMARTS.getTotalTasks()) // no found READY task to run
		nextTask = SMARTS.getTotalTasks();
	return nextTask;
}

int EDF() //@@@@@@ (ex1)
// EDF algorithm
{
	int i = 0;
	for (i; SMARTS.getStatus(i) != READY && i < SMARTS.getTotalTasks(); i++) //go to first READY task.
		if (i == SMARTS.getTotalTasks())
			return i;										//no task is READY, call scheduler (not sure thats what they want)
	int closestCycleTime = SMARTS.getNextCycleCountdown(i); //added "getCyclesTime" function in SMARTS
	int closestTaskNum = i;

	for (i += 1; i < SMARTS.getTotalTasks(); i++)
	{
		if (SMARTS.getStatus(i) == READY && SMARTS.getNextCycleCountdown(i) < closestCycleTime)
		{
			closestCycleTime = SMARTS.getNextCycleCountdown(i);
			closestTaskNum = i;
		}
	}
	return closestTaskNum;
}

int RMS() //@@ex2
{
	int highestPriority =MAXINT;
	int highestPriorityTask =  SMARTS.getTotalTasks();
	for (int i = 0; i < SMARTS.getTotalTasks(); i++)
	{
		//debug

		if (SMARTS.getStatus(i) == READY && SMARTS.getCurrentPriority(i) < highestPriority)
		{
			highestPriority = SMARTS.getCurrentPriority(i);
			highestPriorityTask = i;
		}
	}
	return highestPriorityTask;
}
