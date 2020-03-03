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
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
        int pid, tmpprio, maxprio = -1, maxpid = 0;

        optr = &proctab[currpid];
    
        if ((optr->pstate == PRCURR) && isempty(rdyhead))
            return OK;

        pid = q[rdytail].qprev;
        while (pid != rdyhead) {
            tmpprio = proctab[pid].pinh == 0 ? proctab[pid].pprio : proctab[pid].pinh;
            if (tmpprio > maxprio) {
                maxprio = tmpprio;
                maxpid = pid;
            }
            pid = q[pid].qprev;
        }


	/* no switch needed if current process priority higher than next*/

	if ( ( optr->pstate == PRCURR) && maxpid < (optr->pinh == 0 ? optr->pprio : optr->pinh) ) {
		return(OK);
	}
	
	/* force context switch */

	if (optr->pstate == PRCURR) {
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */
	currpid = maxpid;
        dequeue(maxpid);
	nptr = &proctab[ (currpid = maxpid) ];
        nptr = &proctab[currpid];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
}
