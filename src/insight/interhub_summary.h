/*
 * interhub_summary.h
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#ifndef INTERHUB_SUMMARY_H_
#define INTERHUB_SUMMARY_H_

#include "insight/insight_state.h"

class inter_hub_travel
{
public:
	intarray num_from_to;

	inter_hub_travel(const insight_state& state);
};




#endif /* INTERHUB_SUMMARY_H_ */
