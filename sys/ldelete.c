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
	struct	 lentry	*lptr;
	
	int	 pid;
    	int 	 lock = lockdescriptor;//(int)(lockdescriptor/10);
//	int	 locki= lockdescriptor - lock * 10;

	disable(ps);
	
	if(lock<0 || lock>=NLOCK)
	    {
        	restore(ps);
       	 	return(SYSERR);
    	    }


	if (lock_table[lock].lstate==FREE) {
		restore(ps);
		return(SYSERR);
	}
	
	lptr = &lock_table[lock];
	lptr->lstate = FREE;
	
//	if (locki != lockiter) {
//     	restore(ps);
//	        return(SYSERR);
//    	}
    
	if (nonempty(lptr->lqhead)) {
		while( (pid=getfirst(lptr->lqhead)) != EMPTY) {
		    proctab[pid].plockret = DELETED;
		    ready(pid,RESCHNO);
		}
		resched();
	}
	restore(ps);
	return(OK);
}
