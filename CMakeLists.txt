cmake_minimum_required(VERSION 4.0)
project(StringArt)

find_package(
  JPEG REQUIRED
)


add_executable(${PROJECT_NAME}
  # Required Source Files
  src/main.c
  src/images.c
  src/filters.c
)

target_link_libraries(${PROJECT_NAME}
  JPEG::JPEG
)
