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

#include <iostream>
#include <string>
#include <vector>
#include "features/cnn_feature.h"
#include "features/feature_buffer.h"
#include "gtest/gtest.h"
#include "relocalizers/dimensions_hashing.h"

TEST(DimensionHashing, hashFeatures) {
  std::vector<bool> b1 = {1, 1, 1, 0, 0, 0, 0, 0};
  std::vector<bool> b2 = {1, 1, 1, 0, 0, 1, 0, 0};
  std::vector<bool> b3 = {0, 0, 0, 0, 1, 0, 1, 1};

  iBinarizableFeature::Ptr f1 = iBinarizableFeature::Ptr(new CnnFeature);
  f1->bits = b1;
  iBinarizableFeature::Ptr f2 = iBinarizableFeature::Ptr(new CnnFeature);
  f2->bits = b2;
  iBinarizableFeature::Ptr f3 = iBinarizableFeature::Ptr(new CnnFeature);
  f3->bits = b3;

  std::vector<iBinarizableFeature::Ptr> features = {f1, f2, f3};

  DimensionsHashing hasher;
  hasher.hashFeatures(features);

  EXPECT_EQ(hasher.index[0][0], 0);
  EXPECT_EQ(hasher.index[0][1], 1);
  EXPECT_EQ(hasher.index[1][0], 0);
  EXPECT_EQ(hasher.index[1][1], 1);

  EXPECT_EQ(hasher.index[2][0], 0);
  EXPECT_EQ(hasher.index[2][1], 1);

  EXPECT_EQ(hasher.index[4][0], 2);
  EXPECT_EQ(hasher.index[5][0], 1);
  EXPECT_EQ(hasher.index[6][0], 2);
  EXPECT_EQ(hasher.index[7][0], 2);
}

TEST(DimensionHashing, hashFeature) {
  std::vector<bool> b1 = {1, 1, 1, 0, 0, 0, 0, 0};
  std::vector<bool> b2 = {1, 1, 1, 0, 0, 1, 0, 0};
  std::vector<bool> b3 = {0, 0, 0, 0, 1, 0, 1, 1};

  iBinarizableFeature::Ptr f1 = iBinarizableFeature::Ptr(new CnnFeature);
  f1->bits = b1;
  iBinarizableFeature::Ptr f2 = iBinarizableFeature::Ptr(new CnnFeature);
  f2->bits = b2;
  iBinarizableFeature::Ptr f3 = iBinarizableFeature::Ptr(new CnnFeature);
  f3->bits = b3;

  std::vector<iBinarizableFeature::Ptr> features = {f1, f2, f3};

  DimensionsHashing hasher;
  hasher.hashFeatures(features);

  std::vector<bool> b4 = {1, 1, 1, 0, 1, 0, 0, 0};
  iBinarizableFeature::Ptr query = iBinarizableFeature::Ptr(new CnnFeature);
  query->bits = b4;

  std::vector<int> cand = hasher.hashFeature(query);
  std::sort(cand.begin(), cand.end());
  EXPECT_EQ(cand[0], 0);
  EXPECT_EQ(cand[1], 1);
}

// // this is not a unit test, but it is easier to implement it here :)
// TEST(DimensionsHashing, performanceTest) {

//   std::vector<BinaryFeature::Ptr> featurePtrs;

//   for (size_t i = 0; i < featureNames.size(); ++i) {
//     BinaryFeature::Ptr featurePtr = BinaryFeature::Ptr(new BinaryFeature);
//     featurePtr->loadFromFile(featureNames[i]);
//     featurePtr->binarize();
//     featurePtrs.push_back(featurePtr);
//     fprintf(stderr, ".");
//   }
//   fprintf(stderr, "\n");
//   printf("Features were loaded and binarized\n");

//   DimensionsHashing hasher;

//   hasher.hashFeatures(featurePtrs);
// }
