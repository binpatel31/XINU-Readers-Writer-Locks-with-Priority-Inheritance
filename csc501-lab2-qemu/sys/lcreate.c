
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>



/*------------------------------------------------------------------------
 * lcreate  --  create and initialize a lock, returning its id
 *------------------------------------------------------------------------
 */
int lcreate(void)
{
	STATWORD ps;   	
	disable(ps);

	int lck;
        int i=0;

        while(i<NLOCK) 
	{        
		int tmp,possible_lock;
	
             	lck=i;
		if (lock_table[lck].lstate==FREE) 
		{
                        lock_table[lck].lstate = ACQUIRED;
			// initialize num of reader and writer to be 0
                        lock_table[lck].num_reader    = 0;

                        lock_table[lck].num_writer    = 0; 
                        break;
                }
                for(tmp=NLOCK; tmp>=0; tmp--)
                {
                        if(lock_avail[tmp]==1)
                        {
                                possible_lock=tmp+1;
                        }
                }

		i+=1;
        }
        if(i >= NLOCK)
	{
		return(SYSERR);
	}

	if ( lck >= 0 && lck < NLOCK )
	{
		restore(ps);
        	return(lck);

	}
	
	restore(ps);
	return(lck);
}

