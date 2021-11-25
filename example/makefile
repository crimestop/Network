CC=clang++
#CC=g++-10

TEST := \
.timer.o\
.prog.o

TEST2 := \
.timer.o\
.test.o

NETPATH := ..
JSONPATH := /Users/chaowang/Desktop/work/program/json
GRAPHVIZPATH := /usr/local/Cellar/graphviz/2.44.1
TATPATH := ../third_party/TAT
BOOSTPATH:= /Users/chaowang/Desktop/work/program/boost_1_75_0
MKLPATH:= /opt/intel/mkl
KAHYPAEPATH:= ../third_party/kahypar/include

USE_LAPACK := -lblas -llapack
USE_MKL :=  $(MKLPATH)/lib/intel64/libmkl_intel_lp64.so $(MKLPATH)/lib/intel64/libmkl_sequential.so $(MKLPATH)/lib/intel64/libmkl_core.so -lpthread -lm -ldl

LINKOPT := $(USE_MKL) -L $(GRAPHVIZPATH)/lib -lgvc -lcgraph -L ../third_party/kahypar/build/lib/ -rpath ../third_party/kahypar/build/lib/ -lkahypar -L $(BOOSTPATH)/stage/lib -rpath $(BOOSTPATH)/stage/lib -lboost_program_options
MAKEOPT := -g -std=c++17 -Ofast -march=native -I $(TATPATH)/include -DTAT_USE_TIMER -DTAT_USE_EASY_CONVERSION -DTAT_USE_VALID_DEFAULT_TENSOR -DTAT_USE_SIMPLE_NAME -DTAT_USE_SINGULAR_MATRIX -DTAT_USE_SIMPLE_NOSYMMETRY -DTAT_USE_COPY_WITHOUT_WARNING -DNDEBUG -DTAT_USE_BOOST_PMR -I$(JSONPATH)/include -I$(TEBDPATH)/include  -I$(BOOSTPATH)  -I/usr/local/include  -I$(MKLPATH)/include -I $(GRAPHVIZPATH)/include -I$(NETPATH)/include -DNET_USE_LIB_KAHYPAR -DNET_GRAPH_VIZ -DNET_SHOW_FIG_USE_GWENVIEW -I $(KAHYPAEPATH)/ -DKAHYPAR_ENABLE_HEAVY_DATA_STRUCTURE_ASSERTIONS -DKAHYPAR_ENABLE_HEAVY_PREPROCESSING_ASSERTIONS -DKAHYPAR_ENABLE_HEAVY_COARSENING_ASSERTIONS -DKAHYPAR_ENABLE_HEAVY_INITIAL_PARTITIONING_ASSERTIONS -DKAHYPAR_ENABLE_HEAVY_REFINEMENT_ASSERTIONS

all: run.out test.out
run: run.out
test2: test.out

test.out:$(TEST2)
	$(CC) $(FFLAGS) -o $@ $^ $(LINKOPT) 
run.out:$(TEST)
	$(CC) $(FFLAGS) -o $@ $^ $(LINKOPT) 
.%.o :	%.cpp
	$(CC) $(FFLAGS) -c -o $@ -c $(<) $(MAKEOPT)
clean:
	rm .*.o *.mod *.out 
.PHONY:	all clean run.out test.out
