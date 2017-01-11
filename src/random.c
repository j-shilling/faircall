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
#include <time.h>
#include <limits.h>

int
get_rand_int (int min, int max)
{
  double val = 0;
  int r;

    {
      /*
       * Generate a random number using
       * /dev/urandom if possible.
       */
      FILE *f = fopen ("/dev/urandom", "r");

      if (!f)
	goto fallback;

      unsigned char rand = 0;
      size_t ret = fread (&rand, sizeof(rand), 1, f);

      if (ret != 1)
	goto fallback;

      val = (double) rand / (double) ((long) UCHAR_MAX + 1);
    }

  end: r = (int) (val * (max - min + 1));

  return r + min;

  fallback:
    {
      /*
       * If some problem occured earlier
       * come here to generate val in a less
       * uniform way.
       */
      srand (time (NULL));
      val = (double) rand () / (double) RAND_MAX;
      goto end;
    }
}
