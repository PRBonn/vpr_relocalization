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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <set>
#include <string>
#include <vector>

#include "database/list_dir.h"
#include "features/ibinarizable_feature.h"
#include "features/ifeature.h"
#include "features/cnn_feature_mean.h"
#include "features/vgg_feature_mean.h"
// #include "relocalizers/lsh_cv_hashing.h"

std::vector<iBinarizableFeature::Ptr> readFeatures(
    const std::string &path2folder) {
  printf("Reading features\n");
  std::vector<iBinarizableFeature::Ptr> v;
  std::vector<std::string> featureNames = listDir(path2folder);
  for (std::size_t i = 0; i < featureNames.size(); ++i) {
    iBinarizableFeature::Ptr featurePtr =
        // iBinarizableFeature::Ptr(new CnnFeatureMean);
        iBinarizableFeature::Ptr(new VggFeatureMean);
    featurePtr->loadFromFile(path2folder + featureNames[i]);
    v.push_back(featurePtr);
    std::cerr << ".";
  }
  printf("\n");
  return v;
}

/**
 * @brief      Determines if j is the neighbour of i based on the neighbourhood
 * size s.
 *
 * @param[in]  i     center index
 * @param[in]  j     query index
 * @param[in]  s     neighbourhood size
 *
 * @return     True if neighbour, False otherwise.
 */
bool isNeighbour(int i, int j, int s) {
  if (abs(i - j) <= s) {
    return true;
  }
  return false;
}

int main(int argc, char const *argv[]) {
  printf("\n=== Estimating similar places within one trajectory ===\n");
  if (argc < 4) {
    printf("[ERROR] Not enough input parameters\n");
    printf(
        "Proper usage: ./estimate_similar_places path2ref outputFile(.txt) "
        "nonMatchCost\n");
    return 1;
  }
  std::string path2ref = argv[1];
  std::string outputFile = argv[2];
  float nonMatchCost = atof(argv[3]);
  std::vector<iBinarizableFeature::Ptr> featurePtrs = readFeatures(path2ref);

  std::ofstream out(outputFile);
  if (!out) {
    printf("[ERROR} File %s cannot be opened\n", outputFile.c_str());
    return 1;
  }

  // outputting scores for thr debugging
  std::ofstream outScore("scores.txt");
  if(!outScore) {
    std::cerr << "[EstimatingSimilarPlaces] Costs file cannot be opened\n";
  }

  int neighSize = 10;

  std::cout << "Matching features\n";
  for (int i = 0; i < featurePtrs.size(); ++i) {
    std::cerr << ".";
    for (int j = i; j >= 0; --j) {
      double score = featurePtrs[i]->computeSimilarityScore(featurePtrs[j]);
      double cost = featurePtrs[i]->score2cost(score);
      outScore << i << " " << " " << j << " " << score << std::endl;
      if ((cost < nonMatchCost) && !isNeighbour(i, j, neighSize)) {
        // accept match
        std::cout<< "\n" << i << " " << j << " " << cost <<std::endl;

        out << i << " " << j << std::endl;
      }
    }
  }
  outScore.close();
  out.close();
  return 0;
}
