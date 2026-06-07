# *********************************************
# 使用方法：
# 1. mkdir -p build && cd build
# 2. cmake \
#      -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/cmake \
#      -DCMAKE_BUILD_TYPE=Release \
#      ..
#    或者：
#    cmake \
#      -DCMAKE_PREFIX_PATH=/opt/Qt/5.15.2/gcc_64/lib/cmake \
#      -DCMAKE_BUILD_TYPE=Release \
#      ..
# 3. cmake --build . --parallel
# *********************************************

cmake_minimum_required(VERSION 3.16)
project(iiCTs LANGUAGES CXX)

# 1) C++11 和 Qt 自动化
set(CMAKE_CXX_STANDARD        11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC             ON)
set(CMAKE_AUTOUIC             ON)
set(CMAKE_AUTORCC             ON)

# 2) 找齐 Qt5 模块（一次到位，含 Charts, OpenGL, X11Extras）
find_package(Qt5 REQUIRED COMPONENTS
  Core Gui Widgets Sql Concurrent Charts OpenGL
)

# 3) 平台特定的Qt模块
if(UNIX AND NOT APPLE)
  find_package(Qt5 REQUIRED COMPONENTS X11Extras)
endif()

# 3) 编译宏
add_compile_definitions(
  QT_NO_DEPRECATED_WARNINGS
  SCALE_RATE=0.6
  CTISS_USERS_DB_FILENAME=\"iiCTs_users.db\"
)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  # add_compile_definitions(QT_NO_DEBUG_OUTPUT)
endif()

# 4) 收集源文件、头文件、UI、QRC
file(GLOB_RECURSE CPP_SOURCES   "${CMAKE_SOURCE_DIR}/source/*.cpp")
file(GLOB_RECURSE HEADER_FILES   "${CMAKE_SOURCE_DIR}/include/*.h")
set(UI_FILES
  "${CMAKE_SOURCE_DIR}/ui/mainwindow.ui"
)
set(QRC_FILES
  "${CMAKE_SOURCE_DIR}/guide/guide.qrc"
  "${CMAKE_SOURCE_DIR}/images/images.qrc"
  "${CMAKE_SOURCE_DIR}/version/version.qrc"
)

# 5) 生成可执行 Target
add_executable(iiCTs
  ${CPP_SOURCES}
  ${HEADER_FILES}    # AUTOMOC 会扫描带 Q_OBJECT 的头
  ${UI_FILES}
  ${QRC_FILES}
)

# 6) 包含目录
target_include_directories(iiCTs PRIVATE
  "${CMAKE_SOURCE_DIR}/include"
  "${CMAKE_SOURCE_DIR}/include/mainui"
  "${CMAKE_SOURCE_DIR}/include/panel"
  "${CMAKE_SOURCE_DIR}/include/widget"
  "${CMAKE_SOURCE_DIR}/include/tgwl"
)

# 6.1) 确保MOC能找到所有头文件
set_target_properties(iiCTs PROPERTIES
  AUTOMOC_MOC_OPTIONS "-I${CMAKE_SOURCE_DIR}/include"
)

# 7) 链接 Qt5
target_link_libraries(iiCTs PRIVATE
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  Qt5::Sql
  Qt5::Concurrent
  Qt5::Charts
  Qt5::OpenGL
)

# 7.1) 平台特定的链接库
if(UNIX AND NOT APPLE)
  target_link_libraries(iiCTs PRIVATE Qt5::X11Extras)
endif()

# 8) 安装
install(TARGETS iiCTs RUNTIME DESTINATION bin)
