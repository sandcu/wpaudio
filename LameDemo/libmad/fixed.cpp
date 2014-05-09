/*
 * libmad - MPEG audio decoder library

 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include "fixed.h"

/*
 * NAME:	fixed->abs()
 * DESCRIPTION:	return absolute value of a fixed-point number
 */
mad_fixed_t mad_f_abs(mad_fixed_t x)
{
  return x < 0 ? -x : x;
}
