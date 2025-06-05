/*
 * schedulelist_insert_test.c
 *
 *  Created on: Jun 3, 2025
 *      Author: Mariëlle
 */

#include "schedulelist_insert_test.h"

void Create_Test_Schedules(uint8_t amount){
	for(uint8_t i = 1; i <= amount; i++){
		Schedule schedule = { 0 };
		ScheduleTimestamp timestamp_start = {
				.year = 25,
				.month = 1,
				.weekday = 3,
				.date = 1,
				.hours = i,
				.minutes = 30,
				.seconds = 0
		};
		ScheduleTimestamp timestamp_end = {
				.year = 25,
				.month = 1,
				.weekday = 3,
				.date = 1,
				.hours = i+1,
				.minutes = 20,
				.seconds = 0
		};
		schedule.lamp_config = (LampConfig ) { .state = ON, .brightness = 255 };
		schedule.time_start = timestamp_start;
		schedule.time_end = timestamp_end;

	    ScheduleNode* insert_after = NULL;
	    ScheduleList_Can_Insert(&schedule, &insert_after);

	    ScheduleFuncStatus status;
		if (insert_after == NULL) {
			status = ScheduleList_Insert_First(schedule);
		} else {
			status = ScheduleList_Insert_After(insert_after, schedule);
		}
	}
}

uint8_t Test_Valid_Insert(void) {
	ScheduleList_Clear();
	for (uint8_t i = 1; i <= (SCHEDULE_LIST_MAX_LENGTH); i++) {
		Schedule schedule = { 0 };
		ScheduleTimestamp timestamp_start = {
				.year = 25,
				.month = 5,
				.weekday = 3,
				.date = 1,
				.hours = i,
				.minutes = 30,
				.seconds = 0
		};
		ScheduleTimestamp timestamp_end = {
				.year = 25,
				.month = 5,
				.weekday = 3,
				.date = 1,
				.hours = (i + 1),
				.minutes = 20,
				.seconds = 0
		};
		schedule.lamp_config = (LampConfig ) { .state = ON, .brightness = 255 };
		schedule.time_start = timestamp_start;
		schedule.time_end = timestamp_end;

	    ScheduleNode* insert_after = NULL;
	    ScheduleValidationResult result = ScheduleList_Can_Insert(&schedule, &insert_after);

		if (result != SCHEDULE_VALID_INSERT) {
			return 10 + i;
		}
		ScheduleFuncStatus status;
		if (insert_after == NULL) {
			status = ScheduleList_Insert_First(schedule);
		} else {
			status = ScheduleList_Insert_After(insert_after, schedule);
		}
		if (status != SCHEDULE_FUNC_OK) {
			return 20 + i;
		}
	}
	ScheduleList_Clear();
		for (uint8_t i = SCHEDULE_LIST_MAX_LENGTH; i >= 1; i--) {
			Schedule schedule = { 0 };
			ScheduleTimestamp timestamp_start = {
					.year = 25,
					.month = 5,
					.weekday = 3,
					.date = 1,
					.hours = i,
					.minutes = 30,
					.seconds = 0
			};
			ScheduleTimestamp timestamp_end = {
					.year = 25,
					.month = 5,
					.weekday = 3,
					.date = 1,
					.hours = (i + 1),
					.minutes = 20,
					.seconds = 0
			};
			schedule.lamp_config = (LampConfig ) { .state = ON, .brightness = 255 };
			schedule.time_start = timestamp_start;
			schedule.time_end = timestamp_end;

		    ScheduleNode* insert_after = NULL;
		    ScheduleValidationResult result = ScheduleList_Can_Insert(&schedule, &insert_after);

			if (result != SCHEDULE_VALID_INSERT) {
				return 10 + i;
			}
			ScheduleFuncStatus status;
			if (insert_after == NULL) {
				status = ScheduleList_Insert_First(schedule);
			} else {
				status = ScheduleList_Insert_After(insert_after, schedule);
			}
			if (status != SCHEDULE_FUNC_OK) {
				return 20 + i;
			}
		}
	return 0;
}

uint8_t Test_Max_Schedules(void){
	ScheduleList_Clear();
	Create_Test_Schedules(SCHEDULE_LIST_MAX_LENGTH);
	Schedule schedule = { 0 };
	ScheduleTimestamp timestamp_start = {
			.year = 25,
			.month = 5,
			.weekday = 3,
			.date = 1,
			.hours = 20,
			.minutes = 30,
			.seconds = 0
	};
	ScheduleTimestamp timestamp_end = {
			.year = 25,
			.month = 5,
			.weekday = 3,
			.date = 1,
			.hours = 21,
			.minutes = 20,
			.seconds = 0
	};
	schedule.lamp_config = (LampConfig ) { .state = ON, .brightness = 255 };
	schedule.time_start = timestamp_start;
	schedule.time_end = timestamp_end;

	ScheduleNode *insert_after = NULL;
	ScheduleValidationResult result = ScheduleList_Can_Insert(&schedule, &insert_after);
	if (result != SCHEDULE_LIST_FULL) {
		return 20;
	}
	return 0;
}

uint8_t Test_Invalid_Data(void) {
	ScheduleList_Clear();
	for (uint8_t i = 1; i <= (SCHEDULE_LIST_MAX_LENGTH); i++) {
		Schedule schedule = { 0 };
		ScheduleTimestamp timestamp = {
				.year = 25,
				.month = 5,
				.weekday = 3,
				.date = 16,
				.hours = 11,
				.minutes = 30,
				.seconds = 0
		};
		schedule.lamp_config = (LampConfig ) {
			.state = ON,
			.brightness = 255
		};
		schedule.time_start = timestamp;
		schedule.time_end = timestamp;

		ScheduleNode *insert_after = NULL;
		ScheduleValidationResult result = ScheduleList_Can_Insert(&schedule, &insert_after);

		if (result != SCHEDULE_INVALID_DATA) {
			return 30 + i;
		}
	}
	for (uint8_t i = 1; i <= (SCHEDULE_LIST_MAX_LENGTH); i++) {
		Schedule schedule = { 0 };
		ScheduleTimestamp timestamp_start = {
				.year = 25,
				.month = 5,
				.weekday = 3,
				.date = 1,
				.hours = i,
				.minutes = 20,
				.seconds = 0
		};
		ScheduleTimestamp timestamp_end = {
				.year = 25,
				.month = 5,
				.weekday = 3,
				.date = 1,
				.hours = (i - 1),
				.minutes = 30,
				.seconds = 0
		};
		schedule.lamp_config = (LampConfig ) { .state = ON, .brightness = 255 };
		schedule.time_start = timestamp_start;
		schedule.time_end = timestamp_end;

		ScheduleNode *insert_after = NULL;
		ScheduleValidationResult result = ScheduleList_Can_Insert(&schedule, &insert_after);

		if (result != SCHEDULE_INVALID_DATA) {
			return 30 + i;
		}
	}
	return 0;
}

uint8_t Test_Overlap_Next(void){
	ScheduleList_Clear();
	Create_Test_Schedules(5);
	Schedule schedule = { 0 };
	ScheduleTimestamp timestamp_start = {
			.year = 25,
			.month = 1,
			.weekday = 3,
			.date = 1,
			.hours = 1,
			.minutes = 0,
			.seconds = 0
	};
	ScheduleTimestamp timestamp_end = {
			.year = 25,
			.month = 1,
			.weekday = 3,
			.date = 1,
			.hours = 2,
			.minutes = 0,
			.seconds = 0
	};
	schedule.lamp_config = (LampConfig ) { .state = ON, .brightness = 255 };
	schedule.time_start = timestamp_start;
	schedule.time_end = timestamp_end;

	ScheduleNode *insert_after = NULL;
	ScheduleValidationResult result = ScheduleList_Can_Insert(&schedule, &insert_after);
	if (result != SCHEDULE_INVALID_OVERLAP_NEXT) {
			return 40;
		}
	for(uint8_t i = 1; i < 5; i++) {
		Schedule schedule = { 0 };
		ScheduleTimestamp timestamp_start = {
					.year = 25,
					.month = 1,
					.weekday = 3,
					.date = 1,
					.hours = i,
					.minutes = 25,
					.seconds = 0
			};
			ScheduleTimestamp timestamp_end = {
					.year = 25,
					.month = 1,
					.weekday = 3,
					.date = 1,
					.hours = i+1,
					.minutes = 35,
					.seconds = 0
			};
			schedule.lamp_config = (LampConfig ) { .state = ON, .brightness = 255 };
			schedule.time_start = timestamp_start;
			schedule.time_end = timestamp_end;

			ScheduleNode *insert_after = NULL;
			ScheduleValidationResult result = ScheduleList_Can_Insert(&schedule, &insert_after);
			if (result != SCHEDULE_INVALID_OVERLAP_NEXT) {
					return 40+i;
				}
	}
	Schedule schedule2 = { 0 };
		ScheduleTimestamp timestamp_start2 = {
				.year = 25,
				.month = 1,
				.weekday = 3,
				.date = 1,
				.hours = 3,
				.minutes = 25,
				.seconds = 0
		};
		ScheduleTimestamp timestamp_end2 = {
				.year = 25,
				.month = 1,
				.weekday = 3,
				.date = 1,
				.hours = 3,
				.minutes = 30,
				.seconds = 0
		};
		schedule2.lamp_config = (LampConfig) { .state = ON, .brightness = 255 };
		schedule2.time_start = timestamp_start2;
		schedule2.time_end = timestamp_end2;

		ScheduleNode* insert_after2 = NULL;
		ScheduleValidationResult result2 = ScheduleList_Can_Insert(&schedule2, &insert_after2);

		if (result2 != SCHEDULE_INVALID_OVERLAP_NEXT) {
				return 49;
			}
	return 0;
}

uint8_t Test_Overlap_Previous(void){
	ScheduleList_Clear();
	Create_Test_Schedules(5);
	for(uint8_t i = 1; i < 5; i++) {
		Schedule schedule = { 0 };
		ScheduleTimestamp timestamp_start = {
					.year = 25,
					.month = 1,
					.weekday = 3,
					.date = 1,
					.hours = i+1,
					.minutes = 10,
					.seconds = 0
			};
			ScheduleTimestamp timestamp_end = {
					.year = 25,
					.month = 1,
					.weekday = 3,
					.date = 1,
					.hours = i+1,
					.minutes = 25,
					.seconds = 0
			};
			schedule.lamp_config = (LampConfig ) { .state = ON, .brightness = 255 };
			schedule.time_start = timestamp_start;
			schedule.time_end = timestamp_end;

			ScheduleNode *insert_after = NULL;
			ScheduleValidationResult result = ScheduleList_Can_Insert(&schedule, &insert_after);
			if (result != SCHEDULE_INVALID_OVERLAP_PREVIOUS) {
					return 50+i;
				}
		}
	Schedule schedule = { 0 };
	ScheduleTimestamp timestamp_start = {
			.year = 25,
			.month = 1,
			.weekday = 3,
			.date = 1,
			.hours = 3,
			.minutes = 20,
			.seconds = 0
	};
	ScheduleTimestamp timestamp_end = {
			.year = 25,
			.month = 1,
			.weekday = 3,
			.date = 1,
			.hours = 3,
			.minutes = 25,
			.seconds = 0
	};
	schedule.lamp_config = (LampConfig ) { .state = ON, .brightness = 255 };
	schedule.time_start = timestamp_start;
	schedule.time_end = timestamp_end;

	ScheduleNode* insert_after = NULL;
	ScheduleValidationResult result = ScheduleList_Can_Insert(&schedule, &insert_after);

	if (result != SCHEDULE_INVALID_OVERLAP_PREVIOUS) {
			return 50;
		}
	return 0;
}

uint8_t Test_Day_Differences(void){
	ScheduleList_Clear();
	Create_Test_Schedules(2);
	Schedule schedule = { 0 };
			ScheduleTimestamp timestamp_start = {
					.year = 25,
					.month = 1,
					.weekday = 3,
					.date = 2,
					.hours = 1,
					.minutes = 30,
					.seconds = 0
			};
			ScheduleTimestamp timestamp_end = {
					.year = 25,
					.month = 1,
					.weekday = 3,
					.date = 2,
					.hours = 2,
					.minutes = 20,
					.seconds = 0
			};
			schedule.lamp_config = (LampConfig ) { .state = ON, .brightness = 255 };
			schedule.time_start = timestamp_start;
			schedule.time_end = timestamp_end;

			ScheduleNode* insert_after = NULL;
			ScheduleValidationResult result = ScheduleList_Can_Insert(&schedule, &insert_after);

			if (result != SCHEDULE_VALID_INSERT) {
					return 60;
				}
			Schedule schedule2 = { 0 };
			ScheduleTimestamp timestamp_start2 = {
					.year = 25,
					.month = 1,
					.weekday = 3,
					.date = 1,
					.hours = 2,
					.minutes = 21,
					.seconds = 0
			};
			ScheduleTimestamp timestamp_end2 = {
					.year = 25,
					.month = 1,
					.weekday = 3,
					.date = 2,
					.hours = 2,
					.minutes = 29,
					.seconds = 0
			};
			schedule2.lamp_config = (LampConfig ) { .state = ON, .brightness = 255 };
			schedule2.time_start = timestamp_start2;
			schedule2.time_end = timestamp_end2;

			ScheduleNode* insert_after2 = NULL;
			ScheduleValidationResult result2 = ScheduleList_Can_Insert(&schedule2, &insert_after2);

			if (result2 != SCHEDULE_INVALID_OVERLAP_NEXT) {
					return 61;
				}
			Schedule schedule3 = { 0 };
			ScheduleTimestamp timestamp_start3 = {
					.year = 24,
					.month = 12,
					.weekday = 3,
					.date = 31,
					.hours = 2,
					.minutes = 21,
					.seconds = 0
			};
			ScheduleTimestamp timestamp_end3 = {
					.year = 25,
					.month = 1,
					.weekday = 3,
					.date = 1,
					.hours = 2,
					.minutes = 29,
					.seconds = 0
			};
			schedule3.lamp_config = (LampConfig ) { .state = ON, .brightness = 255 };
			schedule3.time_start = timestamp_start3;
			schedule3.time_end = timestamp_end3;

			ScheduleNode* insert_after3 = NULL;
			ScheduleValidationResult result3 = ScheduleList_Can_Insert(&schedule3, &insert_after3);

			if (result3 != SCHEDULE_INVALID_OVERLAP_NEXT) {
					return 62;
				}
	return 0;
}
