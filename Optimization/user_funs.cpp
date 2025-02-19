﻿#include"user_funs.h"

matrix ff0T(matrix x, matrix ud1, matrix ud2)
{
	matrix y;
	y = pow(x(0) - ud1(0), 2) + pow(x(1) - ud1(1), 2);
	return y;
}

matrix ff0R(matrix x, matrix ud1, matrix ud2)
{
	matrix y;
	matrix Y0 = matrix(2, 1), MT = matrix(2, new double[2]{ m2d(x),0.5 });
	matrix* Y = solve_ode(df0, 0, 0.1, 10, Y0, ud1, MT);
	int n = get_len(Y[0]);
	double teta_max = Y[1](0, 0);
	for (int i = 1; i < n; ++i)
		if (teta_max < Y[1](i, 0))
			teta_max = Y[1](i, 0);
	y = abs(teta_max - m2d(ud1));
	Y[0].~matrix();
	Y[1].~matrix();
	return y;
}

matrix df0(double t, matrix Y, matrix ud1, matrix ud2)
{
	matrix dY(2, 1);
	double m = 1, l = 0.5, b = 0.5, g = 9.81;
	double I = m*pow(l, 2);
	dY(0) = Y(1);
	dY(1) = ((t <= ud2(1))*ud2(0) - m*g*l*sin(Y(0)) - b*Y(1)) / I;
	return dY;
}

// Own Functions LAB1
// ------------------

matrix ff1T(matrix x, matrix ud1, matrix ud2)
{
	double t = -pow(0.1 * m2d(x) - 2 * M_PI,2);
	return -cos(0.1 * m2d(x)) * pow(M_E, t) + 0.00002 * pow(m2d(x),2);
}

matrix flow_and_temp(double t, matrix Y, matrix ud1, matrix ud2)
{
	// Result vector - changes in volume and temp
	matrix dY(3, 1);

	// Physical coefficients: viscosity, contraction, gravitational acceleration
	double a = 0.98, b = 0.63, g = 9.81;

	// A tank parameters
	double area_a = 0.5;
	double temp_a = 90.0;

	// B tank parameters
	double area_b = 1.0;
	double outflow_area_b = 0.00365665;

	// Inflow parameters
	double inflow_rate = 0.01;
	double temp_increase = 20.0;

	// Get outflows from each tank
	double outflow_a = Y(0) > 0 ? a * b * m2d(ud2) * sqrt(2 * g * Y(0) / area_a) : 0;
	double outflow_b = Y(1) > 1 ? a * b * outflow_area_b * sqrt(2 * g * Y(1) / area_b) : 0;

	// Changes in volume and temp:
	dY(0) = -outflow_a;	// Change in volume of tank A
	dY(1) = outflow_a + inflow_rate - outflow_b;	// Change in volume of tank B
	dY(2) = inflow_rate / Y(1) * (temp_increase - Y(2)) + outflow_a / Y(1) * (temp_a - Y(2));	// Change in temp in tank B

	return dY;
}

matrix simulate_flow_temp(matrix x, matrix ud1, matrix ud2)
{
	// Initial conditions
	double volume_a = 5, volume_b = 1, temp_b = 20;
	double conditions[3] = { volume_a, volume_b, temp_b };
	matrix Y0 = matrix(3, conditions);

	// Time
	double start_time = 0.0;
	double end_time = 2000.0;
	double time_step = 1.0;

	// Solve differential equation
	matrix* results = solve_ode(flow_and_temp, start_time, time_step, end_time, Y0, ud1, x);

	// Get max temp in B tank
	double max_temp_b = results[1](0, 2);

	for (int i = 1; i < get_len(results[0]); i++)
	{
		if (max_temp_b < results[1](i, 2))
			max_temp_b = results[1](i, 2);
	}

	// Get absolute deviation from the temp of 50
	double temp_deviation = abs(max_temp_b - 50);
	return temp_deviation;
}

// Own Functions LAB2
// ------------------

matrix ff2T(matrix x, matrix ud1, matrix ud2)
{
	return matrix( pow( x(0) ) + pow( x(1) )
		- cos( 2.5 * M_PI * x(0) ) - cos(2.5 * M_PI * x(1) ) + 2 );
}

matrix df2(double t, matrix Y, matrix ud1, matrix ud2)
{
	// Physical parameters
	double m1 = 1.0;	// Arm mass
	double m2 = 5.0;	// Weight Mass
	double l = 1.0;		// Arm length
	double b = 0.5;		// Friction coefficient
	double I = ((m1 / 3) + m2) * pow(l, 2);

	// Control values
	double k1 = m2d(ud1);
	double k2 = m2d(ud2);

	// Targets
	double alpha_target = M_PI;	// Target angle in radians
	double omega_target = 0.0;	// Target speed

	// Getting the moment of force
	double alpha_diff = alpha_target - Y(0);
	double omega_diff = omega_target - Y(1);
	double M_t = k1 * alpha_diff + k2 * omega_diff;

	// Get derivatives
	matrix dY(2, 1);
	dY(0) = Y(1);	// Derivative of an angle is speed
	dY(1) = (M_t - b * Y(1)) / I;	// Derivative of speed

	return dY;
}

matrix ff2R(matrix x, matrix ud1, matrix ud2)
{
	matrix k1 = x(0);
	matrix k2 = x(1);

	// Initial conditions
	double start_time = 0.0;
	double end_time = 100.0;
	double time_step = 0.1;

	// Initial arm properties
	double start_angle = 0.0;
	double start_speed = 0.0;
	matrix Y0 = matrix(2, new double[2] {start_angle, start_speed});

	// Arm motion simulation
	matrix* results = solve_ode(df2, start_time, time_step, end_time, Y0, k1, k2);

	// Get the quality function of Q
	double Q = 0.0;
	for (int i = 0; i < get_len(results[0]); i++)
	{
		double alpha_diff = results[1](i, 0) - M_PI;	// Target is pi rad
		double omega_diff = results[1](i, 1);			// Target is speed of 0

		double M_t = m2d(k1) * alpha_diff + m2d(k2) * omega_diff;

		Q += (10 * pow(alpha_diff, 2) + pow(omega_diff, 2) + pow(M_t, 2)) * time_step;
	}

	// Memory cleanup
	delete[] results;

	// Return the result
	return matrix(Q);
}
// Own Functions LAB3
// ------------------

// the boundary g functions
matrix g1(matrix x, matrix ud1)
{
	return -x(0) + 1;
}
matrix g2(matrix x, matrix ud1)
{
	return -x(1) + 1;
}
matrix g3(matrix x, matrix a)
{
	return sqrt(pow(x(0), 2) + pow(x(1), 2)) - m2d(a);
}

matrix ff3T(matrix x, matrix ud1, matrix ud2) {
//std::cout << "FF3T" << std::endl;
	double tk = M_PI * std::sqrt(m2d(pow(x(0) * M_1_PI) + pow(x(1) * M_1_PI)));
	return sin(tk)/tk;
	//return pow(x(0), 2) + pow(x(1), 2); // example from lecture
}

matrix df3(double t, matrix Y, matrix ud1, matrix ud2)
{
	// Parameters
	const double m = 0.6;	// Mass [kg]
	const double r = 0.12; // Radius [m]
	const double g = 9.81; // Gravitational acceleration [m/s^2]
	const double c = 0.47; // Air resistance coefficient 
	const double rho = 1.2; // Air density [kg/m^3]
	const double s = M_PI * pow(r, 2); // Cross-section area [m2]

	// Movement vectors
	double v_x = Y(1); // Horizontal speed
	double v_y = Y(3); // Vertical speed
	double v = sqrt(v_x * v_x + v_y * v_y); // Total speed

	// Forces
	double d_x = 0.5 * c * rho * s * v * v_x; // Horizontal resistance
	double d_y = 0.5 * c * rho * s * v * v_y; // Vertical resistance
	double m_x = rho * v_y * ud1(0) * 2 * M_PI * pow(r, 3); // Magnus's force horizontal
	double m_y = rho * v_x * ud1(0) * 2 * M_PI * pow(r, 3); // Magnus's force vertical

	// Differential euqations describing motion
	matrix dY(4, 1);
	dY(0) = v_x;						// dx / dt
	dY(1) = -(d_x + m_x) / m;			// dvx / dt
	dY(2) = v_y;						// d / dt
	dY(3) = -(m * g + d_y + m_y) / m;	// dvy / dt

	// Return the state
	return dY;
}

matrix ff3R(matrix x, matrix ud1, matrix ud2)
{
	// Starting parameters
	const double x_0 = 0;	// Starting horizontal position
	const double vx_0 = x(0); // Starting horizontal speed
	const double y_0 = 100;	// Starting vertical position
	const double vy_0 = 0;	// Starting vertical speed
	const double omega_0 = x(1); // Starting rotation
	matrix Y_0(4, new double[4] {x_0, vx_0, y_0, vy_0});

	// Time
	const double start_time = 0.0;
	const double end_time = 7.0;
	const double time_step = 0.01;

	// Resolve differential equation
	matrix* result = solve_ode(df3, start_time, time_step, end_time, Y_0, omega_0, NULL);

	// Find indecies of y = 0 and y = 50
	int y_end_index = 0, y_50_index = 0;
	for (int i = 0; i < get_len(result[0]); i++)
	{
		double current_y = result[1](i, 2);

		// Check if current Y is closer to 50
		double dist_to_50 = abs(current_y - 50);
		double best_dist_to_50 = abs(result[1](y_50_index, 2) - 50);

		if (dist_to_50 < best_dist_to_50) 
			y_50_index = i;

		// Check if current Y is closer to 0
		double dist_to_0 = abs(current_y);
		double best_dist_to_0 = abs(result[1](y_end_index, 2));
		
		if (dist_to_0 < best_dist_to_0)
			y_end_index = i;
	}

	// Assign closest values
	double x_end = result[1](y_end_index, 0);
	double x_50 = result[1](y_50_index, 0);

	// Apply penalties
	const double scaling_factor = 600000;
	double total_penalty = 0.0;

	// Penalty for exceeding starting speed
	if (abs(vx_0) > 10)
		total_penalty += scaling_factor * pow(abs(vx_0) - 10, 2);

	// Penalty for exceeding starting rotation
	if (abs(omega_0) > 15)
		total_penalty += scaling_factor * pow(abs(omega_0) - 15, 2);

	// Penalty for missing the target at y = 50
	if (x_50 < 4.5)
		total_penalty += pow(4.5 - x_50, 2) * scaling_factor;
	else if (x_50 > 5.5)
		total_penalty += pow(x_50 - 5.5, 2) * scaling_factor;

	// Give a final score
	double score = -x_end + total_penalty;

	// Free dynamic memory
	result[0].~matrix();
	result[1].~matrix();

	// Return the score
	return score;
}

// Own Functions LAB4
// ------------------

matrix ff4T(matrix x, matrix ud1, matrix ud2)
{
	if (isnan(ud2(0)))
		return (pow(x(0) + 2.0 * x(1) - 7.0) + pow(2.0 * x(0) + x(1) - 5.0));
	else 
		return (pow((ud1(0) + x(0) * ud2(0)) + 2.0 * (ud1(1) + x(0) * ud2(1)) - 7.0)
			+ pow(2.0 * (ud1(0) + x(0) * ud2(0)) + (ud1(1) + x(0) * ud2(1)) - 5.0));
	
}

matrix gradff4T(matrix x, matrix ud1, matrix ud2)
{

	return matrix(2, new double[2] {
			10.0 * x(0) + 8.0 * x(1) - 34,
			8.0 * x(0) + 10.0 * x(1) - 38,
		});
}

matrix Hff4T(matrix x, matrix ud1, matrix ud2)
{
	matrix rt(2, 2);
	rt(0, 0) = 10.0;
	rt(0, 1) = 8.0;
	rt(1, 0) = 8.0;
	rt(1, 1) = 10;

	return rt;
}

double get_h_l4(matrix theta, matrix X, int col)
{
	// Get z = thetha0 * 1 + theta1 * x1 + theta2 * x2
	double z = theta(0, 0) * X(0, col) + theta(1, 0) * X(1, col) + theta(2, 0) * X(2, col);
	double h = 1.0 / (1.0 + exp(-z));

	return h;
}

matrix get_cost(matrix theta, matrix Y, matrix X) 
{
	// Get size
	int* size_Y = get_size(Y);
	int m = size_Y[1];
	delete[] size_Y;

	double cost = 0.0;
	for (int j = 0; j < m; ++j) 
	{
		double h = get_h_l4(theta, X, j);

		// Add to the cost
		if (Y(0, j) == 1) 
		{
			// Slightly increase the value to avoid log(0)
			cost += -log(h + 1e-15);
		}
		else 
		{
			cost += -log(1.0 - h + 1e-15);
		}
	}

	// Average cost value
	cost /= static_cast<double>(m);

	// Return the result as matrix
	return cost;
}

matrix get_gradient(matrix theta, matrix Y, matrix X)
{
	// Get size
	int* size_Y = get_size(Y);
	int m = size_Y[1];
	delete[] size_Y;

	// Initialize gradient as 3x1 matrix with zeros
	matrix grad(3, 1, 0.0);

	for (int j = 0; j < m; ++j) 
	{
		double h = get_h_l4(theta, X, j);

		// Get the error
		double error = h - Y(0, j);

		// Update the graident
		grad(0, 0) += error * X(0, j);	// For theta0 (bias)
		grad(1, 0) += error * X(1, j);	// For theta1
		grad(2, 0) += error * X(2, j);	// For theta2
	}

	// Average gradient value
	for (int j = 0; j < 3; ++j)
	{
		grad(j, 0) /= static_cast<double>(m);
	}

	return grad;
}

matrix get_accuracy(matrix theta, matrix X, matrix Y, int cols)
{
	int guessed = 0;
	for (int j = 0; j < cols; j++)
	{
		double h = get_h_l4(theta, X, j);

		int prediction = (h >= 0.5) ? 1 : 0;
		if (prediction == Y(0, j)) 
		{
			guessed++;
		}
	}

	return (static_cast<double>(guessed) / cols) * 100.0;
}


// Own Functions LAB5
// ------------------

static double weights[101];
static int weight_i = -1;

void set_weight(int i)
{
	weight_i = i;
}

void init_weights()
{
	for (int i = 0; i < 101; i++)
	{
		weights[i] = static_cast<double>(i) * 0.01;
	}
}

double get_weight()
{
	return weights[weight_i];
}

/* Test Function */
matrix ff5T1_1(matrix x, matrix ud1, matrix ud2)
{
	if (isnan(ud2(0)))
		return (pow( x(0) - 2.0 )+pow( x(1) - 2.0 ));
	else
		return (pow(ud1(0) + x(0) * ud2(0) - 2.0) + pow(ud1(1) + x(0) * ud2(1) - 2.0));
}
matrix ff5T2_1(matrix x, matrix ud1, matrix ud2)
{
	if (isnan(ud2(0)))
		return (pow(x(0) + 2.0) + pow(x(1) + 2.0));
	else
		return (pow(ud1(0) + x(0) * ud2(0) + 2.0) + pow(ud1(1) + x(0) * ud2(1) + 2.0));
}
matrix ff5T3_1(matrix x, matrix ud1, matrix ud2)
{
	return weights[weight_i] * ff5T1_1(x, ud1, ud2) + (1.0 - weights[weight_i]) * ff5T2_1(x, ud1, ud2);
}
matrix ff5T1_10(matrix x, matrix ud1, matrix ud2)
{
	if (isnan(ud2(0)))
		return 10.0*(pow(x(0) - 2.0) + pow(x(1) - 2.0));
	else
		return 10.0*(pow(ud1(0) + x(0) * ud2(0) - 2.0) + pow(ud1(1) + x(0) * ud2(1) - 2.0));
}
matrix ff5T2_10(matrix x, matrix ud1, matrix ud2)
{
	if (isnan(ud2(0)))
		return 0.1*(pow(x(0) + 2.0) + pow(x(1) + 2.0));
	else
		return 0.1*(pow(ud1(0) + x(0) * ud2(0) + 2.0) + pow(ud1(1) + x(0) * ud2(1) + 2.0));
}
matrix ff5T3_10(matrix x, matrix ud1, matrix ud2)
{
	return weights[weight_i] * ff5T1_10(x, ud1, ud2) + (1.0 - weights[weight_i]) * ff5T2_10(x, ud1, ud2);
}
matrix ff5T1_100(matrix x, matrix ud1, matrix ud2)
{
	if (isnan(ud2(0)))
		return 100.0 * (pow(x(0) - 2.0) + pow(x(1) - 2.0));
	else 
		return 100.0 * (pow(ud1(0) + x(0) * ud2(0) - 2.0) + pow(ud1(1) + x(0) * ud2(1) - 2.0));
}
matrix ff5T2_100(matrix x, matrix ud1, matrix ud2)
{
	if (isnan(ud2(0)))
		return 0.01 * (pow(x(0) + 2.0) + pow(x(1) + 2.0));
	else
		return 0.01 * (pow(ud1(0) + x(0) * ud2(0) + 2.0) + pow(ud1(1) + x(0) * ud2(1) + 2.0));
}
matrix ff5T3_100(matrix x, matrix ud1, matrix ud2)
{
	return weights[weight_i] * ff5T1_100(x, ud1, ud2) + (1.0 - weights[weight_i]) * ff5T2_100(x, ud1, ud2);
}

/* Real Problem */
// Constants
const double density = 7800.0; // kg/m^3 (rho)
const double force = 1000.0; // 1 kN (P)
const double youngs_modulus = 207e9; // E in Pa (GPa -> Pa)
const double pi = 3.141592653;

matrix ff5R_stress(matrix x) 
{
	double beam_length = x(0, 0);
	double csd = x(1, 0);	// cross-sectional diameter

	// Calculate stress
	double stress = (32.0 * force * beam_length) / (pi * pow(csd, 3));
	return matrix(stress);
}

matrix ff5R_mass(matrix x) 
{
	double beam_length = x(0);
	double csd = x(1);	// cross-sectional diameter

	// Cross-sectional area
	double csa = pi * pow(csd / 2.0, 2); // m^2

	// Calculate mass
	double mass = density * csa * beam_length; // kg
	return matrix(mass);
}

matrix ff5R_deflection(matrix x) 
{
	double beam_length = x(0, 0);
	double csd = x(1, 0);	// cross-sectional diameter

	// Deflection in meters
	double deflection = (64.0 * force * pow(beam_length, 3)) / (3.0 * youngs_modulus * pi * pow(csd, 4));
	return matrix(deflection);
}

matrix ff5R_penalty(matrix x) 
{
	double beam_length = x(0, 0);
	double csd = x(1, 0);	// cross-sectional diameter

	// Deflection in meters
	double deflection = m2d(ff5R_deflection(x));

	// Stress in Pa
	double stress = m2d(ff5R_stress(x));

	// Penalty and constraints
	double penalty = 0.0;
	const double penalty_modifier = 1e15;
	const double penalty_base = 1e5;
	const double max_length = 1;	// Meters
	const double min_length = 0.200;	// Meters
	const double csd_max = 0.050;	// Meters
	const double csd_min = 0.010;	// Meters
	const double reflection_max = 5.0e-3;     // Meters
	const double stress_max = 300e6;  // MPa -> Pa

	/* Apply penalties as proportion of offset */

	// Beam length
	if (beam_length < min_length)
	{
		double diff = min_length - beam_length;
		penalty += penalty_modifier * (diff / min_length) + penalty_base;
	}

	if (beam_length > max_length)
	{
		double diff = beam_length - max_length;
		penalty += penalty_modifier * (diff / max_length) + penalty_base;
	}

	// Cross sectionnal diameter
	if (csd < csd_min)
	{
		double diff = csd_min - csd;
		penalty += penalty_modifier * (diff / csd_min) + penalty_base;
	}

	if (csd > csd_max)
	{
		double diff = csd - csd_max;
		penalty += penalty_modifier * (diff / csd_max) + penalty_base;
	}

	// Deflection
	if (deflection > reflection_max)
		penalty += penalty_modifier * (deflection - reflection_max) / reflection_max + penalty_base;

	if (deflection < 0.f)
		penalty += penalty_modifier * (-deflection);

	// Stress
	if (stress > stress_max)
		penalty += penalty_modifier * (stress - stress_max) / stress_max + penalty_base;

	if (stress < 0.f)
		penalty += penalty_modifier * (-stress) + penalty_base;

	// Return the total penalty
	return matrix(penalty);
}

matrix ff5R(matrix x, matrix ud1, matrix ud2)
{
	// If ud2 is not NaN apply adjustments from ud1 and ud2
	matrix xi = x;
	if (!isnan(ud2(0)))
		xi = ud1 + m2d(x) * ud2;

	// Debug; Set [entry] to true to enable
	static bool entry = false;
	if (entry)
	{
		std::cout << "----------\n";
		std::cout << "ff5R args:\n";
		std::cout << "x vals:\n" << x << "\n";
		std::cout << "xi vals:\n" << x << "\n";
		std::cout << "ud1 vals:\n" << ud1 << "\n";
		std::cout << "ud2 vals:\n" << ud2 << "\n";
		std::cout << "----------\n";

		entry = false;
	}
	// Get the weight
	double weight = weights[weight_i];

	// Get mass, deflection and penalty
	matrix mass = ff5R_mass(xi);
	matrix deflection = ff5R_deflection(xi);
	matrix penalty = ff5R_penalty(xi);

	// Objective function calculation (weighted combination)
	double objective = weight * m2d(mass) + (1.0 - weight) * m2d(deflection) + m2d(penalty);
	return matrix(objective);
}

// Own Functions LAB5
// ------------------

matrix ff6_T(matrix x, matrix ud1, matrix ud2)
{
	static double d_2o5_pi = 2.5 * pi;
	return x(0) * x(0) + x(1) * x(1) - cos(d_2o5_pi * x(0)) - cos(d_2o5_pi * x(1)) + 2.0;
}

/* Real Problem */
matrix ff6R_derivative(double time, matrix Y, matrix dampness, matrix ud2)
{
	// Physical constants
	double upper_mass = 5.0, lower_mass = 5.0;
	double upper_spring = 1.0, spring_coupling = 1.0;
	double external_force = 1.0;

	// Damping coefficients
	double upper_damping = dampness(0, 0);
	double damping_coupling = dampness(1, 0);

	// State
	double upper_position = Y(0, 0), lower_position = Y(2, 0);
	double upper_velocity = Y(1, 0), lower_velocity = Y(3, 0);

	// Get acceleration
	double upper_acceleration = (-upper_damping * upper_velocity - damping_coupling
		* (upper_velocity - lower_velocity) - upper_spring
		* upper_position - spring_coupling * (upper_position - lower_position))
		/ upper_mass;

	double lower_acceleration = (external_force + damping_coupling
		* (upper_velocity - lower_velocity) + spring_coupling
		* (upper_position - lower_position))
		/ lower_mass;

	// Pack derivatives
	matrix dY(4, 1);
	dY(0, 0) = upper_velocity;
	dY(1, 0) = upper_acceleration;
	dY(2, 0) = lower_velocity;
	dY(3, 0) = lower_acceleration;

	// Return derivatives
	return dY;
}

matrix ff6R_motion(matrix dampness)
{
	// Damping coefficients
	double upper_damping = dampness(0, 0);
	double damping_coupling = dampness(1, 0);

	// Time parameters
	double start_time = 0.0;
	double stop_time = 100.0;
	double time_step = 0.1;

	// Initial state
	double upper_position = 0.0, lower_position = 0.0;
	double upper_velocity = 0.0, lower_velocity = 0.0;
	matrix Y0(4, new double[4]{upper_position, upper_velocity, lower_position, lower_velocity});

	// Solve differential equation
	matrix* solution = solve_ode(ff6R_derivative, start_time, time_step, stop_time, Y0, dampness);
	
	// Only copy the positions
	int rows = 1001;
	matrix positions(rows, 2);

	for (int i = 0; i < rows; i++)
	{
		positions(i, 0) = solution[1](i, 0);
		positions(i, 1) = solution[1](i, 2);
	}

	delete[] solution;

	// Return the positions
	return positions;
}

double ff6R_error(matrix simulation_results, matrix reference_data)
{
	// Get the size of the matrix
	int *size = get_size(simulation_results);
	int rows = size[0];
	delete[] size;

	// Calculate error
	double error = 0.0;
	for (int i = 0; i < rows; ++i)
	{
		double sim_upper_pos = simulation_results(i, 0);
		double sim_lower_pos = simulation_results(i, 1);
		double ref_upper_pos = reference_data(i, 0);
		double ref_lower_pos = reference_data(i, 1);

		error += pow(sim_upper_pos - ref_upper_pos, 2) + pow(sim_lower_pos - ref_lower_pos, 2);
	}

	return error;
}

matrix ff6R(matrix x, matrix ud1, matrix ud2)
{
	// Debug: Print the number of function calls
	static int calls = 0;
	calls += 1;
	if (calls % 100 == 0)
	{
		std::cout << "ff6R calls: " << calls << "\n";
	}

	// Run simulation for given dampness coefficients
	matrix simulation_results = ff6R_motion(x);

	// Return the error
	return ff6R_error(simulation_results, ud2);
}