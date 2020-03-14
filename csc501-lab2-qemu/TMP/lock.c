
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
	int wait = 0;
    int item, i;


    
    if(ldes1<0 || ldes1>=NLOCK)
    {
		restore(ps);
		return(SYSERR);
    }

    if (lock_table[ldes1].lstate == FREE) 
	{
        restore(ps);
        return(SYSERR);
    }
	
   if (lock_table[ldes1].num_reader == 0 && lock_table[ldes1].num_writer != 0)
   {
	   wait=1;
	   goto here;
   }
   if (lock_table[ldes1].num_reader != 0 && lock_table[ldes1].num_writer == 0 && type == WRITE)
   {
	   wait=1;
   }
   else if(lock_table[ldes1].num_reader == 0)
   {
	   if(lock_table[ldes1].num_writer == 0)
	   {
			wait=0;
	   }
   }
    else if (lock_table[ldes1].num_reader != 0 && lock_table[ldes1].num_writer == 0) 
	{
		if(type == READ)
		{
			for(item=q[lock_table[ldes1].lqtail].qprev; priority < q[item].qkey; item = q[item].qprev )
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
        proctab[currpid].pstate = PROCESS_LOCK;
        proctab[currpid].plock = ldes1;

        insert(currpid, lock_table[ldes1].lqhead, priority);
		proctab[currpid].plockret = OK;
		
        q[currpid].qtype = type;
        
        
        
        // inherit the priority
		//########################################
		int p=-1;
		q[currpid].lck_qtime = ctr1000;
		int tmplockid = q[lock_table[ldes1].lqhead].qnext;
		for(tmplockid = q[lock_table[ldes1].lqhead].qnext; tmplockid!= lock_table[ldes1].lqtail;  tmplockid = q[tmplockid].qnext)
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
		lock_table[ldes1].lprio = maxprio;
		
		//########################################
		i=0;
        while(i<NPROC) 
		{
            if(lock_table[ldes1].process_holding_lock[i] > 0)   //pinh of all process.....transitivity
			{
        		//##########################3
				int k=0;
				maxprio = -1;
				
				while(k<NLOCK) 
				{
			 
					if(proctab[i].have_lock[k] > 0 && lock_table[k].lprio > maxprio) 
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

    	lock_table[ldes1].process_holding_lock[currpid] = 1;
    	lock_table[ldes1].process_waiting_queue[currpid] = 1; 
	proctab[currpid].have_lock[ldes1] = 1;
    
	if(currpid >= 0)
	{
		 
		//###################################
		int k=0;
		maxprio = -1;
		
		
		while(k<NLOCK) 
		{
	 
			if(proctab[currpid].have_lock[k] > 0 && lock_table[k].lprio > maxprio) 
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
        lock_table[ldes1].num_writer++;
	}
	else
	{
        lock_table[ldes1].num_reader++;
    	}
    restore(ps);
    return(OK);
}
