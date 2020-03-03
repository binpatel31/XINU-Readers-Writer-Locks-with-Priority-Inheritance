#include <kernel.h>

#include <q.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>

void update_lprio(int lid)
{
    int item, maxprio = -1;
    int p=0;
    int tmplockid = q[lock_table[lid].lqhead].qnext;
        for(tmplockid = q[lock_table[lid].lqhead].qnext; tmplockid!= lock_table[lid].lqtail;  tmplockid = q[tmplockid].qnext)
        {
		if (proctab[tmplockid].pprio <= maxprio)
		{
			maxprio=p;
		}
               else// if (proctab[tmplockid].pprio > maxprio)
                {
            		maxprio = proctab[tmplockid].pprio;
			p=maxprio;
                }
        }
    lock_table[lid].lprio = maxprio;
}

