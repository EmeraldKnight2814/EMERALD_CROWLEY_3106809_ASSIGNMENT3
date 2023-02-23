# DANIEL_CROWLEY_3106809_ASSIGNMENT3


This program will multiply two matrices together using the stripe method of matrix multiplication. 
You will have three matrices A,B and C that represent the operation A x B = C. 
You will be required to determine how many nodes are in your compute group and create even stripes for A to be divided amongst the nodes. 
All nodes will receive a copy of B. 
Each node will do a matrix multiplication of its stripe of A with the matrix B to generate its stripe of C. 
Finally all nodes will use the gather operation to send back the stripes to the coordinator node wherein it will be reassembled into a single matrix.


PLEASE NOTE: This program works with some key assumptions
Assumption 1: All matricies put into this program are Square (N x N)
Assumption 2: N is evenly divisible by all nodes

matA.dat:

5, 2, 0, 4, 3, 1, 5, 9,

0, 5, 5, 9, 1, 4, 6, 7,

0, 0, 7, 1, 4, 4, 8, 0,

3, 9, 4, 7, 0, 3, 5, 3,

6, 1, 3, 7, 6, 9, 6, 6,

5, 9, 5, 2, 4, 6, 0, 7,

6, 4, 9, 9, 0, 7, 6, 1,

5, 5, 3, 2, 4, 3, 5, 6


matB.dat:

3, 6, 5, 1, 6, 3, 8, 9,

2, 5, 6, 9, 0, 1, 2, 3,

5, 8, 1, 9, 5, 9, 2, 7,

3, 8, 7, 9, 0, 9, 0, 6,

1, 8, 8, 3, 6, 8, 7, 3,

9, 4, 5, 8, 8, 3, 8, 0,

7, 0, 3, 8, 7, 3, 7, 4,

9, 2, 1, 0, 2, 8, 9, 8


multiplication result SHOULD BE:

159	118	118	116	109	167	189	176

204	175	151	254	119	225	164	187

134	112	90	180	147	140	130	99

157	169	155	247	103	165	136	168

239	217	205	243	213	250	266	210

185	201	167	209	141	193	207	187

212	230	180	308	181	231	181	215

166	151	140	171	140	169	197	173
