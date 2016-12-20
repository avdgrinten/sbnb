
#include <ilcplex/ilocplex.h>
#include "branching.h"

using namespace std;

int NO_FIXING_VALUE_FOUND = -1;

Branching::Branching(BranchingRule rule) {
  switch (rule) {
    case BranchingRule::FIRST_FRACTIONAL :
      NO_FIXING_VALUE_FOUND = NO_FRACTIONAL_FOUND;
      this->IndexOfNextVariableToFix = IndexOfFirstFractional;
      break;
  }
}

vector<BnBProblem *> *Branching::Branch(IloCplex &cplex, IloNumVarArray &variables) {

  vector<BnBProblem *> *sub_problems = new vector<BnBProblem *>;

  IloNumArray solutions(cplex.getEnv());
  cplex.getValues(solutions, variables);

  int index_of_problem_to_be_branched = this->IndexOfNextVariableToFix(solutions);
  if (index_of_problem_to_be_branched == NO_FIXING_VALUE_FOUND) {
    return sub_problems;
  }

  IloNumVar problem_to_be_branched = variables[index_of_problem_to_be_branched];
  IloNum value_to_be_branched = cplex.getValue(problem_to_be_branched);

  IloModel model = cplex.getModel();

  IloConstraint less_than_or_equal_to_constraint(problem_to_be_branched <= floor(value_to_be_branched));
  BnBProblem *less_than_or_equal_to_problem = new BnBProblem(&cplex, &variables, &less_than_or_equal_to_constraint);
  sub_problems->insert(sub_problems->begin(), less_than_or_equal_to_problem);

  IloConstraint greater_than_or_equal_to_constraint(ceil(value_to_be_branched) <= problem_to_be_branched);
  BnBProblem *greater_than_or_equal_to_problem = new BnBProblem(&cplex, &variables, &greater_than_or_equal_to_constraint);
  sub_problems->insert(sub_problems->begin(), greater_than_or_equal_to_problem);

  return sub_problems;

}
