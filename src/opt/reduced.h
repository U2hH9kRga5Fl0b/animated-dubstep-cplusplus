/*
 * reduced.h
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#ifndef REDUCED_H_
#define REDUCED_H_

#include "model/solution.h"


Solution* create();

void move_dumpsters(Solution* solution);
void move_dumpster(int yard1, int yard2, dumpster_size size, int deadline_driver, int deadline_stop);








#endif /* REDUCED_H_ */
