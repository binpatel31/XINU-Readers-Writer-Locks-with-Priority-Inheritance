#include <kernel.h>

#include <q.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

void change_lck_proc_prio(int lid)
{
    int maxprio = -1;
    int p=-1;
    int proc;// = q[lock_table[lid].lqhead].qnext;
        for(proc = q[lock_table[lid].lqhead].qnext; proc!= lock_table[lid].lqtail;  proc = q[proc].qnext)
        {
		if (proctab[proc].pprio <= maxprio)
		{
			maxprio=p;
		}
               else// if (proctab[tmplockid].pprio > maxprio)
                {
            		maxprio = proctab[proc].pprio;
			p=maxprio;
                }
        }
    lock_table[lid].lprio = maxprio;
}

