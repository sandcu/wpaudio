

# ifndef LIBMAD_SYNTH_H
# define LIBMAD_SYNTH_H

# include "fixed.h"
# include "frame.h"

# ifdef __cplusplus
extern "C" {
# endif
struct mad_pcm {
  unsigned int samplerate;		/* 取样率 (Hz) */
  unsigned short channels;		/* 声道数 */
  unsigned short length;		/* 每声道取样数 */
  mad_fixed_t samples[2][1152];		/* PCM 输出取样 [ch][sample] */
};

struct mad_synth {
  mad_fixed_t filter[2][2][2][16][8];	/* 多相滤波输出 */
  					/* [ch][eo][peo][s][v] */

  unsigned int phase;			/* 当前处理阶段 */

  struct mad_pcm pcm;			/* PCM 输出 */
};

/* 单声道 PCM 选择 */
enum {
  MAD_PCM_CHANNEL_SINGLE = 0
};

/* 双声道 PCM 选择 */
enum {
  MAD_PCM_CHANNEL_DUAL_1 = 0,
  MAD_PCM_CHANNEL_DUAL_2 = 1
};

/* 立体声 PCM 选择 */
enum {
  MAD_PCM_CHANNEL_STEREO_LEFT  = 0,
  MAD_PCM_CHANNEL_STEREO_RIGHT = 1
};

void mad_synth_init(struct mad_synth *);

# define mad_synth_finish(synth)  /* nothing */

void mad_synth_mute(struct mad_synth *);

void mad_synth_frame(struct mad_synth *, struct mad_frame const *);

# ifdef __cplusplus
}
# endif

# endif


