#  The Finite Element Method Using deal.II

## Lecture 1 : Introduction

- Deal II presentation
- options for installation
- IDE and Paraview VTK Viewer

## Lecture 2 : Getting started with deal II

Gain familiarity with two core classes :

- **Triangulation** class
- **DoFHandler** class

Create and interrogate **meshes**
Create and interrogate **sparsity patterns**



#### 				Lab01: using step 1 as a base

- [x] Compiling and running with Eclipse (see exercises/Eclipse/Eclipse.md)

- [x] Write a helper function

- [x] Test with different meshes



## Lecture 3: A brief re-hash of the Finite Element Method

- strong/weak forms

- degrees of freedom

- basis functions

- mapping of the reference cell

- quadrature

  

  #### Lab02: using step 2 as a base

- [x] Compile and run step-2

- [x] Look at the generated sparsity patterns

- [x] Investigate influence of the polynomial degree and refinement on the sparsity pattern

- [ ] Check that this is true for the mesh in (b) (look for row_length(i) and output them for each row).

- [ ] Can you construct a 2d mesh (without hanging nodes) that has a row with more entries?

- [ ] Print all entries for row 42 for the original renumbered sparsity pattern.

- [ ] Renumber the DoFs using the `boost::king_ordering` algorithm. What does the sparsity pattern look like now?

  

## Lecture 4: Solving Poisson’s Equation

- translation of weak form to assembly loops
- Gauss quadrature 
- Mapping
- **FEValues** class
- Applying boundary conditions (Dirichlet, Neumann)



#### 				Lab03 using step-3 as a base:

- [x] Compile and run step-3
- [x] Making the code dimension-independent
- [x] Visualization using Paraview features:  `Warp by scalar`, `Clip`, `Contour`
- [x] Implement different boundary conditions on different faces of the domain and visualize the effects
- [x] Convergence of the mean



## Lecture 5: Error Estimation and Adaptive Mesh Refinement

- a priori error estimation computing **L2 and H1 norms**

  

  #### Lab 04: step-3to5

- [x] Using exact right hand side and exact solution to compute the L2 and H1 norms with Gauss quadrature 

- [x] Visualization of the L2 and H1 norms per cell and their convergence in time

- [x] Parsed convergence error tables

**additional : **

- [x] Adding a timer using `TimerOutput` class through `#include <deal.II/base/timer.h>`



## Lecture 6: Adaptive Mesh Refinement

- Hanging nodes

- a posteriori error estimation using **Kelly error indicator**

- Error-based refinement marking

  
  
  #### 			Lab 04: step-5to6
  
- [ ] Use the `KellyErrorEstimator` to predict the regions of the geometry where the solution approximation is inaccurate
- [ ] Perform local cell marking and refinement using the cell-based estimated error.





## Lecture 7: Shared Memory Parallelization





## Lecture 8 : MPI Parallelization, part I

- [ ] Using the docker container of deal ii (see docker.md)



## Lecture 9 : MPI Parallelization, part II