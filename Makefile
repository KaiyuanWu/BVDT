SRC:=$(wildcard *.cpp)
OBJ:=$(patsubst %.cpp,%.o,$(SRC))
CXX:=g++
bvdt:$(OBJ) algo.pb.o
	$(CXX)  -o main $(OBJ) algo.pb.o -L/usr/local/lib/  -lprotobuf 
.cpp.o:$<
	$(CXX)  -c $< -o $@
algo.pb.o:algo.pb.cc
	$(CXX) -c $< -o $@
algo.pb.cc:algo.proto
	protoc --cpp_out=. algo.proto
clean:
	rm -fr $(OBJ) main algo.pb.*
