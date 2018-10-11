#ifndef _NET_H
#define _NET_H

struct netif {
	/** pointer to next in linked list */
	struct netif *next;

	int (* pkt_xmit)(struct netif *netif, void *p);
	void  (* pkt_tx_done)(struct netif *netif);
	void  (* pkt_recv)(struct netif *netif, int q);
	int   (*init)(struct netif *netif);
	int   (*open)(struct netif *netif);
	int   (*close)(struct netif *netif);

	/** This field can be set by the device driver and could point
	*  to state information for the device. */
	void *adapter; //state
	/** number of this interface */
	unsigned char num;
#ifdef IOT_CFG80211_SUPPORT
	UCHAR IfType;
	UCHAR name[10];
#endif /* IOT_CFG80211_SUPPORT */
	UCHAR dev_addr[MAC_ADDR_LEN];
	UCHAR OpMode;
};

extern struct netif *
(*netif_add_wifi_n9)(struct netif *netif, void *adapter, int (* setup)(struct netif *netif));
void netif_set_default_wifi_n9(struct netif *netif);

void netif_init_wifi_n9(void);


#endif /* _NET_H */
