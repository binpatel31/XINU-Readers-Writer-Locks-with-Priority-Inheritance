/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct  pentry *pptr;		/* points to proc. table for pid*/
        struct  lentry *lptr;

	int	dev;

	disable(ps);

	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
        /* fall through	*/
	//==========================================
        case PRLOCK:
                        dequeue(pid);
                        lock_table[pptr->plock].process_holding_lock[pid] = 0;
                        change_lck_proc_prio(pptr->plock);
                       // lptr = &lock_table[pptr->plock];
                        int i;
			while(i<NPROC)
			{
				if (lock_table[pptr->plock].process_holding_lock[i]>0)
				{
					change_pinh_proc(i);
				}
				i+=1;
			}
			//for(i=0; i<NPROC; i++) {
                         //   if(lptr->process_holding_lock[i] > 0)
                          //      change_pinh_proc(i);
                        //}

                        pptr->pstate = PRFREE;
                        break;

	//=========================================
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}
