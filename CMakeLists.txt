#设置最低cmake版本
cmake_minimum_required(VERSION 3.16)

#创建工程，添加C++和C语言编译器支持
project(TESTAPP LANGUAGES C CXX)

#设置C++20标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 如果没有手动设置构建类型，则默认为Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(Qt5_DIR "D:/soft-pc/Qt/Qt5.9.0/5.9/mingw53_32/lib/cmake/Qt5")
set(QML_IMPORT_PATH "D:/soft-pc/Qt/Qt5.9.0/5.9/mingw53_32/qml")


#设置dll文件路径
set(EXTERNAL_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/)
set(PNNETSDK_PATH "${EXTERNAL_LIB_PATH}")
set(PNNETSDK_BINARY ${PNNETSDK_PATH}Sample.dll)
set(INPOUT_PATH "${EXTERNAL_LIB_PATH}")
set(INPOUT_BINARY ${INPOUT_PATH}inpout32.dll)


#在widgets项目中,这句代码用来预处理.ui界面文件的，QML项目里好像用不到
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


#查找用到的一些库 ，cmake中必须要先查找得到
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets SerialPort Bluetooth Charts WebSockets Qml Quick Concurrent)
find_package(Qt5 5.9 REQUIRED COMPONENTS Core Gui Qml Quick WebSockets)
qt5_wrap_cpp(cmakeandqt_MOC Samples.h)

#EDFlib
#edflib.c 应该用 C 编译器编译，需要给edflib目标设置LINKER_LANGUAGE C，明确用C编译方式
    add_library(edflib STATIC)
    target_sources(edflib PRIVATE ${EXTERNAL_LIB_PATH}edflib/edflib.c)
    target_include_directories(edflib PUBLIC ${EXTERNAL_LIB_PATH}edflib)
    set_target_properties(edflib PROPERTIES LINKER_LANGUAGE C)
# Libusb
    set(Libusb_PATH ${EXTERNAL_LIB_PATH}libusb/)
    set(LIBUSB_INCLUDE_DIR ${Libusb_PATH}include)
    set(LIBUSB_LIB_DIR ${Libusb_PATH}lib)
   include_directories(${LIBUSB_INCLUDE_DIR})
    link_directories(${LIBUSB_LIB_DIR})
    set(LIBUSB_LIBS usb-1.0)

set(HEADERS
    Samples.h
    inpout32.h
    AmplifierClient.h

)

set(SRC
    src/main.cpp
    src/AmplifierClient.cpp

 )

set(RESOURCES
    drop.png
)

set(QML
    qml/App.qml
    qml/Bpp01.qml
    qml/Screen01.qml
)

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SRC}
    ${RESOURCES}
    ${QML}
    qml.qrc

)


#把当前路径设置为C++头文件搜索路径
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

#添加项目依赖的库(动态库，静态库，Object或Interface库)
# 依赖库（全部集中在这里统一link）
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Concurrent
    Qt5::Qml
    Qt5::Quick
    Qt5::SerialPort
    Qt5::Bluetooth
    Qt5::Charts
    Qt5::WebSockets
    ${LIBUSB_LIBS}
    edflib
)


set(BINARY
     ${PNNETSDK_BINARY}
     ${INPOUT_BINARY}
    )


set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/installed)
INSTALL(FILES ${BINARY} DESTINATION ${CMAKE_INSTALL_PREFIX})
INSTALL(FILES ${BINARY} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
install(DIRECTORY SDK devices DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY SDK devices DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

