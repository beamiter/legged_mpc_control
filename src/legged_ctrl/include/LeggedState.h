/*
 *  Important shared variables between the robot interface and the cotroller 
 */

#pragma once
#include <Eigen/Dense>
#include <ros/ros.h>

#include "LeggedParams.h"

namespace legged
{
class LeggedFeedback {
    public:
        LeggedFeedback() {
            reset();
        }
    void reset();


    // IMU sensor data
    Eigen::Vector3d imu_acc;
    Eigen::Vector3d imu_ang_vel;


    // important feedback kinematics variables
    Eigen::Vector3d root_pos;
    Eigen::Quaterniond root_quat;
    Eigen::Vector3d root_euler;     // here we use roll, pitch, yaw
    Eigen::Matrix3d root_rot_mat;
    Eigen::Matrix3d root_rot_mat_z;
    Eigen::Vector3d root_lin_vel;
    Eigen::Vector3d root_ang_vel;
    Eigen::Vector3d root_acc;

    Eigen::Vector4d foot_force_sensor;             // the force we get from foot contact sensor, drifts a lot
    Eigen::Vector4d foot_sensor_bias;       // since the foot contact sensor is not accurate, we need to record an initial bias
    Eigen::Matrix<double, 3, NUM_LEG> foot_force_tauEst; // a foot force estimation based on joint torque feedback 

    // process foot force to get contact 
    Eigen::Vector4d foot_force_min;
    Eigen::Vector4d foot_force_max;
    Eigen::Vector4d foot_force_contact_threshold;
    Eigen::Vector4d foot_contact_flag;

    bool foot_force_bias_record = false;
    Eigen::Matrix<double, NUM_DOF, 1> joint_pos;
    Eigen::Matrix<double, NUM_DOF, 1> joint_vel;
    Eigen::Matrix<double, NUM_DOF, 1> joint_tauEst; // unitree sdk's tauEst is the torque feedback from the motor

    Eigen::Matrix<double, 3, NUM_LEG> foot_pos_world; // in the world frame
    Eigen::Matrix<double, 3, NUM_LEG> foot_pos_abs;   // in a frame which centered at the robot frame's origin but parallels to the world frame
    Eigen::Matrix<double, 3, NUM_LEG> foot_pos_rel;   // in the robot frame

    Eigen::Matrix<double, 3, NUM_LEG> foot_vel_world;
    Eigen::Matrix<double, 3, NUM_LEG> foot_vel_abs;
    Eigen::Matrix<double, 3, NUM_LEG> foot_vel_rel;
    Eigen::Matrix<double, 12, 12> j_foot;

 
    // state estimation result
    bool estimated_contacts[NUM_LEG];  // true if the estimator thinks the foot has contact
    Eigen::Vector3d estimated_root_pos;
    Eigen::Vector3d estimated_root_vel;
};

class LeggedCtrl {
    public:
        LeggedCtrl() {
            reset();
        }
    void reset();


    Eigen::Vector4d gait_counter;          // use time directly, the value in gait_counter is the phase time in the gait (0-1)
    Eigen::Vector4d curr_gait_time;          // how long in seconds does current phase lasts

    // control target
    Eigen::Vector3d root_pos_d;
    Eigen::Vector3d root_euler_d;
    Eigen::Vector3d root_lin_vel_d_rel;
    Eigen::Vector3d root_lin_vel_d_world;
    Eigen::Vector3d root_ang_vel_d_rel;
    Eigen::Vector3d root_ang_vel_d_world;


    // terrain related
    double terrain_pitch_angle;  // the estimated terrain angle on pitch direction

    // foot position target
    Eigen::Matrix<double, 3, NUM_LEG> foot_pos_target_world; // in the world frame
    Eigen::Matrix<double, 3, NUM_LEG> foot_pos_target_abs; // in a frame which centered at the robot frame's origin but parallels to the world frame
    Eigen::Matrix<double, 3, NUM_LEG> foot_pos_target_rel; // in the robot frame
    Eigen::Matrix<double, 3, NUM_LEG> foot_vel_target_rel; 

    bool plan_contacts[NUM_LEG];         // flag to decide leg in the stance/swing

    // MPC output
    // TODO: do not hardcode state size, use parameter
    Eigen::Matrix<double, 18, 1> optimized_state;  //[position, euler(ZYX), foot position]
    Eigen::Matrix<double, 24, 1> optimized_input;  //[3*4 contact force,    foot velocity]

    // final results to the robot 
    Eigen::Matrix<double, NUM_DOF, 1> joint_ang_tgt;
    Eigen::Matrix<double, NUM_DOF, 1> joint_vel_tgt;
    Eigen::Matrix<double, NUM_DOF, 1> joint_tau_tgt;

    Eigen::Matrix<double, NUM_DOF, 1> prev_joint_ang_tgt; // to calculate joint_vel_tgt


    double movement_mode = 0;
};

class LeggedJoyCmd {
    public:
    LeggedJoyCmd() {}

    // joystick command
    double velx = 0.0;
    double velx_forward = 0.0;
    double velx_backward = 0.0;
    double vely = 0.0;
    double velz = 0.0;

    double pitch_rate = 0.0;
    double roll_rate = 0.0;
    double yaw_rate = 0.0;

    double pitch_ang = 0.0;
    double roll_ang = 0.0;
    double body_height = 0.1;

    //  0 is standing, 1 is walking
    int ctrl_state = 0;
    bool ctrl_state_change_request = false;
    int prev_ctrl_state = 0;
    bool exit = false;    
};

class LeggedParam {
    public:
    LeggedParam() {}

    bool load(ros::NodeHandle &_nh);

    int run_type;             // simulation(0) or hardware(1)
    int robot_type;           // a1(0) or go1(1)
    int mpc_type;             // lci_mpc (0) or convex mpc (1)
    int low_level_type;       // basic tau ctrl (0) or whole body control (1)
    int kf_type;

    double gait_counter_speed;    // delta time
    Eigen::Matrix<double, 3, NUM_LEG> default_foot_pos_rel;

    // MPC parameters
    Eigen::VectorXd q_weights;
    Eigen::VectorXd r_weights;

    // swing leg parameters
    Eigen::Matrix<double, NUM_DOF_PER_LEG, NUM_LEG> kp_foot;
    Eigen::Matrix<double, NUM_DOF_PER_LEG, NUM_LEG> kd_foot;
    Eigen::Matrix<double, NUM_DOF_PER_LEG, 1> km_foot;

    double robot_mass;
    Eigen::Matrix3d a1_trunk_inertia;

    // joystick mapping 
    int joystick_left_updown_axis;
    int joystick_left_horiz_axis;
    int joystick_right_updown_axis;
    int joystick_right_horiz_axis;
    int joystick_mode_switch_button;
    int joystick_exit_button;

    // joystick parameters
    double joystick_velx_scale;
    double joystick_vely_scale;
    double joystick_height_vel;
    double joystick_max_height;
    double joystick_min_height;

    double joystick_yaw_rate_scale;
    double joystick_roll_rate_scale;
    double joystick_pitch_rate_scale;

    // contact detection flags
    double foot_sensor_max_value;
    double foot_sensor_min_value;
    double foot_sensor_ratio;

    // casadi EKF related
    double ekf_inital_cov;
    double ekf_noise_process_pos_xy;
    double ekf_noise_process_pos_z;
    double ekf_noise_process_vel_xy;
    double ekf_noise_process_vel_z;
    double ekf_noise_process_rot;
    double ekf_noise_process_foot;

    double ekf_noise_measure_fk;
    double ekf_noise_measure_vel;
    double ekf_noise_measure_height;

    double ekf_noise_opti_pos;
    double ekf_noise_opti_vel;
    double ekf_noise_opti_yaw;
    

};

class LeggedState {
    public:
    LeggedState() {
        ctrl.reset();
        fbk.reset();
    }

    LeggedCtrl      ctrl;
    LeggedFeedback  fbk;
    LeggedJoyCmd    joy;
    LeggedParam     param;

    // put other unclassified variables here
    // Need to be aware of deadlock in thread 1 and thread 2
    bool estimation_inited = false;
    bool mpc_solver_inited = false;
};

}  // namespace legged

