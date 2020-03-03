
#include <kernel.h>

#include <q.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

int lock(int ldes1, int type, int priority) 
{

    STATWORD ps; 

    disable(ps);	
    
	int  maxprio = -1;
	
	int lock=ldes1;
    int wait = 0;
    int item, i;


    
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
        proctab[currpid].plock = ldes1;

        insert(currpid, lock_table[lock].lqhead, priority);
		proctab[currpid].plockret = OK;
		
        q[currpid].qtype = type;
        q[currpid].qtime = ctr1000;
        
        
        // inherit the priority
		//########################################
		int p=-1;
		int tmplockid = q[lock_table[lock].lqhead].qnext;
		for(tmplockid = q[lock_table[lock].lqhead].qnext; tmplockid!= lock_table[lock].lqtail;  tmplockid = q[tmplockid].qnext)
		{
			if(proctab[tmplockid].pprio <= maxprio)
			{
				maxprio = p;
			}
			else //if  (proctab[tmplockid].pprio > maxprio)
			{
				maxprio = proctab[tmplockid].pprio;
				p=maxprio;
			}
		}
		lock_table[lock].lprio = maxprio;
		
		//########################################
		i=0;
        while(i<NPROC) 
		{
            if(lock_table[lock].process_holding_lock[i] > 0)   //pinh of all process.....transitivity
			{
        		//##########################3
				int k=0;
				maxprio = -1;
				
				while(k<NLOCK) 
				{
			 
					if(proctab[i].lockheld[k] > 0 && lock_table[k].lprio > maxprio) 
					{
							maxprio = lock_table[k].lprio;       //lock_table[i].lprio
					}
						k+=1;
				}

				if(proctab[i].pprio <= maxprio)
				{
					proctab[i].pinh = maxprio;
				}
				else
				{
					proctab[i].pinh = 0;
				}
				//##########################3
			}
			i+=1;
		}

        resched();
        restore(ps);
        return proctab[currpid].plockret;
    }

    lock_table[lock].process_holding_lock[currpid] = 1;
    lock_table[lock].process_waiting_queue[currpid] = 1; 
	proctab[currpid].lockheld[lock] = 1;
    
	if(currpid >= 0)
	{
		 
		//###################################
		int k=0;
		maxprio = -1;
		
		
		while(k<NLOCK) 
		{
	 
			if(proctab[currpid].lockheld[k] > 0 && lock_table[k].lprio > maxprio) 
			{
					maxprio = lock_table[k].lprio;
			}
				k+=1;
		}

		if(proctab[currpid].pprio <= maxprio)
		{
			proctab[currpid].pinh = maxprio;
		}
		else
		{
			proctab[currpid].pinh = 0;
		}
		//###################################
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
