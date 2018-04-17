
/**********************************************************************

   File          : cmsc312-p2-mfu.c

   Description   : This is most frequently used replacement algorithm
                   (see .h for applications)

   By            : Trent Jaeger, Yuquan Shan

***********************************************************************/
/**********************************************************************
Copyright (c) 2016 The Pennsylvania State University
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of The Pennsylvania State University nor the names of its contributors may be used to endorse or promote products derived from this softwiare without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <sched.h>

/* Project Include Files */
#include "cmsc312-p2.h"

/* Definitions */

/* second chance list */

typedef struct mfu_entry{  
  int pid;
  ptentry_t *ptentry;
  struct mfu_entry *next;
  struct mfu_entry *prev;
} mfu_entry_t;

typedef struct mfu{
  mfu_entry_t *first;
} mfu_t;

mfu_t *page_list;

/**********************************************************************

    Function    : init_mfu
    Description : initialize mfu list
    Inputs      : fp - input file of data
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int init_mfu( FILE *fp )
{
  page_list = (mfu_t *)malloc(sizeof(mfu_t));
  page_list->first = NULL;
  return 0;
}


/**********************************************************************

    Function    : replace_mfu
    Description : choose victim based on mfu algorithm, take the frame 
                  associated the page with the largest count as victim 
    Inputs      : pid - process id of victim frame 
                  victim - frame assigned from fifo -- to be replaced
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int replace_mfu( int *pid, frame_t **victim )
{
  /* Task 3 */
  mfu_entry_t *current = page_list->first;
  mfu_entry_t *most_count = current;
  while(current->next){
    current = current->next;
    if(current->ptentry->ct > most_count->ptentry->ct){
      most_count = current;
    }
  }

  // Remove most_count from the linked list
  if(most_count->next){
    most_count->next->prev = most_count->prev;
  }
  if(most_count->prev){
    most_count->prev->next = most_count->next;
  } else{
    page_list->first = most_count->next;
  }

  // Set victim to the frame given by the frame value of most_count's ptentry
  *victim = &(physical_mem[most_count->ptentry->frame]);
  *pid = most_count->pid;
  free(most_count);

  printf("replace_mfu: Selected frame %i for replacement\n", most_count->ptentry->frame);
  return 0;
}


/**********************************************************************

    Function    : update_mfu
    Description : create container for the newly allocated frame (and 
                  associated page), and insert it to the end (with respect
                  to page_list->first) of page list 
    Inputs      : pid - process id
                  f - frame
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int update_mfu( int pid, frame_t *f )
{
  /* Task 3 */
  // Make new list entry
  mfu_entry_t *list_entry = ( mfu_entry_t *)malloc(sizeof(mfu_entry_t));
  list_entry->pid = pid;
  list_entry->ptentry = &(processes[pid].pagetable[f->page]);
  list_entry->next = NULL;
  list_entry->prev = NULL;

  // If the page list has items, put this one at the end
  if(page_list->first){
    mfu_entry_t *last = page_list->first;
    while(last->next){
      last = last->next;
    }
    last->next = list_entry;
    list_entry->prev = last;
  }
  else{ // Else put this one as first
    page_list->first = list_entry;
  }
  
  return 0;
}
