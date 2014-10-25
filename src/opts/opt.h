/*
 * opt.h
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#ifndef OPT_H_
#define OPT_H_

class opt
{
public:
	opt();
	virtual ~opt();


	bool feasible();
	void apply();

	int get_hash();
};




#endif /* OPT_H_ */
