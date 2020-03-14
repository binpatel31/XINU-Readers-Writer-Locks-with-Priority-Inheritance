
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>


int lock_avail[NLOCK];

void linit(void) 
{

    	int i=0;

   	//=========
    	for(i;i<NLOCK;i++)
	{
		lock_avail[i]=0;
	}
    	//========
	i=0;
    
	while(i < NLOCK) 
	{
        	lock_table[i].lstate = FREE;
		
		lock_table[i].lqhead = newqueue();
        	
		lock_table[i].lqtail = 1 + (lock_table[i].lqhead); 
			
		lock_table[i].lprio  = -1;	
                int j=0;
        
        	while (j<NPROC) 
		{
			//j=pid here and i = lock here
             		lock_table[i].process_holding_lock[j] = 0;
			lock_table[i].waiting_time_process[j] = 0;
			lock_table[i].process_ret_del[j]=0;
			j+=1;
        	}
		i+=1;
    	}
}
