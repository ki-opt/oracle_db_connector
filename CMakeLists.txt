# cmake -DCMAKE_BUILD_TYPE=Debug .. => debugモード
# cmake .. => releaseモード(-O2)
# make ビルド
# make && ./main or make run_cppでビルド/実行

# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.13)

# project名を設定
project(OracleDbConnector)

# C++標準(C++23)を設定
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ビルドタイプの設定（デフォルトはRelease）
# cmake -DCMAKE_BUILD_TYPE=Debug ..とすることでdebugモード
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()
# Releaseモードの最適化レベルを-O2に変更
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

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

# C++実行ファイルを作成
set(SOURCES
	src/main.cpp
	#cpp/
	#ここにコンパイルしたいファイルを書く
	#
)
set(HEADER
	#src/include/
	src/external/inipp.h
	#headerをここに書いてもよい, その場合はadd_executableを変更
)
add_executable(main ${SOURCES} ${HEADERS}) # headerを含める場合
target_link_libraries(main PRIVATE pybind11::embed)	# pybind11のembedライブラリリンク

# コンパイルオプション
target_compile_options(main PRIVATE ${COMMON_COMPILE_OPTIONS})

# プロジェクトルートディレクトリをインクルードパスに追加
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR})

# カスタムターゲット：C++実行ファイルの実行
add_custom_target(run_cpp
	COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH="${PYTHON_SITE_PACKAGES}:${PYTHON_PATH}" $<TARGET_FILE:main>
	DEPENDS main
	COMMENT "Running C++ executable with proper PYTHONPATH"
)
