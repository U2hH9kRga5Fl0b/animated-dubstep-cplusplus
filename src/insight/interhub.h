/*
 * interhub.h
 *
 *  Created on: Nov 5, 2014
 *      Author: thallock
 */

#ifndef INTERHUB_H_
#define INTERHUB_H_

#include <iostream>
#include <iomanip>

class interhub
{
public:
	int begin, end;
	int path[2];

	interhub(int begin_, int end_) : begin{begin_}, end{end_} { path[0] = path[1] = -1; }

	friend std::ostream& operator<<(std::ostream& out, const interhub& h)
	{
		return out << "[from hub:" << std::setw(3) << h.begin
				<< "][to hub:" << std::setw(3) << h.end
				<< "][inter reqs:" << std::setw(3) << h.path[0] << "," << std::setw(3) << h.path[1] << "]";
	}
};

#endif /* INTERHUB_H_ */
