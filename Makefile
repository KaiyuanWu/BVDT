SRC:=$(wildcard src/bvdt/*.cpp)
OBJ:=$(patsubst %.cpp,%.o,$(SRC))
CXX:=g++
bvdt: algo.pb.o $(OBJ)
	$(CXX)  -o bvdt $(OBJ) src/proto/algo.pb.o -L/usr/local/lib/  -lprotobuf 
algo.pb.o:algo.pb.cc
	$(CXX) -c src/proto/algo.pb.cc -o src/proto/$@
algo.pb.cc:src/proto/algo.proto
	protoc --cpp_out=. $<
.cpp.o:$<
	$(CXX)  -Iincludes/ -Isrc/proto/ -c $< -o $@
clean:
	rm -fr $(OBJ) bvdt src/proto/algo.pb.* *.o
