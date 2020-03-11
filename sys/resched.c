/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	
	register struct	pentry	*nptr;	
        int proc;

        optr = &proctab[currpid];
    
        if ((optr->pstate == PRCURR) && isempty(rdyhead))
            return OK;

	// find max prio  pid process 
        proc = q[rdytail].qprev;
	int max_prio = -1;
	int max_pid = -1;
        while (proc != rdyhead) 
	{
		int cur; 
            	if (proctab[proc].pinh == 0)
		{
			cur = proctab[proc].pprio;
		}
		else
		{
			cur = proctab[proc].pinh;
		}
            	if (cur > max_prio) 
		{
                	max_prio = cur;
                	max_pid = proc;
		}
            	proc  = q[proc].qprev;
        }

	/* no switch needed if current process priority higher than next*/

	if (( optr->pstate == PRCURR) && max_pid < (optr->pinh == 0 ? optr->pprio : optr->pinh) ) {
		return(OK);
	}
	
	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */
	currpid = max_pid;
        dequeue(max_pid);
//	nptr = &proctab[ (currpid = max_pid) ];
        nptr = &proctab[currpid];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
}
