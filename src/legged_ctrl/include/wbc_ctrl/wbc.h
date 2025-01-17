//
// Created by qiayuan on 2022/7/1.
//

#pragma once

#include "wbc_ctrl/HoQp.h"

#include <ocs2_legged_robot/gait/MotionPhaseDefinition.h>
#include <ocs2_centroidal_model/PinocchioCentroidalDynamics.h>
#include <ocs2_pinocchio_interface/PinocchioEndEffectorKinematics.h>

namespace legged
{
using namespace ocs2;
using namespace legged_robot;

// Decision Variables: x = [\dot u^T, F^T, \tau^T]^T
class Wbc
{
public:
  Wbc(const std::string& task_file, PinocchioInterface& pino_interface, CentroidalModelInfo& info,
      const PinocchioEndEffectorKinematics& ee_kinematics, int swing_task_type, bool verbose);
  vector_t update(const vector_t& state_desired, const vector_t& input_desired, vector_t& measured_rbd_state,
                  size_t mode);

private:
  void loadTasksSetting(const std::string& task_file, bool verbose);

  Task formulateFloatingBaseEomTask();
  Task formulateTorqueLimitsTask();
  Task formulateNoContactMotionTask();
  Task formulateFrictionConeTask();
  Task formulateBaseAccelTask();
  Task formulateSwingLegTask();
  Task formulateContactForceTask();

  size_t num_decision_vars_;
  PinocchioInterface& pino_interface_;
  const CentroidalModelInfo& info_;
  PinocchioCentroidalDynamics centroidal_dynamics_;
  CentroidalModelPinocchioMapping mapping_;
  std::unique_ptr<PinocchioEndEffectorKinematics> ee_kinematics_;

  vector_t state_desired_, input_desired_, measured_q_, measured_v_;
  matrix_t j_, dj_;
  contact_flag_t contact_flag_;
  size_t num_contacts_;

  // Task Parameters:
  int swing_task_type_;   // 0 - state desired and input desired contains JOINT SPACE target  (joint angles & angle velocities)
                         // 1 - state desired and input desired contains WORK SPACE target   (end effector position & velocity)
  vector_t torque_limits_;
  scalar_t friction_coeff_, swing_kp_, swing_kd_;
};

}  // namespace legged
