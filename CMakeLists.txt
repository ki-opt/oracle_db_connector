
# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.13)

project(OracleDbConnector)

# C++標準を設定
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ビルドタイプの設定（デフォルトはRelease）
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

# IPO（Interprocedural Optimization）の設定
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported AND CMAKE_BUILD_TYPE STREQUAL "Release")
	message(STATUS "IPO/LTO is supported and enabled for Release build")
	set_target_properties(main PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
	set_target_properties(cpp_module PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
	message(STATUS "IPO/LTO not supported or Debug build: ${ipo_error}")
endif()

# Pythonを検索
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# pybind11を検索・設定
find_package(pybind11 QUIET)

# デバッグ情報を表示
message(STATUS "Python3_VERSION: ${Python3_VERSION}")
message(STATUS "Python3_EXECUTABLE: ${Python3_EXECUTABLE}")
message(STATUS "pybind11_VERSION: ${pybind11_VERSION}")

# C++実行ファイルを作成（C++からPythonを呼び出す場合）
add_executable(main cpp/main.cpp)
target_link_libraries(main PRIVATE pybind11::embed)

# コンパイルオプション
target_compile_options(main PRIVATE ${COMMON_COMPILE_OPTIONS})

# カスタムターゲット：C++実行ファイルの実行
add_custom_target(run_cpp
	COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH="${PYTHON_SITE_PACKAGES}:${PYTHON_PATH}" $<TARGET_FILE:main>
	DEPENDS main
	COMMENT "Running C++ executable with proper PYTHONPATH"
)

# カスタムターゲット：Python環境の診断
add_custom_target(diagnose_python
	COMMAND ${Python3_EXECUTABLE} -c "import sys; print('Python executable:', sys.executable)"
	COMMAND ${Python3_EXECUTABLE} -c "import sys; print('Python path:', sys.path)"
	COMMAND ${Python3_EXECUTABLE} -c "try: import oracledb; print('oracledb version:', oracledb.version); except ImportError as e: print('oracledb import error:', e)"
	COMMENT "Diagnosing Python environment"
)