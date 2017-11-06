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

#ifndef SRC_RELOCALIZERS_DIMENSIONS_HASHING_H_
#define SRC_RELOCALIZERS_DIMENSIONS_HASHING_H_

#include <string>
#include <unordered_map>
#include <vector>
#include "database/online_database.h"
#include "features/ibinarizable_feature.h"
#include "relocalizers/irelocalizer.h"

/**
 * @brief      Class for hashing based on the dimension activation of the
 * features. For further details check the related paper.
 */
class DimensionsHashing : public iRelocalizer {
 public:
  using Ptr = std::shared_ptr<DimensionsHashing>;
  using ConstPtr = std::shared_ptr<const DimensionsHashing>;
  using InvertedIndex = std::unordered_map<int, std::vector<int> >;

  std::vector<int> getCandidates(int quId) override;
  void setDatabase(OnlineDatabase::Ptr database);

  std::vector<int> hashFeature(const iBinarizableFeature::ConstPtr& fPtr) const;
  /**
   * @brief      constructs the hash table.
   *
   * @param[in]  features  The features
   */
  void hashFeatures(const std::vector<iBinarizableFeature::Ptr>& features);
  void showIndex() const;

  void loadIndex(const std::string& filename);
  void saveIndex(const std::string& filename) const;

  /**
   * should be called explicitly, if loading index. Initializes _dimWeights
   */
  void weightIndex(int refSize);

  /**
   * representation of the hash table
   */
  InvertedIndex index;

 private:
  OnlineDatabase::Ptr _database = nullptr;
  /**
   * weights to check for feature occurance. The more freaquent the feature
   * occurs in the reference sequence the less infomative it is.
   */

  std::unordered_map<int, double> _dimWeights;
};

#endif  // SRC_RELOCALIZERS_DIMENSIONS_HASHING_H_
