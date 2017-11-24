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
#include <unordered_map>
#include "database/list_dir.h"
#include "features/cnn_feature.h"
// #include "features/vgg_feature.h"

#include "relocalizers/dimensions_hashing.h"

void hashDimensions(const std::string &path2folder,
                    const std::vector<std::string> &featureNames,
                    const std::string &outputName) {
  printf("Performing Dimensions Hashing\n");
  std::vector<iBinarizableFeature::Ptr> featurePtrs;
  fprintf(stderr, "[INFO] Reading %lu features \n", featureNames.size());

  for (size_t i = 0; i < featureNames.size(); ++i) {
    iBinarizableFeature::Ptr featurePtr =
        iBinarizableFeature::Ptr(new CnnFeature);
        // iBinarizableFeature::Ptr(new VggFeature);
    featurePtr->loadFromFile(featureNames[i]);
    featurePtrs.push_back(featurePtr);
    fprintf(stderr, ".");
  }
  fprintf(stderr, "\n");

  printf("Features were loaded and binarized\n");

  DimensionsHashing hasher;

  hasher.hashFeatures(featurePtrs);
  hasher.saveIndex(outputName);
}

int main(int argc, char const *argv[]) {
  printf("====== Hashing features ========\n");
  if (argc < 3) {
    printf(
        "Not enough input parameters. Proper usage: path2folder  "
        "outputName(txt)\n");
    return 0;
  }
  std::string path2folder = argv[1];
  std::string outputName = argv[2];

  // read In features
  std::vector<std::string> featureNames = listDir(path2folder);
  hashDimensions(path2folder, featureNames, outputName);

  printf("Done.\n");

  return 0;
}
