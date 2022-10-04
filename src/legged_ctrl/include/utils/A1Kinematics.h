//
// Created by shuoy on 8/10/21.
//

#ifndef VILEOM_A1KINEMATICS_H
#define VILEOM_A1KINEMATICS_H

#include <eigen3/Eigen/Dense>

#include <iostream>

#define PI 3.14159
namespace legged {
    
class A1Kinematics {

    public:
        A1Kinematics() = default;
        ~A1Kinematics() = default;

        const int RHO_OPT_SIZE = 3;
        const int RHO_FIX_SIZE = 5;
        // rho opt are contact offset cx cy cz
        // rho fix are body offset x& y, thigh offset, upper leg length, lower leg length
        // functions with eigen interface
        // forward kinematics 3x1
        Eigen::Vector3d fk(Eigen::Vector3d q, Eigen::VectorXd rho_opt, Eigen::VectorXd rho_fix);
        // jacobian   3x3
        Eigen::Matrix3d jac(Eigen::Vector3d q, Eigen::VectorXd rho_opt, Eigen::VectorXd rho_fix);
        // the partial derivative of fk wrt rho opt   3x3
        Eigen::Matrix3d dfk_drho(Eigen::Vector3d q, Eigen::VectorXd rho_opt, Eigen::VectorXd rho_fix);
        // the partial derivative of jacobian wrt q    9x3
        Eigen::Matrix<double, 9, 3> dJ_dq(Eigen::Vector3d q, Eigen::VectorXd rho_opt, Eigen::VectorXd rho_fix);
        // the partial derivative of jacobian wrt rho opt   9x3
        Eigen::Matrix<double, 9, 3> dJ_drho(Eigen::Vector3d q, Eigen::VectorXd rho_opt, Eigen::VectorXd rho_fix);
        // inverse kinematics of a leg given a point p in body frame 
        Eigen::Vector3d inv_kin(Eigen::Vector3d p, Eigen::VectorXd rho_opt, Eigen::VectorXd rho_fix, Eigen::Vector3d guess);
        // inverse kinematics of a leg given a point p in body frame, using analytical solution
        Eigen::Vector3d inv_kin(Eigen::Vector3d p, Eigen::VectorXd rho_opt, Eigen::VectorXd rho_fix);
    private:
        // functions with basic C++ interface, generated by Matlab
        void autoFunc_fk_derive(const double in1[3], const double in2[3], const double in3[5], double p_bf[3]);
        void autoFunc_d_fk_dq(const double in1[3], const double in2[3], const double in3[5], double jacobian[9]);
        void autoFunc_d_fk_dc(const double in1[3], const double in2[3], const double in3[5], double d_fk_dc[9]);
        void autoFunc_dJ_dq(const double in1[3], const double in2[3], const double in3[5], double dJ_dq[27]);
        void autoFunc_dJ_dpho(const double in1[3], const double [3], const double [5], double dJ_dpho[27]);
};


}
#endif //VILEOM_A1KINEMATICS_H
