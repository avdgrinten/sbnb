
#include <vector>
#include "branch_and_bound_class.h"
#include "branching/branching.h"
#include "node_selection/bnb_node_selection_interface.h"
#include "node_selection/depth_first_traversal_class.h"
#include "core/optimization_problem_class.h"

BranchAndBound::BranchAndBound(IloModel* model, IloNumVarArray* variables)
  : model_(model),
    variables_(variables),
    best_solution_(IloNumArray(model_->getEnv())),
    cplex_(IloCplex(*model)) {

  cplex_.setOut(model_->getEnv().getNullStream());
}

/*
 * @ file
 *
 * 0.generate root problem and add to node selection
 * while (NodeSelection has next node) {
 *    1. get next node from NodeSelection
 *    2. solve OptimizationProblem of Node and find optimal solution z for variables x*
 *    3. if infeasible or unbounded, process next node (skip loop / break)
 *    4. if x* integer set global_lower_bound LB to max{LB, z}, process next node 
 *    5. if z <= LB, process next node
 *    6.  - get (next) variable to fixate from BranchingRule
 *        - generate two (sub-) OptimizationProblems with Constraints from BranchingRule
 *        - add new Problems to NodeSelection
 * }
 * TODO: Where to remove contraints from model
 */
void BranchAndBound::optimize() {

  OptimizationProblem problem(&cplex_, variables_);
  Node<OptimizationProblem*> root(&problem);

  DepthFirstTraversal<OptimizationProblem*> node_selection;
  node_selection.SetNextNode(&root);

  while (node_selection.HasNextNode()) {

    Node<OptimizationProblem*> actual_node = *node_selection.next_node();
    OptimizationProblem actual_problem = *actual_node.content;

    actual_problem.Solve();

    IloNumArray actual_solution = actual_problem.GetSolution();

    // find optimal solution z for variables x*/
    // TODO: find objective value/solution 'z'

    if (actual_problem.IsInfeasible() || actual_problem.IsUnbounded()) {
      continue;
    }

    Branching branching(FIRST_FRACTIONAL);
    std::vector<OptimizationProblem*> branches = *branching.Branch(cplex_, actual_solution, *variables_);

    if (branches.size() == 0) {
      // set global_lower_bound LB to max{LB, z} and process next node;
      continue;
    }
    // if z <= LB and process next node

    /*  - get (next) variable to fixate from BranchingRule
     *  - generate two (sub-) OptimizationProblems with Constraints from BranchingRule
     *  - add new Problems to NodeSelection
     */

    for (auto branch : branches) {
      Node<OptimizationProblem*> *new_problem = new Node<OptimizationProblem*>(branch);
      //node_selection.SetNextNode(new_problem);
    }
  }
}

const IloNumArray &BranchAndBound::GetBestSolution() const {
  return best_solution_;
}
