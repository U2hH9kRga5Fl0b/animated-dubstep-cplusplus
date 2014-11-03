/*
 * inventory.h
 *
 *  Created on: Oct 26, 2014
 *      Author: thallock
 */

#ifndef INVENTORY_H_
#define INVENTORY_H_

#include <map>

class City;
class Action;

class InventoryTimeline
{
public:
	InventoryTimeline(const City* city);
	~InventoryTimeline();

	void clear(const City* original);
	void action_performed(int driver, int stop, int time, const Action* action);

	bool in_capacity() const;

	friend std::ostream& operator<<(std::ostream& out, const InventoryTimeline& line);
private:
	// std::map<int, inventory> timeline;
	void* timeline;
};

#endif /* INVENTORY_H_ */
