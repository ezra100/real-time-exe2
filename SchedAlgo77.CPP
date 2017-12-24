/********************************************************************/
/* A Small Real Time System for the Real-Time laboratory                 */
/* built by: A.Teitelbaum on an idea of H.G.Mendelbaum                  */
/* Jerusalem College of Technology, 5759-64 (1999)                            */
/* SchedAlgo64.CPP, short-term scheduler algorithms                */
/****************************************************************/


#include "smarts77.h"

int roundRobin()
// Round Robin short-term algorithm 
{
	int count;
	int nextTask = (SMARTS.getCurrentTask() + 1) % SMARTS.getTotalTasks();
	for (count = 0;
		SMARTS.getStatus(nextTask) != READY && count < SMARTS.getTotalTasks();
		count++)
		nextTask = ++nextTask%SMARTS.getTotalTasks();
	if (count == SMARTS.getTotalTasks())	// no found READY task to run
		nextTask = SMARTS.getTotalTasks();
	return nextTask;
}


int EDF() //@@@@@@ (ex1)
// EDF algorithm 
{
	int i = 0;
	for (i; SMARTS.getStatus(i) != READY && i < SMARTS.getTotalTasks(); i++) //go to first READY task.
		if (i == SMARTS.getTotalTasks())
			return i; //no task is READY, call scheduler (not sure thats what they want)
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
	int i = 0;
	for (i; SMARTS.getStatus(i) != READY && i < SMARTS.getTotalTasks(); i++) //go to first READY task.
		if (i == SMARTS.getTotalTasks())
			return i; //no task is READY, call scheduler
	int highestPriority = SMARTS.getPriority(i); //high (first to occur) is actually low number in code
	int highestPriorityTask = i;
	for (i += 1; i < SMARTS.getTotalTasks(); i++)
	{
		if (SMARTS.getStatus(i) == READY && SMARTS.getPriority(i) < highestPriority)
		{
			highestPriority = SMARTS.getPriority(i);
			highestPriorityTask = i;
		}
	}
	return highestPriorityTask;
}
