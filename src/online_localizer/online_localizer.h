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

#ifndef SRC_ONLINE_LOCALIZER_ONLINE_LOCALIZER_H_
#define SRC_ONLINE_LOCALIZER_ONLINE_LOCALIZER_H_

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <queue>

#include "online_localizer/ilocvisualizer.h"
#include "online_localizer/path_element.h"
#include "successor_manager/node.h"
#include "successor_manager/successor_manager.h"

/**
 * @brief      Class for online localization
 */
class OnlineLocalizer {
 public:
  using PredMap = std::unordered_map<int, std::unordered_map<int, Node> >;
  using AccCostsMap = std::unordered_map<int, std::unordered_map<int, double> >;

  OnlineLocalizer();
  ~OnlineLocalizer() {}
  void setQuerySize(int size) { _querySize = size; }
  bool setSuccessorManager(SuccessorManager::Ptr succManager);
  bool setVisualizer(iLocVisualizer::Ptr vis);
  bool setExpansionRate(double rate);
  bool setNonMatchingCost(double non_match);

  /**
   * @brief      dumps path to the file. Line format: quId refId status (0-
   * hidden, 1-real)
   *
   * @param[in]  filename  The filename
   */
  void printPath(const std::string &filename) const;

  bool isReady() const;
  void run();
  void processImage(int quId);
  // core working function
  void matchImage(int quId);
  std::vector<PathElement> getCurrentPath() const;
  std::vector<PathElement> getLastNmatches(int N) const;

  // TODO: move these into protected
  // more on private side
  void updateSearch(const NodeSet &successors);
  void updateGraph(const Node &parent,
                   const NodeSet &successors);
  Node getProminentSuccessor(const NodeSet &successors) const;
  bool predExists(const Node &node) const;
  bool nodeWorthExpanding(const Node &node) const;
  double computeAveragePathCost() const;

  bool isLost(int N, double perc) const;

  void visualize() const;

 private:
  int _querySize = 0;
  int _slidingWindowSize = 5; // frames
  bool _needReloc = false;
  double _expansionRate = -1.0;
  double _nonMatchCost = -1.0;

   std::priority_queue<Node> _frontier;
  // stores parent for each node
  PredMap _pred;
  // stores the accumulative  cost for each node
  AccCostsMap _accCosts;
  Node _currentBestHyp;

  SuccessorManager::Ptr _successorManager = nullptr;
  iLocVisualizer::Ptr _vis = nullptr;

  NodeSet _expandedRecently;
};

#endif  // SRC_ONLINE_LOCALIZER_ONLINE_LOCALIZER_H_
