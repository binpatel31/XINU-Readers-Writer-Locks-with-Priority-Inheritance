// lock.c
#include <kernel.h>

#include <q.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

int lock(int ldes1, int type, int priority) 
{

    STATWORD ps; 

    disable(ps);	
//   struct  lentry  *lptr;
//    struct  lentry  *tmplptr;
//    struct  pentry  *pptr;
    
// int lock = (int)(ldes1/10);
// int locki= ldes1 - lock * 10;
    
	int lock=ldes1;
    int wait = 0;
    int item, i;
    int tmpprio;

    
    
   // lptr = &lock_table[lock];
 
    if(lock<0 || lock>=NLOCK)
    {
		restore(ps);
		return(SYSERR);
    }

    if (lock_table[lock].lstate == FREE) 
	{
        restore(ps);
        return(SYSERR);
    }
	
	//temp_num_reader = lock_table[lock].num_reader;
    
// Verify the lock request is in current locks iteration
//    if (locki != lockiter) {
//        restore(ps);
//        return(SYSERR);
//    }
	

    if (lock_table[lock].num_reader == 0 && lock_table[lock].num_writer == 0) {
        // no lock at all
        wait = 0;
    }
    else if ( (lock_table[lock].num_reader == 0 && lock_table[lock].num_writer != 0) || (lock_table[lock].num_reader != 0 && lock_table[lock].num_writer == 0 && type == WRITE) ) 
	{
        // write lock or read lock and requested by write
        wait = 1;   
    }
    else if (lock_table[lock].num_reader != 0 && lock_table[lock].num_writer == 0 && type == READ) 
	{
    //	item = q[lptr->lqtail].qprev;
		for(item=q[lock_table[lock].lqtail].qprev; priority < q[item].qkey; item = q[item].qprev )
		{
			if (q[item].qtype == WRITE)
			{
					wait = 1;
					break;
			}
		}
/*    
    while (priority < q[item].qkey) { 
	    // check if need to wait for higher priority WRITE
            if (q[item].qtype == WRITE)
                wait = 1;
            item = q[item].qprev;
        }
*/
}
    if (wait == 1) 
	{
       // pptr = &proctab[currpid];
        proctab[currpid].pstate = PRLOCK;
        proctab[currpid].plock  =   ldes1 ;//(int)(ldes1 / 10);

        insert(currpid, lock_table[lock].lqhead, priority);

        q[currpid].qtype   = type;
        q[currpid].qtime   = ctr1000;
        proctab[currpid].plockret     = OK;

        // update lprio (The current inherited priority of the process)
        update_lprio(lock);

        // update pinh for all procs that hold this lock.
        //tmplptr = &lock_table[lock];
        for(i=0; i<NPROC; i++) {
            if(lock_table[lock].process_holding_lock[i] > 0)
                update_pinh(i);
        }

        resched();
        restore(ps);
        return proctab[currpid].plockret;
    }

    lock_table[lock].process_holding_lock[currpid]          = 1;
    proctab[currpid].lockheld[lock] = 1;
    update_pinh(currpid);

    if (type == READ)
        lock_table[lock].num_reader++;
    else
        lock_table[lock].num_writer++;
    
    restore(ps);
    return(OK);
}

void update_lprio(int lid)
{
    int item, maxprio = -1;
  //  struct lentry *lptr = &lock_table[lid];

    int tmplockid = q[lock_table[lid].lqtail].qprev;
	for(tmplockid = q[lock_table[lid].lqtail].qprev; tmplockid!= lock_table[lid].lqhead;  tmplockid = q[tmplockid].qprev)
	{
		if (proctab[tmplockid].pprio > maxprio)
		{
            maxprio = proctab[tmplockid].pprio;
		}
	}
/*
    while (tmplockid!= lock_table[lid].lqhead) {
        if (proctab[tmplockid].pprio > maxprio)
            maxprio = proctab[tmplockid].pprio;
        tmplockid = q[tmplockid].qprev;
    }
*/
    lock_table[lid].lprio = maxprio;
}

void update_pinh(int pid)
{
    int i, maxprio = -1;
    struct pentry *pptr = &proctab[pid];

    for(i=0; i<NLOCK; i++) {
        // find lock held by the process
        if(pptr->lockheld[i] > 0) {
            if (lock_table[i].lprio > maxprio)
                maxprio = lock_table[i].lprio;
        }
    }

    if(pptr->pprio > maxprio)
        pptr->pinh = 0;
    else
        pptr->pinh = maxprio;

}
