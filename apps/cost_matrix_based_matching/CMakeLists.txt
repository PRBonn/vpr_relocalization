find_package(OpenCV REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)


add_executable(cost_matrix_based_matching_dh cost_matrix_based_matching_dh.cpp)
target_link_libraries( cost_matrix_based_matching_dh
    online_localizer
    cost_matrix_database
    successor_manager
    full_matrix_visualizer
    config_parser
    dimensions_hashing
    cnn_feature
    ${OpenCV_LIBS}
)


add_executable(cost_matrix_based_matching_lsh cost_matrix_based_matching_lsh.cpp)
target_link_libraries( cost_matrix_based_matching_lsh
    online_localizer
    cost_matrix_database
    successor_manager
    full_matrix_visualizer
    config_parser
    lsh_cv_hashing
    cnn_feature
    ${OpenCV_LIBS}
)
