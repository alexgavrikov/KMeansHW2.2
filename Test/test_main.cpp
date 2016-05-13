/*
 * test_main.cpp
 *
 *  Created on: 12 мая 2016 г.
 *      Author: user
 */

#include <cstdlib>
#include <iostream>

#include "../Common/read_write.h"

double Distance(const Point& point1, const Point& point2);
double FindDistanceToNearestCentroid(const Points& centroids,
                                     const Point& point);
double ComputeAverageDistanceToNearestCentroid(const Points& test_data,
                                               const Points& centroids);

int main(int argc, char** argv) {
  if (argc != 4) {
    std::printf(
        "Usage: %s input_file_with_test_data input_file_with_centroids output_file\n",
        argv[0]);
    return 1;
  }

  char* test_data_file = argv[1];
  std::ifstream test_data_input;
  test_data_input.open(test_data_file, std::ifstream::in);
  if (!test_data_input) {
    std::cerr << "Error: test_data_input file could not be opened\n";
    return 1;
  }

  Points test_data = ReadPoints(test_data_input);
  test_data_input.close();

  char* centroids_file = argv[2];
  std::ifstream centroids_input;
  centroids_input.open(centroids_file, std::ifstream::in);
  if (!centroids_input) {
    std::cerr << "Error: centroids_input file could not be opened\n";
    return 1;
  }

  Points centroids = ReadPoints(centroids_input);
  centroids_input.close();

  char* output_file = argv[3];
  std::ofstream output;
  output.open(output_file, std::ifstream::out);
  if (!output) {
    std::cerr << "Error: output file could not be opened\n";
    return 1;
  }
  double average_distance_to_nearest_centroid =
      ComputeAverageDistanceToNearestCentroid(test_data, centroids);

  output.precision(15);
  output << average_distance_to_nearest_centroid << std::endl;
  output.close();

  return 0;
}

double Distance(const Point& point1, const Point& point2) {
  double distance_sqr = 0;
  size_t dimensions = point1.size();
  for (size_t i = 0; i < dimensions; ++i) {
    distance_sqr += (point1[i] - point2[i]) * (point1[i] - point2[i]);
  }

  return distance_sqr;
}

double FindDistanceToNearestCentroid(const Points& centroids,
                                     const Point& point) {
  double min_distance = Distance(point, centroids[0]);
  size_t centroid_index = 0;
  for (size_t i = 1; i < centroids.size(); ++i) {
    double distance = Distance(point, centroids[i]);
    if (distance < min_distance) {
      min_distance = distance;
      centroid_index = i;
    }
  }
  return min_distance;
}

double ComputeAverageDistanceToNearestCentroid(const Points& test_data,
                                               const Points& centroids) {
  const size_t data_size = test_data.size();
  const size_t dimensions = test_data[0].size();
  long double distances_sum = 0;
  for (const Point& test_data_point : test_data) {
    distances_sum += FindDistanceToNearestCentroid(centroids,
                                                   test_data_point);
  }

  return distances_sum / data_size;
}


