

add_library(dimensions_hashing dimensions_hashing.cpp)
target_link_libraries(dimensions_hashing
	cnn_feature
    online_database
	timer
)

add_library(lsh_cv_hashing lsh_cv_hashing.cpp)
target_link_libraries(lsh_cv_hashing 
    timer
    online_database
    ${OpenCV_LIBS}
)