// void linit(void)
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>


int nextlock;


void linit(void) 
{
    int i=0;

    nextlock = NLOCK - 1;


    while(i < NLOCK) 
	{
        	lock_table[i].lstate = FREE;
		
		lock_table[i].lqhead = newqueue();
        	
		lock_table[i].lqtail = 1 + (lock_table[i].lqhead); 
			
		lock_table[i].lprio  = -1;	
                
		int j=0;
        
        	while (j<NPROC) 
		{
             		lock_table[i].process_holding_lock[j] = 0;
			j+=1;
        	}
		i+=1;
    	}
}
