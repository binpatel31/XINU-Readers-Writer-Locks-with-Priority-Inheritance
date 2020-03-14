
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

#include<stdio.h>

#include <lock.h>

int ldelete (int lockdescriptor)
{
	STATWORD ps;    
	disable(ps);
	struct	 lentry	*lptr;
	
	
	if(lockdescriptor<0 || lockdescriptor>=NLOCK)
	    {
        	restore(ps);
       	 	return(SYSERR);
    	    }


	if (lock_table[lockdescriptor].lstate==FREE) {
		restore(ps);
		return(SYSERR);
	}

	lock_table[lockdescriptor].lstate = FREE;
	lock_avail[lockdescriptor]=0;

       // return type of process should be DELETED for those process who is waiting for lock which is DELETED 
	
	int pid;
	if (nonempty(lock_table[lockdescriptor].lqhead)) 
	{
		pid = getfirst(lock_table[lockdescriptor].lqhead);
		while(pid != EMPTY) 
		{
		    proctab[pid].plockret = DELETED;
		    ready(pid,RESCHNO);
		    pid = getfirst(lock_table[lockdescriptor].lqhead);
		    lock_table[lockdescriptor].process_ret_del[pid]=1;
		}
		resched();
	}
	restore(ps);
	return(OK);
}
