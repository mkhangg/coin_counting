project(hw2)
cmake_minimum_required(VERSION 2.8)

# find required libs in opencv
find_package(OpenCV REQUIRED)

# create sub-projects
add_executable(count_coin count_coin.cpp)
target_link_libraries(count_coin ${OpenCV_LIBS})
