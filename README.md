# DGOracle
Computing A Near-Maximum Independent Set In Dynamic Graphs, ICDE 2019 

`DGOracle_DGOne` and `DGOracle_DGTwo` receive an initial good quality independent set as an oracle, which supervise the following updating procedure. If the initial IS is not provided, the above two algorithms will degenerate to `DGOne` and `DGTwo`.

```
#define INPUT "original_graph/"
#define RESULT "mis/"
#define INST "instruction/"
#define INSTNUM 1000  
```

`INPUT` indicates the path of graphs.

The first line contains two integers $n$ and $m$, the number of nodes and edges.

In the following $m$ lines, each line contains an undirected edge $(u, v)$. 

Tips:
1. The ids of nodes are in $[0, n-1]$.
2. $(u, v)$ and $(v, u)$ are regarded as the same edge.
3. Each edge appears in the input file merely once.
4. A self-loop edge like $(u, u)$ is not allowed. 
More graphs can be found on the standford platform (http://snap.stanford.edu/snap/index.html).
However, you may need to conduct some preprocessing on them first.

`RESULT` indicates the path of inital independent sets.

`INST` indicates the path of updating instructions.
In this project, each instruction file contains $10,000$ instructions.
The code uses `INSTNUM` to control the number of instructions tested, which is set as $1,000$ by default.

```
int main(int argc, char* argv[])
{
    if(argc>=2) strcpy(FILEID,argv[1]);
    else strcpy(FILEID,"1");
    if(argc>=3) get_percent(argv[2]);
    init();
    extend_sup();
    check_sup();
    bd = sup_bdone_init();
    extend_match();
    test_inst();
    check();
    output_update_ana();
    return 0;
}
```

`get_percent()` is a function used to get an optional parameter `percent`.
It decides the percentage of initial independent set used. This parameter varies from 1.0 to 0.5 in order to test what if the initial IS has a bad quality.

`init()` reads the graph and initial IS from input files.

`extend_sup()` is used to expend the initial IS if it is not maximal.

`check_sup()` checks whether the initial IS is really an independent set.

`bd = sup_bdone_init();` Initialize the index structure.

`extend_match();` Initialize the index structure for remaining nodes that are not matched in the above step.

`test_inst();` Conduct updating.

`check()` is used to check whether the final IS is really an independent set.

`output_update_ana();` Output the experimental results.
You may need to edit this function to get results you need, where test_misnum represents the size of the final independent set.
