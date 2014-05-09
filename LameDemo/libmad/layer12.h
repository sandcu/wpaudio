/*
 * libmad - MPEG audio decoder library
 */

# ifndef LIBMAD_LAYER12_H
# define LIBMAD_LAYER12_H

# include "stream.h"
# include "frame.h"

int mad_layer_I(struct mad_stream *, struct mad_frame *);
int mad_layer_II(struct mad_stream *, struct mad_frame *);

# endif
