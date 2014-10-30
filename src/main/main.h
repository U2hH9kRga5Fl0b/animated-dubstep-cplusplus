/*
 * arguments.hpp
 *
 *  Created on: Oct 29, 2014
 *      Author: thallock
 */

#ifndef ARGUMENTS_HPP_
#define ARGUMENTS_HPP_

#include "model/solution.h"

extern City* city;
extern std::string infile;
extern std::string outfile;

void parse_args(int argc, char **argv);

// options...
void generate_random();
void generate_seed();
void make_video();
void ensure_feasible();
void solve();

#endif /* ARGUMENTS_HPP_ */
