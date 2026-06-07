#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.16)
project(GammaTherapy)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

# [MODIFICATION] <<< --- 从这里开始是新增的Qt配置 --- >>>
# 1. 查找 Qt5 库
#    我们需要 Widgets, OpenGL 和 X11Extras 模块来支持嵌入式可视化
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets OpenGL X11Extras)

# 2. 启用 Qt 的 MOC (Meta-Object Compiler)
#    这是所有使用 Q_OBJECT 宏的Qt类所必需的。EmbeddingHelper.cc 内部
#    创建了QApplication等，间接需要此功能。
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
# [MODIFICATION] <<< --- Qt配置结束 --- >>>

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include)

# 使用 file(GLOB ...) 会自动包含 src/ 目录下的所有 .cc 文件,
# 包括您新增的 EmbeddingHelper.cc
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable
add_executable(GammaTherapy GammaTherapy.cc ${sources})

# [MODIFICATION] 3. 添加必要的编译宏定义
#    这会直接解决 "G4VIS_USE_QT is not defined" 的错误。
target_compile_definitions(GammaTherapy PUBLIC G4VIS_USE_QT)

#----------------------------------------------------------------------------
# 配置 ROOT (您的原有配置保持不变)
set(ROOT_DIR "/home/day/Geant4/ROOT/root-6.26.00") # 您可以按需修改此路径
find_package(ROOT REQUIRED COMPONENTS Core Hist Graf Gpad Tree)
include_directories(${ROOT_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# [MODIFICATION] 4. 链接所有必需的库
#    在原有基础上，增加了 Qt5 的所有必要模块
target_link_libraries(GammaTherapy
    PUBLIC
    ${Geant4_LIBRARIES}
    ${ROOT_LIBRARIES}
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::OpenGL
    Qt5::X11Extras
)

#----------------------------------------------------------------------------
# Copy all scripts to the build directory (您的原有配置保持不变)
set(GammaTherapy_SCRIPTS
    be.in be_w.in cu_w.in GammaTherapy.in GammaTherapy.out vis.mac
  )

foreach(_script ${GammaTherapy_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()


install(TARGETS GammaTherapy DESTINATION bin)