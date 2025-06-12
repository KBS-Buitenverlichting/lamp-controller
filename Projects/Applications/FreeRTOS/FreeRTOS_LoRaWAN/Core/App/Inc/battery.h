/*********************************************************************
 * @file   battery.h
 * @brief  File for handling battery percentage request
 *
 * @author KBS Buitenverlichting
 * @date   9 May 2025
 *********************************************************************/
#pragma once

/**
 * @brief Starts the task that handles the battery percentage request and executes it
 *
 * @param[in] argument Any argument the function may receive
 */
void Start_Get_Battery_Level_Task(void const *argument);
