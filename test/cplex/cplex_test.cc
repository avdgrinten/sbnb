#include <gtest/gtest.h>
#include <ilcplex/ilocplex.h>
#include <iostream>
#include "example_model.h"

using namespace std;

/** \brief tests a ILOModel
 *
 *  this test class tests a IloModel which is given to
 *  createSampleModel to add the optimizeproblem and constraints
 */

TEST(Cplex, model_test){

  IloEnv env;
  IloModel model(env);
  IloNumVarArray variables(env);

  variables.add(IloNumVar(env,0.0,IloInfinity,ILOINT));
  variables.add(IloNumVar(env,0.0,IloInfinity,ILOINT));

  createSampleModel(&model, &variables);
  IloCplex cplex(model);

  try{
    cplex.solve();
  } catch(IloException& exception){
    cerr << exception << endl;
  }

  EXPECT_EQ(2, cplex.getValue(variables[0]));
  EXPECT_EQ(2, cplex.getValue(variables[1]));

  env.end();
}


