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

#ifndef SRC_SUCCESSOR_MANAGER_NODE_H_
#define SRC_SUCCESSOR_MANAGER_NODE_H_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/**
 * @brief      Container class for a node in the graph
 */
class Node {
 public:
  Node() {}
  Node(int quId, int refId, double idvCost);
  int quId = -1;
  int refId = -1;
  double idvCost = -1.0;
  double accCost = -1.0;
  void set(int quId, int refId, double idvCost);
  void print() const;

  /** WARNING: use only for the priority queue.
 * Comparison is based on the accumulated cost **/
  bool operator<(const Node &rhs) const { return this->accCost > rhs.accCost; }
};

#define SOURCE_NODE Node(-1, 0, 0.0)

/** WARNING: compares only the coordinates (quId, refId). Used mostly in
 * tests**/
bool operator==(const Node &lhs, const Node &rhs);

// custom specialization of std::hash can be injected in namespace std
// This makes a node hashable to use for std::unordered_set<Node>
namespace std {
template <>
struct hash<Node> {
  std::size_t operator()(Node const &node) const {
    std::string s = std::to_string(node.quId) + std::to_string(node.refId);
    std::size_t const h(std::hash<std::string>{}(s));
    return h;
  }
};
}  // namespace std

typedef std::unordered_set<Node> NodeSet;

#endif  // SRC_SUCCESSOR_MANAGER_NODE_H_
