## General

`successor_manager` is a class that handles the communication between the `localizer` and the `database`. Given the node from the localizer that should be opened the successor manager returns the set of 'successor' nodes. It knows how to handle similar places in the reference sequence as well as how to get successor in the case the LOST signal was triggered.

### Node

`Node` class is a container for a node in the graph. It is hashable, so can be used in `std::unordered_set`. 
The `operator<` only compares the nodes based on accumulated cost.

### Estimating similar places

If you want to add additional information about the similar places in the reference trajectory, it should be specified in the following format:

```
ref_id_1 ref_id_2
ref_id_1 ref_id_3
```

Use this [app](../../estimate_similar_places/readme.md) to generate the file with similar places.
