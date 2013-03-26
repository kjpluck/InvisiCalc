/*
    Copyright 2009 by Kevin Pluck

    This file is part of InvisiCalc.

    InvisiCalc is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    InvisiCalc is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with InvisiCalc.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef EVALUATE_H_INCLUDED
#define EVALUATE_H_INCLUDED

//Define tokens for functions and constants. The underscore is to help destinguish single letter defines e.g. _C
#define _SQRT 1
#define _PI 2
#define _SIN 3
#define _COS 4
#define _TAN 5
#define _ASIN 6
#define _ACOS 7
#define _ATAN 8
#define _E 9
#define _C 10
#define _EXP 11
#define _LOG 12
#define _SINH 13
#define _COSH 14
#define _TANH 15
#define _GAMMA 16
#define _PHI 17
#define _LOG10 18


/*
Evaluate
    Returns:
         1 - If strInfixCode can be evaluated
         0 - If strInfixCode cannot be evaluated
        -1 - If strInfixCode can be evaluated but a divide by zero occured
        -2 - If strInfixCode can be evaluated but an invalid input occured E.g.: sqrt(-1)
*/
int Evaluate(char *strInfixCode,double *fltResult);

#endif // EVALUATE_H_INCLUDED
