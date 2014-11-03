/*
 * directionsdb.h
 *
 *  Created on: Nov 2, 2014
 *      Author: thallock
 */

#ifndef DIRECTIONSDB_H_
#define DIRECTIONSDB_H_

#include "direction/directions.h"
#include <cfloat>

#include <boost/filesystem.hpp>

class directions_db
{
public:
	directions_db(const std::string& directory)
	{
		boost::filesystem::path root { directory };

		if (!boost::filesystem::exists(root) || !boost::filesystem::is_directory(root))
		{
			return;
		}

		boost::filesystem::directory_iterator end_iter;
		for (boost::filesystem::directory_iterator dir_iter { root }; dir_iter != end_iter; ++dir_iter)
		{
			if (!boost::filesystem::is_regular_file(dir_iter->status()))
			{
				continue;
			}

			directions d { dir_iter->path().string() };
			if (!d.valid())
			{
				continue;
			}
			all_directions.push_back(d);
		}
	}

	const directions& get_best_directions(const location& start, const location& stop)
	{
		const directions* best = nullptr;
		double best_cost = DBL_MAX;

		auto end = all_directions.end();
		for (auto it = all_directions.begin(); it != end; ++it)
		{
			double c = it->get_cost(start, stop);
			if (c >= best_cost)
			{
				continue;
			}

			best = &(*it);
			best_cost = c;
		}

		return *best;
	}

	friend std::ostream& operator<<(std::ostream& out, const directions_db& db)
	{
		auto end = db.all_directions.end();
		for (auto it = db.all_directions.begin(); it != end; ++it)
		{
			out << (*it) << '\n';
		}
		return out;
	}

	std::list<directions> all_directions;
}
;

#endif /* DIRECTIONSDB_H_ */
