# Relocalizers

## Dimension Hashing (DH)

Works with binary features. Takes dimensions explicitly into account. 
The keys in the inverted index are the dimension **ids** that have 1 in at least one feature from the reference database.
The values are the feature Ids, which have 1 for a dimension 'key'. For example 
`index[5] = {3, 10, 112}`, means that features `3,10 and 112` have 1 in the dimension 5.

When getting the candidates for the query features. The query feature is binarized and for every dimension that equals to 1, we collect all the featureIds stored in the index by accessing the hash-table based on dimension ids.

Then applying the voting scheme, we can compute how many times a featuresId was voted for. Afterwards, we select the features Ids with the biggest number of votes, namely 30% of features with higher  votes.

* Highly depends on binarization. Since the more dimensions have "1" are activated the more computations should be performed, while quering.
* Fast with **mid-binarization**

More details can be found in the related paper [Relocalization using hashing](http://www.ipb.uni-bonn.de/wp-content/papercite-data/pdf/vysotska2017irosws.pdf).

## Locality sensitive hashing (LSH)

In this framework we use the OpenCV implementation of the Locality Sensitive Hashing (LSH), namely Multi-probe LSH. 
This implementation also needs the features to be binary features. We find that **mean-binarization** works better for this hashing method.

* use **mean-binarization**
* has some proven theoretical limits

## Binarization

Before using one of the proposed relocalizers, one needs to binarize the features.

**Mean binarization**: Takes a feature vector of double values and thresholds it based on the mean value of the vector.

**Mid binarization**:  Takes a feature vector of double values and thresholds it based on the median (middle) value of the vector.



