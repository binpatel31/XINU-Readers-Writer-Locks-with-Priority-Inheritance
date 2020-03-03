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
    
	int lock=ldes1;
    int wait = 0;
    int item, i;
    int tmpprio;

    
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
	
   if (lock_table[lock].num_reader == 0 && lock_table[lock].num_writer != 0)
   {
	   wait=1;
	   goto here;
   }
   if (lock_table[lock].num_reader != 0 && lock_table[lock].num_writer == 0 && type == WRITE)
   {
	   wait=1;
   }
   else if(lock_table[lock].num_reader == 0)
   {
	   if(lock_table[lock].num_writer == 0)
	   {
			wait=0;
	   }
   }
    else if (lock_table[lock].num_reader != 0 && lock_table[lock].num_writer == 0) 
	{
		if(type == READ)
		{
			for(item=q[lock_table[lock].lqtail].qprev; priority < q[item].qkey; item = q[item].qprev )
			{
				if (q[item].qtype == WRITE)
				{
						wait = 1;
						break;
				}
			}
		}
	}
here:
    if (wait == 1) 
	{
        proctab[currpid].pstate = PRLOCK;
        proctab[currpid].plock  =   ldes1;

        insert(currpid, lock_table[lock].lqhead, priority);
		proctab[currpid].plockret     = OK;
		
        q[currpid].qtype   = type;
        q[currpid].qtime   = ctr1000;
        
        
        update_lprio(lock); 	// inherit the priority
		i=0;
        while(i<NPROC) 
		{
            if(lock_table[lock].process_holding_lock[i] > 0)   //pinh of all process.....transitivity
			{
                update_pinh(i);
			}
			i+=1;
		}

        resched();
        restore(ps);
        return proctab[currpid].plockret;
    }

    lock_table[lock].process_holding_lock[currpid] = 1;
    proctab[currpid].lockheld[lock] = 1;
    if(currpid >= 0)
	{
		update_pinh(currpid);
	}
    if (type == WRITE)
	{
        lock_table[lock].num_writer++;
	}
	else
	{
        lock_table[lock].num_reader++;
    	}
    restore(ps);
    return(OK);
}

void update_lprio(int lid)
{
    int item, maxprio = -1;
 
    int tmplockid = q[lock_table[lid].lqtail].qprev;
	for(tmplockid = q[lock_table[lid].lqtail].qprev; tmplockid!= lock_table[lid].lqhead;  tmplockid = q[tmplockid].qprev)
	{
		if (proctab[tmplockid].pprio > maxprio)
		{
            maxprio = proctab[tmplockid].pprio;
		}
	}
    lock_table[lid].lprio = maxprio;
}

void update_pinh(int pid)
{
    int i=0, maxprio = -1;
    struct pentry *pptr = &proctab[pid];
	
    while(i<NLOCK) 
	{
 
        if(pptr->lockheld[i] > 0 && lock_table[i].lprio > maxprio) 
	{
        	maxprio = lock_table[i].lprio;
	}
		i+=1;
    }

    if(pptr->pprio <= maxprio)
	{
        pptr->pinh = maxprio;
    }
	else
	{
        pptr->pinh = 0;
	}
}
