/*
 * stats.cc
 *
 *  Created on: Nov 25, 2014
 *      Author: thallock
 */

#include "main/global.h"

#include <fstream>

int get_int(std::ifstream& in)
{
	int tmp;
	in >> tmp;
	return tmp;
}

void compare_dists(int len, double* d1, double *d2)
{
	double sum1 = 0;
	double sum2 = 0;

	std::ofstream f{"r_test.txt"};

	f << "x=c(";
	for (int i = 0; i < len; i++)
	{
		f << std::setw(12) << d1[i] << ((i == len-1) ? ")" : ",");
	}
	f << '\n';
	f << "y=c(";
	for (int i = 0; i < len; i++)
	{
		f << std::setw(12) << d2[i] << ((i == len-1) ? ")" : ",");
	}
	f << '\n';

	f << "ks.test(x, y)\n" ;

	for (int i = 0; i < len; i++)
	{
		sum1 += d1[i];
		sum2 += d2[i];
	}

	std::cout << "avg1: " << (sum1 / len) << std::endl;
	std::cout << "avg2: " << (sum2 / len) << std::endl;
}

void stats()
{
	std::ifstream stats_file {"output/robust/results.txt"};

	get_int(stats_file);

	double total1 = get_int(stats_file);
	double total2 = get_int(stats_file);
	double max    = get_int(stats_file);
	int iterations = get_int(stats_file);

	double *cs = new double[iterations];
	double *t1 = new double[iterations];
	double *t2 = new double[iterations];

	double *dist1 = new double[iterations];
	double *dist2 = new double[iterations];

	for (int i = 0; i < iterations; i++)
	{
		cs[i] = get_int(stats_file);
		t1[i] = get_int(stats_file);
		t2[i] = get_int(stats_file);

		dist1[i] = t1[i] / (total1 - cs[i]);
		dist2[i] = t2[i] / (total2 - cs[i]);
	}

	compare_dists(iterations, dist1, dist2);

	delete[] cs;
	delete[] t1;
	delete[] t2;
	delete[] dist1;
	delete[] dist2;
}
