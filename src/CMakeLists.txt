include_directories(.)
set(SOURCES main.cpp contactInfoAnalysis.cpp fillingFactorAnalysis.cpp dim3vectors.cpp fileInterface.cpp generalAlgorithms.cpp)
add_executable(sphere_packing_analysis ${SOURCES})
target_link_libraries(sphere_packing_analysis PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(
   sphere_packing_analysis PUBLIC
   "${PROJECT_BINARY_DIR}"
)