// void linit(void)
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

struct lentry lock_table[NLOCK];
int nextlock;
int lockiter;

void linit(void) {

    struct lentry *lptr;
    int i, j;

    nextlock = NLOCK - 1;
    lockiter = 0;

    for (i = 0; i < NLOCK; i++) {
        (lptr = &lock_table[i])->lstate = FREE;
        lptr->lqtail = 1 + (lptr->lqhead = newqueue()); 
		
	lptr->lprio  = -1;	
                        
        for (j=0 ; j<NPROC ; j++) {
             lptr->process_holding_lock[j] = 0;
        }


    }

}
