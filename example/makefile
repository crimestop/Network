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
TATPATH := /Users/chaowang/Desktop/work/program/TAT
BOOSTPATH:= /Users/chaowang/Desktop/work/program/boost_1_75_0
LINKOPT := -lblas -llapack -L $(GRAPHVIZPATH)/lib -lgvc -lcgraph -L /usr/local/lib -rpath /usr/local/lib -lkahypar -L $(BOOSTPATH)/stage/lib -rpath $(BOOSTPATH)/stage/lib -lboost_program_options
MAKEOPT := -g -std=c++17  -O3 -I $(TATPATH)/include -DTAT_USE_VALID_DEFAULT_TENSOR -DTAT_USE_SIMPLE_NAME -DTAT_USE_SINGULAR_MATRIX -DTAT_USE_SIMPLE_NOSYMMETRY -DTAT_USE_COPY_WITHOUT_WARNING -I $(GRAPHVIZPATH)/include -I$(NETPATH)/include -DNET_GRAPH_VIZ -DNET_SHOW_FIG_USE_ITERM -I$(JSONPATH)/include -I$(BOOSTPATH) -I/usr/local/include

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
