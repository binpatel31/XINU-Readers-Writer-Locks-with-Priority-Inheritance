#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

void lock_reader (char *msg, int lck, int lprio, int sleeptime)
{
    kprintf ("  %s: Normal process MED with priority %d\n", msg, getprio(currpid));
    sleep (3);
    kprintf ("MED PROC === doing Remaining task\n");
    kprintf ("%s: to release its CPU\n", msg);
}

void lock_writer (char *msg, int lck, int lprio, int sleeptime, int inc)
{
    int ret;
    kprintf ("%s: request to acquire lock with priority %d\n", msg,getprio(currpid));
    if(inc != -1)
	{
		kprintf("Increase priority of LOW PRIOR %s to %d\n", proctab[inc].pname,getprio(currpid));
	}
    ret = lock (lck, WRITE, lprio);
    if(ret == OK)
	{
		kprintf ("%s: acquired the lock with priorty %d\n", msg,getprio(currpid));
  		sleep (sleeptime);
		kprintf("%s doing task first =====\n",msg);
    		sleep(sleeptime);
		kprintf("%s doing task second =====\n",msg);
		kprintf("%s: released the lock with priority %d\n",msg, getprio(currpid));
    		releaseall (1, lck);
		
	}
    if (ret == SYSERR) 
	{
        kprintf ("  %s: SYSERR\n", msg);
        return;
    }
    if (ret == DELETED) 
	{
        kprintf ("  %s: DELETED\n", msg);
        return;
    }
}
void semaph_reader (char *msg, int sem, int lprio, int sleeptime)
{
    kprintf (" %s: Normal process MED with priority %d\n", msg, getprio(currpid));
    kprintf ("%s: to release its CPU\n", msg);
}

void semaph_writer (char *msg, int sem, int lprio, int sleeptime)
{
    int ret;
    kprintf ("%s: request to acquire lock with priority %d\n", msg,getprio(currpid));
    // if(inc != -1)
    ret = wait(sem);
    if(ret == OK)
        {
                kprintf("%s: acquired the lock with priorty %d\n", msg,getprio(currpid));
                kprintf("%s doing task first =====\n",msg);
                //sleep(sleeptime);
                sleep(2);
                kprintf("%s doing task second =====\n",msg);
                kprintf("%s: released the lock with priority %d\n",msg, getprio(currpid)); 
                signal(sem);

        }
}

//==================================================================================
int main()
{ 
	kprintf("Using Lock ===================================================\n");

    int lock  = lcreate();                                      
    int low_prio_writer = create(lock_writer, 2000, 10, "writer", 5, "writer LOW", lock, 20, 1, -1);
    int med_prio_reader = create(lock_reader, 2000, 25, "reader", 4, "reader MED", lock, 20, 1);
    int high_prio_writer = create(lock_writer, 2000, 40, "writer", 5, "writer HIGH", lock, 20, 1, low_prio_writer);

    kprintf("Starting LOW PRIORITY PROCESS\n");
    resume(low_prio_writer);
    sleep(1);
//============
    kprintf("Starting MEDIUM PRIORITY PROCESS\n");
    resume(med_prio_reader);
    sleep(1);
//============

    kprintf("Starting HIGH PRIORITY PROCESS\n");
    resume(high_prio_writer);
    sleep(1);
//===========

    sleep(1);

    kprintf ("Lock Finished =================================================!\n\n\n");

	kprintf("=========================================================================================\n\n");

    kprintf("Using Semaphores=================================================\n");
    
    int sem  = screate(1);
    low_prio_writer = create(semaph_writer, 2000, 10, "writer", 3, "writer LOW", sem, 1);
    med_prio_reader = create(semaph_reader, 2000, 25, "reader", 3, "reader MED", sem, 1);
    high_prio_writer = create(semaph_writer, 2000, 40 ,"writer",3,  "writer HIGH", sem, 1);

    kprintf("Starting LOW PRIORITY PROCESS\n");
    resume(low_prio_writer);
    // running low prio process which will execute after medium prio process...so differ from lock
    sleep(1);

    kprintf("Starting MEDIUM PRIORITY PROCESS\n");
    resume(med_prio_reader);
    sleep(1);
	
    kprintf("Starting HIGH PRIORITY PROCESS\n");
    resume(high_prio_writer);
    sleep(1);
    sleep(1);

    kprintf ("\nSemaphore Finished ===========================================\n");

    return 0;


}
