/*
 * landfill.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "model/landfill.h"

#include <cstdlib>

#define HALF_HOUR (30 * 60)


Landfill::Landfill(int loc) : waittime{HALF_HOUR + rand() % HALF_HOUR}, location{loc} {}
Landfill::~Landfill() {}
