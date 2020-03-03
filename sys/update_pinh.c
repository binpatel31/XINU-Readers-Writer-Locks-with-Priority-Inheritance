#include <kernel.h>

#include <q.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

void update_pinh(int pid)
{
    	int i=0, maxprio = -1;
   	while(i<NLOCK)
        {

        	if(proctab[pid].lockheld[i] > 0 && lock_table[i].lprio > maxprio)
        	{
                	maxprio = lock_table[i].lprio;
        	}
                i+=1;
    	}

    	if(proctab[pid].pprio <= maxprio)
        {
        	proctab[pid].pinh = maxprio;
    	}
        else
        {
       		proctab[pid].pinh = 0;
        }
}

