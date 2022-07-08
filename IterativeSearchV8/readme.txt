The program is implemented using Intel SSE Instruction Set. 
Please Install Intel Parallel Studio XE or Intel oneAPI for Intel C++ compiler. 
It runs under Release x64 using Intel C++ compiler.

To set the cipher or table type, we refer to the two macros named "CIPHER" and "TABLE" in "config.h".
The options for the cipher are PRESENT, GIFT_64, RECTANGLE, KONT_256, PUFFIN, TRIFLE, ICEBERG, EPCBC_48, EPCBC_96. 
The options for the table type are DDT and LAT2.
For example, if we want to search differential trails for GIFT-64, we set:
#define CIPHER GIFT_64
#define TABLE DDT

To set the experiment parameters, we refer to several variables in "main.cpp".
"max_an" is the maximum number of active S-boxes in each round.
"rd" is the number of rounds of a propagation.
"rd_ext,ws_est" is the two parameters controlling the searching scope of extended trails. 

In "main.cpp", the graph is generated according to "max_an".
If "FIND_BEST_ITERATIVE_TRAIL" is 1, then iterative trails are searched using Johnson's algorithm; 
If "FIND_BEST_ITERATIVE_CLUSTER" is 1, then iterative clusters are searched using a breadth-first computation; 
The extended trails are enumerated according to "rd_ext,ws_est". The number of rounds is "rd". 
If "FIND_BEST_TRAIL" is 1, then the best trail is found.
If "FIND_BEST_CLUSTER" is 1, then the best cluster is found.

The output files are in the "answer" folder. 
For example, if we set:
#define CIPHER GIFT_64,
#define TABLE DDT,
max_an=2,
rd=13 ,
rd_ext=3,ws_est=12,
then
"gift64_ddt_2_iterative_trail.txt" contains the best iterative trails,
"gift64_ddt_2_iterative_cluster.txt" contains the average weight growth of the best iterative cluster w.r.t. the number of rounds,
"gift64_ddt_2_iterative_structure.gv" is the file for graphviz to draw the directed graph of the iterative structure,
"gift64_ddt_2_(3,12.000000)_13_info.txt" contains the best cluster and the information during finding it.

