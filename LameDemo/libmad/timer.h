
# ifndef LIBMAD_TIMER_H
# define LIBMAD_TIMER_H

# ifdef __cplusplus
extern "C" {
# endif

typedef struct {
  signed long seconds;		
  unsigned long fraction;
} mad_timer_t;

extern mad_timer_t const mad_timer_zero;

# define MAD_TIMER_RESOLUTION	352800000UL

enum mad_units {
  MAD_UNITS_HOURS	 =    -2,
  MAD_UNITS_MINUTES	 =    -1,
  MAD_UNITS_SECONDS	 =     0,


  MAD_UNITS_DECISECONDS	 =    10,
  MAD_UNITS_CENTISECONDS =   100,
  MAD_UNITS_MILLISECONDS =  1000,


  MAD_UNITS_8000_HZ	 =  8000,
  MAD_UNITS_11025_HZ	 = 11025,
  MAD_UNITS_12000_HZ	 = 12000,

  MAD_UNITS_16000_HZ	 = 16000,
  MAD_UNITS_22050_HZ	 = 22050,
  MAD_UNITS_24000_HZ	 = 24000,

  MAD_UNITS_32000_HZ	 = 32000,
  MAD_UNITS_44100_HZ	 = 44100,
  MAD_UNITS_48000_HZ	 = 48000,


  MAD_UNITS_24_FPS	 =    24,
  MAD_UNITS_25_FPS	 =    25,
  MAD_UNITS_30_FPS	 =    30,
  MAD_UNITS_48_FPS	 =    48,
  MAD_UNITS_50_FPS	 =    50,
  MAD_UNITS_60_FPS	 =    60,


  MAD_UNITS_75_FPS	 =    75,


  MAD_UNITS_23_976_FPS	 =   -24,
  MAD_UNITS_24_975_FPS	 =   -25,
  MAD_UNITS_29_97_FPS	 =   -30,
  MAD_UNITS_47_952_FPS	 =   -48,
  MAD_UNITS_49_95_FPS	 =   -50,
  MAD_UNITS_59_94_FPS	 =   -60
};

# define mad_timer_reset(timer)	((void) (*(timer) = mad_timer_zero))

int mad_timer_compare(mad_timer_t, mad_timer_t);

# define mad_timer_sign(timer)	mad_timer_compare((timer), mad_timer_zero)

void mad_timer_negate(mad_timer_t *);
mad_timer_t mad_timer_abs(mad_timer_t);

void mad_timer_set(mad_timer_t *, unsigned long, unsigned long, unsigned long);
void mad_timer_add(mad_timer_t *, mad_timer_t);
void mad_timer_multiply(mad_timer_t *, signed long);

signed long mad_timer_count(mad_timer_t, enum mad_units);
unsigned long mad_timer_fraction(mad_timer_t, unsigned long);
void mad_timer_string(mad_timer_t, char *, char const *,
		      enum mad_units, enum mad_units, unsigned long);

# ifdef __cplusplus
}
# endif

# endif


