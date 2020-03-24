/* ---------------------------------------------------------------------
 *
 * Copyright (C) 1999 - 2016 by the deal.II authors
 *
 * This file is part of the deal.II library.
 *
 * The deal.II library is free software; you can use it, redistribute
 * it, and/or modify it under the terms of the GNU Lesser General
 * Public License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * The full text of the license can be found in the file LICENSE at
 * the top level of the deal.II distribution.
 *
 * ---------------------------------------------------------------------

 *
 * Authors: Wolfgang Bangerth, 1999,
 *          Guido Kanschat, 2011
 */



#include <deal.II/grid/tria.h>
#include <deal.II/dofs/dof_handler.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_refinement.h>

#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/dofs/dof_accessor.h>

#include <deal.II/fe/fe_q.h>

#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_values.h>
#include <deal.II/base/quadrature_lib.h>

#include <deal.II/base/function.h>
#include <deal.II/base/function_parser.h>

#include <deal.II/base/parsed_convergence_table.h>

#include <deal.II/base/timer.h>

#include <deal.II/numerics/vector_tools.h>
#include <deal.II/numerics/matrix_tools.h>

#include <deal.II/lac/affine_constraints.h>
#include <deal.II/lac/vector.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/sparse_matrix.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/precondition.h>

#include <deal.II/numerics/data_out.h>
#include <deal.II/numerics/error_estimator.h>

// now include shared-data specific librarires:
#include <deal.II/meshworker/copy_data.h>
#include <deal.II/meshworker/scratch_data.h>

#include <fstream>
#include <iostream>


using namespace dealii;


template<int dim>
class Step3
{
public:
  Step3 ();

  void run (const unsigned int n_cycles);


private:
  void make_grid ();
  void compute_error ();
  void estimate_error ();
  void mark_cells_for_refinement ();
  void refine_grid();
  void setup_system ();
  void assemble_system ();
  void solve ();
  void output_results (const unsigned int cycle) const;

  Triangulation<dim>     triangulation;
  // scalar Lagrange finite element that yields 
  // the finite element space of continuous, piecewise
  // polynomials of degree p in each coordinate direction:
  FE_Q<dim>              fe;
  DoFHandler<dim>        dof_handler;
  // constraint declaration inserted here because it
  // associates constraints of the trianguation
  AffineConstraints<double> constraints;

  SparsityPattern      sparsity_pattern;
  SparseMatrix<double> system_matrix;

  Vector<double>       solution;
  Vector<double>       system_rhs;

  // store DoFs where the grid needs refinement: 
  Vector<float> error_estimator; 

  FunctionParser<dim> exact_solution;
  FunctionParser<dim> exact_rhs_function;

  ParsedConvergenceTable error_table;

  Vector<double> L2_error_per_cell;
  Vector<double> H1_error_per_cell;

  // timer class:
  TimerOutput timer;
  // if this timer needs to be called in a cst function,
  // then add "mutable" keyword before declaration. in our case,
  // "TimerOutput::Scope timer_section(timer, "..");" instructions
  // are called outside of the cst functions, so, no need of "mutable"
};


template<int dim>
Step3<dim>::Step3 ()
  :
  fe (1),
  dof_handler (triangulation),
  exact_solution("exp(x)*exp(y)"),
  exact_rhs_function("-2*exp(x)*exp(y)"), // laplacian of the exact solution
  //error_table(string, list_of_error_norms) :
  error_table({"u"}, { {VectorTools::H1_norm, VectorTools::L2_norm} } ),
  // initialize timer with the string of the output we want,
  // then the output frequency: in our case, only a final summary,
  // and which times we want to see: here wall and cpu time
  timer(std::cout, TimerOutput::summary,
                   TimerOutput::cpu_and_wall_times)
  {}


template<int dim>
void Step3<dim>::make_grid ()
{
  GridGenerator::hyper_cube (triangulation, -1, 1);
  triangulation.refine_global (3);
}


template<int dim>
void Step3<dim>::estimate_error ()
{
  // reinit the error_estimator from previous fillings: 
  error_estimator.reinit(triangulation.n_active_cells());
  // Generate formulas with dim-1 quadrature points
  // of the same order as the one we used for assembling the system:
  QGauss<dim-1> face_quadrature(fe.degree+1);
  KellyErrorEstimator<dim>::estimate(
            dof_handler,
            face_quadrature,
            {}, //empty map of boundary ids
            solution, // field whose error must be stored on the error estimator
            error_estimator);
}

template<int dim>
void Step3<dim>::mark_cells_for_refinement ()
{
  // refinement according to the Kelly error estimation can be done using
  // - either a fixed number of cells
  // - either using a fixed fraction of the nuber of cells
  GridRefinement::refine_and_coarsen_fixed_number(
               triangulation,
               error_estimator,
           // top fraction of cells for refinement:
               0.33,
          // bottom fraction of cells below which cells are marked for coarsening:
               0.0); 
}


template<int dim>
void Step3<dim>::refine_grid ()
{
  // either we refine globally :
    //triangulation.refine_global (1);

  // or we refine locally to observe the effects of the constraints:
    //for (auto cell : triangulation.active_cell_iterators())
    //  if( cell->center()[1] > 0 )
    //    cell->set_refine_flag();
    //triangulation.execute_coarsening_and_refinement();

  // or we perform refinement (and coarsening if bottom fraction>0) 
  // according to the marked cells :
  triangulation.execute_coarsening_and_refinement ();

}




template<int dim>
void Step3<dim>::setup_system ()
{

  std::cout << "Number of active cells: "
	            << triangulation.n_active_cells()
	            << std::endl;
  dof_handler.distribute_dofs (fe);
  std::cout << "Number of degrees of freedom: "
            << dof_handler.n_dofs()
            << std::endl;

  // Clear all entries of the constraint matrix, ie reset the flags  
  // determining whether new entries are accepted or not: 
  constraints.clear();
  // Compute the constraints resulting from the presence of hanging nodes:
  DoFTools::make_hanging_node_constraints(dof_handler, constraints);

  VectorTools::interpolate_boundary_values(dof_handler,
                                           0,
                                           exact_solution,
      // apply eventual constraints instead of exact_solution where needed:
                                           constraints);
  // close here the constraint so that after that close instruction
  // the things will be organized in memory in an optimized way 
  constraints.close();

  DynamicSparsityPattern dsp(dof_handler.n_dofs());
  // Compute which entries of a matrix built on the given dof_handler may 
  // possibly be nonzero, and create a sparsity pattern object that represents
  // these nonzero locations. 
  DoFTools::make_sparsity_pattern(dof_handler, dsp, constraints);
  sparsity_pattern.copy_from(dsp);

  system_matrix.reinit (sparsity_pattern);

  solution.reinit (dof_handler.n_dofs());
  system_rhs.reinit (dof_handler.n_dofs());

  // (re) define the vectors storing the norms
  // according to the new number of active cells:
  L2_error_per_cell.reinit(triangulation.n_active_cells());
  H1_error_per_cell.reinit(triangulation.n_active_cells());
}



template<int dim>
void Step3<dim>::assemble_system ()
{
  QGauss<dim>  quadrature_formula(2);
  // we replace "FEValues<dim> fe_values (fe, quadrature_formula,update...)
  // with a "ScratchData object" that is copiable, and consequently usable as 
  // copied-argument to the functions that will be called in parallel.
  MeshWorker::ScratchData<dim> scratch (fe, // const FiniteElement< dim, spacedim > & 	fe
                                        quadrature_formula, //	const Quadrature< dim > & quad
                         update_quadrature_points | update_values // const UpdateFlags & flags
                         | update_gradients | update_JxW_values);

  const unsigned int   dofs_per_cell = fe.dofs_per_cell;
  const unsigned int   n_q_points    = quadrature_formula.size();

  // now replace the next 2 lines: 
  //FullMatrix<double>   cell_matrix (dofs_per_cell, dofs_per_cell);
  //Vector<double>       cell_rhs (dofs_per_cell);
  // with the CopyData object of template :
  // <int n_matrices = 1, int n_vectors = n_matrices, int n_dof_indices = n_matrices>
  MeshWorker::CopyData<1,1,1> copy_data (dofs_per_cell);
  // this object stores n=dofs-per-cell matrices and n=dofs-per-cells vectors, 
  // accessibles through "copy_data.matrices/vectors[n]"
  // next line not anymore needed as local_dof_indices exists in copy_data object
  //std::vector<types::global_dof_index> local_dof_indices (dofs_per_cell);

  // declare rhs_values to store the right hand side values at the quadrature points:
  std::vector<double>  rhs_values (n_q_points);


// next 3 lines can be expressed in modern c++ as:
// for (auto cell: dof_handler.active_cell_iterator())
  typename DoFHandler<dim>::active_cell_iterator cell = dof_handler.begin_active();
  typename DoFHandler<dim>::active_cell_iterator endc = dof_handler.end();
  for (; cell!=endc; ++cell)
    {
      auto &fe_values=scratch.reinit(cell); // replaces : fe_values.reinit (cell);

      copy_data.matrices[0]=0; // 0'th matrix = 0 : replaces :  cell_matrix = 0;
      copy_data.vectors[0]=0; // 0'th vector = 0 : replaces :  cell_rhs = 0;
      
      // query fe_value to get the mapped quadrature points
      const auto &q_points = scratch.get_quadrature_points(); //replaces: fe_values.get_quadrature_points();

      // next line is not needed anymore
      // exact_rhs_function.value_list(q_points,rhs_values);

      for (unsigned int q_index=0; q_index<n_q_points; ++q_index)
        {
          for (unsigned int i=0; i<dofs_per_cell; ++i)
            for (unsigned int j=0; j<dofs_per_cell; ++j)
              // increment ex cell_matrix:
              copy_data.matrices[0](i,j) += (fe_values.shape_grad (i, q_index) *
                                   fe_values.shape_grad (j, q_index) *
                                   fe_values.JxW (q_index));

          for (unsigned int i=0; i<dofs_per_cell; ++i)
              // increment ex cell_rhs:
              copy_data.vectors[0](i) += (fe_values.shape_value (i, q_index) *
                            exact_rhs_function.value(q_points[q_index]) * //rhs_values[q_index] *
                            fe_values.JxW (q_index));
        }
      //cell->get_dof_indices (local_dof_indices);
      cell->get_dof_indices (copy_data.local_dof_indices[0]);

      constraints.distribute_local_to_global( // void
                          copy_data.matrices[0], // cell_matrix,
                          copy_data.vectors[0],          // cell_rhs,
                          copy_data.local_dof_indices[0], // const std::vector< size_type > &local_dof_indices,
                          system_matrix,     // MatrixType &global_matrix,
                          system_rhs         // VectorType &global_vector,
			  );                 // bool use_inhomogeneities_for_rhs=false)
      // above distribute_local_to_global substitutes the one below used with global refinement:      
//    for (unsigned int i=0; i<dofs_per_cell; ++i)
//      for (unsigned int j=0; j<dofs_per_cell; ++j)
//        system_matrix.add (local_dof_indices[i],
//                           local_dof_indices[j],
//                           cell_matrix(i,j));

//    for (unsigned int i=0; i<dofs_per_cell; ++i)
//      system_rhs(local_dof_indices[i]) += cell_rhs(i);

  }


//std::map<types::global_dof_index,double> boundary_values;
//// Exact solution used for boundary conditions
//VectorTools::interpolate_boundary_values (dof_handler,
//                                          0,
//                                          exact_solution,
//                                          boundary_values);

//MatrixTools::apply_boundary_values (boundary_values,
//                                    system_matrix,
//                                    solution,
//                                    system_rhs);
}


template<int dim>
void Step3<dim>::solve ()
{
  SolverControl           solver_control (1000, 1e-12);
  SolverCG<>              solver (solver_control);

  solver.solve (system_matrix, solution, system_rhs,
                PreconditionIdentity());

  // next line sets  solution on all constrained degrees of freedom 
  // to be the average of the solution at the 2 DoFs
  // between which the constrained DoF was inserted :
  constraints.distribute(solution);
  // it is indispensable if the constraints are not considered in the
  // sparsity pattern, otherwise it can be commented out.
}


template<int dim>
void Step3<dim>::compute_error ()
{
  // quadrature function type with degree higher than what we used
  // for the assembly, in order to be more precise:
  QGauss<dim> error_quadrature(2*fe.degree+1);
  VectorTools::integrate_difference(
		  dof_handler,
		  solution,
		  exact_solution,
		  L2_error_per_cell,
		  error_quadrature, //const Quadrature<dim> & q,
		  VectorTools::L2_norm //const NormType & norm,
		  //const Function<dim,double> *weight = nullptr,
		  //const double exponent = 2.0
		  );

  VectorTools::integrate_difference(
 		  dof_handler,
 		  solution,
 		  exact_solution,
 		  H1_error_per_cell,
 		  error_quadrature, //const Quadrature<dim> & q,
 		  VectorTools::H1_norm );

  // l2_norm is the sum of the squared of the L2_error_per_cell:
  std::cout << "L2 norm of error : " << L2_error_per_cell.l2_norm() << std::endl;
  // l2_norm is the sum of the squared of the H1_error_per_cell:
  std::cout << "H1 norm of error : " << H1_error_per_cell.l2_norm() << std::endl;

}


template<int dim>
void Step3<dim>::output_results (const unsigned int cycle) const
{
  DataOut<dim> data_out;
  data_out.attach_dof_handler (dof_handler);
  data_out.add_data_vector (solution, "solution");
  data_out.add_data_vector (L2_error_per_cell, "L2_error");
  data_out.add_data_vector (H1_error_per_cell, "H1_error");
  data_out.add_data_vector (error_estimator, "error_estimator");
  data_out.build_patches ();

  std::ofstream output ("solution_"+std::to_string(cycle)+".vtu");
  data_out.write_vtu(output);
  std::cout << "Mean value: "
            << VectorTools::compute_mean_value (dof_handler,
                                                QGauss<dim>(fe.degree + 1),
                                                solution,
                                                0)
            << std::endl;
}


template<int dim>
void Step3<dim>::run (const unsigned int n_cycles)
{
  {TimerOutput::Scope timer_section(timer, "Make grid");
  make_grid ();
  }
  for(unsigned int cycle=0;cycle<n_cycles;++cycle){
	std::cout<<"--------------- cycle "<<cycle<<" --------------"<<std::endl;

    {TimerOutput::Scope timer_section(timer, "setup_system");
	setup_system ();
    }
    {TimerOutput::Scope timer_section(timer, "assemble_system");
    assemble_system ();
    }
    {TimerOutput::Scope timer_section(timer, "solve");
    solve ();
    }
    // first method to compute errors:
    {TimerOutput::Scope timer_section(timer, "compute_error");
    compute_error();
    }
    // second method:
    {TimerOutput::Scope timer_section(timer, "compute_error_from_exact");
    error_table.error_from_exact(dof_handler,solution,exact_solution);
    }
    // a posteriori method to estimate errors using Kelly estimator :
    {TimerOutput::Scope timer_section(timer, "estimate_error");
    estimate_error();
    }
    {TimerOutput::Scope timer_section(timer, "mark_cells_for_refinement");
    mark_cells_for_refinement();
    }
    {TimerOutput::Scope timer_section(timer, "output_results");
    output_results (cycle);
    }
    {TimerOutput::Scope timer_section(timer, "refine_grid");
    refine_grid();
    }
  }
  TimerOutput::Scope timer_section(timer, "output_table");
  error_table.output_table(std::cout);
}



int main ()
{
  deallog.depth_console (2);

  Step3<2> laplace_problem;
  laplace_problem.run (6);


  return 0;
}
