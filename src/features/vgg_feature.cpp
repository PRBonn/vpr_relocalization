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

#include "vgg_feature.h"
#include <algorithm>
#include <numeric>
#include <math.h>
#include <fstream>
#include <limits>
// #include "tools/timer/timer.h"

void VggFeature::loadFromFile(const std::string &filename) {
  // Timer timer;
  // timer.start();
  std::ifstream in(filename.c_str());
  if (!in) {
    printf("[ERROR][OnlineDatabase] Feature %s cannot be loaded\n",
           filename.c_str());
    exit(EXIT_FAILURE);
  }

  double value;
  while (in >> value) {
    dim.push_back(value);
  }
  in.close();
  // timer.stop();
  // cout << "Feature loading time: ";
  // timer.print_elapsed_time(TimeExt::MSec);
  binarize();
}

void VggFeature::binarize() {
  bits.clear();
  int des_max = 255, des_min = 0;  // des_ - desired params
  auto min_maxEl = std::minmax_element(dim.begin(), dim.end());

  double d_min, d_max;
  d_min = *min_maxEl.first;
  d_max = *min_maxEl.second;
  double tmp = (des_max - des_min) / (d_max - d_min);

  bits.resize(dim.size(), 0);

  int thresh = des_max / 2;
  for (int f = 0; f < dim.size(); ++f) {
    int d_int = (dim[f] - d_min) * tmp + des_min;
    bool value = d_int < thresh ? 0 : 1;
    bits[f] = value;
  }
}

double VggFeature::computeSimilarityScore(const iFeature::ConstPtr& rhs) const {
  const auto featurePtr = std::static_pointer_cast<const VggFeature>(rhs);
  if (!featurePtr) {
    printf(
        "[ERROR][Feature] It seems like you are trying to match features of "
        "different type\n");
    exit(EXIT_FAILURE);
  }
  double norm_qr =
      sqrt(std::inner_product(dim.begin(), dim.end(), dim.begin(), 0.0L));
  double norm_db =
      sqrt(std::inner_product(featurePtr->dim.begin(), featurePtr->dim.end(),
                              featurePtr->dim.begin(), 0.0L));
  double prod_qr_db = std::inner_product(
      featurePtr->dim.begin(), featurePtr->dim.end(), dim.begin(), 0.0L);
  double cos_dist = prod_qr_db / (norm_qr * norm_db);
  return cos_dist;
}

double VggFeature::score2cost(double score) const {
  double cost;
  if (score < 1e-09) {
    cost = std::numeric_limits<double>::max();
    printf("[INFO] The cost of comparing two images is suspiciously small.\n");
  } else {
    cost = 1. / score;
  }
  return cost;
}

void VggFeature::disp() const {
  printf("[WARNING][VggFeature] No disp() function implemented yet\n");
}
