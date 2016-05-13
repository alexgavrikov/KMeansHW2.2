/*
 * server_main.cpp
 *
 *  Created on: 11 мая 2016 г.
 *      Author: user
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include "../Common/read_write.h"
#include "server.h"

int main(int argc, char** argv) {

  if (argc != 6) {
    std::printf("Usage: %s clusters_number input_file output_file port_to_bind slaves_num\n",
                argv[0]);
    return 1;
  }

  char* input_file = argv[2];
  std::ifstream input;
  input.open(input_file, std::ifstream::in);
  if (!input) {
    std::cerr << "Error: input file could not be opened" << std::endl;
    return 1;
  }

  Points data = ReadPoints(input);
  input.close();

  char* output_file = argv[3];
  std::ofstream output;
  output.open(output_file, std::ifstream::out);
  if (!output) {
    std::cerr << "Error: output file could not be opened" << std::endl;
    return 1;
  }

  KMeansHeadServer k_means_head_server;
  const size_t port = std::atoi(argv[4]);
  const size_t slaves_count = std::atoi(argv[5]);
  k_means_head_server.PrepareCluster(slaves_count, port, "");

  srand(123);
  const size_t K = std::atoi(argv[1]);
  // DoKMeansJob clears data. Memory optimization
  Points centroids = k_means_head_server.DoKMeansJob(data, K);
  // data is empty

  WriteOutput(centroids, output);
  output.close();

  return 0;
}

