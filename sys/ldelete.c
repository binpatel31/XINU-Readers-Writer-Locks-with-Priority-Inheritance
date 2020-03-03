//  int ldelete (int lockdescriptor)
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * ldelete  --  delete a lock by releasing its table entry
 *------------------------------------------------------------------------
 */
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

   //return type should be DELETED for those process who was blocked due to this lock and now this lock is deleted. 
	
	int pid;
	if (nonempty(lock_table[lockdescriptor].lqhead)) 
	{
		pid = getfirst(lock_table[lockdescriptor].lqhead);
		while(pid != EMPTY) 
		{
		    proctab[pid].plockret = DELETED;
		    ready(pid,RESCHNO);
		    pid = getfirst(lock_table[lockdescriptor].lqhead);
		}
		resched();
	}
	restore(ps);
	return(OK);
}
