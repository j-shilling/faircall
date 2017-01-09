#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

#include "stats.h"

static int
doublecmp (const void *a, const void *b)
{
	return ( *(double *)a - * (double *)b );
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
	double *deviations = (double *) malloc (sizeof (double) * len);

	for (int i = 0; i < len; i++)
	{
		deviations[i] =
			(vals[i] - m) * (vals[i] - m);
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
	qsort (vals, len, sizeof (double), doublecmp);

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
	qsort (vals, len, sizeof (double), doublecmp);

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
	qsort (vals, len, sizeof (double), doublecmp);

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
			ret ++;
	}

	return ret;
}

bool
low_variance(double *vals, int len)
{
	return (coefficient_of_variance(vals, len) < 1);
}

bool
no_outliers (double *vals, int len)
{
	return (outliers (vals, len) == 0);
}

#ifdef TEST_STATS_

#include <stdio.h>

void
main()
{
	double vals[] = {
		1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0
	};

	int len = 8;

	printf ("median (4.5) = %f\n", median (vals, len));
	printf ("lowq (2.5) = %f\n", lowq(vals, len));
	printf ("upq (6.5) = %f\n", upq(vals, len));
}

#endif
