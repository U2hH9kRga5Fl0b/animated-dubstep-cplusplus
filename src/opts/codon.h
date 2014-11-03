/*
 * codon.h
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#ifndef CODON_H_
#define CODON_H_

#include "model/solution.h"

#include "opt/neighbor.h"

#include <cinttypes>
#include <list>


class exchange_subpath /*: public neighbor_operation*/
{
public:
	int startaction1, stopaction1;
	int startaction2, stopaction2;

	exchange_subpath(int driver1, int begin1, int end1,
			 int driver2, int begin2, int end2);
	~exchange_subpath();

	int get_time_improvement(Solution* solution);

	bool operator==(const exchange_subpath& other) const;
};




void exchange_subpath_search(Solution* solution, int fail_threshold);
void ensure_local_minima(Solution* solution);

#endif /* CODON_H_ */
