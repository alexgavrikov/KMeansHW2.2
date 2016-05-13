/*
 * server.h
 *
 *  Created on: 11 мая 2016 г.
 *      Author: user
 */

#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <vector>
#include "../Common/client_server.h"
#include "../Common/types.h"

class KMeansHeadServer : private ClientServerBase {
public:
  ~KMeansHeadServer() {
    for (const auto& slave_socket : slave_clients_sockets) {
      Send("CLOSE", slave_socket);
      close(slave_socket);
    }
  }

  void PrepareCluster(const size_t slaves_count,
                      int slaves_port,
                      const std::string& host);
  Points DoKMeansJob(Points& data, const size_t K) const;

private:
  void Bind(int listen_socket, int port, const std::string &host) const;
  void GiveJobsToSlaves(Points& data, const size_t K) const;
  void SendCentroidsToSlaves(const Points& centroids) const;
  bool CheckIfConverged() const;
  Points GetRefreshedCentroids(const size_t dimensions,
                               const size_t K) const;

  static unsigned int UniformRandom(unsigned int max_value);
  static Point GetRandomPosition(const Points& centroids);
  static Points InitCentroidsRandomly(const Points& data, const size_t K);
  static std::vector<size_t> ExtractClustersSubsizesFromMessage(std::stringstream& stream,
                                                                const size_t K);
  static void CombineCentroids(Points& centroids, const Points& centroids_new_parts);
  static void CombineClustersSizes(std::vector<size_t>& clusters_sizes,
                                   const std::vector<size_t>& clusters_subsizes);

  std::vector<int> slave_clients_sockets;
};



#endif /* SERVER_SERVER_H_ */
