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

#include <string>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <math.h>
#include "cnn_feature_mean.h"


void CnnFeatureMean::loadFromFile(const std::string &filename) {
  // Timer timer;
  // timer.start();
  std::ifstream in(filename.c_str());
  if (!in) {
    printf("[ERROR][OnlineDatabase] Feature %s cannot be loaded\n",
           filename.c_str());
    exit(EXIT_FAILURE);
  }
  int n, r, c;
  in >> n >> r >> c;
  int num_of_elements = n * r * c;
  dim.reserve(num_of_elements);

  while (!in.eof()) {
    double value;
    in >> value;
    dim.push_back(value);
  }
  in.close();
  // timer.stop();
  // cout << "Feature loading time: ";
  // timer.print_elapsed_time(TimeExt::MSec);
  binarize();
}

void CnnFeatureMean::binarize() {
  bits.clear();
  int des_max = 255, des_min = 0;  // des_ - desired params
  auto min_maxEl = std::minmax_element(dim.begin(), dim.end());

  double sum = std::accumulate(dim.begin(), dim.end(), 0.0);
  double mean = sum / dim.size();

  double d_min, d_max;
  d_min = *min_maxEl.first;
  d_max = *min_maxEl.second;
  double tmp = (des_max - des_min) / (d_max - d_min);

  bits.resize(dim.size(), 0);

  int thresh = (mean - d_min) * tmp + des_min;
  for (int f = 0; f < dim.size(); ++f) {
    int d_int = (dim[f] - d_min) * tmp + des_min;
    bool value = d_int < thresh ? 0 : 1;
    bits[f] = value;
  }
}
