#include <iostream>
#include <ctime>
#include <string>
#include "core/branch_and_bound_class.h"
#include "node_selection/breadth_first_traversal_class.h"

void SolveLP(int traversal_flag, int selection_flag, char* file_path);
void ShowUsage();

int main(int argc, char* argv[]) {
  int selection_flag = 0; // Depth First Traversal = 0, Breadth First Traversal = 1
  int branching_flag = 0; // First Fractional = 0
  char *file_path = NULL;
  int option_character;
  opterr = 0;

  while ((option_character = getopt (argc, argv, "dbfhP:")) != -1)
    switch (option_character) {
      case 'd':
        selection_flag = 0;
        break;
      case 'b':
        selection_flag = 1;
        break;
      case 'f':
        branching_flag = 0;
        break;
      case 'h':
        ShowUsage();
        return 0;
      case 'P':
        file_path = optarg;
        break;
      case '?':
        if (optopt == 'P')
          fprintf (stderr, "Option -%c requires an argument. See \"sbnb -h\" for help.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'. See \"sbnb -h\" for help.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'. See \"sbnb -h\" for help.\n", optopt);
        return 1;
      default:
        abort();
      }


  if (file_path)
    SolveLP(selection_flag, branching_flag, file_path);
  else
    std:cerr << "No file path specified. See \"sbnb -h for help.\"" << std::endl;

  return 0;
}

void ShowUsage() {
    std::cout << "USAGE: sbnb [-d|-b] [-f] -P file_path" << std::endl;
    std::cout << "\t -d: Set node selection to 'Depth first traversal' (default)" << std::endl;
    std::cout << "\t -b: Set node selection to 'Breadth first traversal'" << std::endl;
    std::cout << "\t -f: Set branching rule to 'First fractional' (default)" << std::endl;
    std::cout << "\t -P file_path: Location of linear problem file (.lp/.mps file formats). REQUIRED " << std::endl;
    std::cout << "EXAMPLES:" << std::endl << "\t sbnb -bfP test/testmodels/sample3.mps" << std::endl;
    std::cout << "\t sbnb -P test/testmodels/sample10.mps" << std::endl;
}

void SolveLP(int selection_flag, int branching_flag, char* file_path) {
  IloEnv env;
  IloModel model(env);
  IloNumVarArray vars(env);
  IloCplex cplex(model);
  IloRangeArray constraints(env);
  IloObjective objective;

  NodeSelection* node_selection;
  Branching* branching_rule;

  switch (selection_flag) {
    case 0:
      node_selection = new DepthFirstTraversal();
      break;
    case 1:
      node_selection = new BreadthFirstTraversal();
      break;
    default:
      node_selection = new DepthFirstTraversal();
  }

  switch (branching_flag) {
    case 0:
      branching_rule = new FirstFractional(cplex.getParam(IloCplex::EpRHS));
      break;
    default:
      branching_rule = new FirstFractional(cplex.getParam(IloCplex::EpRHS));
  }

  try {
    cplex.setOut(env.getNullStream());
    cplex.importModel(model, file_path, objective, vars, constraints);
    BranchAndBound bnb(&model, &vars, branching_rule, node_selection);

    const float begin = clock();
    bnb.optimize();
    const float duration = (clock() - begin)/CLOCKS_PER_SEC;

    std::cout << "------- Branch And Bound Summary -------" << endl;
    std::cout << "Objective Value: " << bnb.GetGlobalPrimalBound() << endl;
    std::cout << "Variable Values: " << bnb.GetBestSolution() << endl;
    std::cout << "Elapsed Time: " << duration << " sec" << endl;
  }
  catch (IloException& e) {
    cerr << "Error: " << e << endl;
  }
  env.end();
}
