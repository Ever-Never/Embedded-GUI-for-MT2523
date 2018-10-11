
###################################################
# Sources
LWIP_SRC = middleware/third_party/lwip

C_FILES  += $(LWIP_SRC)/src/api/api_lib.c           \
            $(LWIP_SRC)/src/api/api_msg.c           \
            $(LWIP_SRC)/src/api/err.c               \
            $(LWIP_SRC)/src/api/netbuf.c            \
            $(LWIP_SRC)/src/api/netdb.c             \
            $(LWIP_SRC)/src/api/netifapi.c          \
            $(LWIP_SRC)/src/api/sockets.c           \
            $(LWIP_SRC)/src/api/tcpip.c             \
            $(LWIP_SRC)/src/core/def.c              \
            $(LWIP_SRC)/src/core/dhcp.c             \
            $(LWIP_SRC)/src/core/dns.c              \
            $(LWIP_SRC)/src/core/inet_chksum.c      \
            $(LWIP_SRC)/src/core/init.c             \
            $(LWIP_SRC)/src/core/mem.c              \
            $(LWIP_SRC)/src/core/memp.c             \
            $(LWIP_SRC)/src/core/netif.c            \
            $(LWIP_SRC)/src/core/raw.c              \
            $(LWIP_SRC)/src/core/stats.c            \
            $(LWIP_SRC)/src/core/sys.c              \
            $(LWIP_SRC)/src/core/tcp.c              \
            $(LWIP_SRC)/src/core/tcp_in.c           \
            $(LWIP_SRC)/src/core/lwip_timers.c      \
            $(LWIP_SRC)/src/core/udp.c              \
            $(LWIP_SRC)/src/core/ipv4/autoip.c      \
            $(LWIP_SRC)/src/core/ipv4/icmp.c        \
            $(LWIP_SRC)/src/core/ipv4/igmp.c        \
            $(LWIP_SRC)/src/core/ipv4/ip4.c         \
            $(LWIP_SRC)/src/core/ipv4/ip4_addr.c    \
            $(LWIP_SRC)/src/core/ipv4/ip_frag.c     \
            $(LWIP_SRC)/src/core/ipv6/dhcp6.c       \
            $(LWIP_SRC)/src/core/ipv6/ethip6.c      \
            $(LWIP_SRC)/src/core/ipv6/icmp6.c       \
            $(LWIP_SRC)/src/core/ipv6/inet6.c       \
            $(LWIP_SRC)/src/core/ipv6/ip6.c         \
            $(LWIP_SRC)/src/core/ipv6/ip6_addr.c    \
            $(LWIP_SRC)/src/core/ipv6/ip6_frag.c    \
            $(LWIP_SRC)/src/core/ipv6/mld6.c        \
            $(LWIP_SRC)/src/core/ipv6/nd6.c         \
            $(LWIP_SRC)/src/netif/etharp.c

ifeq ($(MTK_MODEM_ENABLE), y)
C_FILES     += $(LWIP_SRC)/ports/psnetif.c
CFLAGS += -DMTK_TCPIP_FOR_EXTERNAL_MODULE_ENABLE
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_wifi5931_dev/inc/modem_demo/cmux
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_wifi5931_dev/inc/modem_demo/sio
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_wifi5931_dev/inc/modem_demo/sio_uart
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_wifi5931_dev/inc/modem_demo/app
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_example/inc/modem_demo/cmux
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_example/inc/modem_demo/sio
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_example/inc/modem_demo/sio_uart
CFLAGS += -I$(SOURCE_DIR)/project/mt2523_hdk/apps/iot_modem_example/inc/modem_demo/app
endif
ifeq ($(MTK_WIFI_CHIP_USE_MT5931),y)
C_FILES     += $(LWIP_SRC)/ports/mt5931_netif.c
CFLAGS += -DTCPIP_FOR_MT5931_ENABLE
CFLAGS += -I$(SOURCE_DIR)/driver/board/mt25x3_hdk/wifi/mt5931/inc
endif
ifeq ($(MTK_WIFI_CHIP_USE_MT5932),y)
C_FILES     += $(LWIP_SRC)/ports/mt5932_netif.c
endif
ifneq ($(MTK_WIFI_CHIP_USE_MT5931), y)
ifneq ($(MTK_WIFI_CHIP_USE_MT5932), y)
ifneq ($(MTK_MODEM_ENABLE), y)
C_FILES     += $(LWIP_SRC)/ports/ethernetif.c
endif
endif
endif
C_FILES     += $(LWIP_SRC)/ports/sys_arch.c
C_FILES     += $(LWIP_SRC)/ports/lwip_log.c
C_FILES     += $(LWIP_SRC)/ports/pbuf.c
C_FILES     += $(LWIP_SRC)/ports/tcp_out.c


ifeq ($(MTK_MINICLI_ENABLE),y)
C_FILES     += $(LWIP_SRC)/ports/lwip_cli.c
endif


###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/ports/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/lwip
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/lwip/src/include/netif
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/nvdm/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/minicli/inc

