/*
 * schedules.c
 *
 *  Created on: May 15, 2025
 *      Author: jacco
 */

#include <stdlib.h>
#include "schedules.h"

ScheduleList schedules = {0};

void Insert_Schedule_After(ScheduleNode* schedule_node, Schedule new_schedule) {
	ScheduleNode* new_schedule_node = (ScheduleNode*) malloc(sizeof(ScheduleNode));
	new_schedule_node->schedule = new_schedule;
	new_schedule_node->next = schedule->next;
	schedule->next = new_schedule_node;
	schedules.size++;
}

void Remove_Schedule_After(ScheduleNode* schedule_node) {
	ScheduleNode* node_to_remove = schedule_node->next;
}
