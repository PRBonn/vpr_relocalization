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

#include "online_localizer/online_localizer.h"
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <limits>
#include <string>
#include <vector>
#include "tools/timer/timer.h"

using std::vector;
using std::string;

OnlineLocalizer::OnlineLocalizer() {
  // _pred[-1][-1] = Node(-1, -1, 0.0);
  _pred[SOURCE_NODE.quId][SOURCE_NODE.refId] = SOURCE_NODE;
  Node source = SOURCE_NODE;
  source.accCost = 0.0;
  // SOURCE_NODE.accCost = 0.0;
  _accCosts[SOURCE_NODE.quId][SOURCE_NODE.refId] = source.accCost;
  _frontier.push(source);
  _currentBestHyp = source;
}

bool OnlineLocalizer::setSuccessorManager(SuccessorManager::Ptr succManager) {
  if (!succManager) {
    printf("[ERROR][OnlineLocalizer] Successor manager is not set\n");
    return false;
  }
  _successorManager = succManager;
  return true;
}

bool OnlineLocalizer::setVisualizer(iLocVisualizer::Ptr vis) {
  if (!vis) {
    printf("[ERROR][OnlineLocalizer] Visualizer is not set, but wanted!\n");
    return false;
  }
  _vis = vis;
  return true;
}

bool OnlineLocalizer::setExpansionRate(double rate) {
  if (rate < 0.0) {
    printf(
        "[ERROR][OnlineLocalizer] Wrong value for the expanstion rate. The "
        "value should be in [0,1].\n");
    return false;
  }
  _expansionRate = rate;
  return true;
}

bool OnlineLocalizer::setNonMatchingCost(double non_match) {
  // if (non_match < 1.0) {
  if (non_match < 0.0) {
    printf("[ERROR][OnlineLocalizer] Invalid Matching cost\n");
    return false;
  }
  _nonMatchCost = non_match;
  return true;
}

bool OnlineLocalizer::isReady() const {
  if (!_successorManager) {
    printf("[ERROR][OnlineLocalizer] Successor manager is not set\n");
    return false;
  }
  if (_querySize == 0) {
    printf("[ERROR][OnlineLocalizer] Size of the query sequence is not set\n");
    return false;
  }
  if (_expansionRate < 0.0) {
    printf("[ERROR][OnlineLocalizer] Expansion rate is not set\n");
    return false;
  }
  if (_nonMatchCost < 0.0) {
    printf("[ERROR][OnlineLocalizer] Non matching cost is not set\n");
    return false;
  }
  // if(_slidingWindowSize < 0){
  //   printf("[ERROR][OnlineLocalizer] Sliding window size is not set\n");
  //   return false;
  // }
  return true;
}

// frontier picking up routine
void OnlineLocalizer::matchImage(int quId) {
  _expandedRecently.clear();

  std::unordered_set<Node> children;
  if (_needReloc) {
    _frontier = std::priority_queue<Node>();  // reseting priority_queue
    printf("[INFO][OnlineLocalizer] RELOCALIZATION\n");
    Node expandedNode = _currentBestHyp;
    children = _successorManager->getSuccessorsIfLost(expandedNode);
    // add only the most promising node to the frontier
    // need to call update search, since it updates the current best
    // hypothesis
    updateSearch(children);
    children.clear();
    // just one most promising child is added to the graph
    children.insert(_currentBestHyp);
    updateGraph(expandedNode, children);
  } else {
    bool row_reached = false;
    printf("[INFO][OnlineLocalizer] NOT LOST\n");
    while (!_frontier.empty() && !row_reached) {
      // counterNodes++;
      Node expandedNode = _frontier.top();
      _frontier.pop();
      int expanded_row = expandedNode.quId;

      if (!nodeWorthExpanding(expandedNode)) {
        continue;
      }
      // printf("Node %d %d  %d worth expanding\n", expandedNode.quId,
      // expandedNode.refKey.refId, expandedNode.refKey.seqId);
      children = _successorManager->getSuccessors(expandedNode);
      updateGraph(expandedNode, children);
      updateSearch(children);
      if (expanded_row == quId - 1) {
        row_reached = true;
      } else if (expanded_row >= quId) {
        printf(
            "[ERROR][Online Localizer] You have expanded the nodes higher than "
            "current query image id. Something went wrong\n");
        exit(EXIT_FAILURE);
      }
    }
  }
  for (const Node &n : children) {
    _expandedRecently.insert(n);
  }
}

void OnlineLocalizer::processImage(int quId) {
  printf("[DEBUG][OnlineLocalizer] Checking image %d\n", quId);
  if (quId == 0) {
    _needReloc = true;
  }
  matchImage(quId);

  // printf("[INFO] Qu %d frontier empty %d\n", qu, frontier.empty());
  if (_frontier.empty()) {
    printf("[ERROR] Frontier is empty!\n");
    exit(EXIT_FAILURE);
  }
  // Lost if more than 80% hidden nodes
  if (isLost(_slidingWindowSize, 0.8)) {
    _needReloc = true;
    printf("LOST\n");
  } else {
    // not lost anymore
    _needReloc = false;
  }
}

void OnlineLocalizer::run() {
  if (!isReady()) {
    printf(
        "[ERROR][OnlineLocalizer] Online Localizer is not ready to work. Check "
        "if all needed parameters are set.\n");
    exit(EXIT_FAILURE);
  }

  Timer timer;
  // For the first image consider lost
  // for every image in the query set
  for (int qu = 0; qu < _querySize; ++qu) {
    // while the graph is not expanded till row 'qu'
    timer.start();
    processImage(qu);
    timer.stop();
    printf("[OnlineLocalizer] Matched image %d\n", qu);
    timer.print_elapsed_time(TimeExt::MSec);
    printf("==========================================\n");
    visualize();
  }
  printf("[DEBUG][OnlineLocalizer] Localization finished\n");
  if (_vis) {
    _vis->processFinished();
  }
}

bool OnlineLocalizer::nodeWorthExpanding(const Node &node) const {
  if (node == SOURCE_NODE) {
    // source node-> always worth expanding
    return true;
  }
  // if it the current best hypothesis
  if (node == _currentBestHyp) {
    return true;
  }

  int row_dist = _currentBestHyp.quId - node.quId;
  if (row_dist < 0) {
    printf(
        "[ERROR][INTERNAL][OnlineLocalizer] Trying to expand a node further in "
        "future %d than current best hypothesis %d.\n",
        node.quId, _currentBestHyp.quId);
    exit(EXIT_FAILURE);
  }

  double mean_cost = computeAveragePathCost();
  double potential_cost = node.accCost + row_dist * mean_cost * _expansionRate;
  if (potential_cost <
      _accCosts.at(_currentBestHyp.quId).at(_currentBestHyp.refId)) {
    return true;
  } else {
    return false;
  }
}

void OnlineLocalizer::updateSearch(const NodeSet &successors) {
  Node possibleHyp = getProminentSuccessor(successors);
  // printf("[DEBUG][OnlineLocalizer] Prominent child is: ");
  // possibleHyp.print();

  if (possibleHyp.quId > _currentBestHyp.quId) {
    _currentBestHyp = possibleHyp;
  } else if (possibleHyp.quId == _currentBestHyp.quId) {
    double accCost_current =
        _accCosts[_currentBestHyp.quId][_currentBestHyp.refId];
    double accCost_poss = _accCosts[possibleHyp.quId][possibleHyp.refId];
    if (accCost_poss <= accCost_current) {
      _currentBestHyp = possibleHyp;
    }
  }
  //   printf("[DEBUG][OnlineLocalizer] Current best hyp: ");
  // _currentBestHyp.print();
}

Node OnlineLocalizer::getProminentSuccessor(const NodeSet &successors) const {
  double min_cost = std::numeric_limits<double>::max();
  Node minCost_node;
  for (const Node &node : successors) {
    if (node.idvCost < min_cost) {
      min_cost = node.idvCost;
      minCost_node = node;
    }
  }
  return minCost_node;
}

double OnlineLocalizer::computeAveragePathCost() const {
  double mean_cost = 0;
  bool source_reached = false;
  int elInPath = 0;
  Node pred = _currentBestHyp;

  while (!source_reached) {
    if (pred == SOURCE_NODE) {
      source_reached = true;
      continue;
    }
    mean_cost += pred.idvCost;
    elInPath++;
    pred = _pred.at(pred.quId).at(pred.refId);
  }
  mean_cost = mean_cost / elInPath;
  return mean_cost;
}

bool OnlineLocalizer::predExists(const Node &node) const {
  auto quId_found = _pred.find(node.quId);
  if (quId_found == _pred.end()) {
    // not found
    return false;
  }
  auto node_found = quId_found->second.find(node.refId);
  if (node_found == quId_found->second.end()) {
    return false;
  }
  return true;
}

void OnlineLocalizer::updateGraph(const Node &parent,
                                  const NodeSet &successors) {
  if (successors.empty()) {
    printf(
        "[WARNING] No successors to add to the graph. May lead to disconnected "
        "components\n");
  }
  // for every successor
  // check if the child was visited before (The child was visited if there
  // exists a predecessor for it)
  // if yes, check if the proposed accumulated cost is smaller than existing one
  // if no set a pred for a child.
  // printf("Number of successors %lu\n", successors.size());
  for (Node child : successors) {
    if (predExists(child)) {
      // child was visisted before
      double prev_accCost = _accCosts[child.quId][child.refId];
      double poss_accCost = child.idvCost + parent.accCost;
      if (poss_accCost < prev_accCost) {
        // printf("[DEBUG][OnlineLocalizer] The child was visited before\n");
        printf(
            "[DEBUG][OnlineLocalizer] Possible accumulated cost is smaller "
            "than previous one. Frontier should be updated\n");
        printf(
            "[BINGO] If you have reached this line, please contact Olga :)\n");
        exit(0);
        // throw 10;
        // update pred; update accu_costs + update frontier.
        // assign an alternative parent (the one that came in a function) to a
        // child
        _pred[child.quId][child.refId] = parent;
        _accCosts[child.quId][child.refId] = poss_accCost;
        // do not forget to update the accumulated cost for a child for
        // estimating the priority
        child.accCost = poss_accCost;
        // create a child with higher priority ( lower accCost)
        _frontier.push(child);
      } else {
        // printf("Child was visited before, but new cost is smaller\n");
        // child.print();
      }
    } else {
      // new successor
      _pred[child.quId][child.refId] = parent;
      _accCosts[child.quId][child.refId] = child.idvCost + parent.accCost;
      child.accCost = _accCosts[child.quId][child.refId];
      _frontier.push(child);
    }
  }
}

std::vector<PathElement> OnlineLocalizer::getCurrentPath() const {
  std::vector<PathElement> path;
  bool source_reached = false;
  Node pred = _currentBestHyp;
  // pred.print();

  while (!source_reached) {
    if (pred == SOURCE_NODE) {
      source_reached = true;
      continue;
    }
    NodeState state = pred.idvCost > _nonMatchCost ? HIDDEN : REAL;
    PathElement pathEl(pred.quId, pred.refId, state);
    path.push_back(pathEl);
    pred = _pred.at(pred.quId).at(pred.refId);
  }
  return path;
}

/** check starting from the current best node id, if there is a lot of
 * hidden
 * nodes -- meaning LOST **/
// Check N last image matches. If 90% of them are hidden - LOST!

/**
 * @brief      Determines if lost.
 *
 * @param[in]  N  The number of images to check
 * @param[in]  perc              [0,1] ratio of the hidden nodes to be
 * decided
 * if Lost
 *
 * @return     True if lost, False otherwise.
 */
bool OnlineLocalizer::isLost(int N, double perc) const {
  // path within the sliding window
  std::vector<PathElement> path = getLastNmatches(N);
  if (path.size() < N) {
    // not enough points to make decision
    // printf("[INFO][OnlineLocalizer] The path is too short. Not lost\n");
    return false;
  }
  int lostFactor = 0;
  for (size_t i = 0; i < path.size(); ++i) {
    if (path[i].state == HIDDEN) {
      lostFactor++;
    }
  }

  // printf(
  //     "Number of hidden nodes %d path in sliding window %lu percentage %2.4f
  //     "needed perc %2.4f\n",
  //     lostFactor, path.size(), (double)lostFactor / path.size(), perc);

  if ((double)lostFactor / path.size() > perc) {
    printf("[INFO][OnlineLocalizer] LOST localization\n");
    return true;
  }
  return false;
}

std::vector<PathElement> OnlineLocalizer::getLastNmatches(int N) const {
  std::vector<PathElement> path;
  bool source_reached = false;
  Node pred = _currentBestHyp;
  // pred.print();
  int counter = N;

  while (!source_reached && counter > 0) {
    if (pred == SOURCE_NODE) {
      source_reached = true;
      continue;
    }
    NodeState state = pred.idvCost > _nonMatchCost ? HIDDEN : REAL;
    PathElement pathEl(pred.quId, pred.refId, state);
    path.push_back(pathEl);
    pred = _pred.at(pred.quId).at(pred.refId);
    counter--;
  }
  return path;
}

/**
 * @brief      sends path + frontier to the visualizer
 */
void OnlineLocalizer::visualize() const {
  if (!_vis) {
    // visualizer is not set.
    // printf(
    //     "[WARNING][OnlineLocalizer] The visualizer is not set. Skipping "
    //     "plotting\n");
    return;
  }
  // _vis->drawFrontier(_frontier);
  _vis->drawExpansion(_expandedRecently);
  std::vector<PathElement> path = getCurrentPath();
  std::reverse(path.begin(), path.end());
  _vis->drawPath(path);
}

void OnlineLocalizer::printPath(const std::string &filename) const {
  std::ofstream out(filename.c_str());
  if (!out) {
    printf("[ERROR][OnlineLocalizer] Couldn't open the file %s\n",
           filename.c_str());
    printf("======================= The path is NOT saved\n");
    return;
  }
  std::vector<PathElement> path = getCurrentPath();
  for (const PathElement &el : path) {
    out << el.quId << " " << el.refId << " ";
    out << (el.state == NodeState::HIDDEN ? 0 : 1) << "\n";
  }
  out.close();
  printf("[INFO][OnlineLocalizer] Found path was written to %s\n",
         filename.c_str());
}
