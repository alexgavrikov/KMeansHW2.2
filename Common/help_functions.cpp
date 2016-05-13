/*
 * help_functions.cpp
 *
 *  Created on: 11 мая 2016 г.
 *      Author: user
 */

#include "help_functions.h"

void InsertPointToStream(std::stringstream& stream, const Point& point) {
  stream << kNewPointIndicator;
  for (const double component : point) {
    stream << component << kComponentsDelimiter;
  }
}

Points ExtractPointsFromMessage(std::stringstream& stream,
                                const size_t points_count,
                                const size_t dimensions) {
  Points points(points_count, Point(dimensions));
  char delimiter;
  for (size_t point_index = 0; point_index != points.size(); ++point_index) {
    // Reading kNewPointIndicator
    stream >> delimiter;
    for (size_t component_index = 0; component_index != dimensions; ++component_index) {
      stream >> points[point_index][component_index] >> delimiter;
    }
  }

  return points;
}

void InsertPointsToMessage(std::stringstream& stream,
                           PointConstIterator cbegin,
                           PointConstIterator cend) {
  for (auto current_iter = cbegin; current_iter != cend; ++current_iter) {
    InsertPointToStream(stream, *current_iter);
  }
}

// Moving - for memory optimization
void MovePointsToMessage(std::stringstream& stream, PointIterator begin, PointIterator end) {
  for (auto current_iter = begin; current_iter != end; ++current_iter) {
    InsertPointToStream(stream, *current_iter);
    // Gradually we release memory
    Point().swap(*current_iter);
  }
}

