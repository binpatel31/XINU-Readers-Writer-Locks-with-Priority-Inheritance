#include <kernel.h>
#include <proc.h>
#include <stdio.h>

#include <q.h>
#include <lock.h>
//#include <stdlib.h>

void allow_reader(int lock) 
{
    // find high priority waiting writer and allow all process that want reader with prio higher than writer  
	//===========================================
	int proc;
	int max_prio_writer = -100;

	// find the highest-priority waiting writer
	for (proc = q[lock_table[lock].lqtail].qprev; proc != lock_table[lock].lqhead; proc = q[proc].qprev)
	{ 
		if(q[proc].qtype == WRITE)
		{
			if(q[proc].qkey > max_prio_writer)
			{
				max_prio_writer = q[proc].qkey;
			}
		}
	}
	//=========================================
    int tmpitem;

    // reader with priority higher than writer...allow all such readers 

    for(proc = q[lock_table[lock].lqtail].qprev; proc != lock_table[lock].lqhead;proc=proc)
	{
	    if (q[proc].qtype == READ) 
		{
			if(q[proc].qkey >= max_prio_writer)
			{
				tmpitem = q[proc].qprev;
			
				lock_table[proc].process_holding_lock[proc] = 1;
				if (q[proc].qtype == READ)
				{
					lock_table[lock].num_reader++;
					proctab[currpid].have_lock[lock] = 1;
				}
				else if (q[proc].qtype == WRITE)
				{
					lock_table[lock].num_writer++;
					proctab[currpid].have_lock[lock] = 1;
				}
				change_lck_proc_prio(lock);
				int temp_p = 0, prio_temp;
				while(temp_p<NPROC) 
				{
					if(lock_table[lock].process_holding_lock[temp_p] > 0)
					{
						change_pinh_proc(temp_p);
					}
					else
					{
						prio_temp = lock_table[lock].process_holding_lock[temp_p];
					}
					temp_p+=1;
				}

				dequeue(proc);
				ready(proc, RESCHNO);
				//==========================================================================================
				proc = tmpitem;
			}
			///=====
        }
    }
}

int releaseall(numlocks, ldes1)
{
    STATWORD ps;  
	disable(ps);	
 
    int i;
    int priority;
    int lock;
    int choice;
    
	int anyerr = 0;
	int item;
    
    for (i = 0; i < numlocks; i++) 
	{
		// next lock in parameter
        lock = (int)(*((&ldes1) + i)); 
        priority     = 0;
		
		choice       = 0;

        if(lock<0 || lock>=NLOCK)
	    {
        	restore(ps);
        	return(SYSERR);
    	    }

        if (lock_table[lock].lstate==FREE) 
	{
            anyerr = 1;
            continue;
        }

        if (lock_table[lock].num_writer > 0)
	{
            lock_table[lock].num_writer--;
        }
	else if(lock_table[lock].num_reader > 0)
	{
            lock_table[lock].num_reader--;
	}
        
	lock_table[lock].process_holding_lock[currpid] = 0;
		
        proctab[currpid].have_lock[lock] = 0;
        change_pinh_proc(currpid);
		
	//as pinh is updated 
	proctab[currpid].prio_changed=1;
	
        if (isempty(lock_table[lock].lqhead))
		{
            continue;
		}
        // if there are process then choose proc having high priority
        item = q[lock_table[lock].lqtail].qprev;

     
        if (q[q[lock_table[lock].lqtail].qprev].qkey != q[q[q[lock_table[lock].lqtail].qprev].qprev].qkey)    // if more than 2 has same priority
		{
			//item do not have equal priority so check for individual ltypes
            if (q[item].qtype == READ) 
			{
				if(lock_table[lock].num_writer == 0)
				{
					//==========================
					int proc;
					int max_prio_writer = -100;

					// find the highest-priority waiting writer
					for (proc = q[lock_table[lock].lqtail].qprev; proc != lock_table[lock].lqhead; proc = q[proc].qprev)
					{ 
						if(q[proc].qkey > max_prio_writer)
						{
							if(q[proc].qtype == WRITE )
							{
								max_prio_writer = q[proc].qkey;
							}
						}
					}

					// unblock all those readers whose priority is greater than writer
					int next_proc = q[lock_table[lock].lqtail].qprev;
					for(proc = q[lock_table[lock].lqtail].qprev; proc != lock_table[lock].lqhead;proc=proc)
					{ 
						if (q[proc].qkey >= max_prio_writer) 
						{
							if(q[proc].qtype == READ )
							{
								next_proc = q[proc].qprev;
								//======*****************************************************************8
								// update the reader/writer counters
								lock_table[lock].process_holding_lock[proc] = 1;
								if (q[proc].qtype == READ)
								{
									lock_table[lock].num_reader = lock_table[lock].num_reader +1;
									proctab[currpid].have_lock[lock] = 1;
								}
								else if (q[proc].qtype == WRITE)
								{
									lock_table[lock].num_writer = lock_table[lock].num_writer + 1;
									proctab[currpid].have_lock[lock] = 1;
								}
								change_lck_proc_prio(lock);
								int temp_p = 0, prio_temp;
								while(temp_p<NPROC) 
								{
									if(lock_table[lock].process_holding_lock[temp_p] > 0)
									{
										change_pinh_proc(temp_p);
									}
									else
									{
										prio_temp = lock_table[lock].process_holding_lock[temp_p];
									}
									temp_p+=1;
								}

								dequeue(proc);
								ready(proc, RESCHNO);	
								
								proc = next_proc;
							}
						}
					}
					
				}
            }
            else if (q[item].qtype == WRITE && lock_table[lock].num_writer == 0 && lock_table[lock].num_reader ==0) 
			{
			    lock_table[lock].process_holding_lock[item] = 1;

				lock_table[lock].num_writer = lock_table[lock].num_writer+1 ;
				proctab[currpid].have_lock[lock] = 1;

				change_lck_proc_prio(lock);
				int temp_p = 0, prio_temp;
				while(temp_p<NPROC) 
				{
					if(lock_table[lock].process_holding_lock[temp_p] > 0)
					{
						change_pinh_proc(temp_p);
					}
					else
					{
						prio_temp = lock_table[lock].process_holding_lock[temp_p];
					}
					temp_p+=1;
				}

				dequeue(item);
				ready(item, RESCHNO);	
			   //*****
            }
            continue;
        }
		else
		{
			
			int read_time     = -100;
			int write_time    = -100;
		
			//
			// process with longest waiting time should get.
			int p;
			p = q[item].qkey;
			if (p>=0)
			{
				priority = p;
			}
			unsigned long max_read_time=0, max_write_time=0;
			while (q[item].qkey == priority)    // equal priority so find with longest waiting time
			{		
				if (q[item].lck_qtime > max_read_time)
				{
					if (q[item].qtype == READ)
					{
						read_time = item;
					}
				} 
				else if (q[item].qtype == WRITE)
				{
					if(q[item].lck_qtime > max_write_time)
					{
						write_time = item;
					}
				}
				if (read_time >= 0) 
				{
					if(write_time >=0)
					{
						int diff = abs(max_write_time-max_read_time);
						if ( diff <= 400)   //grace period for reader  ..then give chance to reader
						{
							choice = read_time;	
						}
						else
						{
							if(max_read_time < max_write_time) 
							{
								choice = write_time;
							}
							else if(max_read_time > max_write_time) 
							{
								choice = read_time;
							}
						}
					//
					}
				}
				else
				{
					if (read_time >= 0 ) 
					{
						if(write_time == 0)
						{
							choice = read_time;
						}
					}
					else   //write time is there 
					{
						choice = write_time;
					}
				}
				item = q[item].qprev;
			}


			if (q[choice].qtype == READ && lock_table[lock].num_writer == 0) 
			{
				
				allow_reader(lock);    // reader will get chance
			}
			else if (q[choice].qtype == WRITE && lock_table[lock].num_writer == 0 && lock_table[lock].num_reader ==0) 
			{
				lock_table[lock].process_holding_lock[choice] = 1;
				lock_table[lock].num_writer = lock_table[lock].num_writer+1;
				proctab[currpid].have_lock[lock] = 1;

				change_lck_proc_prio(lock);
				int temp_p = 0, prio_temp;
				while(temp_p<NPROC) 
				{
					if(lock_table[lock].process_holding_lock[temp_p] > 0)
					{
						change_pinh_proc(temp_p);
					}
					else
					{
						prio_temp = lock_table[lock].process_holding_lock[temp_p];
					}
					temp_p+=1;
				}

				dequeue(choice);
				ready(choice, RESCHNO);
			}
		}
    }

    if(anyerr) 
	{
        restore(ps);
        return SYSERR;
    }

    restore(ps);
    resched();
    return(OK);
}


