/*
 * interhub.h
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#ifndef INTERHUB_H_
#define INTERHUB_H_

class interhub
{
public:
	int begin, end;
	int path[2];

	interhub(int begin_, int end_) : begin{begin_}, end{end_} { path[0] = path[1] = -1; }
};

#endif /* INTERHUB_H_ */
