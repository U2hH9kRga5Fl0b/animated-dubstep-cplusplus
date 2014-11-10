/*
 * show_robustness.cpp
 *
 *  Created on: Nov 9, 2014
 *      Author: thallock
 */


#include "main/global.h"

#include "robustness/request_changes.h"

void show_robustness()
{
	reduced_results *results = get_results(city, 5, 3, 0);
	log() << *results;
	delete results;
}

