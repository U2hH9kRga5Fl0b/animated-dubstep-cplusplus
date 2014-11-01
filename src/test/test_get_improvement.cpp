/*
 * test_get_improvement.cpp
 *
 *  Created on: Oct 31, 2014
 *      Author: thallock
 */




#if 0

				if (DEBUG)
				{
					Solution other {*solution};
					int old = other.sum_all_times();
					other.exchange(c.driver, c.begin + 1, c.end, alternative.driver, alternative.begin + 1, alternative.end);

					int n=other.sum_all_times();
					int ca=old + improvement;

					if (n != ca)
					{

						err() << "actual: " << n - old << std::endl;
						err() << "calculated: " << improvement << std::endl;

						err() << n << ", " << ca << std::endl;
						err() << "mis-calculated improvement." << std::endl;
						err() << "old=" << old << std::endl;
						err() << "improvement=" << improvement << std::endl;
						err() << "new=" << other.sum_all_times() << std::endl;
						trap();
					}
				}

#endif
