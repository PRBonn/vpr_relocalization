# Estimate similar places

To estimate the similar places within the reference trajectory, we compute matches for every image in the trajectory to all previous images.
The match is considered valid  if the cost of matching 2 images is smaller than specified `non_match_cost` and the images are  not located nearby in the sequence. We consider the neighbourhood of 10 frames here.

**Note**: Other more sophisticated methods can be used for this task, like FAB-MAP2.

**Non-match cost**. You should roughly know the `non_match_cost` for a pair of images. If you know that two images should match  than the `non_match_cost = 1/c`, where c is the cost of matching images, e.g. cosine distance between features. For cosine distance a perfect match will result in 1, so if we say that images with costs 0.7 still represent the same place, then a `non_match_cost` should be 1/0.7 approx. 1.4. This means that every image pair with a smaller cost will be considered as a match and all others as non-match.

## Output format

The outputted file has a following format of every line: 
`refId1 refId2`, which represents that refId1 correspondences to the same places as refId2.
If `refId1` corresponds to the multiple ids, like `refId3, refId4...`, than there will  be multiple lines representing this fact
```
0: refId1 refId2
1: refId1 refId3 
2: refId1 refId4 
... 
```

## Check the result

Overestimation of similar places within reference dataset may lead to slow performance of a localizer, since a lot more place hypothesis should be checked in online phase. In general, it useful to have less false positives for this stage.
During the estimation of similar places, the binary provides the `costs.txt` file with all the matching cost that were computed.
We provide a script to visually inspect if the result of similar place estimation makes sense.
Use `costs.txt` file with `simPlaces.txt` file to visually inspect the matches:
```
python check_sim_places.py
```
Check the details in the script.