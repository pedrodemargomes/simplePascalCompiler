cmake_minimum_required(VERSION 3.23)
project(simplePascalCompiler)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-g)

add_executable(simpascompiler)

target_sources(simpascompiler PRIVATE
	lexer.cpp
	parser.cpp
)

target_compile_definitions(simpascompiler PRIVATE
	$<$<CONFIG:Debug>:DEBUG>
)

