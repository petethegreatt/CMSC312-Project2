
/**********************************************************************

   File          : cmsc312-p2-second.c

   Description   : This is second chance page replacement algorithm
                   (see .h for applications)
                   See http://www.cs.cf.ac.uk/Dave/C/node27.html for info

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

/* Include Files */
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

typedef struct second_entry {  
  int pid;
  ptentry_t *ptentry;
  struct second_entry *next;
  struct second_entry *prev;
} second_entry_t;

typedef struct second {
  second_entry_t *first;
} second_t;

second_t *page_list;

/**********************************************************************

    Function    : init_second
    Description : initialize second-chance list
    Inputs      : fp - input file of data
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int init_second( FILE *fp )
{
  page_list = (second_t *)malloc(sizeof(second_t));
  page_list->first = NULL;
  return 0;
}


/**********************************************************************

    Function    : replace_second
    Description : choose victim based on second chance algorithm (first with ref == 0)
    Inputs      : pid - process id of victim frame 
                  victim - frame assigned from fifo -- to be replaced
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int replace_second( int *pid, frame_t **victim )
{
  /* Task #3 */
  second_entry_t *current = page_list->first;
  while(current->next){
    if(current->ptentry->bits & REFBIT){ // If ref is 1, set it to 0 (i.e. give this entry a second chance)
      current->ptentry->bits &= DIRTYBIT | VALIDBIT;
    }
    else { // If ref is 0, use this entry
      break;
    }
    current = current->next;
  }
  
  // Remove current from the linked list
  if(current->next){
    current->next->prev = current->prev;
  }
  if(current->prev){
    current->prev->next = current->next;
  } else{
    page_list->first = current->next;
  }

  // Set victim to the frame given by the frame value of current's ptentry
  *victim = &(physical_mem[current->ptentry->frame]);
  *pid = current->pid;
  free(current);

  printf("replace_second: Selected frame %i for replacement\n", current->ptentry->frame);
  return 0;
}


/**********************************************************************

    Function    : update_second
    Description : update second chance on allocation 
    Inputs      : pid - process id
                  f - frame
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int update_second( int pid, frame_t *f )
{
  /* Task #3 */
  // Make new list entry
  second_entry_t *list_entry = ( second_entry_t *)malloc(sizeof(second_entry_t));
  list_entry->pid = pid;
  list_entry->ptentry = &(processes[pid].pagetable[f->page]);
  list_entry->next = NULL;
  list_entry->prev = NULL;

  // If the page list has items, put this one at the end
  if(page_list->first){
    second_entry_t *last = page_list->first;
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


