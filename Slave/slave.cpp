/*
 * slave.cpp
 *
 *  Created on: 11 мая 2016 г.
 *      Author: user
 */

#include "slave.h"
#include <sstream>
#include "../Common/help_functions.h"

#include <iostream>

bool KMeansSlaveClient::Run() const {
  // GETTING INITIAL DATA
  std::string initial_data_message = Recieve();
  if (initial_data_message == "CLOSE") {
    return false;
  }

  Points portion_of_data;
  size_t portion_size, dimensions, K;
  {
    std::stringstream stream(std::move(initial_data_message));
    char ch;
    stream >> portion_size >> ch >> dimensions >> ch >> K >> ch;
    portion_of_data = ExtractPointsFromMessage(stream, portion_size, dimensions);
  }
  std::string ok_message("OK");
  Send(ok_message);

  // WORKING LOOP
  KMeansJob(portion_of_data, K);

  return true;
}

void KMeansSlaveClient::KMeansJob(const Points& portion_of_data, const size_t K) const {
  const size_t portion_size = portion_of_data.size();
  const size_t dimensions = portion_of_data[0].size();
  std::vector<size_t> clusters(portion_size);
  while (true) {
    std::string centroids_message = Recieve();

    std::stringstream stream_with_centroids(centroids_message);
    Points centroids = ExtractPointsFromMessage(stream_with_centroids, K, dimensions);

    Points new_centroids(K, Point(dimensions));
    std::vector<size_t> clusters_sizes(K);
    bool converged = JobItself(portion_of_data, centroids, clusters, new_centroids, clusters_sizes);

    std::stringstream converging_info_message_stream;
    converging_info_message_stream << converged << kComponentsDelimiter;
    Send(converging_info_message_stream.str());

    std::string ok_or_bye_message = Recieve();
    if (ok_or_bye_message == "BYE") {
      break;
    }

    std::stringstream new_centroids_and_clusters_subsizes_message;
    new_centroids_and_clusters_subsizes_message.precision(20);
    InsertPointsToMessage(new_centroids_and_clusters_subsizes_message, new_centroids.cbegin(),
                          new_centroids.cend());
    for (const auto& cluster_size : clusters_sizes) {
      new_centroids_and_clusters_subsizes_message << cluster_size << kComponentsDelimiter;
    }
    Send(new_centroids_and_clusters_subsizes_message.str());
  }
}

bool KMeansSlaveClient::JobItself(const Points& portion_of_data,
                                  const Points& centroids,
                                  std::vector<size_t>& clusters,
                                  Points& new_centroids,
                                  std::vector<size_t>& clusters_sizes) {
  const size_t portion_size = portion_of_data.size();
  const size_t dimensions = portion_of_data[0].size();
  bool converged = true;
  for (size_t i = 0; i < portion_size; ++i) {
    size_t nearest_cluster = FindNearestCentroid(centroids, portion_of_data[i]);
    if (clusters[i] != nearest_cluster) {
      clusters[i] = nearest_cluster;
      converged = false;
    }

    for (size_t d = 0; d < dimensions; ++d) {
      new_centroids[clusters[i]][d] += portion_of_data[i][d];
    }
    ++clusters_sizes[clusters[i]];
  }

  return converged;
}

double KMeansSlaveClient::Distance(const Point& point1, const Point& point2) {
  double distance_sqr = 0;
  size_t dimensions = point1.size();
  for (size_t i = 0; i < dimensions; ++i) {
    distance_sqr += (point1[i] - point2[i]) * (point1[i] - point2[i]);
  }

  return distance_sqr;
}

size_t KMeansSlaveClient::FindNearestCentroid(const Points& centroids, const Point& point) {
  double min_distance = Distance(point, centroids[0]);
  size_t centroid_index = 0;
  for (size_t i = 1; i < centroids.size(); ++i) {
    double distance = Distance(point, centroids[i]);
    if (distance < min_distance) {
      min_distance = distance;
      centroid_index = i;
    }
  }
  return centroid_index;
}

