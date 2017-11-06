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

#include <memory>

#include "database/idatabase.h"
#include "database/online_database.h"
#include "online_localizer/ilocvisualizer.h"
#include "online_localizer/online_localizer.h"
#include "successor_manager/successor_manager.h"
#include "tools/config_parser/config_parser.h"

#include "relocalizers/dimensions_hashing.h"

using std::make_shared;

int main(int argc, char *argv[]) {
  printf("===== Online place recognition using DH ====\n");
  if (argc < 2) {
    printf("[ERROR] Not enough input parameters.\n");
    printf("Proper usage: ./feature_based_matching_dh_no_vis config_file.yaml\n");
    exit(0);
  }
  std::string config_file = argv[1];
  ConfigParser parser;
  parser.parseYaml(config_file);
  parser.print();

  OnlineDatabase online_database;
  online_database.setRefFeaturesFolder(parser.path2ref);
  online_database.setQuFeaturesFolder(parser.path2qu);
  online_database.setBufferSize(parser.bufferSize);
  online_database.setFeatureType(
      FeatureFactory::FeatureType::Cnn_Feature);
      //[VGG] uncomment this to use vgg features
      // FeatureFactory::FeatureType::Vgg_Feature);

  if (!online_database.isSet()) {
    printf("[ERROR] database is not set completely\n");
    return false;
  }
  auto onlineDatabasePtr = make_shared<OnlineDatabase>(online_database);

  // initialize Relocalizer
  auto relocalizerPtr = DimensionsHashing::Ptr(new DimensionsHashing);
  relocalizerPtr->loadIndex(parser.hashTable);
  relocalizerPtr->weightIndex(onlineDatabasePtr->refSize());
  relocalizerPtr->setDatabase(onlineDatabasePtr);

  // initialize SuccessorManager
  auto successorManagerPtr = make_shared<SuccessorManager>(SuccessorManager());
  successorManagerPtr->setFanOut(parser.fanOut);
  successorManagerPtr->setDatabase(onlineDatabasePtr);
  successorManagerPtr->setSimilarPlaces(parser.simPlaces);
  successorManagerPtr->setRelocalizer(relocalizerPtr);

  OnlineLocalizer localizer;
  localizer.setQuerySize(parser.querySize);
  localizer.setSuccessorManager(successorManagerPtr);
  localizer.setExpansionRate(parser.expansionRate);  // expand everything
  localizer.setNonMatchingCost(parser.nonMatchCost);

  localizer.run();
  localizer.printPath(parser.pathFile);

  return 0;
}
