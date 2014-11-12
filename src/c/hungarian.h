
#include "intarray.h"

extern "C"
{
int modified_main(const int *int_array_elems, int int_array_m, int int_array_n, int *ret_val);
}


inline int *solve_assignment_problem(const intarray& d2p)
{
	int * assignment = new int[d2p.rows()];
	modified_main(&d2p.at(0, 0), d2p.cols(), d2p.rows(), assignment);

	for (int i = 0; i < d2p.rows(); i++)
	{
		log() << std::setw(5) << assignment[i] << ' ' << std::endl;
	}

	return assignment;
}

