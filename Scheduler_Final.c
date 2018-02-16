#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//struct holds each specific process' information
struct Process{
    char pName[10];
    int arrival;
    int burst;
    int wait;
    int completion;
    int turnaround;
    int index;
    int leftBurst;
};

//implements Queue for sjf and rr
struct QNode{
	struct Process prc;
	struct QNode *next;
};

struct Queue{
	struct QNode *front;
};

//creates Node
struct QNode* newNode(struct Process prc){
	
	struct QNode *temp = malloc(sizeof(struct QNode));
	
	temp->prc = prc;
	temp->next = NULL;
	
	return temp;
}

//creates Queue
struct Queue* createQueue(){
	
	struct Queue *q = malloc(sizeof(struct Queue));
	
	q->front = NULL;
	
	return q;
}

//check if queue is empty
int isEmpty(struct Queue *q){
	return (q->front == NULL);
}

//deQueues
struct QNode* deQueue(struct Queue *q){
	if(q->front == NULL)
	{
		return NULL;
	}
	
	struct QNode *temp = q->front;
	q->front = q->front->next;
	
	return temp;
}

//special enQueue for sjf
void enQueue_sjf(struct Queue *q, struct Process prc){
	struct QNode *temp = newNode(prc);
	
	if (q->front == NULL)
	{
	   q->front = temp;
	   return;
	}
	
	if(q->front->prc.burst > temp->prc.burst)
	{
		temp->next = q->front;
		q->front = temp;
	}
	
	else
	{
		struct QNode *current = q->front;
		struct QNode *prev = NULL;
		
		while(current != NULL && current->prc.burst < temp->prc.burst)
		{
			prev = current;
			current = current->next;
		}
		
		prev->next = temp;
		temp->next = current;
	}
}

void enQueue_RR_first(struct Queue *q, struct Process prc)
{
	struct QNode *temp = newNode(prc);
	
	
	if (q->front == NULL)
	{
	   q->front = temp;
	   return;
	}	
	else
	{
		temp->next = q->front;
		q->front = temp;	
	}
	
}

void enQueue_RR(struct Queue *q, struct Process prc)
{
	struct QNode *temp = newNode(prc);
	
	if (q->front == NULL)
	{
	   q->front = temp;
	   return;
	}	
	else
	{
		struct QNode *current = q->front;
		
		while(current->next != NULL)
		{
			current = current->next;
		}
		
		current->next = temp;
	}
}

//algorithm functions
void fcfs_wait_turnaround(struct Process *prc, int pCount);
void fcfs_schedule_print(struct Process *prc, int pCount, int runFor);

void sjf(struct Process *prc, int pCount, int runFor);
void sjf_wait_turnaround(struct Process *prc, int pCount);

void rr(struct Process *prc, int pCount, int runFor, int quantum);
void rr_wait_turnaround(struct Process *prc, int pCount);

int main() {
	/*
	* Open processes.in file and read values
	*/
    FILE *fp_in = fopen("processes.in", "r");
    
    if(fp_in == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
    
    char buffer[100], instruction[20], use[10], name[10];
    int pCount, runFor, qntm, arr, brst;
	
	//Get number of processes   
    fgets(buffer, 100, fp_in);
    sscanf(buffer, "%s %d", instruction, &pCount);
    
    struct Process prc[pCount];
    
    //get run-for time
    fgets(buffer, 100, fp_in);
    sscanf(buffer, "%s %d", instruction, &runFor);
    
    //get CPU Scheduling algorithm
    fgets(buffer, 100, fp_in);
    sscanf(buffer, "%s %s", instruction, use);
    
    if(strcmp(use, "rr") == 0)
    {
        fgets(buffer, 100, fp_in);
        sscanf(buffer, "%s %d", instruction, &qntm);
    }
    else
    {
        fgets(buffer, 100, fp_in);
    }
    
    //get name, arrival times, and burst times of processes
    int i;
    for(i = 0; i < pCount; i++)
    {
    	fgets(buffer, 100, fp_in);
        sscanf(buffer, "%s %s %s %s %d %s %d", buffer, buffer, name, buffer, &arr, buffer, &brst);
        strcpy(prc[i].pName, name);
        prc[i].arrival = arr;
        prc[i].burst = brst;
        prc[i].index = i;
    }
    
    fclose(fp_in);
    
    /*
	* Close file pointer 
	* Open file pointer for processes.out
	*/
    
    FILE *fp_out = fopen("processes.out", "w");
                
    if(fp_out == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
	
	//Begin printing results          
    fprintf(fp_out, "%d processes\n", pCount);
    
    if(strcmp(use, "rr") == 0)
    {
        fprintf(fp_out, "Using Round-Robin\n");
        fprintf(fp_out, "Quantum %d\n\n", qntm);
        fclose(fp_out);
        rr(prc, pCount, runFor, qntm);
		rr_wait_turnaround(prc, pCount);
    }
    else if(strcmp(use, "fcfs") == 0)
    {
        fprintf(fp_out, "Using First Come First Served\n\n");
        fclose(fp_out);
        fcfs_wait_turnaround(prc, pCount);
        fcfs_schedule_print(prc, pCount, runFor);
    }
    else
    {
        fprintf(fp_out, "Using Shortest Job First (Pre)\n\n");
        fclose(fp_out);
        sjf(prc, pCount, runFor);
        sjf_wait_turnaround(prc, pCount);
    }
    
    fp_out = fopen("processes.out", "a");
    
    //print final wait and turnaround values
    int j;
    for(j = 0; j < pCount; j++)
    {
        fprintf(fp_out, "%s wait %d turnaround %d\n", prc[j].pName, prc[j].wait, prc[j].turnaround);
    }
    
    fclose(fp_out);
    return 0;
}

void fcfs_wait_turnaround(struct Process *prc, int pCount){
	
	/*
	* Create copy of struct and order by arrival time
	*/
	struct Process *temp = malloc(pCount * sizeof(struct Process));
	memcpy(temp, prc, pCount * sizeof(struct Process));
	
	int i, j;
	for(i = 0; i < pCount - 1; i++)
	{
		for(j = 0; j < pCount - 1 - i; j++)
		{
			if(temp[j].arrival > temp[j+1].arrival)
			{
				struct Process temp2;
				temp2 = temp[j];
				temp[j] = temp[j+1];
				temp[j+1] = temp2;
			}
		}
	}
	
	/*
	* Find wait and turnaround values for each process
	* and put them back into the correct process
	*/
	for(i = 0; i < pCount; i++)
	{
		if(i == 0)
		{
			temp[i].wait = 0;
			temp[i].completion = temp[i].arrival + temp[i].burst;
			temp[i].turnaround = temp[i].completion - temp[i].arrival;
		}
		else
		{
			if(temp[i].arrival > temp[i - 1].completion)
			{
				temp[i].wait = 0;
				temp[i].completion = temp[i].arrival + temp[i].burst;
				temp[i].turnaround = temp[i].completion - temp[i].arrival;
			}
			else
			{
				temp[i].completion = temp[i - 1].completion + temp[i].burst;
				temp[i].turnaround = temp[i].completion - temp[i].arrival;
				temp[i].wait = temp[i].turnaround - temp[i].burst;
			}
		}
		
		prc[temp[i].index] = temp[i];
	}
}

void fcfs_schedule_print(struct Process *prc, int pCount, int runFor)
{
	FILE *fp_out = fopen("processes.out", "a");
	
	int time = 0, i, inProgress = 0, started = 0, completed = 0;
	
	while(time <= runFor)
	{
		for(i = 0; i < pCount; i++)
		{
			//check if process arrive and start scheduler
			if(prc[i].arrival == time)
			{
				fprintf(fp_out, "Time %d: %s arrived\n", time, prc[i].pName);
				inProgress++;
				started = 1;
				
				//select it if wait is 0
				if(prc[i].wait == 0)
				{
					fprintf(fp_out, "Time %d: %s selected (burst %d)\n", time, prc[i].pName, prc[i].burst);	
				}
			}
			
			//select if time matches when process is supposed to start
			if(prc[i].wait + prc[i].arrival == time && prc[i].wait != 0)
			{
				fprintf(fp_out, "Time %d: %s selected (burst %d)\n", time, prc[i].pName, prc[i].burst);	
			}
			
			//check if finished
			if(prc[i].completion == time)
			{
				fprintf(fp_out, "Time %d: %s finished\n", time, prc[i].pName);
				completed++;
				inProgress--;
			}
		}
		
		if( ((inProgress == 0 && started == 1) || completed == pCount)  && time != runFor )
		{
			fprintf(fp_out, "Time %d: IDLE\n", time);
		}

		time++;
		
	}
	
	fprintf(fp_out, "Finished at time %d\n\n", (time - 1));
	fclose(fp_out);
}

void sjf(struct Process *prc, int pCount, int runFor){
	
	FILE *fp_out = fopen("processes.out", "a");
	
	int time = 0, i;
	
	struct Queue *myQueue = createQueue();
	
	int current = -1, inProgress = 0, started = 0, completed = 0;
	
	while(time <= runFor)
	{
		//handle finish and decreasing
		if(current != -1)
		{
			prc[current].leftBurst--;
			if(prc[current].leftBurst == 0)
			{
				fprintf(fp_out, "Time %d: %s finished\n", time, prc[current].pName);
				prc[current].completion = time;
				
				current = -1;
				//signal completed
				completed++;
				inProgress--;
			}
		}
		
		for(i = 0; i < pCount; i++)
		{
			//if arrived, inform
			if(prc[i].arrival == time)
			{
				fprintf(fp_out, "Time %d: %s arrived\n", time, prc[i].pName);
				prc[i].leftBurst = prc[i].burst;
				//signal that shceduler started
				started = 1;
				
				//signal one more job added
				inProgress++;
				
				//if no current jobs are being processed, select it
				if(current == -1 && isEmpty(myQueue))
				{
					fprintf(fp_out, "Time %d: %s selected (burst %d)\n", time, prc[i].pName, prc[i].leftBurst);
					//make it the current
					current = prc[i].index;
				}
				//if a job is being processed, compare. 
				//if processed is longer, swap
				else
				{
					if(prc[current].leftBurst >= prc[i].leftBurst)
					{
						//make it the current, enqueue the old one
						fprintf(fp_out, "Time %d: %s selected (burst %d)\n", time, prc[i].pName, prc[i].leftBurst);
						enQueue_sjf(myQueue, prc[current]);
						current = prc[i].index;
					}
					//if the current job is shorter, enqueue the newly arrived value
					else
					{
						enQueue_sjf(myQueue, prc[i]);
					}
				}
			}
			
		}
		
		//check if the queue is empty
		// if not, and there's a job on the queue, select it
		if(isEmpty(myQueue) == 0 && current == -1)
		{
			struct QNode *nextJob = deQueue(myQueue);
			current = nextJob->prc.index;
			fprintf(fp_out, "Time %d: %s selected (burst %d)\n", time, nextJob->prc.pName, nextJob->prc.leftBurst);
		}
		
		if( ((inProgress == 0 && started == 1) || completed == pCount)  && time != runFor )
		{
			fprintf(fp_out, "Time %d: IDLE\n", time);
		}
		
		time++;
	}
	
	fprintf(fp_out, "Finished at time %d\n\n", (time - 1));
	fclose(fp_out);
}


void sjf_wait_turnaround(struct Process *prc, int pCount){
	
	int i;
	
	for(i = 0; i < pCount; i++)
	{
		prc[i].turnaround = prc[i].completion - prc[i].arrival;
		prc[i].wait = prc[i].turnaround - prc[i].burst;	
	}
}

void rr(struct Process *prc, int pCount, int runFor, int quantum){
	
	FILE *fp_out = fopen("processes.out", "a");
	
	int time = 0, i, timeStamp = -1;
	
	int started = 0, completed = 0, lastArrived = -1; 
	
	struct Queue *myQueue = createQueue();
	
	while(time <= runFor)
	{
		//printQueue(myQueue, time);
		//check if scheduler has started and is the beginning of quantum round
		if( (time - timeStamp) % quantum == 0 && started == 1)
		{
			//get next round
			int nextRound = time + quantum;
			int subTimer;
			
			struct QNode *current = deQueue(myQueue);
			int reQueue = 1, newTimeStamp;
			
			for(subTimer = time; subTimer < nextRound; subTimer++)
			{
				
				//loop through sub loop
				//check 2 things
				//1. if current process is done
				//2. if new process arrived
				if(subTimer == time)
				{

					//first, we must inform that a process has been selected
					fprintf(fp_out, "Time %d: %s selected  (burst %d)\n", time, current->prc.pName, current->prc.leftBurst);
				}
				
				//decrease burst
				if(subTimer != time)
				{
					current->prc.leftBurst--;
				}
				
				//check if it's done
				if(current->prc.leftBurst == 0)
				{
					//flag that element does not need to be requeued
					//inform that process is finsihed
					//change timeStamp
					//break out of the subLoop
					//mark completion time
					reQueue = 0;
					fprintf(fp_out, "Time %d: %s finished\n", subTimer, current->prc.pName);
					prc[current->prc.index].completion = subTimer;
					completed++;
					timeStamp = subTimer;
					break;
				}
				
				//check if any process arrived
				for(i = 0; i < pCount; i++)
				{
					if(subTimer == prc[i].arrival && i != lastArrived)
					{
						fprintf(fp_out, "Time %d: %s arrived\n", subTimer, prc[i].pName);
						prc[i].leftBurst = prc[i].burst;
						lastArrived = i;
						//Queue should be empty regardless
						if(subTimer == time)
						{
							enQueue_RR_first(myQueue, prc[i]);
						}
						else
						{
							enQueue_RR_first(myQueue, prc[i]);
						}
						
					}
				}
			}
			//check if the process is done at the change of rounds
			if(current->prc.leftBurst == 1)
			{
				//flag that element does not need to be requeued
				//inform that process is finsihed
				//change timeStamp
				//break out of the subLoop
				//mark completion time
				reQueue = 0;
				fprintf(fp_out, "Time %d: %s finished\n", subTimer, current->prc.pName);
				prc[current->prc.index].completion = subTimer;
				completed++;
				timeStamp = subTimer;
			}
			else
			{
				current->prc.leftBurst--;
			}
			
			//if element is not done, requeue and increase time
			if(reQueue == 1)
			{
				enQueue_RR(myQueue, current->prc);
				time = subTimer;
			}
			else
			{
				time = timeStamp;
			}
				
		}
		//if scheduler hasn't started, find start
		if(started == 0)
		{
			for(i = 0; i < pCount; i++)
			{
				if(time == prc[i].arrival)
				{
					fprintf(fp_out, "Time %d: %s arrived\n", time, prc[i].pName);
					prc[i].leftBurst = prc[i].burst;
					started = 1;
					timeStamp = time;
					
					lastArrived = i;
					//don't change time so the next while loop, it can be processed right away
					
					//Queue should be empty regardless
					enQueue_RR(myQueue, prc[i]);
				}
			}
			//move timer if nothing is found
			if(started == 0)
			{
				time++;
			}
		}
		
		//check if it's idle
		if(completed == pCount)
		{
			for(i = time; i < runFor; i++)
			{
				fprintf(fp_out, "Time %d: IDLE\n", i);
			}
			time = runFor + 1;
		}
	}
	
	fprintf(fp_out, "Finished at time %d\n\n", (time - 1));
	fclose(fp_out);
}

void rr_wait_turnaround(struct Process *prc, int pCount){
	
	int i;
	
	for(i = 0; i < pCount; i++)
	{
		prc[i].turnaround = prc[i].completion - prc[i].arrival;
		prc[i].wait = prc[i].turnaround - prc[i].burst;	
	}
}
