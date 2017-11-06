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

#include "successor_manager/successor_manager.h"
#include <algorithm>
#include <fstream>
// #include <unordered_set>
using std::vector;

bool SuccessorManager::setFanOut(int value) {
  if (value < 0) {
    printf("[ERROR][SuccessorManager] Invalid _fanout\n");
    exit(EXIT_FAILURE);
  }
  if (value == 0) {
    printf(
        "[WARNING][SuccessorManager] You set 0 fanout. You can only model the "
        "situation, where the camera is staying in the reference frame.\n");
    return false;
  }
  _fan_out = value;
  return true;
}

bool SuccessorManager::setDatabase(iDatabase::Ptr database) {
  if (!database) {
    printf("[ERROR][SuccessorManager] Invalid database.\n");
    return false;
  }
  _database = database;
  return true;
}

bool SuccessorManager::setRelocalizer(iRelocalizer::Ptr relocalizer) {
  if (!relocalizer) {
    printf("[ERROR][SuccessorManager] Invalid relocalizer.\n");
    return false;
  }
  _relocalizer = relocalizer;
  return true;
}

/**
 * @brief      Sets the similar places.
 *
 * @param[in]  filename  The filename
 *
 * @return     { description_of_the_return_value }
 */
bool SuccessorManager::setSimilarPlaces(const std::string &filename) {
  std::ifstream in(filename.c_str());
  if (!in) {
    printf("[ERROR][SuccessorManager] Cannot open file %s\n", filename.c_str());
    printf("[======================= Similar places were not set\n");
    return false;
  }
  while (!in.eof()) {
    int ref_id_from, ref_id_to;
    in >> ref_id_from >> ref_id_to;
    _sameRefPlaces[ref_id_from].insert(ref_id_to);
    _sameRefPlaces[ref_id_to].insert(ref_id_from);
  }
  in.close();
  printf("[INFO][SuccessorManager] Similar Places were set\n");
  return true;
}

bool SuccessorManager::isReady() const {
  if (!_database) {
    printf("[ERROR][SuccessorManager] Database is not set. Set it!\n");
    return false;
  }
  return true;
}

/**
 * @brief      Gets the successors.
 *
 * @param[in]  quId   The qu identifier
 * @param[in]  refId  The reference identifier
 *
 * @return     The successors.
 */
std::unordered_set<Node> SuccessorManager::getSuccessors(const Node &node) {
  _successors.clear();

  if (node == SOURCE_NODE) {
    printf(
        "[ERROR][SuccessorManager] Requested to connect source. Robot should "
        "be lost before first image. Use 'getSuccessorsIfLost' function "
        "instead\n");
    exit(EXIT_FAILURE);
  }
  if (node.quId < 0 || node.refId < 0) {
    printf("[ERROR][SuccessorManager] Invalid  image IDs %d %d\n", node.quId,
           node.refId);
    exit(EXIT_FAILURE);
  }
  // check for regular succcessor
  getSuccessorFanOut(node.quId, node.refId);
  // check for additional successors based on similar places
  if (!_sameRefPlaces.empty()) {
    getSuccessorsSimPlaces(node.quId, node.refId);
  } else {
    printf("[DEBUG] Similar Places were not set\n");
  }
  // printf("Successors were computed %d \n", _successors.size());
  return _successors;
}

/**
 * @brief      Gets succesors based on fanout for node (quId, refId) from
 * database. Select followers based on _fanOut;
 *
 * @param[in]  quId   query index
 * @param[in]  refId  reference index
 *
 */
void SuccessorManager::getSuccessorFanOut(int quId, int refId) {
  int left_ref = std::max(refId - _fan_out, 0);
  int right_ref = std::min(refId + _fan_out, _database->refSize() - 1);
  // printf("[DEBUG] For parent %d %d children borders are:\n", quId, refId);
  // printf("[DEBUG] Left: %d, right: %d\n", left_ref, right_ref);

  for (int succ_ref = left_ref; succ_ref <= right_ref; ++succ_ref) {
    Node succ;
    double succ_cost = _database->getCost(quId + 1, succ_ref);
    succ.set(quId + 1, succ_ref, succ_cost);
    _successors.insert(succ);
  }
}

/**
 * @brief      Gets the successors if there are similar places.
 *
 * @param[in]  quId  query index
 */
void SuccessorManager::getSuccessorsSimPlaces(int quId, int refId) {
  auto found = _sameRefPlaces.find(refId);
  std::set<int> simPlaces;
  if (found == _sameRefPlaces.end()) {
    // no similar places for the place refId
    // do not update _successors
  } else {
    simPlaces = _sameRefPlaces[refId];
    for (int simPlace : simPlaces) {
      getSuccessorFanOut(quId, simPlace);
    }
  }
}

/**
 * @brief      Gets the successor by performing the relocalization action.
 * hashes the corresponding feature of the query image and retrieves a set of
 * potential candidates from the pre-computed hash table
 *
 * @param[in]  node  The node
 *
 * @return     The successors if lost.
 */
std::unordered_set<Node> SuccessorManager::getSuccessorsIfLost(
    const Node &node) {
  _successors.clear();
  if (!_relocalizer) {
    printf("[ERROR][SuccessorManager] Relocalizer is not set\n");
    exit(EXIT_FAILURE);
  }
  int succ_qu_id = node.quId + 1;
  std::vector<int> candidates = _relocalizer->getCandidates(succ_qu_id);

  // printf("Relocalizer reported %lu candidates\n", candidates.size());
  if (candidates.empty()) {
    // no similar places found
    printf("[DEBUG] No similar images found\n");
    // propagate one node as if moving
    Node succ;
    double succ_cost = _database->getCost(succ_qu_id, node.refId);
    succ.set(succ_qu_id, node.refId, succ_cost);
    _successors.insert(succ);
  } else {
    // some similar places found
    // printf("[DEBUG] Similar images found %lu\n", candidates.size());
    for (const auto &candId : candidates) {
      Node succ;
      double succ_cost = _database->getCost(succ_qu_id, candId);
      succ.set(succ_qu_id, candId, succ_cost);
      _successors.insert(succ);
      succ.print();
    }
  }

  // for(const auto&n :_successors){
  //   n.print();
  // }
  // printf("Successor manager reported %lu candidates\n", _successors.size());
  return _successors;
}
