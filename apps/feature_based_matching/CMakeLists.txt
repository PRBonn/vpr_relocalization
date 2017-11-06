find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

add_executable(feature_based_matching_dh feature_based_matching_dh.cpp)
target_link_libraries(feature_based_matching_dh
    online_localizer
    online_database
    successor_manager
    dimensions_hashing
    visualizer
    config_parser
    yaml-cpp
)

add_executable(feature_based_matching_lsh feature_based_matching_lsh.cpp)
target_link_libraries(feature_based_matching_lsh
    online_localizer
    online_database
    successor_manager
    visualizer
    config_parser
    yaml-cpp
    lsh_cv_hashing
)

add_executable(feature_based_matching_dh_no_vis feature_based_matching_dh_no_vis.cpp)
target_link_libraries(feature_based_matching_dh_no_vis
    online_localizer
    online_database
    successor_manager
    config_parser
    yaml-cpp
    dimensions_hashing
)