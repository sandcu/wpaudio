/*
 * libmad - MPEG audio decoder library
 */

# ifndef LIBMAD_HUFFMAN_H
# define LIBMAD_HUFFMAN_H

union huffquad {
  struct {
    unsigned short final  :  1;
    unsigned short bits   :  3;
    unsigned short offset : 12;
  } ptr;
  struct {
    unsigned short final  :  1;
    unsigned short hlen   :  3;
    unsigned short v      :  1;
    unsigned short w      :  1;
    unsigned short x      :  1;
    unsigned short y      :  1;
  } value;
  unsigned short final    :  1;
};

union huffpair {
  struct {
    unsigned short final  :  1;
    unsigned short bits   :  3;
    unsigned short offset : 12;
  } ptr;
  struct {
    unsigned short final  :  1;
    unsigned short hlen   :  3;
    unsigned short x      :  4;
    unsigned short y      :  4;
  } value;
  unsigned short final    :  1;
};

struct hufftable {
  union huffpair const *table;
  unsigned short linbits;
  unsigned short startbits;
};

extern union huffquad const *const mad_huff_quad_table[2];
extern struct hufftable const mad_huff_pair_table[32];

# endif
