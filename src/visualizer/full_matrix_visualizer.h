/** vpr_relocalization: a library for visual place recognition in changing 
** environments with efficient relocalization step.
** Copyright (c) 2017 O. Vysotska, C. Stachniss, University of Bonn
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
**/

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
