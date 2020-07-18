
#include "obj.h"
#include "assignment.h"
#include "HungarianAlg.h"

#include <math.h>
#include <Eigen/Dense>

#include <cmath>
#include <ctime>

#include <iostream>
#include <vector>
#include <string>
#include <iostream>

template<typename LeftClass, typename RightClass>
double function_t(LeftClass& lc, RightClass& rc) {
	return std::sqrt((lc.x - rc.x)*(lc.x -	rc.x) + (lc.y - rc.y)*(lc.y -	rc.y)); 
}

#define RANGE 100.0

int main (int argc, char** argv) {
#ifdef (WIN32 || WIN64)
	srand((unsigned)time(NULL));
#else
	srandom((unsigned)time(NULL));
#endif
	int M = 10;
	int N = 15;
	double dist_thresh = 10.0;
	std::vector<ele::LeftObj> left_obj(M);
	std::vector<ele::RightObj> right_obj(N);

	Eigen::MatrixXd l_value = (Eigen::MatrixXd::Random(M, 2)).array().abs() * 100.0;
	for (int i=0; i<l_value.rows(); i++) {
		left_obj[i].x = l_value(i, 0);
		left_obj[i].x = l_value(i, 1);
	}
	Eigen::MatrixXd r_value = (Eigen::MatrixXd::Random(N, 2)).array().abs() * 100.0;
	for (int i=0; i<r_value.rows(); i++) {
		right_obj[i].x = r_value(i, 0);
		right_obj[i].x = r_value(i, 1);
	}

	std::cout << "create Assingment" << std::endl;
	ele::Assignment<ele::LeftObj, ele::RightObj> assign_engine(&left_obj, 
			&right_obj,  function_t<ele::LeftObj, ele::RightObj>, dist_thresh);

	std::cout << "start Solve..." << std::endl;
	assign_engine.Solve();

	std::cout << "Solved and get result" << std::endl;
	std::vector<int> result = assign_engine.Assign(); 

	for (size_t i=0; i<result.size(); i++) {
		if (result[i] < 0)
			std::cout << "Left: " << i << " --> null" << std::endl;
		else
			std::cout << "Left: " << i << " --> Right: " << result[i] << std::endl;
	}

  return 0;
}