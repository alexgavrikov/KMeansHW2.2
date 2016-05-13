/*
 * server.cpp
 *
 *  Created on: 11 мая 2016 г.
 *      Author: user
 */

#include "server.h"
#include <iostream>
#include <memory>
#include <random>
#include "../Common/help_functions.h"

void KMeansHeadServer::PrepareCluster(const size_t slaves_count,
                                     int slaves_port,
                                     const std::string& host) {
  int slaves_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  Bind(slaves_listen_socket, slaves_port, host);
  slave_clients_sockets.reserve(slaves_count);
  for (size_t slave_index = 0; slave_index != slaves_count; ++slave_index) {
    slave_clients_sockets.push_back(accept(slaves_listen_socket, nullptr, nullptr));
  }
}

void KMeansHeadServer::Bind(int listen_socket, int port, const std::string &host) const {
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  if (!host.empty()) {
    int addr;
    if (!ResolveHost(host, addr))
      throw std::runtime_error("can't resolve host");
    address.sin_addr.s_addr = addr;
  } else {
    address.sin_addr.s_addr = INADDR_ANY;
  }
  if (bind(listen_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0)
    throw std::runtime_error("can't bind");
  if (listen(listen_socket, 1) < 0)
    throw std::runtime_error("can't start listening");
}

Points KMeansHeadServer::DoKMeansJob(Points& data, const size_t K) const {
  const size_t data_size = data.size();
  const size_t dimensions = data[0].size();
  Points centroids = InitCentroidsRandomly(data, K);
  // GiveJobsToSlaves clears data. Memory optimization
  GiveJobsToSlaves(data, K);
  // data is empty
  for (bool converged = false; !converged;) {
    SendCentroidsToSlaves(centroids);
    converged = CheckIfConverged();
    std::string message = converged ? "BYE" : "OK";
    for (const auto& slave_socket : slave_clients_sockets) {
      Send(message, slave_socket);
    }
    if (!converged) {
      centroids = GetRefreshedCentroids(dimensions, K);
    }
  }

  return centroids;

}

void KMeansHeadServer::GiveJobsToSlaves(Points& data, const size_t K) const {
  const size_t data_size = data.size();
  const size_t dimensions = data[0].size();

  const double portion_size = static_cast<double>(data_size) / slave_clients_sockets.size();
  for (int slave_index = 0; slave_index != slave_clients_sockets.size(); ++slave_index) {
    const size_t portion_begin_index = slave_index * portion_size;
    const size_t portion_end_index = (slave_index + 1) * portion_size;
    const size_t current_slave_portion_size = portion_end_index - portion_begin_index;
    std::string message;
    {
      std::stringstream message_stream;
      message_stream.precision(20);
      message_stream << current_slave_portion_size << kComponentsDelimiter << dimensions
          << kComponentsDelimiter << K << kComponentsDelimiter;
      // Gradually we move data to message_stream, thus clearing data (releasing memory).
      MovePointsToMessage(message_stream, data.begin() + portion_begin_index,
                          data.begin() + portion_end_index);
      message = message_stream.str();
    }
    Send(message, slave_clients_sockets[slave_index]);
    Recieve(slave_clients_sockets[slave_index]);
  }
  Points().swap(data);
}

void KMeansHeadServer::SendCentroidsToSlaves(const Points& centroids) const {
  std::stringstream centroids_message_stream;
  centroids_message_stream.precision(20);
  InsertPointsToMessage(centroids_message_stream, centroids.cbegin(), centroids.cend());
  for (const auto& slave_socket : slave_clients_sockets) {
    Send(centroids_message_stream.str(), slave_socket);
  }
}

bool KMeansHeadServer::CheckIfConverged() const {
  bool converged = true;
  for (const auto& slave_socket : slave_clients_sockets) {
    std::stringstream message(Recieve(slave_socket));
    bool slave_portion_converged;
    message >> slave_portion_converged;
    converged = converged && slave_portion_converged;
  }

  return converged;
}

Points KMeansHeadServer::GetRefreshedCentroids(const size_t dimensions, const size_t K) const {
  std::vector<size_t> clusters_sizes(K);
  Points centroids(K, Point(dimensions));

  for (const auto& slave_socket : slave_clients_sockets) {
    std::stringstream message(Recieve(slave_socket));
    Points centroids_parts = ExtractPointsFromMessage(message, K, dimensions);
    CombineCentroids(centroids, centroids_parts);

    std::vector<size_t> clusters_subsizes = ExtractClustersSubsizesFromMessage(message, K);
    CombineClustersSizes(clusters_sizes, clusters_subsizes);
  }

  for (size_t i = 0; i < K; ++i) {
    if (clusters_sizes[i] != 0) {
      for (size_t d = 0; d < dimensions; ++d) {
        centroids[i][d] /= clusters_sizes[i];
      }
    }
  }

  for (size_t i = 0; i < K; ++i) {
    if (clusters_sizes[i] == 0) {
      centroids[i] = GetRandomPosition(centroids);
    }
  }

  return centroids;
}

// Gives random number in range [0..max_value]
unsigned int KMeansHeadServer::UniformRandom(unsigned int max_value) {
  unsigned int rnd = ((static_cast<unsigned int>(rand()) % 32768) << 17)
      | ((static_cast<unsigned int>(rand()) % 32768) << 2) | rand() % 4;
  return ((max_value + 1 == 0) ? rnd : rnd % (max_value + 1));
}

// Calculates new centroid position as mean of positions of 3 random centroids
Point KMeansHeadServer::GetRandomPosition(const Points& centroids) {
  size_t K = centroids.size();
  int c1 = rand() % K;
  int c2 = rand() % K;
  int c3 = rand() % K;
  size_t dimensions = centroids[0].size();
  Point new_position(dimensions);
  for (size_t d = 0; d < dimensions; ++d) {
    new_position[d] = (centroids[c1][d] + centroids[c2][d] + centroids[c3][d]) / 3;
  }
  return new_position;
}

// Initialize centroids randomly at data points
Points KMeansHeadServer::InitCentroidsRandomly(const Points& data, const size_t K) {
  Points centroids(K);
  for (size_t i = 0; i < K; ++i) {
    const size_t data_index = UniformRandom(data.size() - 1);
    centroids[i] = data[data_index];
  }

  return centroids;
}

std::vector<size_t> KMeansHeadServer::ExtractClustersSubsizesFromMessage(std::stringstream& stream,
                                                                        const size_t K) {
  std::vector<size_t> clusters_subsizes(K);
  char delimiter;
  for (size_t index = 0; index != K; ++index) {
    stream >> clusters_subsizes[index] >> delimiter;
  }

  return clusters_subsizes;
}

void KMeansHeadServer::CombineCentroids(Points& centroids, const Points& centroids_parts) {
  for (size_t point_index = 0; point_index != centroids.size(); ++point_index) {
    auto& centroid = centroids[point_index];
    const auto& centroid_to_add = centroids_parts[point_index];
    for (size_t component_index = 0; component_index != centroid.size(); ++component_index) {
      centroid[component_index] += centroid_to_add[component_index];
    }
  }
}

void KMeansHeadServer::CombineClustersSizes(std::vector<size_t>& clusters_sizes,
                                           const std::vector<size_t>& clusters_subsizes) {
  for (size_t index = 0; index != clusters_sizes.size(); ++index) {
    clusters_sizes[index] += clusters_subsizes[index];
  }
}
