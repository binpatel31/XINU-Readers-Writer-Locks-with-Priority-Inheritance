#ifndef _LOCK_H_
#define _LOCK_H_

#ifndef NLOCK
#define NLOCK 50
#endif

#define READ   1
#define WRITE  2

#define FREE  3
#define ACQUIRED  4

extern void change_lck_proc_prio(int lock_des_id);
extern void change_pinh_proc(int pid);

struct lentry {
	char lstate;          // FREE and ACQUIRED
    	
	//read or write constant
	int ltype;
	//readers
	int  num_reader;             

	//list of process whose ret type is DELETED if process delete lock
	int process_ret_del[NPROC];   	
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

extern lock_avail[NLOCK];
extern struct lentry lock_table[];

extern unsigned long ctr1000;



#endif
