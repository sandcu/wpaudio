/*
 * libmad - MPEG audio decoder library
 */

# ifndef LIBMAD_DECODER_H
# define LIBMAD_DECODER_H

# include "stream.h"
# include "frame.h"
# include "synth.h"

enum mad_decoder_mode {
  MAD_DECODER_MODE_SYNC  = 0,
  MAD_DECODER_MODE_ASYNC
};

enum mad_flow {
  MAD_FLOW_CONTINUE = 0x0000,	/* 正常进行 */
  MAD_FLOW_STOP     = 0x0010,	/* 正常结束*/
  MAD_FLOW_BREAK    = 0x0011,	/* 结束解码，显示错误 */
  MAD_FLOW_IGNORE   = 0x0020	/* 忽略当前祯 */
};

struct ASYNC_STRUCT {
	long pid;
	int in;
	int out;
} ; 

struct SYNC_STRUCT {
	struct mad_stream stream;
	struct mad_frame frame;
	struct mad_synth synth;
} ; 

struct mad_decoder {
  enum mad_decoder_mode mode;

  int options;

  struct ASYNC_STRUCT async;

  struct SYNC_STRUCT* sync;

  void *cb_data;

  enum mad_flow (*input_func)(void *, struct mad_stream *);
  enum mad_flow (*header_func)(void *, struct mad_header const *);
  enum mad_flow (*filter_func)(void *,
			       struct mad_stream const *, struct mad_frame *);
  enum mad_flow (*output_func)(void *,
			       struct mad_header const *, struct mad_pcm *);
  enum mad_flow (*error_func)(void *, struct mad_stream *, struct mad_frame *);
  enum mad_flow (*message_func)(void *, void *, unsigned int *);
};

void mad_decoder_init(struct mad_decoder *, void *,
		      enum mad_flow (*)(void *, struct mad_stream *),
		      enum mad_flow (*)(void *, struct mad_header const *),
		      enum mad_flow (*)(void *,
					struct mad_stream const *,
					struct mad_frame *),
		      enum mad_flow (*)(void *,
					struct mad_header const *,
					struct mad_pcm *),
		      enum mad_flow (*)(void *,
					struct mad_stream *,
					struct mad_frame *),
		      enum mad_flow (*)(void *, void *, unsigned int *));
int mad_decoder_finish(struct mad_decoder *);

# define mad_decoder_options(decoder, opts)  \
    ((void) ((decoder)->options = (opts)))

int mad_decoder_run(struct mad_decoder *, enum mad_decoder_mode);
int mad_decoder_message(struct mad_decoder *, void *, unsigned int *);

# endif
