/*
 * slave.h
 *
 *  Created on: 11 мая 2016 г.
 *      Author: user
 */

#ifndef SLAVE_SLAVE_H_
#define SLAVE_SLAVE_H_

#include "../Common/client_server.h"
#include "../Common/types.h"

class KMeansSlaveClient : private LocalClient {
public:
  KMeansSlaveClient(const std::string& host, int port)
      : LocalClient(host, port) {
  }
  bool Run() const;

private:
  void KMeansJob(const Points& portion_of_data, const size_t K) const;
  static bool JobItself(const Points& portion_of_data,
                        const Points& centroids,
                        std::vector<size_t>& clusters,
                        Points& new_centroids,
                        std::vector<size_t>& clusters_sizes);
  static double Distance(const Point& point1, const Point& point2);
  static size_t FindNearestCentroid(const Points& centroids,
                                    const Point& point);
};



#endif /* SLAVE_SLAVE_H_ */
