
include_directories(include)
add_library(SLAU src/SLAU/SLAU.cpp src/TaskManager/Worker.cpp)
add_executable(prog src/main.cpp)
target_link_libraries(prog SLAU pthread)
