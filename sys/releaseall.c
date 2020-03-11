// releaseall.c
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <q.h>
#include <lock.h>

LOCAL void admitreader(int ldes);
LOCAL void unblock(int ldes, int item);

int releaseall(numlocks, ldes1)
    int numlocks;
    int ldes1;
{
    STATWORD ps;    
    register struct lentry *lptr;
    int i, item;
    int priority;
    int lock, locki;
    int longread, longwrite;
    int choice;
    unsigned long maxreadtime, maxwritetime;
    int anyerr = 0;

    disable(ps);
    
    for (i = 0; i < numlocks; i++) {

        lock = (int)(*((&ldes1) + i));  //(int)(*((&ldes1) + i) / 10);
        //locki= *((&ldes1) + i) - lock * 10;
        lptr = &lock_table[lock];
        priority     = 0;
	longread     = -1;
	longwrite    = -1;
	maxreadtime  = 0;
	maxwritetime = 0;
	choice       = 0;

        // verify lock
        if(lock<0 || lock>=NLOCK)
	    {
        	restore(ps);
        	return(SYSERR);
    	    }

        if (lptr->lstate==FREE) {
            anyerr = 1;
            continue;
        }

        // verify the lock request is in current locks iteration
//        if (locki != lockiter) {
//            anyerr = 1;
//            continue;
//        }

        // update the count for this resource
        if (lptr->num_writer > 0)
            lptr->num_writer--;
        else if(lptr->num_reader > 0)
            lptr->num_reader--;

        lptr->process_holding_lock[currpid] = 0;
        proctab[currpid].lockheld[lock] = 0;
        update_pinh(currpid);

        if (isempty(lptr->lqhead))
            continue;

        // choose proc with highest priority
        item = q[lptr->lqtail].qprev;

        // check if current item in queue has same priority with its prev 
        if (q[item].qkey != q[q[item].qprev].qkey) {
	    // no equal priority
            if (q[item].qtype == READ && lptr->num_writer == 0) {
                // chose READ to be the next
                admitreader(lock);
            }
            else if (q[item].qtype == WRITE && lptr->num_writer == 0 && lptr->num_reader ==0) {
                // chose WRITE to be next
                unblock(lock, item);
            }
            continue;
        }
	else{
	    // equal waiting priority, process with longest waiting time aquire the lock. 
            priority = q[item].qkey;
            
	    while (q[item].qkey == priority) { 

		// find longest READ and WRITE if any		
		if (q[item].qtype == READ && q[item].qtime > maxreadtime)
		    longread = item;
		else if (q[item].qtype == WRITE && q[item].qtime > maxwritetime)
		    longwrite = item;

		if (longread >= 0 && longwrite >=0) {
		    if ( abs(maxwritetime-maxreadtime) <= 400)   //grace period for reader
			{
				choice = longread;	
		    	}
		    else if(maxreadtime > maxwritetime) {
			choice = longread;
		    }
		    else if(maxreadtime < maxwritetime) {
			choice = longwrite;
		    }
		    
		}
		else if (longread >= 0 && longwrite == 0 ) {
		    choice = longread;
		}
		else if (longread == 0 && longwrite >= 0 ) {
                    choice = longwrite;
                }
		else {
		    // shouldn't be happening
		    choice = -1;
		}

                item = q[item].qprev;
            }


    	    if (q[choice].qtype == READ && lptr->num_writer == 0) {
		// chose READ to be the next
		admitreader(lock);
	    }
	    else if (q[choice].qtype == WRITE && lptr->num_writer == 0 && lptr->num_reader ==0) {
		// chose WRITE to be next
		unblock(lock,choice);
	    }
	}
    }

    if(anyerr) {
        restore(ps);
        return SYSERR;
    }

    restore(ps);
    resched();
    return(OK);
}

LOCAL void admitreader(int lock) {
    // if a reader is chosen to have a lock, then all the other waiting readers having priority not less than that of the highest-priority waiting writer for the same lock should also be admitted. 

    register struct lentry *lptr = &lock_table[lock];
    int item, tmpitem, threshold = -100000;

    // find the highest-priority waiting writer
    item = q[lptr->lqtail].qprev;
    while (item != lptr->lqhead) { 
        if (q[item].qtype == WRITE && q[item].qkey > threshold)
            threshold = q[item].qkey;
        item = q[item].qprev;
    }

    item = q[lptr->lqtail].qprev;
    while (item != lptr->lqhead) { 
        if (q[item].qtype == READ && q[item].qkey >= threshold) {
	    tmpitem = q[item].qprev;
            unblock(lock, item);
            item = tmpitem;
        }
    }
}

LOCAL void unblock(int lock, int item) {

    int i;
    register struct lentry *lptr = &lock_table[lock];

    // update the reader/writer counters
    if (q[item].qtype == READ)
        lptr->num_reader++;
    else if (q[item].qtype == WRITE)
        lptr->num_writer++;

    lptr->process_holding_lock[item]             = 1;
    proctab[currpid].lockheld[lock] = 1;
    update_lprio(lock);
    for(i=0; i<NPROC; i++) {
        if(lptr->process_holding_lock[i] > 0)
            update_pinh(i);
    }

    dequeue(item);
    ready(item, RESCHNO);
}
