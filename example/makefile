#CC=clang++
CC=g++-11

TEST := \
.timer.o\
.prog.o

TEST2 := \
.timer.o\
.test.o

NETPATH := ..
JSONPATH := /Users/chaowang/Desktop/work/program/json
GRAPHVIZPATH := /usr/local/Cellar/graphviz/3.0.0
TATPATH := /Users/chaowang/Desktop/work/program/TAT
BOOSTPATH:= /Users/chaowang/Desktop/work/program/boost_1_75_0
MKLPATH:= /opt/intel/oneapi/mkl/2021.1.1
KAHYPAEPATH:= /Users/chaowang/Desktop/work/program/kahypar

LIBS :=   -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX12.3.sdk -mmacosx-version-min=12.2 

USE_LAPACK := -lblas -llapack -gfortran
USE_MKL :=  $(MKLPATH)/lib/libmkl_intel_lp64.a $(MKLPATH)/lib/libmkl_sequential.a $(MKLPATH)/lib/libmkl_core.a -lpthread -lm -ldl

LINKOPT := $(USE_MKL) -L $(GRAPHVIZPATH)/lib -lgvc -lcgraph -L /usr/local/lib -Wl,-rpath,/usr/local/lib -lkahypar -L $(BOOSTPATH)/stage/lib -Wl,-rpath,$(BOOSTPATH)/stage/lib -lboost_program_options
MAKEOPT := -g -std=c++17 -Ofast -march=native -I $(TATPATH)/include -DTAT_USE_EASY_CONVERSION -DTAT_USE_VALID_DEFAULT_TENSOR -DTAT_USE_SIMPLE_NAME -DTAT_USE_SINGULAR_MATRIX -DTAT_USE_SIMPLE_NOSYMMETRY -DTAT_USE_COPY_WITHOUT_WARNING -DNDEBUG -DTAT_USE_BOOST_PMR -I$(JSONPATH)/include -I$(TEBDPATH)/include  -I$(BOOSTPATH)  -I/usr/local/include  -I$(MKLPATH)/include -I $(GRAPHVIZPATH)/include -I$(NETPATH)/include -DNET_USE_LIB_KAHYPAR -DNET_GRAPH_VIZ -DNET_SHOW_FIG_USE_ITERM  -I $(KAHYPAEPATH)/ -DKAHYPAR_ENABLE_HEAVY_DATA_STRUCTURE_ASSERTIONS -DKAHYPAR_ENABLE_HEAVY_PREPROCESSING_ASSERTIONS -DKAHYPAR_ENABLE_HEAVY_COARSENING_ASSERTIONS -DKAHYPAR_ENABLE_HEAVY_INITIAL_PARTITIONING_ASSERTIONS -DKAHYPAR_ENABLE_HEAVY_REFINEMENT_ASSERTIONS $(LIBS)

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
