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

#include <fstream>
#include <limits>
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "database/list_dir.h"
#include "features/feature_buffer.h"
#include "features/feature_factory.h"
#include "features/ifeature.h"
#include "tools/config_parser/config_parser.h"

FeatureBuffer loadFeatures(const std::string &path2folder,
                           const FeatureFactory &factory) {
  std::vector<std::string> featureNames = listDir(path2folder);
  FeatureBuffer buffer;
  buffer.setBufferSize(featureNames.size());
  printf("Loading features\n");
  for (size_t i = 0; i < featureNames.size(); ++i) {
    iFeature::Ptr featurePtr = factory.createFeature();
    featurePtr->loadFromFile(featureNames[i]);
    buffer.addFeature(i, featurePtr);
    fprintf(stderr, ".");
  }
  printf("\nFeatures were loaded\n");
  return buffer;
}

int main(int argc, char const *argv[]) {
  printf("====== Computing full similarity matrix =====\n");

  if (argc < 2) {
    printf("[ERROR] Not enough input parameters.\n");
    printf(
        "Proper usage: ./create_cost_matrix create_cost_matrix_config.yaml\n");
    exit(0);
  }
  ConfigParser config;
  // config.parse(argv[1]);
  config.parseYaml(argv[1]);
  config.print();

  std::string outputCostName = config.costOutputName;
  if (outputCostName.empty()) {
    printf(
        "ERROR: You haven't specified the output cost name. It may be very "
        "painfull to realize that after 3 hours you write your matrix to an "
        "empty file! Set it\n");
    return 0;
  }
  FeatureFactory factory;
  factory.setFeatureType(FeatureFactory::FeatureType::Cnn_Feature);
  // [VGG] to use other type of feature. Change here for desired type
  // factory.setFeatureType(FeatureFactory::FeatureType::Vgg_Feature);
  FeatureBuffer quFeatures = loadFeatures(config.path2qu, factory);
  FeatureBuffer refFeatures = loadFeatures(config.path2ref, factory);

  int querySize = quFeatures.bufferSize;
  int refSize = refFeatures.bufferSize;

  cv::Mat scores(querySize, refSize, CV_32FC1);
  printf("Computing similarity matrix..\n");
  for (int r = 0; r < querySize; ++r) {
    auto rowFeaturePtr = quFeatures.getFeature(r);
    for (int c = 0; c < refSize; ++c) {
      const auto colFeaturePtr = refFeatures.getFeature(c);
      double score = rowFeaturePtr->computeSimilarityScore(colFeaturePtr);
      scores.at<float>(r, c) = score;
    }
    printf("Computed row %d\n", r);
  }

  double minVal;
  double maxVal;
  cv::Point minLoc;
  cv::Point maxLoc;

  minMaxLoc(scores, &minVal, &maxVal, &minLoc, &maxLoc);
  printf("[INFO] min value: %f max value: %f\n", minVal, maxVal);

  std::string cost_png = "cost_matrix" + std::to_string(querySize) + "_" +
                         std::to_string(refSize) + ".png";
  std::ofstream out(outputCostName);
  out << scores.rows << " " << scores.cols << "\n";
  for (int r = 0; r < scores.rows; ++r) {
    for (int c = 0; c < scores.cols; ++c) {
      out << scores.at<float>(r, c) << " ";
    }
    out << "\n";
  }
  printf("The matrix was saved to the file %s\n", outputCostName.c_str());

  cv::Mat img;
  scores.copyTo(img);
  cv::normalize(img, img, 0, 255, cv::NORM_MINMAX, CV_32FC1);
  imwrite(cost_png, img);
  printf("[INFO] Image was saved to a file %s\n", cost_png.c_str());

  return 0;
}
