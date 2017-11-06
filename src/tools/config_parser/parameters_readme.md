# Parameters

All the parameters needed to run the code can be found in provided `config.yaml` file.
For detailed description of individual parameters please check `config_parser.h`.

Here, we provide the description of important / sensitive parameters. 


### Fan-out
(integer)

Parameter that takes care about different frame rates or camera speeds between the sequences. If the cameras were moving with exactly the same speed and the same frame rate `fan-out = 1`. Typically, we use `fan_out = 5 or 10`, which was working fine for us. If you expect big speed difference make sure you set higher values for this parameter.

Attention: the higher the value of `fan_out` the slower the code will run, since the `fan_out` determines the number of children for each expanded node in the graph.


### Non-match cost
(float)

This parameter specifies the threshold for matching two images.
In our case the features are matched using cosine distance, e.g. 1 corresponds to perfect match and 0 if two images don't match completely.
For the search in the graph, we invert all costs by using 1/cost, e.g. the closer the value is to 1 the better the match.
that's why we set the `non_match_cost` in range from `3.7` to `5.0`.

### Speed vs quality

Adding additional edges to the graph slows the search. This becomes especially noticeable in the cases of wrongly identifying similar places. The more false hypothesis you specify, the longer it takes for the search to check them.

You can speed up the search by not providing the 'similar' places. Then the moment the search will consider the robot to be lost, it will perform the `relocalization` step. This may lead to degradation in the result quality.

### Speed vs memory

To be able to work with large image sequences, in this code we only keep in memory limited number of features. Namely the ones that were recently loaded.
The `buffer_size` specifies the number of features kept in memory.
If you can allow yourself to use more memory you can increase the `buffer_size`. The default value is '100'.

In case the robot is not lost, this may lead to faster search.

