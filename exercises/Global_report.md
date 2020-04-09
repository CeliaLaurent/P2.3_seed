#  The Finite Element Method Using deal.II

## Lecture 1 : Introduction

- *Deal II presentation*
- *options for installation*
- *IDE and Paraview VTK Viewer* :

Setting a project with the Eclipse IDE : [Eclipse/Setup_Eclipse_Project.md](Eclipse/Setup_Eclipse_Project.md)



## Lecture 2 : Getting started with deal II

*Gain familiarity with two core classes :*

- *The **Triangulation** class*
- *The **DoFHandler** class*

*Create and interrogate **meshes***
*Create and interrogate **sparsity patterns***



#### 				Lab01 using step 1 as a base : [lab01/report.md](lab01/report.md)

Source code:  [lab01/my-step1/step-1.cc](lab01/my-step1/step-1.cc)

- [x] Compiling and running with Eclipse (see exercises/Eclipse/Eclipse.md)

- [x] Write a helper function

- [x] Test with different meshes



## Lecture 3: A brief re-hash of the Finite Element Method

- *strong/weak forms*

- *degrees of freedom*

- *basis functions*

- *mapping of the reference cell*

- *quadrature*

  

#### Lab02 using step 2 as a base : [lab02/report.md](lab02/report.md) 

Source code : [lab02/my-step-2/step-2.cc](lab02/my-step-2/step-2.cc)

- [x] Compile and run step-2

- [x] Look at the generated sparsity patterns

- [x] Investigate influence of the polynomial degree and refinement on the sparsity pattern

- [ ] Check that this is true for the mesh in (b) (look for row_length(i) and output them for each row).

- [ ] Can you construct a 2d mesh (without hanging nodes) that has a row with more entries?

- [ ] Print all entries for row 42 for the original renumbered sparsity pattern.

- [ ] Renumber the DoFs using the `boost::king_ordering` algorithm. What does the sparsity pattern look like now?

  

## Lecture 4: Solving Poisson’s Equation

- *translation of weak form to assembly loops*
- *Gauss quadrature* 
- *Mapping*
- *The **FEValues** class*
- *Applying boundary conditions (Dirichlet, Neumann)*



#### 				Lab03 using step-3 as a base: [lab03/report.md](lab03/report.md)

Source code: [lab03/my-step-3/step-3.cc](lab03/my-step-3/step-3.cc)

- [x] Compile and run step-3
- [x] Making the code dimension-independent
- [x] Visualization using Paraview features:  `Warp by scalar`, `Clip`, `Contour`
- [x] Implement different boundary conditions on different faces of the domain and visualize the effects
- [x] Convergence of the mean



## Lecture 5: Error Estimation and Adaptive Mesh Refinement

*A priori error estimation computing **L2 and H1 norms**.*



#### Lab 04: step-3to5: **[lab04/report-step3to5.md](lab04/report-step3to5.md)**

Source code: [lab04/my-step-3to5/step-3to5.cc](lab04/my-step-3to5/step-3to5.cc)

- [x] Using exact right hand side and exact solution to compute the L2 and H1 norms with Gauss quadrature 
- [x] Visualization of the L2 and H1 norms per cell and their convergence in time
- [x] Parsed convergence error tables

**additional : **

- [x] Adding a timer using `TimerOutput` class through `#include <deal.II/base/timer.h>`



## Lecture 6: Adaptive Mesh Refinement

- *a posteriori error estimation using **Kelly error indicator***

- *Error-based refinement marking*

  

#### 			Lab 04: step-5to6 : **[lab04/report-step5to6.md](lab04/report-step5to6.md)**

- [x] Hanging nodes : [lab04/my-step-5to6/step-5to6-hanging-nodes-only.cc](lab04/my-step-5to6/step-5to6-hanging-nodes-only.cc)
- [x] A posteriori error estimation using the `KellyErrorEstimator` to predict the regions of the geometry where the solution approximation is inaccurate : [lab04/my-step-5to6/step-5to6-adding-Kelly-error.cc](lab04/my-step-5to6/step-5to6-adding-Kelly-error.cc)
- [x] Perform local cell marking and refinement using the cell-based estimated error: [lab04/my-step-5to6/step-5to6-final_version_with_error_based_refinement.cc](lab04/my-step-5to6/step-5to6-final_version_with_error_based_refinement.cc)



## Lecture 7: Shared Memory Parallelization

- [lab04/my-steps-to-parallel/step-a-implement-MeshWorker-objects.cc](lab04/my-steps-to-parallel/step-a-implement-MeshWorker-objects.cc)
- [lab04/my-steps-to-parallel/step-b-worker-copier-functions.cc](lab04/my-steps-to-parallel/step-b-worker-copier-functions.cc)
- [lab04/my-steps-to-parallel/step-c-WorkStream_run_parallel.cc](lab04/my-steps-to-parallel/step-c-WorkStream_run_parallel.cc)



## Lecture 8 and 9 : MPI Parallelization

- [x] Using the docker container of deal ii (see [Docker.md](Docker.md))

- [lab04/my-steps-to-parallel/step-d-MPI-and-PETSc.cc](lab04/my-steps-to-parallel/step-d-MPI-and-PETSc.cc)
- [lab04/my-steps-to-parallel/step-e-fix-locally_relevant_solution_plus_parallel_output_plus_system_compression.cc](lab04/my-steps-to-parallel/step-e-fix-locally_relevant_solution_plus_parallel_output_plus_system_compression.cc)



## Final Project starting from step-35 :

This additional work consists in the transformation of the step-35 code, leaving aside the shared-memory parallelization (workstream), and concentrating on the following objectives:

- [ ] replacement the deal-II linear algebra by the Trilinos library
- [ ] implementation of the MPI parallelisation 

