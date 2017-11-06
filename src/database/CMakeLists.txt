
add_library(list_dir list_dir.cpp)

add_library(online_database online_database.cpp)
target_link_libraries(online_database
	timer 
    list_dir
	feature_buffer
    feature_factory
)

find_package( OpenCV REQUIRED )
if( OpenCV_FOUND)
	include_directories( ${OpenCV_INCLUDE_DIRS} )

	add_library(cost_matrix_database cost_matrix_database.cpp)
	target_link_libraries(cost_matrix_database 
		online_database
		${OpenCV_LIBS}
	)

endif()
