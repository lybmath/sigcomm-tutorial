/*
 * Copyright (C) 2019 Xinyu Ma
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "scheduler.h"

#define LEFT(x) ((x) << 1)
#define RIGHT(x) (((x) << 1) | 1)
#define PARENT(x) ((x) >> 1)

typedef struct ndn_event {
  timetick_t tick;
  void* obj;
  ndn_event_callback func;
  void* pparam;
  int32_t iparam;
} ndn_event_t;

static ndn_event_t events[NDN_SCHEDULER_SIZE + 1];
static uint32_t event_cnt;

void
ndn_scheduler_init(void)
{
  event_cnt = 0;
}

bool
ndn_scheduler_post(timetick_t timepoint,
                   void *target,
                   ndn_event_callback reason,
                   uint32_t iparam,
                   void *pparam)
{
  int i;

  if(event_cnt >= NDN_SCHEDULER_SIZE)
    return false;
  
  event_cnt ++;
  events[0].tick = timepoint;
  events[0].obj = target;
  events[0].func = reason;
  events[0].iparam = iparam;
  events[0].pparam = pparam;
  
  for(i = event_cnt; i > 1 && events[PARENT(i)].tick > events[0].tick; i = PARENT(i))
    events[i] = events[PARENT(i)];
  events[i] = events[0];
  
  return true;
}

bool
ndn_scheduler_process(timetick_t now)
{
  int i, large;
  bool ret = false;
  
  while(event_cnt >= 1 && events[1].tick < now) {
    events[1].func(events[1].obj, events[1].iparam, events[1].pparam);
    event_cnt --;
    
    i = 1;
    while(i > 0 || i <= event_cnt) {
      large = event_cnt + 1;
      if(LEFT(i) <= event_cnt && events[LEFT(i)].tick < events[large].tick)
        large = LEFT(i);
      if(RIGHT(i) <= event_cnt && events[RIGHT(i)].tick < events[large].tick)
        large = RIGHT(i);
      events[i] = events[large];
      if(large != event_cnt + 1)
        i = large;
      else
        break;
    }
    
    ret = true;
  }
  return ret;
}
