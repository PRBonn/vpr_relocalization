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

#include "lsh_cv_hashing.h"
#include "database/list_dir.h"
#include "tools/timer/timer.h"

void LshCvHashing::setParams(int tableNum, int keySize, int multi_probe_level) {
  _indexParam =
      new cv::flann::LshIndexParams(tableNum, keySize, multi_probe_level);
}

void LshCvHashing::setDatabase(OnlineDatabase::Ptr database) {
  if (!database) {
    printf("[ERROR][LshAngleBasedRelocalizer] Database is not set\n");
    exit(EXIT_FAILURE);
  }
  _database = database;
}

void LshCvHashing::train(std::vector<iBinarizableFeature::Ptr> features) {
  if (!_indexParam) {
    setParams(_tableNum, _keySize, _multiProbeLevel);
  }
  _matcherPtr =
      cv::Ptr<cv::FlannBasedMatcher>(new cv::FlannBasedMatcher(_indexParam));
  // transform to cv::Mat array of arrays
  cv::Mat matFeatures(features.size(), features[0]->bits.size(), CV_8UC1);
  for (int f = 0; f < features.size(); ++f) {
    for (int d = 0; d < features[f]->bits.size(); ++d) {
      matFeatures.at<uchar>(f, d) = features[f]->bits[d];
    }
  }
  printf("Features were converted to Mat type %d\n", matFeatures.type());
  _matcherPtr->add(matFeatures);
  _matcherPtr->train();

  printf("[INFO][LSH_CV] Training completed\n");

  // m_matcherPtr->match(*query, knnmatches, 1);
}

std::vector<int> LshCvHashing::hashFeature(
    const iBinarizableFeature::ConstPtr& fPtr) {
  std::vector<std::vector<cv::DMatch>> matches;
  cv::Mat feature(1, fPtr->bits.size(), CV_8UC1);
  for (int i = 0; i < fPtr->bits.size(); ++i) {
    feature.at<uchar>(0, i) = fPtr->bits[i];
  }
  // _matcherPtr->match(feature, matches);
  Timer timer;
  timer.start();
  _matcherPtr->knnMatch(feature, matches, 5);
  timer.stop();
  fprintf(stderr, "[LSH][CV] time to extract neighbours: ");
  timer.print_elapsed_time(TimeExt::MicroSec);

  std::vector<int> matchedIds;
  for (int k = 0; k < matches.size(); ++k) {
    for (const auto& match : matches[k]) {
      matchedIds.push_back(match.trainIdx);
    }
  }
  return matchedIds;
}
void LshCvHashing::saveHashes() {
  cv::FileStorage fs("trained_trees.yaml", cv::FileStorage::WRITE);
  _matcherPtr->write(fs);

  cv::FileStorage fsRead("trained_trees.yaml", cv::FileStorage::READ);
  cv::FileNode node = fsRead["indexParams"];
  _matcherPtr->read(node);
}

std::vector<int> LshCvHashing::getCandidates(int quId) {
  if (!_database) {
    printf("[ERROR][LshAngleBasedRelocalizer] Database is not set\n");
    exit(EXIT_FAILURE);
  }
  printf("Getting candidates for a query image\n");
  const auto featurePtr = std::static_pointer_cast<const iBinarizableFeature>(
      _database->getQueryFeature(quId));
  if (!featurePtr) {
    printf("Wrong feature format\n");
  }

  Timer timer;
  timer.start();

  std::vector<int> candidates;
  candidates = hashFeature(featurePtr);

  timer.stop();
  fprintf(stderr, "[HASH] time ");
  timer.print_elapsed_time(TimeExt::MSec);
  fprintf(stderr, "[HASH] candidates size %lu\n", candidates.size());
  return candidates;
}
