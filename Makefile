#! @file
#! @author Vsevolod (Seva) Ivanov
#!

CXX=g++
CXXFLAGS=-std=c++11 -g -Wall -Wextra -Wfatal-errors -pedantic \
		-lGLEW -lGL -lX11 -lpthread -lXrandr -lXi \
		-I./src

GLFW_ARCH=-lglfw
GLFW_LINUX=-lglfw3

CXX_FILES=src/*.cpp

all:
	mkdir -p build

arch: all
	${CXX} ${CXXFLAGS} ${GLFW_ARCH} ${CXX_FILES}\
		-o build/mesh.out

linux: all
	${CXX} ${CXXFLAGS} ${GLFW_LINUX} ${CXX_FILES}\
		-o build/mesh.out

clean:
	rm -rf build/
