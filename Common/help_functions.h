/*
 * help_functions.h
 *
 *  Created on: 11 мая 2016 г.
 *      Author: user
 */

#ifndef COMMON_HELP_FUNCTIONS_H_
#define COMMON_HELP_FUNCTIONS_H_

#include <sstream>
#include "types.h"

static const char kNewPointIndicator = '#';
static const char kComponentsDelimiter = ',';

void InsertPointToStream(std::stringstream& stream, const Point& point);

Points ExtractPointsFromMessage(std::stringstream& stream,
                                const size_t points_count,
                                const size_t dimensions);
void InsertPointsToMessage(std::stringstream& stream,
                           PointConstIterator cbegin,
                           PointConstIterator cend);
void MovePointsToMessage(std::stringstream& stream, PointIterator begin, PointIterator end);



#endif /* COMMON_HELP_FUNCTIONS_H_ */
