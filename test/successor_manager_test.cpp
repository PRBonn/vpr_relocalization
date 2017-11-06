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
#include <iostream>
#include <string>
#include "database/online_database.h"
#include "relocalizers/dimensions_hashing.h"
#include "gtest/gtest.h"

TEST(successorManager, get_successors_source) {
  std::string path2ref = "../test/test_data/ref_features/";
  std::string path2qu = "../test/test_data/query_features/";

  auto onlineDatabasePtr = OnlineDatabase::Ptr(new OnlineDatabase);
  onlineDatabasePtr->setRefFeaturesFolder(path2ref);
  onlineDatabasePtr->setQuFeaturesFolder(path2qu);
  onlineDatabasePtr->setBufferSize(10);

  if (!onlineDatabasePtr->isSet()) {
    printf("[ERROR] database is not set completely\n");
  }
  // iDatabase::Ptr databasePtr = onlineDatabasePtr;
  //
  auto relocalizerPtr = DimensionsHashing::Ptr(new DimensionsHashing);
  relocalizerPtr->loadIndex("../test/test_data/test_ref_hash_dim.txt");
  relocalizerPtr->weightIndex(onlineDatabasePtr->refSize());
  relocalizerPtr->setDatabase(onlineDatabasePtr);

  SuccessorManager successorManager;
  successorManager.setDatabase(onlineDatabasePtr);
  successorManager.setRelocalizer(relocalizerPtr);
  std::unordered_set<Node> succes =
      successorManager.getSuccessorsIfLost(Node(-1, 0, 0.0));
  for (const Node &node : succes) {
    node.print();
  }

  Node node(0, 0, 6.68232);
  EXPECT_TRUE(succes.find(node) != succes.end());
}

TEST(successorManager, get_successors) {
  std::string path2ref = "../test/test_data/ref_features/";
  std::string path2qu = "../test/test_data/query_features/";
  auto onlineDatabasePtr = OnlineDatabase::Ptr(new OnlineDatabase);
  onlineDatabasePtr->setRefFeaturesFolder(path2ref);
  onlineDatabasePtr->setQuFeaturesFolder(path2qu);
  onlineDatabasePtr->setBufferSize(10);

  // printf("[TEST] Database size: %d\n", onlineDatabasePtr->refSize());

  if (!onlineDatabasePtr->isSet()) {
    printf("[ERROR] database is not set completely\n");
  }
  iDatabase::Ptr database = onlineDatabasePtr;
  SuccessorManager successorManager;
  successorManager.setDatabase(database);
  successorManager.setFanOut(1);
  std::unordered_set<Node> succes =
      successorManager.getSuccessors(Node(1, 2, 0.0));
  for (const Node &node : succes) {
    node.print();
  }
  ASSERT_TRUE(succes.size() > 0);

  Node node(2, 1, 5.88258);
  EXPECT_TRUE(succes.find(node) != succes.end());

  node.set(2, 2, 9.01962);
  EXPECT_TRUE(succes.find(node) != succes.end());

  node.set(2, 3, 5.88258);
  EXPECT_TRUE(succes.find(node) != succes.end());

  succes = successorManager.getSuccessors(Node(0, 0, 0.0));
  for (const Node &node : succes) {
    node.print();
  }

  node.set(1, 0, 5.18384);
  EXPECT_TRUE(succes.find(node) != succes.end());

  node.set(1, 1, 6.78146);
  EXPECT_TRUE(succes.find(node) != succes.end());
}

TEST(successorManager, get_successors_simPlaces) {
  std::string path2ref = "../test/test_data/ref_features/";
  std::string path2qu = "../test/test_data/query_features/";
  auto onlineDatabasePtr = OnlineDatabase::Ptr(new OnlineDatabase);
  onlineDatabasePtr->setRefFeaturesFolder(path2ref);
  onlineDatabasePtr->setQuFeaturesFolder(path2qu);
  onlineDatabasePtr->setBufferSize(10);

  iDatabase::Ptr database = onlineDatabasePtr;
  SuccessorManager successorManager;
  successorManager.setDatabase(database);
  successorManager.setFanOut(0);
  successorManager.setSimilarPlaces("../test/test_data/simPlaces_test.txt");
  std::unordered_set<Node> succes =
      successorManager.getSuccessors(Node(1, 2, 0.0));
  for (const Node &node : succes) {
    node.print();
  }

  Node node;
  node.set(2, 2, 0.0);
  EXPECT_TRUE(succes.find(node) != succes.end());

  node.set(2, 0, 0.0);
  EXPECT_TRUE(succes.find(node) != succes.end());
}
