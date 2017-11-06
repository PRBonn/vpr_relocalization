# Features

## Provided features

In this framework, we provide an implementations for the class `ifeature` that are able to read the features produces by two CNN, namely OverFeat ([paper](https://arxiv.org/pdf/1312.6229.pdf)) and VGG-16.
As a pre-processing step, we extract the features from the nets and store them into the '\*.txt' files.
The following classes know how to read and binarize certain features:

* `cnn_feature.h` reads the OverFeat features and perfoms **mid_binarization**
* `cnn_feature_mean.h` reads the OverFeat features and perfoms **mean_binarization**

* `vgg_feature.h` reads the VGG-16 features and perfoms **mid_binarization**
* `vgg_feature_mean.h` reads the VGG-16 features and perfoms **mean_binarization**


For details on binarization, check [relocalizers](../relocalizers/readme.md).

For details on '.txt' formats check [examples](../../examples/readme.md).

## Your own features

To use your own features, you need to derive a class from `ifeature.h`. If you want to use relocalizers you should derive from `ibinarizable_feature.h`.

1. You should reimplement all `virtual` functions.
2. You should update the `feature_factory.h` to let it know, which type of features to work with. Within the main framework all the feature generations are done through `feature_factory.h`.
