# Copy certificates for running the samples and tests provided with the SDK into "project/common/certs_protected/aws", 
# filename "aws_iot_cert_rtos.h". 
# Certificates can be created and downloaded from the AWS IoT Console.
# The IoT Client takes the full path of the certificates as an input parameter while initializing
# This is the default folder for the certificates only for samples and tests. 
# Please make your certification follow as example, put the file named "aws_iot_cert_rtos.h" 
# into "project/common/certs_protected/aws".
# makefile: USE_TEST_CERT := $(strip $(wildcard $(SOURCE_DIR)/project/common/certs_protected/aws))
# ifneq ($(USE_TEST_CERT),)
# CFLAGS += -I$(SOURCE_DIR)/project/common/certs_protected/aws
# else
# CFLAGS += -I$(SOURCE_DIR)/project/common/certs/aws
# endif


# example: 
# #define AWS_IOT_ROOT_CA_CERT                                            \
# "-----BEGIN CERTIFICATE-----\r\n"                                       \
# "MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB\r\n"  \
# ......................................................................  \
# "hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq\r\n"              \
# "-----END CERTIFICATE-----"

# #define AWS_IOT_DEVICE_CERT                                             \
# "-----BEGIN CERTIFICATE-----\r\n"                                       \
# "MIIDWjCCAkKgAwIBAgIVALN9uyFY8vDLucVT/NqpBOksTw+SMA0GCSqGSIb3DQEB\r\n"  \
# ......................................................................  \
# "Xwi1SNKgtXovRE0NrHt1b7XSYGfGEp0NaWoEgogcHFgV51gMd+OfixHyOxfDkA==\r\n"  \
# "-----END CERTIFICATE-----"


# #define AWS_IOT_PRIVATE_KEY                                             \
# "-----BEGIN RSA PRIVATE KEY-----\r\n"                                   \
# "MIIEogIBAAKCAQEAkt0FFfNFnTl+PX4ksiaAIA06XlKx3PLLX9xlmtVwL4gQV2EK\r\n"  \
# ......................................................................  \
# "M2JpnMWZY7DeL1HBK2qSwtAVdoP67pRbWzZpXHJv/DdkV/zRkd4=\r\n"              \
# "-----END RSA PRIVATE KEY-----"
