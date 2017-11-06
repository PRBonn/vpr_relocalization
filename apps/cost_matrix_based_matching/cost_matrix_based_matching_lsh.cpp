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
#include <memory>
#include <string>

#include "database/cost_matrix_database.h"
#include "database/idatabase.h"
#include "features/ibinarizable_feature.h"
#include "online_localizer/ilocvisualizer.h"
#include "online_localizer/online_localizer.h"
#include "successor_manager/successor_manager.h"
#include "tools/config_parser/config_parser.h"
#include "visualizer/full_matrix_visualizer.h"
#include "features/cnn_feature.h"

#include "relocalizers/lsh_cv_hashing.h"
#include "database/list_dir.h"

using std::make_shared;


std::vector<iBinarizableFeature::Ptr> loadFeatures(
    const std::string &path2folder) {
  printf("Loading the features to hash with LSH\n");
  std::vector<std::string> featureNames = listDir(path2folder);
  std::vector<iBinarizableFeature::Ptr> featurePtrs;

  for (size_t i = 0; i < featureNames.size(); ++i) {
    iBinarizableFeature::Ptr featurePtr =
        iBinarizableFeature::Ptr(new CnnFeature);
    // [VGG] Uncomment this to use vgg features
    // iBinarizableFeature::Ptr(new VggFeature);
    featurePtr->loadFromFile(path2folder + featureNames[i]);
    featurePtrs.push_back(featurePtr);
    fprintf(stderr, ".");
  }
  fprintf(stderr, "\n");
  printf("Features were loaded and binarized\n");
  return featurePtrs;
}


int main(int argc, char *argv[]) {
  printf("===== Online place recognition cost matrix based LSH ====\n");
  if (argc < 2) {
    printf("[ERROR] Not enough input parameters.\n");
    printf("Proper usage: ./cost_matrix_based_matching_lsh config_file.yaml\n");
    exit(0);
  }

  std::string config_file = argv[1];
  ConfigParser parser;
  parser.parseYaml(config_file);
  parser.print();


  auto databasePtr = CostMatrixDatabase::Ptr(new CostMatrixDatabase);
  databasePtr->loadFromTxt(parser.costMatrix);
  // to obtain the features, when needed
  databasePtr->setQuFeaturesFolder(parser.path2qu);
  databasePtr->setBufferSize(parser.bufferSize);
  databasePtr->setFeatureType(FeatureFactory::FeatureType::Cnn_Feature);
  // databasePtr->setFeatureType(FeatureFactory::FeatureType::Vgg_Feature);


  // initialize Relocalizer
  auto relocalizerPtr = LshCvHashing::Ptr(new LshCvHashing);
  relocalizerPtr->setParams(1, 12, 2);
  relocalizerPtr->setDatabase(databasePtr);
  std::vector<iBinarizableFeature::Ptr> featurePtrs =
      loadFeatures(parser.path2ref);
  relocalizerPtr->train(featurePtrs);

  // initialize SuccessorManager
  auto successorManagerPtr = SuccessorManager::Ptr(new SuccessorManager);
  successorManagerPtr->setFanOut(parser.fanOut);
  successorManagerPtr->setDatabase(databasePtr);
  successorManagerPtr->setSimilarPlaces(parser.simPlaces);
  successorManagerPtr->setRelocalizer(relocalizerPtr);

  // set the visualizer
  auto visPtr = FullMatrixVisualizer::Ptr(new FullMatrixVisualizer);
  visPtr->setOutImageName(parser.costOutputName);
  visPtr->setDatabase(databasePtr);

  // create localizer and run it
  OnlineLocalizer localizer;
  localizer.setQuerySize(parser.querySize);
  localizer.setSuccessorManager(successorManagerPtr);
  localizer.setExpansionRate(parser.expansionRate);
  localizer.setNonMatchingCost(parser.nonMatchCost);
  localizer.setVisualizer(visPtr);
  localizer.run();

  std::string pathFile = "matched_path.txt";
  localizer.printPath(pathFile);
  printf("Done.\n");
  return 0;
}
