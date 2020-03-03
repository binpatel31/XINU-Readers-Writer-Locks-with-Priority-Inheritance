// int lcreate(void)
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <stdio.h>
#include <lock.h>

LOCAL int newlock();

/*------------------------------------------------------------------------
 * lcreate  --  create and initialize a lock, returning its id
 *------------------------------------------------------------------------
 */
int lcreate(void)
{
	STATWORD ps;   	
	disable(ps);

	int lck;
//==============
        int i=0;
        while(i<NLOCK) 
	{

                lck=nextlock;
                if (nextlock < 0) 
		{
                        nextlock = NLOCK - 1;
                }
		nextlock-=1;
                if (lock_table[lck].lstate==FREE) 
		{
                        lock_table[lck].lstate = ACQUIRED;
                        lock_table[lck].num_reader    = 0;
                        lock_table[lck].num_writer    = 0;
                      
                        break;
                }
		i+=1;
        }
        if(i >= NLOCK)
	{
		return(SYSERR);
	}

//=============
//
	if ( lck >= 0 )
	{
		restore(ps);
        	return(lck);

	}
	else if ( lck == SYSERR ) {
		restore(ps);
		return(SYSERR);
	}
	
	restore(ps);
	return(lck);
}

/*------------------------------------------------------------------------
 * newlock  --  allocate an unused lock and return its index
 *------------------------------------------------------------------------
 */

/*
LOCAL int newlock()
{
	int lock;
	int i;

	for (i=0 ; i<NLOCK; i++) {
		lock=nextlock--;
	
		if (nextlock < 0) {
			nextlock = NLOCK - 1;
			//lockiter++;
		}
		
		if (lock_table[lock].lstate==FREE) {
			lock_table[lock].lstate = ACQUIRED;
			lock_table[lock].num_reader    = 0;
		        lock_table[lock].num_writer    = 0;

			//return(lock*10+lockiter);
			return lock;
		}
	}
	return(SYSERR);
}
*/
