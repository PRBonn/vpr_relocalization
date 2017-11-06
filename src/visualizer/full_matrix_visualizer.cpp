/* Copyright Olga Vysotska, year 2017.
** In case of any problems with the code please contact me.
** Email: olga.vysotska@uni-bonn.de.
*/

#include "visualizer/full_matrix_visualizer.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void FullMatrixVisualizer::drawPath(const std::vector<PathElement> &path) {
  _path = path;
}

void FullMatrixVisualizer::setOutImageName(const std::string &outfileName) {
  if (outfileName.empty()) {
    printf(
        "[WARNING][FullMatrixVisualizer] You are trying to set an empty "
        "filename. Ignoring.\n");
    return;
  }
  _outfileImg = outfileName;
}

void FullMatrixVisualizer::drawExpansion(NodeSet expansion) {
  // _expansion.merge(expansion);
  // std::vector<Node> nodes = expansion.toVector();
  for (const auto &e : expansion) {
    _expansion.insert(e);
  }
}
void FullMatrixVisualizer::drawFrontier(const NodeSet &frontier) {}

void FullMatrixVisualizer::processFinished() {
  // plot everything now
  if (!_database) {
    printf("Oops. It seems like you forgot to set the database :(\n");
    return;
  }
  printf("[INFO] Plotting image...\n");
  cv::Mat costs = _database->getCosts();
  cv::cvtColor(costs, costs, CV_GRAY2BGR);
  if (costs.type() != CV_32FC3) {
    costs.convertTo(costs, CV_32FC3);
  }

  cv::normalize(costs, costs, 0, 255, cv::NORM_MINMAX, CV_32FC3);
  // overlay expansion
  // std::vector<Node> expanded = _expansion.toVector();
  for (const auto &node : _expansion) {
    if (node.quId < 0 || node.quId >= costs.rows) {
      printf("[ERROR] Query index outside the range %d\n", node.quId);
      exit(EXIT_FAILURE);
    }
    if (node.refId < 0 || node.refId >= costs.cols) {
      printf("[ERROR] Reference index outside the range %d\n", node.refId);
      exit(EXIT_FAILURE);
    }
    // green
    costs.at<cv::Vec3f>(node.quId, node.refId) = cv::Vec3f(0.0, 255.0, 0.0);
  }
  printf("[EXPANSION] Number of expanded nodes %lu\n", _expansion.size());

  // overlay path
  for (const auto &el : _path) {
    if (el.quId < 0 || el.quId >= costs.rows) {
      printf("[ERROR] Query index outside the range %d\n", el.quId);
      exit(EXIT_FAILURE);
    }
    if (el.refId < 0 || el.refId >= costs.cols) {
      printf("[ERROR] Reference index outside the range %d\n", el.refId);
      exit(EXIT_FAILURE);
    }

    if (el.state == HIDDEN) {
      // blue
      costs.at<cv::Vec3f>(el.quId, el.refId) = cv::Vec3f(255.0, 0.0, 0.0);
    } else {
      // red
      costs.at<cv::Vec3f>(el.quId, el.refId) = cv::Vec3f(0.0, 0.0, 255.0);
    }
  }

  costs.convertTo(costs, CV_8UC3);
  cv::imwrite(_outfileImg.c_str(), costs);
  printf("[INFO][FullMatrixVisualizer] Image saved to a file %s\n",
         _outfileImg.c_str());
}

void FullMatrixVisualizer::setDatabase(CostMatrixDatabase::Ptr database) {
  if (!database) {
    printf("[ERROR][FullMatrixVisualizer] Invalid database\n");
    return;
  }
  _database = database;
}
