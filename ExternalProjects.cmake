include (ExternalProject)

# Dependency 관련 변수 설정
set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)

# 아래 필요한 셋팅인지 판단필요
#set(DEP_INCLUDE_DIR ${DEP_INSTALL_DIR}/include)
#set(DEP_LIB_DIR ${DEP_INSTALL_DIR}/lib)

ExternalProject_Add(
    extern_glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.3.8"
    GIT_SHALLOW 1
    UPDATE_COMMAND "" PATCH_COMMAND "" TEST_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DGLFW_BUILD_EXAMPLES=OFF
        -DGLFW_BUILD_TESTS=OFF
        -DGLFW_BUILD_DOCS=OFF
)
set(DEP_LIST ${DEP_LIST} extern_glfw)
set(DEP_LIBS ${DEP_LIBS} glfw3)

ExternalProject_Add(
    extern_glad
    GIT_REPOSITORY "https://github.com/Dav1dde/glad"
    GIT_TAG "v0.1.34"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DGLAD_INSTALL=ON
    TEST_COMMAND ""
)

set(DEP_LIST ${DEP_LIST} extern_glad)
set(DEP_LIBS ${DEP_LIBS} glad)