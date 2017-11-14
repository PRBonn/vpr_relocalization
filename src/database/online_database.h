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


#ifndef SRC_DATABASE_ONLINE_DATABASE_H_
#define SRC_DATABASE_ONLINE_DATABASE_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "features/feature_buffer.h"
#include "database/idatabase.h"
#include "features/feature_factory.h"

/**
 * @brief      Container for storing computed feature matches
 */
class MatchMap {
 public:
  MatchMap() {}
  double getMatchCost(int quId, int refId);
  void addMatchCost(int quId, int refId, double cost) {
    _matches[quId][refId] = cost;
  }
  std::unordered_map<int, std::unordered_map<int, double> > _matches;
};

/**
 * @brief      Database for loading and matching features. Saves the computed matching costs.
 */
class OnlineDatabase : public iDatabase {
 public:
  using Ptr = std::shared_ptr<OnlineDatabase>;
  using ConstPtr = std::shared_ptr<const OnlineDatabase>;


  int refSize() override { return _refFeaturesNames.size(); }
  double getCost(int quId, int refId) override;

  void setQuFeaturesFolder(const std::string &path2folder);
  void setRefFeaturesFolder(const std::string &path2folder);
  void setBufferSize(int size);
  void setFeatureType(FeatureFactory::FeatureType type);

  // use for tests / visualization only
  const MatchMap &getMatchMap() const;
  void setMatchMap(const MatchMap &matchMap) { _matchMap = matchMap; }
  bool isSet() const;
  double computeMatchCost(int quId, int refId);

  std::string getQuFeatureName(int id) const;
  std::string getRefFeatureName(int id) const;
  iFeature::ConstPtr getQueryFeature(int quId);

 protected:
  MatchMap _matchMap;
  std::vector<std::string> _quFeaturesNames, _refFeaturesNames;
  FeatureFactory _featureFactory;

 private:
  FeatureBuffer _refBuff, _quBuff;
};

#endif  // SRC_DATABASE_ONLINE_DATABASE_H_
