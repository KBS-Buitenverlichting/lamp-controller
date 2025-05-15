/*
 * schedules.c
 *
 *  Created on: May 15, 2025
 *      Author: jacco
 */

#include <stdlib.h>
#include "schedules.h"

ScheduleList schedules = {0};

void Insert_Schedule_After(ScheduleNode* schedule, ScheduleNode* newSchedule) {
	newSchedule->next = schedule->next;
	schedule->next = newSchedule;
	schedules.size++;
}

void Remove_Schedule(ScheduleNode* schedule) {
	if (schedules.size > 1) {

	}
	schedules.size--;
}
