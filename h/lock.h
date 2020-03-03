#ifndef _LOCK_H_
#define _LOCK_H_

#ifndef NLOCK
#define NLOCK 50
#endif

#define READ   1
#define WRITE  2

#define FREE  3
#define ACQUIRED  4

struct lentry {
	char lstate;          // FREE and ACQUIRED
    	
	//read or write constant
	int ltype;
	//readers
	int  num_reader;             
   	
	//writers
	int  num_writer;             // Count of writers
   	
	int process_priority[NPROC];	 
   	int  lqhead;       
	
	int process_waiting_queue[NPROC]; 
	int  lqtail;        
	
	int waiting_time_process[NPROC];
   	
    	int  lprio;           // Maximum priority among all processes waiting in the queue
    	int  process_holding_lock[NPROC];
};


extern struct lentry lock_table[];
extern int nextlock;

extern unsigned long ctr1000;

extern abs(int arg);

//extern void linit(void);
//extern int  lcreate(void);
//extern int  ldelete(int lockdescriptor);
//extern int  lock(int ldes1, int type, int priority);
//extern int  releaseall(int numlocks, int ldes1, ...);
extern void update_lprio(int lid);
extern void update_pinh(int pid);


#endif
