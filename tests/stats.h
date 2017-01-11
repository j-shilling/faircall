/*
 *    This file is part of faircall.
 *
 *    faircall is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    faircall is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with faircall.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __STATS_H__
#define __STATS_H__

#include <stdbool.h>

double
mean (double *vals, int len);
double
stdev (double *vals, int len);
double
coefficient_of_variance (double *vals, int len);

double
median (double *vals, int len);
double
lowq (double *vals, int len);
double
upq (double *vals, int len);

bool
no_outliers (double *vals, int len);
bool
low_variance (double *vals, int len);

#endif
