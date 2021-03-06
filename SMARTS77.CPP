/****************************************************************/
/* A Small Real Time System for the Real-Time laboratory        */
/* built by: A.Teitelbaum on an idea of H.G.Mendelbaum          */
/* Jerusalem College of Technology, 5759-64 (1999)              */
/* update  Tishrey   5777                                       */
/* SMARTS77.CPP, SMARTS class body                              */
/****************************************************************/

// Modified as part of an execise in Real Time Systems by:
// Ezra Stinmetz עזרא שטיינמעץ
// Rotem Haimov רותם חיימוב
// תשע"ח 2017-8

#include "smarts77.h"
#include <string.h>

/**********    Function     **********/
unsigned getTimerClocks()
// Gets the remaining clocks of the timer register
{
	unsigned clocks;
	/* latch counter #0 */
	outportb(0x43, 0x00);
	/* read counter #0 low byte */
	clocks = inportb(0x40);
	/* read counter #0 high byte */
	clocks += inportb(0x40) << 8;
	return clocks;
}

#pragma region Parallelism
////////////////////////////////////////////////////
/**********    class body:  Parallelism     **********/
Parallelism::Parallelism()
{
	currentTask = 0;
	sleepTasks = 0;
	activeTasks = 0;
	totalTasks = 0;
	cyclesRemainingTasks = 0;
	timeFromStart = 0;
	isThereCyclePassedError = false;
	deadlock = false;
	for (int i = 0; i < MaxTask; i++)
		contextSwitchFlag2[i] = true; //@@ex2 (make each task a flag)
	contextSwitchFlag = true;
	endOfTimeSlice = true;
}

Mutex *Parallelism::getWaitingForMutex(int taskNum)
{
	return context[taskNum].waitingFor;
}

void Parallelism::updateWaitingForMutex(int taskNum, Mutex *mutex)
{
	context[taskNum].waitingFor = mutex;
}
void Parallelism::externalFunctions(void interrupt (*timerInterruptHandler)(...),
									void far *scheduler, void far *userTaskEnd,
									int far (*algorithm)())
// Sets the external functions
{
	this->timerInterruptHandler = timerInterruptHandler;
	this->scheduler = scheduler;
	this->userTaskEnd = userTaskEnd;
	this->algorithm = algorithm;
	contextSched.declare(scheduler, userTaskEnd, 'S', -1, -1); // prepare the stack of the scheduler task
	for (int i = MaxStack - 1; i >= (MaxStack - 14); i--)
		schedCopy[i] = contextSched.stack[i];
}

int Parallelism::declareTask(void far *code, char name, int cycleTime, int cyclesNum) //@@@@@@ ADDED 2 ARGS (LAST 2)
																					  // Insert a new task entry in SMARTS context array [ ]
{
	if (totalTasks < MaxTask - 1)
	{
		context[totalTasks++].declare(code, userTaskEnd, name, cycleTime, cyclesNum); //@@@@@@ ADDED 2 ARGS (LAST 2)
		++activeTasks;
		++cyclesRemainingTasks;
		return true;
	}
	else
		return false;
}

void Parallelism::runTheTasks()
// Start running all tasks declared in SMARTS.
{
	context[totalTasks].status = READY; //for task 'runTheTasks' (IDLE task)
	context[totalTasks].originalPriority = MAXINT;
	context[totalTasks].currentPriority = MAXINT;

	currentTask = totalTasks;

	asm cli;							 // forbids interrupts (clear interrupts) while changing the interrupt vect
										 // saves the original BIOS userInt in our variable 'userIntAddress' to be restored at the end
	userIntAddress = getvect(userInt);   // BIOS userInt 0x60  (unused by PC)
										 // puts the normal BIOS timerInt into the unused userInt address
	setvect(userInt, getvect(timerInt)); // timerInt 0x08

	// sets our SMARTS external function 'timerInterruptHandler' as the new PC hard interrupt time handler
	setvect(timerInt, timerInterruptHandler);
	asm sti; // allows back interrupts (set interrupts)

	// waits for end of runTheTasks (end of all the tasks)
	while (true)
	{
		if (deadlock)
		{
			textcolor(RED);
			cprintf("\n\n\rExit : deadlock");
			break;
		}
		if (cyclesRemainingTasks == 0)
		{
			cprintf("\n\n\rExit :SMARTS finished all tasks");
			break;
		}
		if (isThereCyclePassedError)
		{

			textcolor(RED);
			cprintf("\n\n\rError: task '%c' has passed a cycle without running", errorTaskName);
			break;
		}
	}
	cprintf("\n\rTime passed: %d sec/18", timeFromStart);
	// restore the original BIOS 'interrupt vector' at the end before returning to regular DOS
	asm cli;							 // no interrupts
	setvect(timerInt, getvect(userInt)); // restore original BIOS time handler
	setvect(userInt, userIntAddress);	// restore original BIOS userInt
	asm sti;							 // yes interrupts
}

int Parallelism::getIsThereCyclePassedError()
{
	return isThereCyclePassedError;
}
void Parallelism::printTimePassed()
{
	printf("\nTime: %d sec/18\n", timeFromStart);
}
int Parallelism::getTimePassed()
{
	return timeFromStart;
}
void Parallelism::addOwnedMutex(int i, Mutex *mutex)
{
	context[i].addOwnedMutex(mutex);
}
void Parallelism::removeOwnedMutex(int i, Mutex *mutex)
{
	context[i].removeOwnedMutex(mutex);
}

void Parallelism::countdownTasks()
{ //@@@*
	//cout<<"["<<timeFromStart<<"]";
	for (int i = 0; i < totalTasks; i++)
	{
		//debug
		//printf(" %c:%d,%d,%d; ", context[i].name, context[i].status, context[i].cyclesNumCountdown, context[i].nextCycleCountdown);
		if (context[i].status == FINISHED_ALL_CYCLES)
		{
			continue;
		}
		if (context[i].cyclesNumCountdown > 0)
		{ //if there are still cycles left
			if (context[i].nextCycleCountdown > 0)
			{
				context[i].nextCycleCountdown--;
			}
			else
			{
				if (context[i].status == NOT_ACTIVE)
				{
					context[i].cyclesNumCountdown--;
					context[i].reDeclare();
				}
				else
				{ //if the cycle ended and the task didn't run yet
					//debug
					//printf("error with %c ", context[i].name);
					cyclePassedError(context[i].name);
				}
			}
		}
		else
		{
			if (context[i].status == NOT_ACTIVE)
			{
				context[i].status = FINISHED_ALL_CYCLES;
				cyclesRemainingTasks--;
			}
		}
	}
}

void Parallelism::callScheduler()
// Return the control to the scheduler, this sets ON the software interrupt ProgInt flag
{
	setProgInt();
	asm int timerInt;
}

void Parallelism::restoreSchedStack()
// Restore the scheduler stack
{
	for (int i = MaxStack - 1; i >= (MaxStack - 14); i--)
		contextSched.stack[i] = schedCopy[i];
}

int Parallelism::getCurrentTask()
{
	return currentTask;
}

void Parallelism::setCurrentTask(int taskNum)
// Sets the next task to be run
{
	if (taskNum <= totalTasks)
		currentTask = taskNum;
}

int Parallelism::getTotalTasks()
// Gets total tasks declared
{
	return totalTasks;
}

int Parallelism::getDeadlock()
{
	return deadlock;
}

void Parallelism::setDeadlock()
{
	deadlock = true;
}

int Parallelism::contextSwitchOn() //@@ex2 changed all "contextSwitchFlag" occurences to contextSwitchFlag[i]
// flag which enables context switch
{
	if (endOfTimeSlice) //is current time slice finished ?
	{
		endOfTimeSlice = false;
		contextSwitchFlag2[currentTask] = true;
		contextSwitchFlag = true;
		callScheduler(); // return control to the scheduler
		return 1;
	}
	contextSwitchFlag2[currentTask] = true;
	//contextSwitchFlag=true;
	return 0;
}

void Parallelism::contextSwitchOff() //@@ex2 changed all "contextSwitchFlag" occurences to contextSwitchFlag[i]
// Disable context switch
{
	contextSwitchFlag2[currentTask] = false;
	//contextSwitchFlag = false;
}

int Parallelism::getContextSwitch() //@@ex2 changed all "contextSwitchFlag" occurences to contextSwitchFlag[i]
{
	return contextSwitchFlag2[currentTask];
	//	return contextSwitchFlag;
}

void Parallelism::setProgInt()
// flag indicates to the extern function 'timerInterruptHandler'
// that this is an internal SMARTS software interrupt call,
// and the original BIOS function has no to be called.
{
	progInt = true;
}

void Parallelism::resetProgInt()
{
	progInt = false;
}

int Parallelism::getProgInt()
{
	return progInt;
}

void Parallelism::setEndOfTimeSlice()
// flag indicates that when 'context switch' will be enabled,
// it must also return the control to the scheduler.
{
	endOfTimeSlice = true;
}

char Parallelism::getName(int taskNum) // returns name found or ' ' if not
{
	return (taskNum <= totalTasks) ? context[taskNum].name : ' ';
}

char Parallelism::getCurrentName()
{
	return context[currentTask].name;
}

taskStatus Parallelism::getStatus(int taskNum)
// returns status or undefined if not found
{
	return (taskNum <= totalTasks) ? context[taskNum].status : UNDEFINED;
}
int Parallelism::getNextCycleCountdown(int taskNum) //@@@@@@ added helper func @@@*
{
	return (taskNum <= totalTasks) ? context[taskNum].nextCycleCountdown : -1;
}
taskStatus Parallelism::getCurrentStatus()
{
	return context[currentTask].status;
}

void Parallelism::resume(int taskNum)
{
	if (taskNum < totalTasks)
		context[taskNum].status = READY;
}

void Parallelism::resume(char taskName)
{
	for (int i = 0; i < totalTasks; ++i)
		if (context[i].name == taskName)
			context[i].status = READY;
}

void Parallelism::setCurrentNotActive()
{
	context[currentTask].status = NOT_ACTIVE;
	--activeTasks;
}
void Parallelism::suspended()
{
	context[currentTask].status = SUSPENDED;
	callScheduler();
}

void Parallelism::incrPriority(int taskNum, int priority)
{
	if (taskNum < totalTasks)
		context[taskNum].incrPriority(priority);
}
void Parallelism::setToOriginalPriority(int taskNum)
{
	if (taskNum < totalTasks)
		context[taskNum].setToOriginalPriority();
}

void Parallelism::setTasklToOriginalPriority(int i)
{
	context[i].setToOriginalPriority();
}

Event *Parallelism::getExpectedEvent(int taskNum)
// returns *Event  or  NULL  if not found
{
	return (taskNum <= totalTasks) ? context[taskNum].expectedEvent : NULL;
}

Event *Parallelism::getCurrentExpectedEvent()
{
	return context[currentTask].expectedEvent;
}

void Parallelism::setCurrentExpectedEvent(Event *expectedEvent)
{
	context[currentTask].expectedEvent = expectedEvent;
}

void Parallelism::sleep(int t)
// Current task sleeps for 't' milliseconds
{
	if (t < MAXINT)
	{
		context[currentTask].sleepCount = t / 55 + 1;
		context[currentTask].status = SLEEP;
		++sleepTasks;
		callScheduler(); // return control to scheduler
	}
}

void Parallelism::sleepDecr(int taskNum)
{
	if (taskNum < totalTasks)
		context[taskNum].sleepDecr();
}

void Parallelism::getCurrentStack(unsigned &StackSeg, unsigned &StackPtr)
// Load current task stack pointer
{
	StackSeg = context[currentTask].stackSeg;
	StackPtr = context[currentTask].stackPtr;
}

void Parallelism::setCurrentStack(unsigned StackSeg, unsigned StackPtr)
// Save current task stack pointer
{
	context[currentTask].stackSeg = StackSeg;
	context[currentTask].stackPtr = StackPtr;
}

void Parallelism::getSchedStack(unsigned &StackSeg, unsigned &StackPtr)
// Load scheduler  stack pointer
{
	StackSeg = contextSched.stackSeg;
	StackPtr = contextSched.stackPtr;
}

void Parallelism::handleTimers()
// handling of the sleep status mode
{
	for (int i = totalTasks - 1; i >= 0; --i)
	{
		if (getStatus(i) == SLEEP)
		{
			sleepDecr(i);
			if (getStatus(i) == READY)
				--sleepTasks;
		}
	}
	timeFromStart++;
	countdownTasks(); //@@@*
}

void Parallelism::taskEnd()
// This function is called after the last operation of a task, instead of function return
{
	SMARTS.setCurrentNotActive();
	SMARTS.callScheduler(); // return the control to the scheduler to run a next task
}

void Parallelism::cyclePassedError(char taskName)
{
	isThereCyclePassedError = true;
	errorTaskName = taskName;
}
int Parallelism::getCurrentPriority(int taskNum) //@@ex2
{
	return context[taskNum].currentPriority;
}
/*Task[15]& Parallelism::getContext() //@@ex2
{
	return context;
}*/
#pragma endregion

#pragma region Task
/**********    class body:  Task     **********/
Task::Task()
{
	stack[MaxStack - 14] = _BP;
	stack[MaxStack - 13] = _DI;
	stack[MaxStack - 12] = _SI;
	stack[MaxStack - 11] = _DS;
	stack[MaxStack - 10] = _ES;
	stack[MaxStack - 9] = _DX;
	stack[MaxStack - 8] = _CX;
	stack[MaxStack - 7] = _BX;
	stack[MaxStack - 6] = _AX;
	stackSeg = FP_SEG(&stack[MaxStack - 14]);
	stackPtr = FP_OFF(&stack[MaxStack - 14]);
	status = NOT_ACTIVE;
	sleepCount = 0;
	waitingFor = NULL;
	currentPriority = originalPriority = 0;
	ownedMutexesSize = 0;
}
//-----------------------------------------------------
void Task::declare(void far *code, void far *userTaskEnd, char name, int cycleTime, int cyclesNum) //@@@@@@ ADDED 2 ARGS (LAST 2)
{
	this->code = code;
	this->userTaskEnd = userTaskEnd;
	stack[MaxStack - 5] = FP_OFF(code);
	stack[MaxStack - 4] = FP_SEG(code);
	stack[MaxStack - 3] = _FLAGS;
	stack[MaxStack - 2] = FP_OFF(userTaskEnd);
	stack[MaxStack - 1] = FP_SEG(userTaskEnd);
	this->name = name;
	this->cyclesNum = cyclesNum; //@@@@@@ ADDED (ex1)
	this->cycleTime = cycleTime; //@@@@@@ ADDED (ex1)
	nextCycleCountdown = cycleTime;
	cyclesNumCountdown = cyclesNum - 1; //decrease by 1 since the first round will start automaticaly
	status = READY;
	currentPriority = originalPriority = cycleTime; //@@ex2
}

void Task::reDeclare()
{
	stack[MaxStack - 5] = FP_OFF(code);
	stack[MaxStack - 4] = FP_SEG(code);
	stack[MaxStack - 3] = _FLAGS;
	stack[MaxStack - 2] = FP_OFF(userTaskEnd);
	stack[MaxStack - 1] = FP_SEG(userTaskEnd);
	stack[MaxStack - 14] = _BP;
	stack[MaxStack - 13] = _DI;
	stack[MaxStack - 12] = _SI;
	stack[MaxStack - 11] = _DS;
	stack[MaxStack - 10] = _ES;
	stack[MaxStack - 9] = _DX;
	stack[MaxStack - 8] = _CX;
	stack[MaxStack - 7] = _BX;
	stack[MaxStack - 6] = _AX;
	stackSeg = FP_SEG(&stack[MaxStack - 14]);
	stackPtr = FP_OFF(&stack[MaxStack - 14]);
	sleepCount = 0;
	nextCycleCountdown = cycleTime;
	status = READY;
}
//----------------------------------------------------
void Task::incrPriority(int priority)
{
	if (currentPriority > priority)
	{
		currentPriority = priority;
	}
}
//----------------------------------------------------
void Task::setToOriginalPriority()
{
	currentPriority = originalPriority;
}
//----------------------------------------------------
void Task::sleepDecr()
// Decrements the sleep counter and update the task status accordingly
{
	if (status == SLEEP)
	{
		if (sleepCount > 0)
			--sleepCount;
		if (!sleepCount)
			status = READY;
	}
}
int Task::getOriginalPriority()
{
	return originalPriority;
}

void Task::addOwnedMutex(Mutex *mutex)
{
	if (ownedMutexesSize >= MAX_MUTEXES)
	{
		printf("ERROR, owned mutexes array is full");
		return;
	}
	ownedMutexes[ownedMutexesSize] = mutex;
	ownedMutexesSize++;
}

void Task::removeOwnedMutex(Mutex *mutex)
{
	int index = -1;
	for (int i = 0; i < ownedMutexesSize; i++)
	{
		if (ownedMutexes[i] == mutex)
		{
			index = i;
			break;
		}
	}
	if (index < 0)
	{
		printf("ERROR, owned mutex not found");
		return;
	}
	ownedMutexesSize--;
	ownedMutexes[index] = ownedMutexes[ownedMutexesSize];
	for (i = 0; i < ownedMutexesSize; i++)
	{
		ownedMutexes[i]->updateOwnerPriority();
	}
}
#pragma endregion

#pragma region Mutex
int getPriorityFunc(int task)
{
	return SMARTS.getCurrentPriority(task);
}
Mutex::Mutex() : queue(getPriorityFunc, MaxTask)
{
	size = MaxTask;
	available = 1;
	owner = -1;
	level = 0;
	numberSuspended = 0;
}

void Mutex::updateOwnerPriority()
{
#ifdef priority_inheritance
	if (queue.isEmpty())
	{
		return;
	}
	int priority = SMARTS.getCurrentPriority(queue.peek());

	SMARTS.incrPriority(owner, priority);
	//debug
	//printf("owner: %d, incr priority: %d, actual priority: %d ", owner, priority, SMARTS.getCurrentPriority(owner));
	Mutex *mutex = SMARTS.getWaitingForMutex(owner);
	if (mutex != NULL)
	{
		mutex->updateOwnerPriority();
	}
#endif
}
void Mutex::Acquire()
{
	SMARTS.contextSwitchOff();
	//printf("%d", available);

	if (available >= 1 || owner == SMARTS.getCurrentTask())
	{
		available = 0;
	}
	else
	{
		if (queue.isFull())
		{
			printf("ERROR number of tasks waiting for mutex is too big");
			SMARTS.suspended();
			//TODO handle this kind of error
			return; //cant insert more tasks to queue
					//put in queue the currentTask which consists of {taskNum, Priority}
		}

		//debug
		//printf("suspending %d", taskNum);

		int taskNum = SMARTS.getCurrentTask();
		push(taskNum);
		updateOwnerPriority();
		SMARTS.updateWaitingForMutex(SMARTS.getCurrentTask(), this);
		SMARTS.suspended();

		//printf("passed suspending\n");
	}
	owner = SMARTS.getCurrentTask();
	SMARTS.updateWaitingForMutex(owner, NULL);
	updateOwnerPriority();
	level++;
	SMARTS.addOwnedMutex(owner, this);
	SMARTS.contextSwitchOn();
}
void Mutex::Release()
{
	SMARTS.contextSwitchOff();
	if (owner == SMARTS.getCurrentTask())
	{
		level--;
		if (level)
		{
			return;
		}
		else
		{
			SMARTS.setTasklToOriginalPriority(owner);
			SMARTS.removeOwnedMutex(owner, this);
			owner = -1;
			//debug
			//printf("%c:%d", SMARTS.getName(owner), SMARTS.getCurrentPriority(owner));

			if (!queue.isEmpty())
			{
				//get a task from queue (with highest priority)
				int task = pop();
				//debug
				//printf("waking %d ", task);

				// todo - make sure that if there's another mutex waiting for the owner
				// the owner's priority will be updated by that mutex

				SMARTS.resume(task);
			}
			else
			{
				available = 1;
			}
		}
	}

	SMARTS.callScheduler();
	SMARTS.contextSwitchOn();
}
void Mutex::push(int item) //puts task into the queue
{
	queue.push(item);
}

int Mutex::pop() //gets the highest priority task
{
	return queue.pop();
}
#pragma endregion

#pragma region PriorityQueue
PriorityQueue::PriorityQueue(int (*test)(int), int maxSize)
{
	items = new int[maxSize];
	max = maxSize;
	this->test = test;
	this->size = 0;
}
PriorityQueue::~PriorityQueue()
{
	printf("priority queue DTOR running");
	delete[] items;
}

int PriorityQueue::isEmpty()
{
	return size == 0;
}
int PriorityQueue::isFull()
{
	return size >= max;
}

int PriorityQueue::push(int item)
{
	if (size >= max)
	{
		printf("error, queue is full");
		return false;
	}
	items[size] = item;
	size++;
	// printf("\npush item:%d\n", item);
	// for (int i = 0; i < size; i++)
	// {
	//      printf("%d:%d;", items[i], test(items[i]));
	// }
	return true;
}
int PriorityQueue::pop()
{
	if (isEmpty())
	{
		printf("error, queue is empty");
		return -1;
	}
	int lowestIndex = 0;
	for (int i = 1; i < size; i++)
	{
		//priority is inversed here, the lowest is the highest
		if (test(items[i]) < test(items[lowestIndex]))
		{
			lowestIndex = i;
		}
	}
	size--;
	int lowestItem = items[lowestIndex];
	items[lowestIndex] = items[size];
	//debug
	// printf("\npop highest item:%d\n", lowestItem);
	// for (i = 0; i < size; i++)
	// {
	//      printf("%d:%d;", items[i], test(items[i]));
	// }
	return lowestItem;
}

int PriorityQueue::peek()
{
	if (isEmpty())
	{
		printf("ERROR, can't peek,queue is empty");
		return -1;
	}
	int lowestIndex = 0;
	for (int i = 1; i < size; i++)
	{
		//priority is inversed here, the lowe
		if (test(items[i]) < test(items[lowestIndex]))
		{
			lowestIndex = i;
		}
	}
	// printf("\npeek highest index:%d, size:%d\n", lowestIndex, size);
	// for (i = 0; i < size; i++)
	// {
	//      printf("%d:%d;", items[i], test(items[i]));
	// }
	return items[lowestIndex];
}

#pragma endregion