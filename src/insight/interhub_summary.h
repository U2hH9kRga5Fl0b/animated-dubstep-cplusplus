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


	friend std::ostream& operator<<(std::ostream& out, const inter_hub_travel& travel)
	{
		out << "matrix of \"from depot\" (rows) to \"to depot\" (cols) of inter hub travel already present:" << std::endl;
		out << travel.num_from_to << std::endl;

		out << std::endl << "depot:  ";
		for (int i = 0; i < travel.num_from_to.cols(); i++)
		{
			out << std::setw(3) << i;
		}

		out << std::endl << "sum in: ";
		for (int i = 0; i < travel.num_from_to.cols(); i++)
		{
			out << std::setw(3) << travel.num_from_to.sum_col(i);
		}

		out << std::endl << "sum out:";
		for (int i = 0; i < travel.num_from_to.cols(); i++)
		{
			out << std::setw(3) << travel.num_from_to.sum_row(i);
		}
		out << std::endl;

		return out;
	}
};




#endif /* INTERHUB_SUMMARY_H_ */
