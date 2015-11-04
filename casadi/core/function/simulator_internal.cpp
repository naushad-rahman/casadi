/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010-2014 Joel Andersson, Joris Gillis, Moritz Diehl,
 *                            K.U. Leuven. All rights reserved.
 *    Copyright (C) 2011-2014 Greg Horn
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "simulator_internal.hpp"
#include "integrator.hpp"
#include "../std_vector_tools.hpp"

using namespace std;
namespace casadi {


  SimulatorInternal::SimulatorInternal(const std::string& name, const Function& integrator)
    : FunctionInternal(name), integrator_(integrator) {

    addOption("monitor",      OT_STRINGVECTOR, GenericType(),  "", "initial|step", true);

    grid_ = dynamic_cast<Integrator*>(integrator_.get())->grid_;
  }

  SimulatorInternal::~SimulatorInternal() {
  }

  void SimulatorInternal::init() {
    ischeme_ = Function::integrator_in();
    oscheme_ = Function::integrator_out();

    // Allocate inputs
    ibuf_.resize(get_n_in());
    for (int i=0; i<ibuf_.size(); ++i) {
      input(i) = DMatrix(get_sparsity_in(i));
    }

    // Allocate outputs
    obuf_.resize(get_n_out());
    for (int i=0; i<obuf_.size(); ++i) {
      output(i) = DMatrix(get_sparsity_out(i));
    }

    // Call base class method
    FunctionInternal::init();
  }

  void SimulatorInternal::evaluate() {

    // Pass the parameters and initial state
    integrator_.setInput(input(INTEGRATOR_X0), INTEGRATOR_X0);
    integrator_.setInput(input(INTEGRATOR_Z0), INTEGRATOR_Z0);
    integrator_.setInput(input(INTEGRATOR_P), INTEGRATOR_P);

    if (monitored("initial")) {
      userOut() << "SimulatorInternal::evaluate: initial condition:" << std::endl;
      userOut() << " x0     = "  << input(INTEGRATOR_X0) << std::endl;
      userOut() << " z0     = "  << input(INTEGRATOR_Z0) << std::endl;
      userOut() << " p      = "   << input(INTEGRATOR_P) << std::endl;
    }

    // Reset the integrator_
    dynamic_cast<Integrator*>(integrator_.get())->reset();

    // Advance solution in time
    for (int k=0; k<grid_.size(); ++k) {

      if (monitored("step")) {
        userOut() << "SimulatorInternal::evaluate: integrating up to: " <<  grid_[k] << std::endl;
        userOut() << " x0       = "  << integrator_.input(INTEGRATOR_X0) << std::endl;
        userOut() << " z0       = "  << integrator_.input(INTEGRATOR_Z0) << std::endl;
        userOut() << " p        = "   << integrator_.input(INTEGRATOR_P) << std::endl;
      }

      // Integrate to the output time
      dynamic_cast<Integrator*>(integrator_.get())->integrate(grid_[k]);

      if (monitored("step")) {
        userOut() << " xf  = "  << integrator_.output(INTEGRATOR_XF) << std::endl;
        userOut() << " zf  = "  << integrator_.output(INTEGRATOR_ZF) << std::endl;
      }

      // Save the outputs of the function
      for (int i=0; i<n_out(); ++i) {
        const Matrix<double> &res = integrator_.output(i);
        copy(res->begin(), res->end(), output(i)->begin() + k*res.nnz());
      }
    }
  }

} // namespace casadi
