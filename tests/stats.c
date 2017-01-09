#include <stdlib.h>
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
coefficient_of_varience (double *vals, int len)
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

#ifdef TEST_STATS_

#include <stdio.h>

void
main()
{
	double vals[] = {
		2.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0
	};

	int len = 7;

	printf ("median = %f\nmean = %f\nstdev = %f\n",
			median (vals, len),
			mean (vals, len),
			stdev (vals, len));
}

#endif
