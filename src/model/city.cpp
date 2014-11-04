/*
 * city.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "model/city.h"

#include "model/landfill.h"
#include "main/gen_points.h"

#include <cstdlib>
#include <cmath>
#include <sstream>

#define DIST_SCALE 5000

Action sentinal_action{-1};

// This doesn't work because there is no operation for 'replacing' a dumpster at a yard.
#define ALL_YARD_COMBOS 1

#include <set>

City::City(int num_requests_, int num_landfills_, int num_stagingareas_, int num_trucks_) :
	num_actions      {num_requests_ + NUM_ACTIONS_PER_FILL * num_landfills_ + NUM_ACTIONS_PER_YARD * num_stagingareas_ },
	num_requests     {num_requests_     },
	num_landfills    {num_landfills_    },
	num_stagingareas {num_stagingareas_ },
	num_locations    {num_requests + num_landfills + num_stagingareas},
	num_trucks       {num_trucks_       },
	start_location   {0                 },
	durations        {num_locations     },
	possibles        {num_actions       },
	coords           {new Coord[num_locations]},
	trucks           {new truck_types[num_trucks]},
	actions          {nullptr           },
	donttouch        {                  }
{
	gen_points(     &coords[0], num_landfills,
			&coords[num_landfills], num_stagingareas,
			&coords[num_landfills + num_stagingareas], num_requests,
			UNIFORM_CITY_TYPE);

	int ndx = 0;
	for (int i = 0; i < num_landfills; i++)
	{
		Landfill l { ndx++ };
		donttouch.push_back(Action{l, six    });
		donttouch.push_back(Action{l, nine   });
		donttouch.push_back(Action{l, twelve });
		donttouch.push_back(Action{l, sixteen});
	}

	truck_types tt[] = {small, medium, large};
	for (int i = 0; i < num_trucks; i++)
		trucks[i] = tt[rand() % 3];

	for (int t = 0; t < num_stagingareas; t++)
	{
		yards.push_back(Yard{ndx++});

		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | six,     TRUCK_STATE_NONE });                // index:  0
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | nine,    TRUCK_STATE_NONE });                // index:  1
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | twelve,  TRUCK_STATE_NONE });                // index:  2
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | sixteen, TRUCK_STATE_NONE });                // index:  3

		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_NONE, TRUCK_STATE_EMPTY | six      });               // index:  4
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_NONE, TRUCK_STATE_EMPTY | nine     });               // index:  5
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_NONE, TRUCK_STATE_EMPTY | twelve   });               // index:  6
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_NONE, TRUCK_STATE_EMPTY | sixteen  });               // index:  7

		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | six      , TRUCK_STATE_EMPTY | nine     });  // index:  8
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | six      , TRUCK_STATE_EMPTY | twelve   });  // index:  9
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | six      , TRUCK_STATE_EMPTY | sixteen  });  // index: 10

		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | nine     , TRUCK_STATE_EMPTY | six      });  // index: 11
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | nine     , TRUCK_STATE_EMPTY | twelve   });  // index: 12
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | nine     , TRUCK_STATE_EMPTY | sixteen  });  // index: 13

		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | twelve   , TRUCK_STATE_EMPTY | six      });  // index: 14
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | twelve   , TRUCK_STATE_EMPTY | nine     });  // index: 15
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | twelve   , TRUCK_STATE_EMPTY | sixteen  });  // index: 16

		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | sixteen  , TRUCK_STATE_EMPTY | six      });  // index: 17
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | sixteen  , TRUCK_STATE_EMPTY | nine     });  // index: 18
		donttouch.push_back(Action { yards.at(t), TRUCK_STATE_EMPTY | sixteen  , TRUCK_STATE_EMPTY | twelve   });  // index: 19
	}

	for (int i = 0; i < num_requests; i++)
		donttouch.push_back(Action{ndx++});

	if (ndx != num_locations)
	{
		std::cerr << "wrong number of locations!" << std::endl;
		exit(-1);
	}

	double avg_drive_time = 0.0;
	for (int i = 0; i < num_locations; i++)
	for (int j = 0; j < num_locations; j++)
		avg_drive_time += durations.at(i,j) = (int)(DIST_SCALE * coords[i].dist(coords[j]));

	log() << "average drive time: " << (avg_drive_time / (60.0 * num_locations * (num_locations - 1))) << " minutes" << std::endl;

	actions = donttouch.data();
	if (num_actions != (int) donttouch.size())
	{
		std::cerr << "wrong number of actions!" << std::endl;
		std::cerr << "expected=" << num_actions << " number of actions=" << donttouch.size() << std::endl;
		exit(-1);
	}

	int avg_num_possibles = 0;
	possibles = -1;
	for (int i=0; i<num_actions; i++)
	{
		int ndx=0;
		for (int j=num_actions-1; j>=0; j--)
		{
			if (actions[j].entr_state != actions[i].exit_state)
			{
				continue;
			}
			if (actions[i].begin_time > actions[j].end_time)
			{
				continue;
			}
			possibles.at(i, ndx++) = j;
			avg_num_possibles++;
		}
	}
	log() << "average number of possibles actions: " << (avg_num_possibles / num_actions) << std::endl;


	first_landfill_index    = 0;
	first_stagingarea_index = first_landfill_index + num_landfills * NUM_ACTIONS_PER_FILL;
	first_request_index     = first_stagingarea_index + num_stagingareas * NUM_ACTIONS_PER_YARD;

	start_location = first_stagingarea_index;

	sentinal_action.entr_state = TRUCK_STATE_NONE;
	sentinal_action.exit_state = TRUCK_STATE_NONE;
	sentinal_action.location   = start_location;
	sentinal_action.begin_time = 0;
	sentinal_action.end_time   = INT_MAX;
	sentinal_action.wait_time  = 0;
	sentinal_action.value = 0;
	sentinal_action.accessible[0] = sentinal_action.accessible[1] = sentinal_action.accessible[2] = 1;
}

City::~City()
{
	delete[] coords;
	delete[] trucks;
}



std::ostream& operator<<(std::ostream& out, const City& c)
{
	out << "actions:\n";
	for (int i = 0; i < c.num_actions; i++)
	{
		out << std::setw(4) << i << ':' << c.actions[i] << std::endl;
	}
	out << "possibles:\n";
	for (int i = 0; i < c.num_actions; i++)
	{
		out << std::setw(5) << i << ':';
		for (int j = 0; j < c.num_actions; j++)
		{
			if (c.possibles.at(i, j) < 0)
			{
				break;
			}
			out << std::setw(3) << c.possibles.at(i, j) << ' ';
		}
		out << std::endl;
	}
	out << "trucks:\n";
	for (int i = 0; i < c.num_trucks; i++)
	{
		out << std::setw(4) << i << ':' << truck_name(c.trucks[i]) << std::endl;
	}

	out << "locs:\n";
	for (int i = 0; i < c.num_locations; i++)
	{
		out << "\t" << std::setw(3) << i << ":" << std::setw(8) << c.coords[i].x << ", " << std::setw(8) << c.coords[i].y << "\n";
	}

	out << "distances:\n";
	out << c.durations << std::endl;
	return out;
}

std::string City::get_decription(int location) const
{
	INBOUNDS(0, location, num_locations);
	int anaction = -1;

	for (int i = 0; i < num_actions; i++)
	{
		if (actions[i].location == location)
		{
			anaction = i;
			break;
		}
	}
	if (anaction < 0)
	{
		std::cerr << "unable to find an action for location " << location << std::endl;
		trap();
	}


	if (is_landfill(anaction))
	{
		return "L";
	}
	else if (is_staging_area(anaction))
	{
		return "Y";
	}
	else
	{
		std::stringstream ss;
		ss << (actions[anaction].entr_state & TRUCK_SIZE_MASK) << "->" << (actions[anaction].exit_state & TRUCK_SIZE_MASK);
		return ss.str();
	}
}
