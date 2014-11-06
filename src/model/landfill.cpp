/*
 * landfill.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "model/landfill.h"
#include "common.h"

#include <cstdlib>

Landfill::Landfill(int loc) : waittime{TIME_FOR_LANDFILL}, location{loc} {}
Landfill::~Landfill() {}
