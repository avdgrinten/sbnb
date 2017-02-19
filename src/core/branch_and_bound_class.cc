#include <vector>
#include "branch_and_bound_class.h"

BranchAndBound::BranchAndBound(IloModel* model, IloNumVarArray* variables, Branching* branching, NodeSelection* node_selection)
  : model_(model),
    variables_(variables),
    best_solution_(IloNumArray(model_->getEnv())),
    cplex_(IloCplex(*model)),
    branching_(branching),
    node_selection_(node_selection){

  if (IsMaximizationProblem()) {
    global_primal_bound_ = -IloInfinity;
  } else {
    global_primal_bound_ = IloInfinity;
  }

  // relaxate variables
  model_->add(IloConversion(cplex_.getEnv(), *variables_, ILOFLOAT));
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
 *    6.  - get (next) variable to fixate from Branching_Rule
 *        - generate two (sub-) OptimizationProblems with Constraints from Branching_Rule
 *        - add new Problems to NodeSelection
 * }
 */
void BranchAndBound::optimize() {
  OptimizationProblem* root_problem = new OptimizationProblem(&cplex_, variables_);
  Node* current_node = new Node(root_problem);

  std::vector<IloConstraint*> branched_constraints;
  IloNumArray current_solution_variables;

  node_selection_->AddNode(current_node);


  while (node_selection_->HasNextNode()) {
    std::cout << "[NODE " << ++number_nodes_ <<"]" << std::endl;
    Node* previous_node = current_node;
    current_node = node_selection_->NextNode();
    InstallFixings(previous_node, current_node);

    OptimizationProblem* current_problem = current_node->problem;
    current_problem->Solve();
    std::cout << "----- ";
    if (current_problem->IsInfeasible() || current_problem->IsUnbounded()) {
      std::cout << "solution is unfeasible or unbounded: fathom node" << std::endl;
      current_node->Fathom();
      continue;
    }

    double objective_value = current_problem->GetObjectiveValue();
    if (!IsBetterObjectiveValue(objective_value)) {
      std::cout << "solution has worse objective value: fathom node" << std::endl;
      current_node->Fathom();
      continue;
    }

    current_solution_variables = current_problem->GetSolution();
    branched_constraints = branching_->Branch(current_solution_variables, *variables_);
    std::cout <<  "better objective value [" << objective_value << "] : ";
    bool is_integer_solution = branched_constraints.size() == 0;
    if (is_integer_solution) {
      std::cout <<  "integer solution - fathom node" << std::endl;
      global_primal_bound_ = objective_value;
      best_solution_ = current_solution_variables;
      current_node->Fathom();
    } else {
      std::cout <<  "non-integer solution - branch" << std::endl;
      GenerateSubproblems(branched_constraints, current_node, *node_selection_);
    }
  }
}

void BranchAndBound::InstallFixings(const Node* previous_node, const Node* current_node) const {

  //Find common ancestor and add new fixings to model
  while(!current_node->problem->HasFixingInstalled()) {
    current_node->problem->InstallFixing();
    current_node = current_node->GetParent();
  }

 //Remove old fixings from model
  while(previous_node != current_node) {
    previous_node->problem->RemoveFixing();
    previous_node = previous_node->GetParent();
  }
}

void BranchAndBound::GenerateSubproblems(std::vector<IloConstraint*> &branched_constraints,
                                         Node* parent_node,
                                         NodeSelection &node_selection_) {

  OptimizationProblem* sub_problem = new OptimizationProblem(&cplex_, variables_, branched_constraints[0]);
  Node* sub_problem_node_1 = new Node(sub_problem, parent_node);

  sub_problem = new OptimizationProblem(&cplex_, variables_, branched_constraints[1]);
  Node* sub_problem_node_2 = new Node(sub_problem, parent_node);

  parent_node->SetFirstChild(sub_problem_node_1);
  sub_problem_node_1->SetNextSibling(sub_problem_node_2);

  node_selection_.AddNode(sub_problem_node_1);
  node_selection_.AddNode(sub_problem_node_2);
}

const IloNumArray &BranchAndBound::GetBestSolution() const {
  return best_solution_;
}

const double BranchAndBound::GetGlobalPrimalBound() const {
  return global_primal_bound_;
}

const bool BranchAndBound::IsBetterObjectiveValue(double objective_value) const {
  if (IsMaximizationProblem()) {
    return (objective_value > global_primal_bound_);
  } else {
    return (objective_value < global_primal_bound_);
  }
}

const bool BranchAndBound::IsMaximizationProblem() const {
  return cplex_.getObjective().getSense() == IloObjective::Sense::Maximize;
}
