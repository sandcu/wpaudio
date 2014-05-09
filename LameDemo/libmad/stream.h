# ifndef LIBMAD_STREAM_H
# define LIBMAD_STREAM_H

# include "bit.h"
# ifdef __cplusplus
extern "C" {
# endif

# define MAD_BUFFER_GUARD	8
# define MAD_BUFFER_MDLEN	(511 + 2048 + MAD_BUFFER_GUARD)

enum mad_error {
  MAD_ERROR_NONE	   = 0x0000,	

  MAD_ERROR_BUFLEN	   = 0x0001,	
  MAD_ERROR_BUFPTR	   = 0x0002,	

  MAD_ERROR_NOMEM	   = 0x0031,	

  MAD_ERROR_LOSTSYNC	   = 0x0101,	
  MAD_ERROR_BADLAYER	   = 0x0102,	
  MAD_ERROR_BADBITRATE	   = 0x0103,	
  MAD_ERROR_BADSAMPLERATE  = 0x0104,	
  MAD_ERROR_BADEMPHASIS	   = 0x0105,

  MAD_ERROR_BADCRC	   = 0x0201,	
  MAD_ERROR_BADBITALLOC	   = 0x0211,
  MAD_ERROR_BADSCALEFACTOR = 0x0221,
  MAD_ERROR_BADFRAMELEN	   = 0x0231,	
  MAD_ERROR_BADBIGVALUES   = 0x0232,	
  MAD_ERROR_BADBLOCKTYPE   = 0x0233,	
  MAD_ERROR_BADSCFSI	   = 0x0234,	
  MAD_ERROR_BADDATAPTR	   = 0x0235,	
  MAD_ERROR_BADPART3LEN	   = 0x0236,	
  MAD_ERROR_BADHUFFTABLE   = 0x0237,	
  MAD_ERROR_BADHUFFDATA	   = 0x0238,	
  MAD_ERROR_BADSTEREO	   = 0x0239	
};

# define MAD_RECOVERABLE(error)	((error) & 0xff00)

struct mad_stream {
  unsigned char const *buffer;	
  unsigned char const *bufend;	
  unsigned long skiplen;		

  int sync;				
  unsigned long freerate;		

  unsigned char const *this_frame;	
  unsigned char const *next_frame;	
  struct mad_bitptr ptr;		

  struct mad_bitptr anc_ptr;		
  unsigned int anc_bitlen;		

  unsigned char (*main_data)[MAD_BUFFER_MDLEN];
					
  unsigned int md_len;		

  int options;				
  enum mad_error error;			
};

enum {
  MAD_OPTION_IGNORECRC      = 0x0001,	
  MAD_OPTION_HALFSAMPLERATE = 0x0002	
# if 0  
  MAD_OPTION_LEFTCHANNEL    = 0x0010,	
  MAD_OPTION_RIGHTCHANNEL   = 0x0020,	
  MAD_OPTION_SINGLECHANNEL  = 0x0030	
# endif
};

void mad_stream_init(struct mad_stream *);
void mad_stream_finish(struct mad_stream *);

# define mad_stream_options(stream, opts)  \
    ((void) ((stream)->options = (opts)))

void mad_stream_buffer(struct mad_stream *,
		       unsigned char const *, unsigned long);
void mad_stream_skip(struct mad_stream *, unsigned long);

int mad_stream_sync(struct mad_stream *);

char const *mad_stream_errorstr(struct mad_stream const *);

# ifdef __cplusplus
}
# endif

# endif


