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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

#include "stats.h"

static int
doublecmp (const void *a, const void *b)
{
  return (*(double *) a - *(double *) b);
}

double
mean (double *vals, int len)
{
  double total = 0;

  for (int i = 0; i < len; i++)
    total += vals[i];

  return total / (double) len;
}

double
stdev (double *vals, int len)
{
  double m = mean (vals, len);
  double *deviations = (double *) malloc (sizeof(double) * len);

  for (int i = 0; i < len; i++)
    {
      deviations[i] = (vals[i] - m) * (vals[i] - m);
    }

  double variance = mean (deviations, len);
  free (deviations);

  return sqrt (variance);
}

double
coefficient_of_variance (double *vals, int len)
{
  return stdev (vals, len) / mean (vals, len);
}

double
median (double *vals, int len)
{
  qsort (vals, len, sizeof(double), doublecmp);

  if ((len % 2) == 0)
    {
      return (vals[len / 2] + vals[len / 2 - 1]) / 2;
    }
  else
    {
      return (vals[len / 2]);
    }

}

double
lowq (double *vals, int len)
{
  qsort (vals, len, sizeof(double), doublecmp);

  if ((len % 2) == 0)
    {
      return median (vals, len / 2);
    }
  else
    {
      return median (vals, len / 2);
    }
}

double
upq (double *vals, int len)
{
  qsort (vals, len, sizeof(double), doublecmp);

  if ((len % 2) == 0)
    {
      return median (vals + (len / 2), len / 2);
    }
  else
    {
      return median (vals + (len / 2 + 1), len / 2);
    }
}

int
outliers (double *vals, int len)
{
  double q1 = lowq (vals, len);
  double q3 = upq (vals, len);

  double iqr = q3 - q1;

  double min = q1 - (iqr * 1.5);
  double max = q3 + (iqr * 1.5);

  int ret = 0;

  for (int i = 0; i < len; i++)
    {
      if ((vals[i] < min) || (vals[i] > max))
	{
	  printf ("%f is not in (%f, %f)\n", vals[i], min, max);
	  ret++;
	}
    }

  return ret;
}

bool
low_variance (double *vals, int len)
{
  return (coefficient_of_variance (vals, len) < 1);
}

bool
no_outliers (double *vals, int len)
{
  return (outliers (vals, len) == 0);
}
