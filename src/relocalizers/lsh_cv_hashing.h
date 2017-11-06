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

#ifndef SRC_RELOCALIZERS_LSH_CV_HASHING_H_
#define SRC_RELOCALIZERS_LSH_CV_HASHING_H_

#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include "database/online_database.h"
#include "features/ibinarizable_feature.h"
#include "relocalizers/irelocalizer.h"

#include "opencv2/features2d/features2d.hpp"

/**
 * @brief      Performs locality sensitive hashing for angle-based similarity
 * measures.
 */
class LshCvHashing : public iRelocalizer {
 public:
  using Ptr = std::shared_ptr<LshCvHashing>;
  using ConstPtr = std::shared_ptr<const LshCvHashing>;

  std::vector<int> getCandidates(int quId) override;

  void setDatabase(OnlineDatabase::Ptr database);

  void train(std::vector<iBinarizableFeature::Ptr> features);
  /**
   * @brief      Not working for now, for unknown reason
   */
  void saveHashes();
  void setParams(int tableNum, int keySize, int multi_probe_level);

  std::vector<int> hashFeature(const iBinarizableFeature::ConstPtr& fPtr);

 private:
  int _tableNum = 25;  // number of hash tables
  // number of bits in the hash key
  int _keySize = 15;
  // number of bits to shift to het neighbouring buckets
  int _multiProbeLevel = 2;

  cv::Ptr<cv::FlannBasedMatcher> _matcherPtr;
  cv::Ptr<cv::flann::IndexParams> _indexParam;
  OnlineDatabase::Ptr _database = nullptr;
};


// table_number the number of hash tables to use (between 10 and 30 usually).
// key_size the size of the hash key in bits (between 10 and 20 usually).
// multi_probe_level the number of bits to shift to check for neighboring buckets (0 is regular LSH, 2 is recommended).



#endif  // SRC_RELOCALIZERS_LSH_CV_HASHING_H_
