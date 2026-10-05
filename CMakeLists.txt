cmake_minimum_required(VERSION 3.23)
project(simplePascalCompiler)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)

add_executable(simpascompiler)

target_compile_options(simpascompiler PRIVATE
	-fsanitize=address
	-fno-omit-frame-pointer
	-g
)

target_link_options(simpascompiler PRIVATE
	-fsanitize=address
)

target_sources(simpascompiler PRIVATE
	lexer.cpp
	parser.cpp
	codeGenVisitor.cpp
)

target_compile_definitions(simpascompiler PRIVATE
	$<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(simpascompiler PRIVATE
	${LLVM_INCLUDE_DIRS}
)

target_compile_definitions(simpascompiler PRIVATE
	${LLVM_DEFINITIONS}
)

llvm_map_components_to_libnames(LLVM_LIBS
	core
)

target_link_libraries(simpascompiler PRIVATE
	${LLVM_LIBS}
)
