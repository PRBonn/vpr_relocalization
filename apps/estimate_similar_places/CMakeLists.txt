add_executable(estimate_similar_places estimate_similar_places.cpp)
target_link_libraries(estimate_similar_places 
    cnn_feature_mean
    vgg_feature_mean
    list_dir
)