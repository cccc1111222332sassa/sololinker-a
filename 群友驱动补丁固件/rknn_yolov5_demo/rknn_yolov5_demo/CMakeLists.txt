cmake_minimum_required(VERSION 3.4.1)

project(rknn_yolov5_demo)

set( CMAKE_C_COMPILER /home/ubuntu/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-gcc)
set( CMAKE_CXX_COMPILER /home/ubuntu/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-c++)
set(ENV{STAGING_DIR} /home/ubuntu/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--allow-shlib-undefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--allow-shlib-undefined")

# install target and libraries
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/rknn_yolov5_demo_${CMAKE_SYSTEM_NAME})

set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# 设置交叉编译的Opencv路径
#set(OpenCV_DIR /home/ubuntu/luckfox-pico/buildroot-2023.02.6/output/build/opencv4-4.6.0/buildroot-build)
#set(OpenCV_DIR /home/ubuntu/luckfox-pico/opencv-mobile-4.8.1-luckfox-pico)
set(OpenCV_DIR /home/ubuntu/luckfox-pico/fawen_project/rv110x_lib/opencv4.6.0)

#set(OpenCV_DIR /home/ubuntu/luckfox-pico/rv110/rknpu2-1.5.2/examples/3rdparty/opencv/opencv-linux-armhf/share/OpenCV/)
# 查找Opencv包
find_package(OpenCV REQUIRED)

# rknn api
set(RKNN_API_PATH ${CMAKE_SOURCE_DIR}/librknn_api)
set(RKNN_RT_LIB ${RKNN_API_PATH}/armhf/librknnmrt.so)

include_directories(${RKNN_API_PATH}/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty)
set(rga ${CMAKE_SOURCE_DIR}/rga_rv1106/)
file(GLOB_RECURSE rgah ${rga}include/*.h)
message("---666----rgah------------"${rgah})#批量添加头文件
# rknn_yolov5_demo
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/rga_rv1106/include/)
#include_directories(v/rga_rv1106/include/rga/)#头文件搜索路径\
include_directories(${CMAKE_SOURCE_DIR}/utils/)#头文件搜索路径\
include_directories(/home/ubuntu/luckfox-pico/fawen_project/resize_demo/utils/3rdparty/libdrm/include/)#头文件搜索路径\
include_directories(/home/ubuntu/luckfox-pico/fawen_project/resize_demo/utils/3rdparty/libdrm/include/libdrm/)#头文件搜索路径\
include_directories(/home/ubuntu/luckfox-pico/fawen_project/rknn_yolov5_demo/include)#头文件搜索路径\
add_executable(rknn_yolov5_demo
            src/main.cpp
            src/postprocess.cc
            src/video_capa.cpp
            #src/rga.cpp
            ${rgah}
            ${CMAKE_SOURCE_DIR}/utils/utils.cpp
            ${CMAKE_SOURCE_DIR}/utils/dma_alloc.cpp
            ${CMAKE_SOURCE_DIR}/utils/drm_alloc.cpp
            )

target_link_libraries(rknn_yolov5_demo
${RKNN_RT_LIB}
${OpenCV_LIBS} -L/home/ubuntu/luckfox-pico/fawen_project/rknn_yolov5_demo/rga_rv1106/lib/Linux/armhf/
-lrga)

#/home/ubuntu/luckfox-pico/fawen_project/rknn_yolov5_demo/rga_rv1106/lib/
# install target and libraries
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/rknn_yolov5_demo_${CMAKE_SYSTEM_NAME})
install(TARGETS rknn_yolov5_demo DESTINATION ./ )
install(PROGRAMS ${RKNN_RT_LIB} DESTINATION lib)
install(DIRECTORY model DESTINATION ./)
