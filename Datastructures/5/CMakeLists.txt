cmake_minimum_required(VERSION 3.5)
project(Matrix)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp MatrixBase.h ClassicMatrix.h Generator.h SparseMatrix.h SparseMatrixOptimized.h Helper.h)
add_executable(Matrix ${SOURCE_FILES})