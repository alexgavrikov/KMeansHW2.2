/*
 * slave_main.cpp
 *
 *  Created on: 11 мая 2016 г.
 *      Author: user
 */

#include <cstdlib>
#include <fstream>
#include "slave.h"

int main(int argc, char** argv) {

  if (argc != 2) {
    std::printf("Usage: %s port_to_bind\n", argv[0]);
    return 1;
  }
  int port_to_bind = std::atoi(argv[1]);
  KMeansSlaveClient k_means_slave_client("", port_to_bind);

  for (bool job_is_needed = true; job_is_needed;) {
    job_is_needed = k_means_slave_client.Run();
  }

  return 0;
}


