/* Copyright Olga Vysotska, year 2017.
** In case of any problems with the code please contact me.
** Email: olga.vysotska@uni-bonn.de.
*/

#ifndef SRC_VISUALIZER_FULL_MATRIX_VISUALIZER_H_
#define SRC_VISUALIZER_FULL_MATRIX_VISUALIZER_H_
#include <memory>
#include <string>
#include <vector>
#include "database/cost_matrix_database.h"
#include "online_localizer/ilocvisualizer.h"

/**
 * @brief      Overlayes the computed path as well as expanded nodes (green)
 * over the provided cost matrix. Saves the image after the localization is
 * finished for the entire query sequence.
 */
class FullMatrixVisualizer : public iLocVisualizer {
 public:
  using Ptr = std::shared_ptr<FullMatrixVisualizer>;
  using ConstPtr = std::shared_ptr<const FullMatrixVisualizer>;

  void setDatabase(CostMatrixDatabase::Ptr database);
  void setOutImageName(const std::string &outfileName);

  void drawPath(const std::vector<PathElement> &path) override;
  void drawFrontier(const NodeSet &frontier) override;
  void drawExpansion(NodeSet expansion) override;
  void processFinished() override;

 private:
  std::vector<PathElement> _path;
  NodeSet _expansion;
  CostMatrixDatabase::Ptr _database;
  std::string _outfileImg = "result.png";
};

#endif  // SRC_VISUALIZER_FULL_MATRIX_VISUALIZER_H_
