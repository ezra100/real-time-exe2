/********************************************************************/
/* A Small Real Time System for the Real-Time laboratory            */
/* built by: A.Teitelbaum on an idea of H.G.Mendelbaum              */
/* Jerusalem College of Technology, 5759-64 (1999)                  */
/* update  Tishrey   5777                                           */
/* SMARTS77.H, main header file                                     */
/********************************************************************/

// Modified as part of an execise in Real Time Systems by:
// Ezra Stinmetz עזרא שטיינמעץ
// Rotem Haimov רותם חיימוב
// תשע"ח 2017-8

//comment the next line in order to disable priority inheritance
#define priority_inheritance 0

#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <values.h>
#include <iostream.h>

/**********    Constants    **********/
#define true 1
#define false 0
#define timerInt 0x08
#define userInt 0x60
#define MaxTask 15
#define MaxStack 512
const int MAX_MUTEXES = 10;
enum taskStatus
{
	READY,
	NOT_ACTIVE,
	SUSPENDED,
	SLEEP,
	UNDEFINED,
	FINISHED_ALL_CYCLES
};			 //@@@*
class Mutex; ////

class PriorityQueue
{
  private:
        int *items;
        int size;
        int max;
        int (*test)(int);

  public:
        PriorityQueue(int (*test)(int), int size);
        ~PriorityQueue();
        int pop();
        int peek();
        int push(int item);
        int isEmpty();
        int isFull();
};
/**********    Classes    **********/
class Event
{ //
  private:
	int flag;		  // arrived flag
	void *data;		  // the parameter
	char source;	  // source task name
	char dest;		  // destination task name
	int senderWait;   // tells if the sender has to wait till the reciver receives
	int testAndSet(); // test and set 'flag' in an 'atomic transaction'
  public:
	Event();
	// sends an event to 'dest', if 'synch' is true waits until the receiver gets it.
	void send(char dest, void *param, int synch);
	// waits for the event, and returns the parameter.
	void *wait(char &sourceP); //'sourceP' will contain the task sender ID.
	// check for for an arrived event. 'sourceP' will contain the task sender ID.
	int arrived(char &sourceP);
	// resets the event
	void reset();
};

// SMARTS manages an entry of this type for each task
class Task
{
	void far *code;
	void far *userTaskEnd;
	Mutex *ownedMutexes [MAX_MUTEXES];
	int ownedMutexesSize;

  public:
	void addOwnedMutex(Mutex *);
	void removeOwnedMutex(Mutex *); //this also updates the owner's priority
	void reDeclare();
	int cyclesNum;			//@@@@@@ cycles num (ex1)
	int cycleTime;			//@@@@@@ cycles time (ex1)
	int nextCycleCountdown; //@@@*
	int cyclesNumCountdown; //@@@*
	Mutex *waitingFor;
	char name;					 // task name
	unsigned stack[MaxStack];	// the stack
	unsigned stackSeg, stackPtr; // the stack pointer
	int currentPriority;		 // the priority of the task
	int originalPriority;		 // the original priority
	taskStatus status;			 // the status of the task
	Event *expectedEvent;		 // the event that this task is waiting for
	int sleepCount;				 // the sleep remainder time
	Task();
	void declare(void far *code, void far *taskEnd, char name, int cycleTime, int cyclesNum); //@@@@@@ ADDED 2 ARGS (LAST 2)
	void sleepDecr();
	void incrPriority(int priority);
	void setToOriginalPriority();

	int getOriginalPriority();
};
//SMARTS uses this mutex
class Mutex //@@ex2	(implemented with a Proirity Queue)
{
  private:
	int size; // = totalTasks
	int available;
	int numberSuspended;
	int owner;
	int level;
	//int* queue;
	PriorityQueue queue;			 //2d array to save each task's {taskNum, Priority}
	void push(int task); //puts task into the queue. task consists of {taskNum, Priority}
	int pop();			 //gets the highest priority task
	void fixQueue(int);  //make all elements gathered together after "Get" function (so there wont be holes)
  public:
	void updateOwnerPriority();
	Mutex();
	void Acquire();
	void Release();
};
//                           Main class
class Parallelism
{
  private:
	Task context[MaxTask];		  // all tasks entries
	Task contextSched;			  // scheduler entry
	unsigned schedCopy[MaxStack]; // a copy of the scheduler stack
	int timeFromStart;
	int totalTasks;					 // total declared tasks
	int currentTask;				 // time slice holding task
	int deadlock;					 // deadlock flag
	int progInt;					 // 'program interrupt' flag
	int contextSwitchFlag;			 // enables/disables context switch
	int contextSwitchFlag2[MaxTask]; //@@ex2 (make each task a flag)
	int endOfTimeSlice;				 // indicates that has to be a context switch as soon as possible
									 // a pointer to the new timer interrupt handler
	int isThereCyclePassedError;
	void interrupt (*timerInterruptHandler)(...);
	// a pointer to the original BIOS timer interrupt handler
	void interrupt (*userIntAddress)(...);
	void far *scheduler;   // a pointer to the scheduler
	void far *userTaskEnd; // a pointer to the function called at the end of each task
	void cyclePassedError(char);
	int cyclesRemainingTasks;
	char errorTaskName;
	//array of mutexes owned by this task

  public:
	void addOwnedMutex(int i, Mutex *);
	void removeOwnedMutex(int i, Mutex *); //this also updates the owner's priority
	int getTimePassed();
	void printTimePassed();
	Mutex *getWaitingForMutex(int taskNum);
	void updateWaitingForMutex(int taskNum, Mutex *);
	int getIsThereCyclePassedError();
	int sleepTasks;			   // indicates how many task is sleeping, and it can't be deadlock
	int activeTasks;		   // indicates how many task is activate
	int traceInd;			   // debugging index
	long TScount;			   // counts the clocks of the current task
	unsigned timerClocksBegin; // time start of the current task
	unsigned timerClocksEnd;   // time finish of the current task
	int far (*algorithm)();	// short-term scheduler algorithm function
	Parallelism();
	// sets the addresses of the external functions
	void externalFunctions(void interrupt (*timerInterruptHandler)(...),
						   void far *scheduler, void far *userTaskEnd,
						   int far (*algorithm)());
	int declareTask(void far *code, char name, int cycleTime, int cyclesNum); //@@@@@@ ADDED 2 ARGS (LAST 2) // inserts a new task entry
	void runTheTasks();														  // start running the tasks
	void callScheduler();													  // return to the control to the scheduler
	void restoreSchedStack();												  // restore the 'scheduler' stack
	int getCurrentTask();													  // get the current running task entry number
	void setCurrentTask(int taskNum);										  // set the next running task number
	int getTotalTasks();													  // get total declared tasks
	int getDeadlock();														  // get deadlock flag
	void setDeadlock();														  // set deadlock flag
	int contextSwitchOn();													  // enable context switch
	void contextSwitchOff();												  // disable context switch
	int getContextSwitch();													  // get 'context switch' flag status
	void setProgInt();														  // set the 'program interrupt' flag
	void resetProgInt();													  // clear the 'program interrupt' flag
	int getProgInt();														  // get the 'program interrupt' flag status
	void setEndOfTimeSlice();												  // set the 'end of time slice' flag
	char getName(int taskNum);
	char getCurrentName();
	taskStatus getStatus(int taskNum);
	taskStatus getCurrentStatus();
	void resume(int taskNum);
	void resume(char taskName);
	void setCurrentNotActive();
	void suspended();
	void incrPriority(int taskNum, int priority);
	void setToOriginalPriority(int taskNum);
	void setTasklToOriginalPriority(int i);
	Event *getExpectedEvent(int taskNum);
	Event *getCurrentExpectedEvent();
	void setCurrentExpectedEvent(Event *expectedEvent);
	void sleep(int t); // sleep current task for 't' milliseconds
	void sleepDecr(int taskNum);
	void getCurrentStack(unsigned &StackSeg, unsigned &StackPtr);
	void setCurrentStack(unsigned StackSeg, unsigned StackPtr);
	void getSchedStack(unsigned &StackSeg, unsigned &StackPtr);
	void handleTimers();
	void taskEnd();
	void countdownTasks();					//@@@* countdown the time for each task to run
	int getNextCycleCountdown(int taskNum); //@@@@@@ added helper func
	int getCurrentPriority(int taskNum);	//@@ex2
};

//=======================
/**********    Functions    **********/
// reads the number of the remaining clocks from the timer register
extern unsigned getTimerClocks();
;
// external functions for 'externalFunctions' function
void far interrupt timerInterruptHandler(...);
void scheduler();
void myTaskEnd();
// 'short term' scheduler algorithms
int roundRobin();
int EDF(); //@@@@@@ ADDED (ex1)
		   // main scheduler object
int RMS(); //@@ex2
extern Parallelism SMARTS;
//extern Mutex mutex; //@@ex2 - probably needed
