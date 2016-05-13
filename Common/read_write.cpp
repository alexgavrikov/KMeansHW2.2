/*
 * read_write.cpp
 *
 *  Created on: 11 мая 2016 г.
 *      Author: user
 */

#include "read_write.h"
#include <cstdlib>
#include <string>

Points ReadPoints(std::ifstream& input) {
  Points data;
  for (std::string id_str;
      std::getline(input, id_str, '\t');) {
    Point point;
    char last_delimiter;
    for (last_delimiter = '\t';
        last_delimiter != '\r' && last_delimiter != '\n'; last_delimiter =
            input.get()) {
      std::string component_str;
      input >> component_str;
      point.push_back(std::atof(component_str.c_str()));
    }
    if (last_delimiter == '\r') {
      const char slash_n = input.get();
    }

    data.emplace_back(std::move(point));
  }

  return data;
}

void WriteOutput(const Points& centroids, std::ofstream& output) {
  output.precision(20);
  for (size_t centroid_id = 0; centroid_id != centroids.size();
      ++centroid_id) {
    output << " centroid_id:" << centroid_id;
    for (const double component : centroids[centroid_id]) {
      output << '\t' << component;
    }
    output << std::endl;
  }
}


