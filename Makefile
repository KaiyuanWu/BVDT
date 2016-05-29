#Set DLIB_DIR and OOQP_DIR to your dlib and OOQP package locations
DLIB_DIR:=
OOQP_DIR:=

OOQP_INC:=$(OOQP_DIR)/include
OOQP_LIB:=$(OOQP_DIR)/lib/libooqpbase.a $(OOQP_DIR)/lib/libooqpbound.a $(OOQP_DIR)/lib/libooqpdense.a $(OOQP_DIR)/lib/libooqpgendense.a $(OOQP_DIR)/lib/libooqpgondzio.a 

EXTRA_LDFLAG:=-L/opt/X11/lib/ -lX11   -L/usr/local/gfortran/lib/ -lgfortran -lquadmath -lm  -lblas -llapack

SRC:=$(wildcard src/bvdt/*.cpp) $(DLIB_DIR)/dlib/all/source.cpp
OBJ:=$(patsubst %.cpp,%.o,$(SRC)) 
CXX:=g++



bvdt: algo.pb.o $(OBJ)
	$(CXX)  -o bvdt $(OBJ) $(OOQP_LIB) $(EXTRA_LDFLAG) src/proto/algo.pb.o -L/usr/local/lib/  -lprotobuf 
algo.pb.o:algo.pb.cc
	$(CXX) -I. -I$(DLIB_DIR) -c src/proto/algo.pb.cc -o src/proto/$@
algo.pb.cc:src/proto/algo.proto
	protoc --cpp_out=. $<
.cpp.o:$<
	$(CXX)  -Iincludes/ -Isrc/proto/ -I$(DLIB_DIR) -I$(OOQP_INC)  -c $< -o $@
clean:
	rm -fr $(OBJ) bvdt src/proto/algo.pb.* *.o
