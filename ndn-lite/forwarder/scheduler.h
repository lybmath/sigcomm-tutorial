/*
 * Copyright (C) 2019 Xinyu Ma
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef FORWARDER_SCHEDULER_H_
#define FORWARDER_SCHEDULER_H_

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO: This is a temporary solution. Wait for the timer to be implemented.
typedef uint64_t timetick_t;

/**
 * The maximum events in the scheduler
 */
#define NDN_SCHEDULER_SIZE 127

typedef void(*ndn_event_callback)(void *self,
                                  uint32_t iparam,
                                  void *pparam);


void
ndn_scheduler_init(void);

// TODO: Current scheduler uses absolute timepoint due to the absence of timer.
bool
ndn_scheduler_post(timetick_t timepoint,
                   void *target,
                   ndn_event_callback reason,
                   uint32_t iparam,
                   void *pparam);

// TODO: Current scheduler gives current time explicitly due to the absence of timer.
bool
ndn_scheduler_process(timetick_t now);

#ifdef __cplusplus
}
#endif

#endif // #define FORWARDER_SCHEDULER_H_
