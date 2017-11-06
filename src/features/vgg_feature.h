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


#ifndef SRC_FEATURES_VGG_FEATURE_H_
#define SRC_FEATURES_VGG_FEATURE_H_
#include <string>
#include <vector>
#include "features/ibinarizable_feature.h"

/**
 * @brief      Class for VGG-16 features. Knows how to aread a feature from a
 * file. Mid binarization by default.
 */
class VggFeature : public iBinarizableFeature {
 public:
  using Ptr = std::shared_ptr<VggFeature>;
  using ConstPtr = std::shared_ptr<const VggFeature>;

  /**
   * @brief      Loads a from file. Expects feature to be stored in predefined
   * format.
   * If you would like other format. Consider writing your own class, derived
   * from ifeature.h
   *
   * @param[in]  filename  The filename
   */
  void loadFromFile(const std::string &filename) override;
  /**
   * @brief      computes the cosine distance between two vectors
   *
   * @param[in]  rhs   The right hand side
   *
   * @return     The similarity score.
   */
  double computeSimilarityScore(const iFeature::ConstPtr& rhs) const override;
  /**
   * @brief      weight/cost is an inverse of a score.
   *
   * @param[in]  score  The score
   *
   * @return    weight/cost. If cost is near to 0, returns the
   * std::numeric_limits<double>::max()
   */
  double score2cost(double score) const override;
  void disp() const override;

  virtual ~VggFeature() {}

  std::vector<double> dim;
  using iBinarizableFeature::bits;

 private:
  void binarize();
};

#endif  // SRC_FEATURES_VGG_FEATURE_H_
