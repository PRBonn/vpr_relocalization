add_executable(hash_features hash_features.cpp)
target_link_libraries(hash_features 
    list_dir
    vgg_feature
    dimensions_hashing
)
