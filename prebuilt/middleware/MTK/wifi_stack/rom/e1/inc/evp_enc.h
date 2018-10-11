#ifndef _EVP_ENC_H
#define _EVP_ENC_H


#if 1
/*
//#undef SWAP32
//#define SWAP32(x) \
//    ((unsigned long)( \
//    (((unsigned long)(x) & (unsigned long) 0x000000ffUL) << 24) | \
//    (((unsigned long)(x) & (unsigned long) 0x0000ff00UL) <<  8) | \
//    (((unsigned long)(x) & (unsigned long) 0x00ff0000UL) >>  8) | \
//    (((unsigned long)(x) & (unsigned long) 0xff000000UL) >> 24) ))
*/
#define GETU32(p) cpu2be32(get_unaligned((u32 *) (p))) //cpu2be32((*(u32 *)(p)))
#define PUTU32(ct, st) put_unaligned(cpu2be32(st), (u32*)(ct)) //{ *((u32 *)(ct)) = cpu2be32((st)); }    
#else

#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_AMD64) || defined(_M_X64))
# define SWAP(x) (_lrotl(x, 8) & 0x00ff00ff | _lrotr(x, 8) & 0xff00ff00)
# define GETU32(p) SWAP(*((u32 *)(p)))
# define PUTU32(ct, st) { *((u32 *)(ct)) = SWAP((st)); }
#else
# define GETU32(pt) (((u32)(pt)[0] << 24) ^ ((u32)(pt)[1] << 16) ^ ((u32)(pt)[2] <<  8) ^ ((u32)(pt)[3]))
# define PUTU32(ct, st) { (ct)[0] = (u8)((st) >> 24); (ct)[1] = (u8)((st) >> 16); (ct)[2] = (u8)((st) >>  8); (ct)[3] = (u8)(st); }
#endif

#endif



#define AES_KEY128_LENGTH 16


#define AES_ENCRYPT	1
#define AES_DECRYPT	0

/* Because array size can't be a const in C, the following two are macros.
   Both sizes are in bytes. */
#define AES_MAXNR 14
#define AES_BLOCK_SIZE 16

/* This should be a hidden type, but EVP requires that the size be known */
struct aes_key_st {
#ifdef AES_LONG
    unsigned long rd_key[4 *(AES_MAXNR + 1)];
#else
    unsigned int rd_key[4 *(AES_MAXNR + 1)];
#endif
    int rounds;
};
typedef struct aes_key_st AES_KEY;

typedef struct _EVP_CIPHER_CTX_ {
	unsigned long flag;
	unsigned long type;  
	unsigned long encrypt; //1: Encrypt 0: Decrypt, 
	unsigned char key[16];
	unsigned char iv[8 + 16];
	unsigned long bufferlen;
	unsigned char buffer[AES_BLOCK_SIZE];
	AES_KEY        aesKey;
} EVP_CIPHER_CTX, *PEVP_CIPHER_CTX;

#endif  /* _EVP_ENC_H */
