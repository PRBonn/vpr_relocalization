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

#include "dimensions_hashing.h"
#include <math.h>
#include <tools/timer/timer.h>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>

void DimensionsHashing::setDatabase(OnlineDatabase::Ptr database) {
  if (!database) {
    printf("[ERROR][DimensionsHashing] Database is not set\n");
    exit(EXIT_FAILURE);
  }
  _database = database;
}

std::vector<int> DimensionsHashing::getCandidates(int quId) {
  if (!_database) {
    printf("[ERROR][DimensionsHashing] Database is not set\n");
    exit(EXIT_FAILURE);
  }
  const auto featurePtr = std::static_pointer_cast<const iBinarizableFeature>(
      _database->getQueryFeature(quId));

  // printf("[POINTER] %p\n", featurePtr.get());
  if (!featurePtr) {
    printf(
        "[WARNING] The feature pointer is empty. Probably a wrong type is "
        "set.\n");
  }
  return hashFeature(featurePtr);
}

/** the bits should be already computed for the features **/
void DimensionsHashing::hashFeatures(
    const std::vector<iBinarizableFeature::Ptr>& features) {
  for (size_t f = 0; f < features.size(); ++f) {
    for (size_t b = 0; b < features[f]->bits.size(); ++b) {
      if (features[f]->bits[b]) {
        // if selected bin is 1
        index[b].push_back(f);
      }
    }
  }
  this->weightIndex(features.size());
}

/* prints the constructed inverted index. Not recommended for features with big
dimensions.
For every entry of the index we save the list of features ids that have the
corresponding dimension activated **/
void DimensionsHashing::showIndex() const {
  printf("Constructed index: \n");
  for (const auto& pair : index) {
    printf("Dim %d: ", pair.first);
    for (int id : pair.second) {
      printf("%d, ", id);
    }
    printf("\n");
  }
}

/**
 * @brief      Performs a query for the dimension hashing given a feature. After
 * selecting the candidates from the hash table, accepts those that have
 * occurance more than 70%.For more details check readme.md
 *
 * @param[in]  fPtr  pointer to the binarizable feature
 *
 * @return     vector of candidates ids
 */
std::vector<int> DimensionsHashing::hashFeature(
    const iBinarizableFeature::ConstPtr& fPtr) const {
  Timer timer;
  timer.start();
  if (_dimWeights.empty()) {
    printf(
        "[ERROR][DimensionsHashing] The IDF weights were not computed. Can't "
        "hash a feature\n");
    exit(EXIT_FAILURE);
  }
  std::unordered_map<int, float> featureOcc;
  for (int d = 0; d < fPtr->bits.size(); ++d) {
    if (fPtr->bits[d]) {
      // if bit equals to 1, take the dimension id and check in the index
      auto dim_found = index.find(d);
      if (dim_found != index.end()) {
        // an entry for dimension d exists
        // then add all associated feature ids
        for (int featureId : dim_found->second) {
          auto found = featureOcc.find(featureId);
          if (found == featureOcc.end()) {
            // new featureId
            // featureOcc[featureId] = 1.0;
            featureOcc[featureId] = _dimWeights.at(d);
          } else {
            // featureOcc[featureId] += 1.0;
            featureOcc[featureId] += _dimWeights.at(d);
          }
        }
      }
    }
  }

  std::vector<int> candidates;
  if (featureOcc.empty()) {
    return candidates;
  }
  // find min and max occurance
  float maxOcc = 0;
  float minOcc = std::numeric_limits<float>::max();
  for (const auto& o : featureOcc) {
    if (o.second < minOcc) {
      minOcc = o.second;
    }
    if (o.second > maxOcc) {
      maxOcc = o.second;
    }
  }
  printf("[HASH] Max occurance: %3.2f; min occurance: %3.2f\n", maxOcc, minOcc);
  // accept all occurances more than 70%
  float accOcc = 0.7 * (maxOcc - minOcc) + minOcc;
  printf("[HASH] Accepted occurance for candidates: %3.2f\n", accOcc);

  // printf("[HASH] hist: ");
  for (const auto& o : featureOcc) {
    // printf("%d ", o.second);
    if (o.second >= accOcc) {
      candidates.push_back(o.first);
    }
  }
  // printf("\n");
  printf("[HASH] Selected %lu candidates: \n", candidates.size());
  // for (int i : candidates) {
  //   printf("%d ", i);
  // }
  // printf("\n");

  timer.stop();
  fprintf(stderr, "[HASH] time: ");
  // timer.print_elapsed_time(TimeExt::MSec);
  timer.print_elapsed_time(TimeExt::MicroSec);
  return candidates;
}

void DimensionsHashing::saveIndex(const std::string& filename) const {
  std::ofstream out(filename.c_str());
  if (!out) {
    printf("[ERROR][DimensionsHashing] Can't open output file %s\n",
           filename.c_str());
    exit(EXIT_FAILURE);
  }

  for (const auto& pair : this->index) {
    out << pair.first << " ";
    for (size_t i = 0; i < pair.second.size(); ++i) {
      out << pair.second[i] << " ";
    }
    out << std::endl;
  }
  out.close();
  printf("[INFO][DimensionsHashing] Index was printed to a file %s\n",
         filename.c_str());
}

void DimensionsHashing::loadIndex(const std::string& filename) {
  std::ifstream in(filename.c_str());
  if (!in) {
    printf("[ERROR][DimensionsHashing] can't read file %s\n", filename.c_str());
    exit(EXIT_FAILURE);
  }
  while (!in.eof()) {
    std::string line;
    std::getline(in, line);
    std::stringstream ss(line);
    int bin;
    ss >> bin;
    std::vector<int> binElements;
    while (!ss.eof()) {
      int refId;
      ss >> refId;
      binElements.push_back(refId);
    }
    this->index[bin] = binElements;
  }
  printf("[INFO][DimensionsHashing] Hash table was loaded\n");
}

void DimensionsHashing::weightIndex(int refSize) {
  if (index.empty()) {
    printf(
        "[ERROR][DimensionsHashing] The index is empty. Nothing to weight.\n");
    exit(EXIT_FAILURE);
  }

  for (const auto& el : index) {
    _dimWeights[el.first] =
        log(static_cast<double>(refSize) / el.second.size());
  }
  printf("[INFO][DimensionsHashing] IDF weights for the index were computed\n");
}
