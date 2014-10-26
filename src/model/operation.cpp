/*
 * operation.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: thallock
 */

#include "model/operation.h"

bool op_follows_op[6][6] =
{
//              Pickup       Dropoff        Store        Unstore       Dump        Replace
/*Pickup */   { false       , true        , true        , false       , false    , false     },
/*Dropoff*/   { false       , false       , false       , true        , true     , false     },
/*Store  */   { false       , false       , false       , true        , true     , false     },
/*Unstore*/   { false       , true        , true        , false       , false    , false     },
/*Dump   */   { true        , false       , false       , false       , false    , true      },
/*Replace*/   { false       , false       , false       , true        , true     , false     },
};


char operation_to_char(const operation o)
{
	switch(o)
	{
		case Pickup:  return 'P';
		case Dropoff: return 'D';
		case Store:   return 'S';
		case Unstore: return 'U';
		case Dump:    return 'E';
		case Replace: return 'R';
	}
	std::cerr << "bad operation: " << o << std::endl;
	exit(-1);
	return '?';
}
