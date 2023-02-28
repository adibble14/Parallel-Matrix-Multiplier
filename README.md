# Parallel-Matrix-Multiplier
The purpose of this assignment is to implement a multi-threaded C program that uses a shared bounded 
buffer to coordinate the production of NxM matrices for consumption in matrix multiplication. For two 
matrices M1 and M2 to be multiplied, the number of columns of M1 must equal the number of rows of 
M2. The program will perform parallel work using multiple threads to: (1) produce NxM matrices and 
place them into a shared buffer, and (2) consume NxM matrices from the bounded buffer for pairing 
with another matrix for matrix multiplication having a valid number of rows and columns. Matrices 
consumed from the bounded buffer with an invalid number of elements for multiplication are discarded 
and the buffer is queried again to obtain a new candidate matrix for multiplication. 
