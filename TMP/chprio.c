/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);

	if (isbadpid(pid) || newprio<=0 || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;
	if(pptr -> pstate == PRREADY) {
		dequeue(pid);
		insert(pid, rdyhead, pptr -> pprio);
	}

        change_pinh_proc(pid);
        if (pptr->pstate == PROCESS_LOCK) 
	{
		if(pptr->prio_changed == 0)
		{
			pptr->prio_changed=1;
		}
            	change_lck_proc_prio(pptr->plock);
            	//updating new pinh
		change_pinh_proc(pptr->plock);
        }

	restore(ps);
	return(newprio);
}
