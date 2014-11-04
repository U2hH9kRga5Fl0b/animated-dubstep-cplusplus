/*
 * inventory.cpp
 *
 *  Created on: Oct 26, 2014
 *      Author: thallock
 */

#include "model/inventory.h"
#include "model/city.h"
#include <algorithm>
#include <map>



namespace
{

int num_none = INT_MAX;

class invinfo
{
public:
	invinfo() {}
	~invinfo() {}

	void operator=(const Yard& y)
	{
		yard = &y;
		inventory[0] = y.initial[0];
		inventory[1] = y.initial[1];
		inventory[2] = y.initial[2];
		inventory[3] = y.initial[3];
	}

	void operator=(const invinfo& prev)
	{
		yard = prev.yard;
		inventory[0] = prev.inventory[0];
		inventory[1] = prev.inventory[1];
		inventory[2] = prev.inventory[2];
		inventory[3] = prev.inventory[3];
	}

	void apply_action(const Action* a, bool reverse)
	{
		if (a->location != yard->location)
		{
			return;
		}

		switch(a->entr_state & TRUCK_SIZE_MASK)
		{
			case none   : trap();                         break;
			case six    : inventory[0] += (reverse?-1:1); break;
			case nine   : inventory[1] += (reverse?-1:1); break;
			case twelve : inventory[2] += (reverse?-1:1); break;
			case sixteen: inventory[3] += (reverse?-1:1); break;
			default:
				err() << "truck state:" << a->exit_state << std::endl;
				trap();
		}
		switch(a->exit_state & TRUCK_SIZE_MASK)
		{
			case none   : trap();                         break;
			case six    : inventory[0] -= (reverse?-1:1); break;
			case nine   : inventory[1] -= (reverse?-1:1); break;
			case twelve : inventory[2] -= (reverse?-1:1); break;
			case sixteen: inventory[3] -= (reverse?-1:1); break;
			default:
				err() << "truck state: " << a->exit_state << std::endl;
				trap();
		}
	}

	int& operator[](dumpster_size s)
	{
		switch(s)
		{
		case none   : return num_none;
		case six    : return inventory[0];
		case nine   : return inventory[1];
		case twelve : return inventory[2];
		case sixteen: return inventory[3];
		default:
			std::cerr << "bad inventory size" << std::endl;
			trap();
			return num_none;
		}
	}
	const int& operator[](dumpster_size s) const
	{
		switch(s)
		{
		case none   : return num_none;
		case six    : return inventory[0];
		case nine   : return inventory[1];
		case twelve : return inventory[2];
		case sixteen: return inventory[3];
		default:
			std::cerr << "bad inventory size" << std::endl;
			trap();
			return num_none;
		}
	}

	bool in_capacity() const
	{
		return          0 <= inventory[0] && 0 <= inventory[1] && 0 <= inventory[2] && 0 <= inventory[3] &&
				inventory[0] + inventory[1] + inventory[2] + inventory[3] < yard->capacity;
	}

	friend std::ostream& operator<<(std::ostream& out, const invinfo& inv)
	{
		out << "{" << std::setw(3) << inv.yard->location << ":";
		for (int j = 0; j < 4; j++)
		{
			out << std::setw(3) << inv.inventory[j] << " ";
		}
		return out << "}";
	}

	const Yard* yard;
	int inventory[4];
};




class inventory
{
public:
	inventory(const City* city) :
		len{city->num_stagingareas},
		driver{-1},
		stop{-1},
		infos { new invinfo[len] }
	{
		for (int i = 0; i < len; i++)
		{
			infos[i] = city->yards[i];
		}
	}
	inventory(
			int driver_,
			int stop_,
			const inventory* prev) :
		len{prev->len},
		driver{driver_},
		stop{stop_},
		infos{new invinfo[len]}
	{
		for (int i = 0; i < len; i++)
		{
			infos[i] = prev->infos[i];
		}
	}

	virtual ~inventory()
	{
		delete[] infos;
	}

	inventory(const inventory& other) = delete;
	inventory& operator=(const inventory& other) = delete;

	void apply_action(int& index, const Action* action, bool reverse)
	{
		if (index < 0 || infos[index].yard->location != action->location)
		{
			for (int i = 0; i < len; i++)
			{
				if(infos[i].yard->location == action->location)
				{
					index = i;
					break;
				}
			}
		}
		INBOUNDS(0, index, len);

		infos[index].apply_action(action, reverse);
	}

	bool in_capacity() const
	{
		for (int i = 0; i < len; i++)
		{
			if (!infos[i].in_capacity())
			{
				return false;
			}
		}
		return true;
	}

	friend std::ostream& operator<<(std::ostream& out, const inventory& inv)
	{
		out << std::setw(3) << inv.driver << std::setw(3) << inv.stop << " ";
		for (int i = 0; i < inv.len; i++)
		{
			out << inv.infos[i] << " ";
		}
		return out;
	}
private:
	int len;
	int driver;
	int stop;
	invinfo* infos;
};

}

struct myfunc
{
	myfunc(const Action* action_, bool reverse_) :
		reverse{reverse_},
		action{action_},
		ndx{-1} {}
	~myfunc() {}

	void operator()(std::pair<int, inventory*> it)
	{
		it.second->apply_action(ndx, action, reverse);
	}
private:
	bool reverse;
	const Action* action;
	int ndx;
};

InventoryTimeline::InventoryTimeline(const City* city_) :
	city{city_},
	timeline{new std::map<int, inventory*>}
{
	std::map<int, inventory*>* tl = (std::map<int, inventory*>*) timeline;
	inventory* sent = new inventory{city};
	tl->insert(std::pair<int, inventory*> { -1, sent });
}
InventoryTimeline::~InventoryTimeline()
{
	auto tl = (std::map<int, inventory*>*) timeline;
	std::for_each(tl->begin(), tl->end(), [](const std::pair<int, inventory*>& p) { delete p.second; });
	delete tl;
}

void InventoryTimeline::clear()
{
	auto tl = (std::map<int, inventory*>*) timeline;
	std::for_each(tl->begin(), tl->end(), [](const std::pair<int, inventory*>& p) { delete p.second; });
	tl->clear();
	tl->insert(std::pair<int, inventory*> { -1, new inventory{city} });
}

void InventoryTimeline::action_performed(int driver, int stop, int time, const Action* action)
{
	if (!ENFORCE_INVENTORIES)
	{
		return;
	}
	if (action->location < city->num_landfills || action->location >= city->num_stagingareas + city->num_landfills)
	{
		return;
	}

	if (DEBUG && ((action->exit_state & TRUCK_STATE_FULL) || (action->entr_state & TRUCK_STATE_FULL)))
	{
		trap();
	}

	std::map<int, inventory*>* tl = (std::map<int, inventory*>*) timeline;

	auto it = tl->lower_bound(time);

	const inventory* prev = it == tl->end() ? tl->rbegin()->second : it->second;
	tl->insert(it, std::pair<int, inventory*> { time, new inventory { driver, stop,  prev} });


	std::advance(it, 1);
	std::for_each(it, tl->end(), myfunc{action, false});
}

bool InventoryTimeline::in_capacity() const
{
	if (!ENFORCE_INVENTORIES)
	{
		return true;
	}
	std::map<int, inventory*>& tl = *(std::map<int, inventory*>*) timeline;
	return std::all_of(tl.begin(), tl.end(), [](const std::pair<int, inventory*>& it)
	{
		return it.second->in_capacity();
	});
}

std::ostream& operator<<(std::ostream& out, const InventoryTimeline& line)
{
	if (!ENFORCE_INVENTORIES)
	{
		return out;
	}
	std::map<int, inventory*>* tl = (std::map<int, inventory*>*) line.timeline;
	auto end = tl->end();
	for (auto it = tl->begin(); it != end; ++it)
	{
		out << "\t" << std::setw(5) << it->first << ":" << (*it->second) << std::endl;
	}
	return out;
}
