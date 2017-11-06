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


#ifndef SRC_FEATURES_FEATURE_BUFFER_H_
#define SRC_FEATURES_FEATURE_BUFFER_H_

#include <memory>
#include <unordered_map>
#include <vector>
#include "ifeature.h"

/**
 * @brief      Class for feature buffer. Stores the features kept in memory
 * during the search
 */
class FeatureBuffer {
 public:
  bool inBuffer(int id) const;
  /**  sets buffer size + reserves the space for map **/
  void setBufferSize(int size);
  /** returns empty vector if a feature is not in buffer */
  iFeature::ConstPtr getFeature(int id) const;
  void addFeature(int id, const iFeature::ConstPtr& feature);

  void deleteFeature();

  // this is needed to remember which feature came first.
  int bufferSize = -1;
  std::vector<int> ids;
  std::unordered_map<int, const iFeature::ConstPtr> featureMap;
};

#endif  // SRC_FEATURES_FEATURE_BUFFER_H_
