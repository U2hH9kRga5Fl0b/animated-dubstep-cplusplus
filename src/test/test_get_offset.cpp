/*
 * test_get_offset.cpp
 *
 *  Created on: Nov 3, 2014
 *      Author: thallock
 */

#include "main/global.h"
#include "model/city.h"

void test_get_offset()
{
	dumpster_size sizes[] = {none, six, nine, twelve, sixteen};
	do
	{
		dumpster_size size1 = sizes[rand() % 5];
		int a;

		if (size1 != none)
		{
			int loffset = get_matching_landfill_index(size1);
			a = city->first_landfill_index + loffset + (rand() % city->num_landfills) * NUM_ACTIONS_PER_FILL;
			if ((city->get_action(a).entr_state & TRUCK_SIZE_MASK) != size1)
			{
				trap();
			}
			if ((city->get_action(a).exit_state & TRUCK_SIZE_MASK) != size1)
			{
				trap();
			}
			if ((city->get_action(a).entr_state & TRUCK_STATE_MASK) != TRUCK_STATE_FULL)
			{
				trap();
			}
			if ((city->get_action(a).exit_state & TRUCK_STATE_MASK) != TRUCK_STATE_EMPTY)
			{
				trap();
			}
		}

		dumpster_size size2 = sizes[rand() % 5];
		if (size1 == size2)
		{
			continue;
		}

		int soffset = get_matching_staging_area_index(
				(dumpster_size) (size1 & TRUCK_SIZE_MASK),
				(dumpster_size) (size2 & TRUCK_SIZE_MASK));

		a = city->first_stagingarea_index + soffset + (rand() % city->num_stagingareas) * NUM_ACTIONS_PER_YARD;
		if ((city->get_action(a).entr_state & TRUCK_SIZE_MASK) != size1)
		{
			trap();
		}
		if ((city->get_action(a).exit_state & TRUCK_SIZE_MASK) != size2)
		{
			trap();
		}
		if ((city->get_action(a).entr_state & TRUCK_STATE_MASK) == TRUCK_STATE_FULL)
		{
			trap();
		}
		if ((city->get_action(a).exit_state & TRUCK_STATE_MASK) == TRUCK_STATE_FULL)
		{
			trap();
		}
	} while(true);
}
