/*
 * array.h
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include "common.h"

#include <iostream>
#include <iomanip>


class intarray
{
public:
	intarray(int m_, int n_) : m{m_}, n{n_}, elems{new int[m * n]} { INBOUNDS(0,m,INT_MAX); INBOUNDS(0,n,INT_MAX);  }
	intarray(int n_) : intarray {n_, n_} {}
	~intarray() { delete elems; }

	inline int rows() const { return m; }
	inline int cols() const { return n; }

	inline int& at(int i, int j)
	{
		INBOUNDS(0,i,m);
		INBOUNDS(0,j,n);
		return elems[i * n + j];
	}
	inline const int& at(int i, int j) const
	{
		INBOUNDS(0,i,m);
		INBOUNDS(0,j,n);
		return elems[i * n + j];
	};

	intarray& operator=(int num) {for (int i=0; i<m*n; i++) elems[i] = num; return *this;}


	void set_num_columns(int new_num_cols, int fill=-1)
	{
		int *newelems = new int[m * new_num_cols];

		for (int i = 0; i < m; i++)
		{
			for (int j = 0; j < new_num_cols; j++)
			{
				newelems[i * new_num_cols + j] =  j<n ? at(i, j) : fill;
			}
		}

		delete elems;
		elems = newelems;
		n = new_num_cols;
	}

	int binary_search(int row, int val, int valid_max) const
	{
		int min = 0;
		if (valid_max <= min)
		{
			return min;
		}
		if (val > at(row, valid_max))
		{
			return valid_max;
		}
		if (val < at(row, min))
		{
			return min;
		}

		while (min+1 < valid_max)
		{
			int mid = (valid_max + min) / 2;

			int a = at(row, mid);
			if (a == val)
			{
				return mid;
			}
			if (a < val)
			{
				min = mid;
				continue;
			}
			if (a > val)
			{
				valid_max = mid;
			}
		}

		INBOUNDS(at(row, min), val, 1+at(row, valid_max));

		return min;
	}

	friend std::ostream& operator<<(std::ostream& o, const intarray& a)
	{
		for (int i = 0; i < a.m; i++)
		{
			for (int j = 0; j < a.n; j++)
			{
				o << std::setw(5) << a.at(i, j) << ' ';
			}
			o << std::endl;
		}
		return o;
	}
private:
	int m, n;
	int *elems;
};


class int3array
{
public:
	int3array(int m_, int n_, int o_) : m{m_}, n{n_}, o{o_}, elems{new int[m * n * o]} {}
	int3array(int n_) : int3array {n_, n_, n_} {}
	~int3array() { delete elems; }

	inline int& at(int i, int j, int k) { return elems[i * m * n +  m * j + k]; }
	inline const int& at(int i, int j, int k) const { return elems[i * m * n + m * j + k]; };

	int3array& operator=(int num) {for (int i=0; i<m*n*o; i++) elems[i] = num; return *this;}

	friend std::ostream& operator<<(std::ostream& out, const int3array& a)
	{
		for (int i = 0; i < a.m; i++)
		{
			for (int j = 0; j < a.n; j++)
			{
				for (int k = 0; k < a.o; i++)
				{
					out << std::setw(5) << a.at(i, j, k) << ' ';
				}
			}
			out << std::endl;
		}
		return out;
	}
private:
	int m, n, o;
	int *elems;
};

#endif /* ARRAY_H_ */
