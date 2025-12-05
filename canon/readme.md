## This is about Matrix Multiplication by using Parllel Computing - With MPI

### METHOD 1 : Transditional Matrix Multiplication
2 processor and Matrix A and Matrix B to both processor

### METHOD 2 : Rows - Column Matrix Multiplication 
- BoardCast & Scatter & Gather

### METHOD 3 : Rows - Column Matrix Multiplication 
- ScatterV & GatherV
- first learning about catesian (split processes in a grid form)

### METHOD 4 : Canon's Algorithm (Final Version)
- learning to use MPI_create_subarrays to send sub-matrix to different processes
- Cartesian create 2D grid processor
- final version of Canon's Algorithm for Matrix Multiplication using MPI
- Not the cleanest code but works fine

### METHOD 5 : Clean Canon's Algorithm Code
- Just Clean Code of Method 4

---
Run experiment in bash file 
```bash
bash experirment.sh %{file_name}
```