#ifndef __RT_PKT_H__
#define __RT_PKT_H__

//#include "connsys_driver.h"

//#include <os/includes.h>
//#include "sys_hal_types.h"
// Note: packet buffer size should be  multiple of (L1_CACHE_BYTES)!!

//#define NET_BUF_TRUE_SIZE_1024	1536
#ifdef RX_MAX_2KB
#define NET_BUF_TRUE_SIZE_2048	1600 //1536 ///1600 //2048
#else
#define NET_BUF_TRUE_SIZE_2048	1600 ///1600 //2048
#endif // RX_MAX_2KB
#define NET_BUF_TRUE_SIZE_4096	4096


#ifndef NET_BUF_TRUE_SIZE_128
#define NET_BUF_TRUE_SIZE_128	128
#endif
#ifndef NET_BUF_TRUE_SIZE_256
#define NET_BUF_TRUE_SIZE_256	256
#endif
#ifndef NET_BUF_TRUE_SIZE_512
#define NET_BUF_TRUE_SIZE_512	512
#endif
#ifndef NET_BUF_TRUE_SIZE_1024
#define NET_BUF_TRUE_SIZE_1024	1024
#endif
#ifndef NET_BUF_TRUE_SIZE_2048
#define NET_BUF_TRUE_SIZE_2048	1600
#endif
#ifndef NET_BUF_TRUE_SIZE_4096
#define NET_BUF_TRUE_SIZE_4096	4096
#endif

/* six different buffer size : 128 ~ 4096 */
#define NET_BUF_N_128			4 //82 //10+72(6 NET_BUF_N_2048) //10 //64
#define NET_BUF_N_256			10 //64
#define NET_BUF_N_512			12
#define NET_BUF_N_1024		0 //32

#define NET_BUF_N_2048		32 //64 //wayne_note 28    
#define NET_BUF_N_4096		0



#define NET_BUF_128			0
#define NET_BUF_256			1
#define NET_BUF_512			2
#define NET_BUF_1024			3
#define NET_BUF_2048			4
#define NET_BUF_4096			5

#define NET_BUF_MAX_IDX		NET_BUF_4096
#define NET_BUF_MIN_IDX		NET_BUF_128

#define NET_BUF_TYPES		 (NET_BUF_MAX_IDX-NET_BUF_MIN_IDX+1)

#define NET_BUF_ALIGN_SIZE	1 //16
#define RT_MEM_ALIGN(x, size)	( ((unsigned long)x+(size-1)) & ~(size-1) )

#define NUM_PKT_BLK	\
	(10+(NET_BUF_N_128+NET_BUF_N_256+NET_BUF_N_512+\
		NET_BUF_N_1024+NET_BUF_N_2048+NET_BUF_N_4096))



#if 1 //George To be enabled during clean up

#if (MT7687_BORA00005120 == 1)
#undef NET_BUF_N_2048
#if (MT7687_BORA00005292 == 1)
#define NET_BUF_N_2048 128
#else
#define NET_BUF_N_2048 64 //NET_BUF_N_2048=5
#endif

#else
#undef NET_BUF_N_2048
#define NET_BUF_N_2048 32 //NET_BUF_N_2048=5
#endif

#if (MT7687_BORA00005188 == 1)
#undef NET_BUF_N_512
#define NET_BUF_N_512 12
#endif

#undef NET_BUF_N_256
#define NET_BUF_N_256 10 //NET_BUF_N_2048=5

#undef NUM_PKT_BLK
#define NUM_PKT_BLK	\
			(10+(NET_BUF_N_128+NET_BUF_N_256+NET_BUF_N_512+\
				NET_BUF_N_1024+NET_BUF_N_2048+NET_BUF_N_4096))

#if (MT7687_BORA00004763 == 1)
#ifdef NET_BUF_TRUE_SIZE_2048
#undef NET_BUF_TRUE_SIZE_2048
#define NET_BUF_TRUE_SIZE_2048 1600
#else
#define NET_BUF_TRUE_SIZE_2048 1600
#endif
#endif

#endif




struct head_list  {
	struct node_obj*		next;
	int 			qlen;	
#ifdef DEBUG
	unsigned int 		min_qlen;
	unsigned int		fail;
#endif
};

struct node_obj {
	struct node_obj*		next;			
	struct head_list*		list;
#ifdef DEBUG
	unsigned int 		min_qlen;
	unsigned int		fail;
#endif
};

#if 0//Navaneeth IP stack integration

/*TODO NAV - Move defines from here*/
#if 0 // George-Removed as the lwip header file has been added
typedef enum {
  PBUF_TRANSPORT,
  PBUF_IP,
  PBUF_LINK,
  PBUF_RAW_TX,
  PBUF_RAW
} pbuf_layer;

typedef enum {
  /** pbuf data is stored in RAM, used for TX mostly, struct pbuf and its payload
      are allocated in one piece of contiguous memory (so the first payload byte
      can be calculated from struct pbuf)
      pbuf_alloc() allocates PBUF_RAM pbufs as unchained pbufs (although that might
      change in future versions) */
  PBUF_RAM,
  /** pbuf data is stored in ROM, i.e. struct pbuf and its payload are located in
      totally different memory areas. Since it points to ROM, payload does not
      have to be copied when queued for transmission. */
  PBUF_ROM,
  /** pbuf comes from the pbuf pool. Much like PBUF_ROM but payload might change
      so it has to be duplicated when queued before transmitting, depending on
      who has a 'ref' to it. */
  PBUF_REF,
  /** pbuf payload refers to RAM. This one comes from a pool and should be used
      for RX. Payload can be chained (scatter-gather RX) but like PBUF_RAM, struct
      pbuf and its payload are allocated in one piece of contiguous memory (so
      the first payload byte can be calculated from struct pbuf) */
  PBUF_POOL
} pbuf_type;


/** indicates this packet's data should be immediately passed to the application */
#define PBUF_FLAG_PUSH      0x01U
/** indicates this is a custom pbuf: pbuf_free calls pbuf_custom->custom_free_function()
    when the last reference is released (plus custom PBUF_RAM cannot be trimmed) */
#define PBUF_FLAG_IS_CUSTOM 0x02U
/** indicates this pbuf is UDP multicast to be looped back */
#define PBUF_FLAG_MCASTLOOP 0x04U
/** indicates this pbuf was received as link-level broadcast */
#define PBUF_FLAG_LLBCAST   0x08U
/** indicates this pbuf was received as link-level multicast */
#define PBUF_FLAG_LLMCAST   0x10U
/** indicates this pbuf includes a TCP FIN flag */
#define PBUF_FLAG_TCP_FIN   0x20U


//extern wpa_supplicant_api_callback __g_wpa_supplicant_api;
enum CONNSYS_HEADROOM_OFFSET_TYPE {
    CONNSYS_HEADROOM_OFFSET_NON_QOS, /* 0 */
    CONNSYS_HEADROOM_OFFSET_QOS,     /* 1 */
};



#endif
struct ppbuf {
  /** next pbuf in singly linked pbuf chain */
  struct pbuf *next;

  /** pointer to the actual data in the buffer */
  void *payload;

  /**
   * total length of this buffer and all next buffers in chain
   * belonging to the same packet.
   *
   * For non-queue packet chains this is the invariant:
   * p->tot_len == p->len + (p->next? p->next->tot_len: 0)
   */
  UINT_16 tot_len;

  /** length of this buffer */
  UINT_16 len;

  /** pbuf_type as u8_t instead of enum to save space */
  UINT_8 /*pbuf_type*/ type;

  /** misc flags */
  UINT_8 flags;

  /**
   * the reference count always equals the number of pointers
   * that refer to this pbuf. This can be pointers from an application,
   * the stack itself, or pbuf->next pointers from a chain.
   */
  UINT_16 ref;
};

#endif

struct pbuf_boson {
  /** next pbuf in singly linked pbuf chain */
  struct pbuf_boson *next;

  /** pointer to the actual data in the buffer */
  void *payload;

  /**
   * total length of this buffer and all next buffers in chain
   * belonging to the same packet.
   *
   * For non-queue packet chains this is the invariant:
   * p->tot_len == p->len + (p->next? p->next->tot_len: 0)
   */
  unsigned short tot_len;

  /** length of this buffer */
  unsigned short len;

  /** pbuf_type as u8_t instead of enum to save space */
  unsigned char /*pbuf_type*/ type;

  /** misc flags */
  unsigned char flags;

  /**
   * the reference count always equals the number of pointers
   * that refer to this pbuf. This can be pointers from an application,
   * the stack itself, or pbuf->next pointers from a chain.
   */
  unsigned short ref;
};

extern struct pbuf_boson *(*pbuf_alloc_boson)(unsigned short length);
extern unsigned char (*pbuf_free_boson)(struct pbuf_boson *p);

struct net_buf_blk {
	/* These two member must be first */
	struct net_buf_blk*		 next;			
	struct net_buf_blk_head* list;
	unsigned int				size;
	unsigned int				ref;
	unsigned int				idx;	
	unsigned int				fid;	// FID of PSE buffer
	unsigned char* 			buf;
#if 1//Navaneeth IP stack integration
	struct pbuf_boson* pbuf_pkt;
#endif	
};

typedef struct net_buf_blk * 	PBUF_BLK_OBJ;
typedef char *					PBUF_OBJ;

//char*	net_buf_blk_mem;

#define NET_PKT_CB_SIZE			20
 						
struct netif;

struct net_pkt_blk {
	struct net_pkt_blk*		next;			
	struct net_pkt_blk_head* list;
	struct netif*			dev;
	unsigned int			len;
	unsigned char*			head;	/* Head of buffer    */
	unsigned char*			data;	/* Data head pointer */
	unsigned char*			tail;	/* Tail pointer      */
	unsigned char*			end;	/* End pointer       */
	struct net_buf_blk* 	buf_blk;
	unsigned char			cb[NET_PKT_CB_SIZE]; 
};


struct net_pkt_blk_head {
	struct net_pkt_blk*	next;
	unsigned int 		qlen;
#ifdef DEBUG
	unsigned int 		min_qlen;
	unsigned int		fail;
#endif
};


struct net_buf_blk_head {
	struct net_buf_blk*	next;
	unsigned int 		qlen;
#ifdef DEBUG
	unsigned int 		min_qlen;
	unsigned int		fail;
#endif
};




struct net_buf_type  {
	unsigned int	nblk;
	char*	buf_mem;
	struct net_buf_blk_head freelist;
	struct net_pkt_blk_head pkt_list;
};


typedef struct net_pkt_blk	RT_PKT;

static inline int net_pkt_headroom(const struct net_pkt_blk *pkt)
{
	return pkt->data - pkt->head;
}



#if 0
int netlib_packet_buffer_init(void);
void net_pkt_reserve(struct net_pkt_blk *pkt_blk, int len);
struct net_pkt_blk * net_pkt_get(int size_idx, UINT_16 u2PseFid);
struct net_pkt_blk * net_pkt_alloc(int size_idx);
struct net_pkt_blk *net_pkt_copy(struct net_pkt_blk *pkt_blk);
struct net_pkt_blk * net_pkt_clone(struct net_pkt_blk *pkt);
unsigned char * net_pkt_put(struct net_pkt_blk *pkt, unsigned int len);
unsigned char * net_pkt_push(struct net_pkt_blk *pkt, unsigned int len);
unsigned char * net_pkt_pull(struct net_pkt_blk *pkt, unsigned int len);
void net_pkt_free(struct net_pkt_blk * pkt_blk);
void net_pkt_show(void);
#endif

#define	NET_ENQUEUE(_head, _node)	\
	net_enqueue((struct head_list* )_head, (struct node_obj* )_node)
#define	NET_ENQUEUE_UNLOCK(_head, _node)	\
	net_enqueue_unlock((struct head_list* )_head, (struct node_obj* )_node)
#define NET_DEQUEUE(_head, _node) \
	_node = (typeof(*_node) *) net_dequeue((struct head_list *)(_head))

#define NUM_PKT_BLK	\
	(10+(NET_BUF_N_128+NET_BUF_N_256+NET_BUF_N_512+\
		NET_BUF_N_1024+NET_BUF_N_2048+NET_BUF_N_4096))
extern struct net_pkt_blk * (*net_pkt_get_87)(int size_idx,void *buffer);

#endif /* __RT_PKT_H__ */
