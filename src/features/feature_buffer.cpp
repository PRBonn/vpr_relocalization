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


#include "feature_buffer.h"

#include <limits>

bool FeatureBuffer::inBuffer(int id) const {
  auto feature = featureMap.find(id);
  if (feature == featureMap.end()) {
    // not found
    return false;
  }
  return true;
}

iFeature::ConstPtr FeatureBuffer::getFeature(int id) const {
  auto found = featureMap.find(id);
  return found->second;
}

/** internal function. deletes the latest added feature from the buffer **/
void FeatureBuffer::deleteFeature() {
  featureMap.erase(ids[0]);
  ids.erase(ids.begin());
}

void FeatureBuffer::addFeature(int id, const iFeature::ConstPtr& feature) {
  if (ids.size() > static_cast<size_t>(std::numeric_limits<int>::max())) {
    fprintf(stderr,
            "[ERROR] ids vector size does not fit in integer type. Cannot add "
            "feature with id: %d.\n",
            id);
    return;
  }
  if (static_cast<int>(ids.size()) == bufferSize) {
    deleteFeature();
  }
  ids.push_back(id);
  if (featureMap.count(id) > 0) {
    fprintf(stderr, "[WARNING] feature with id %d exists. Overwriting.\n", id);
  }
  // Map stores const pointers, so we cannot use operator[] here.
  featureMap.emplace(id, feature);
}

/**  sets buffer size + reserves the space for map **/
void FeatureBuffer::setBufferSize(int size) {
  bufferSize = size;
  featureMap.reserve(size);
}
