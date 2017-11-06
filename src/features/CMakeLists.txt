add_library(cnn_feature cnn_feature.cpp)
add_library(cnn_feature_mean cnn_feature_mean.cpp)
target_link_libraries(cnn_feature_mean cnn_feature)

add_library(vgg_feature vgg_feature.cpp)
add_library(vgg_feature_mean vgg_feature_mean.cpp)
target_link_libraries(vgg_feature_mean vgg_feature)

add_library(feature_factory feature_factory.cpp)
target_link_libraries( feature_factory 
	cnn_feature
    cnn_feature_mean
    vgg_feature
    vgg_feature_mean
)

add_library(feature_buffer feature_buffer.cpp)