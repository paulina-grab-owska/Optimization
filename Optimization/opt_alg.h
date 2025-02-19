//Ten plik nie powinien by� edytowany

#pragma once

#include"solution.h"

solution MC(matrix(*ff)(matrix, matrix, matrix), int N, matrix lb, matrix ub, double epsilon, int Nmax, matrix ud1 = NAN, matrix ud2 = NAN);

double* expansion(matrix(*ff)(matrix, matrix, matrix), double x0, double d, double alpha, int Nmax, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);
solution fib(matrix(*ff)(matrix, matrix, matrix), double a, double b, double epsilon, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);
solution lag(matrix(*ff)(matrix, matrix, matrix), double a, double b, double epsilon, double gamma, int Nmax, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);

// @param matrix x0 - matrix 1x2 (vertical vector): initial position
// @param double s0 - (scalar): initial step size
solution HJ(matrix(*ff)(matrix, matrix, matrix), matrix x0, double s, double alpha, double epsilon, int Nmax, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);
solution HJ_trial(matrix(*ff)(matrix, matrix, matrix), solution XB, double s, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);

// @param matrix x0 - matrix 1x2 (vertical vector): initial position
// @param matrix s0 - matrix 1x2 (vertical vector): initial step size for 2 axis
solution Rosen(matrix(*ff)(matrix, matrix, matrix), matrix x0, matrix s0, double alpha, double beta, double epsilon, int Nmax, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);

// @param matrix x0 - matrix 1x2 (vertical vector): initial position
solution pen(matrix(*ff)(matrix, matrix, matrix), matrix x0, double c, double dc, double epsilon, int Nmax, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);
// @param matrix x0 - matrix 1x2 (vertical vector): initial position
solution sym_NM(matrix(*ff)(matrix, matrix, matrix), matrix x0, double s, double alpha, double beta, double gamma, double delta, double epsilon, int Nmax, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);

/// method of steepest descent
// @param matrix x0 - matrix 1x2 (vertical vector): initial position
solution SD(matrix(*ff)(matrix, matrix, matrix), matrix(*gf)(matrix, matrix, matrix), matrix x0, double h0, double epsilon, int Nmax, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);
// conjugate gradient method
// @param matrix x0 - matrix 1x2 (vertical vector): initial position
solution CG(matrix(*ff)(matrix, matrix, matrix), matrix(*gf)(matrix, matrix, matrix), matrix x0, double h0, double epsilon, int Nmax, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);
solution Newton(matrix(*ff)(matrix, matrix, matrix), matrix(*gf)(matrix, matrix, matrix), matrix(*Hf)(matrix, matrix, matrix), matrix x0, double h0, double epsilon, int Nmax, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);
solution golden(matrix(*ff)(matrix, matrix, matrix), double a, double b, double epsilon, int Nmax, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);

solution Powell(matrix(*ff)(matrix, matrix, matrix), matrix x0, double epsilon, int Nmax, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);

/*
* @param ff - function pointer
* @param N - number of decision variables
* @param lb - lower boundary
* @param ub - upper boundary
* @param mi - number of base population
* @param lambda - number of temporary population
* @param sigma0 - starting value of the mutation range coefficient
* @param epsilon - precision
* @param Nmax - maximum number of iterations
*/
solution EA(matrix(*ff)(matrix, matrix, matrix), int N, matrix lb, matrix ub, int mi, int lambda, double sigma0, double epsilon, int Nmax, matrix ud1 = NAN, matrix ud2 = NAN); // throw (string);
