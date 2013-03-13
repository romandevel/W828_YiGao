/**
 * SSL 的接口函数,主要用于https
 * 目前对OPENSSL不熟悉，暂采用全局变量进行保存ssl的句柄，并采用仅允许开辟一个ssl，需及时释放
 */
#include "https.h"
#ifdef HAVE_SSL


static SSL_CTX *gpSSL_ctx = NULL;    //SSL上下文

/**
 * 初始化SSL上下文
 */
int protocol_https_init(void)
{
    Ssl_InitToSdram();      //初始化static var
	SSL_library_init();
	//ERR_load_ERR_strings();
    gpSSL_ctx = SSL_CTX_new(SSLv23_client_method()) ;
    if (gpSSL_ctx == NULL)
    	return HYWB_ERR;
	return HYWB_OK;
}

/**
 * 释放上下文资源
 */
int protocol_https_destroy(void)
{
	SSL_CTX_free(gpSSL_ctx);
	CRYPTO_cleanup_all_ex_data();
	EVP_cleanup();
	//ERR_free_strings();
	return HYWB_OK;
}

/**
 * 获取SSL上下文
 */
SSL_CTX *protocol_https_get_ssl_ctx(void)
{
	return gpSSL_ctx;
}

/**
 * 释放ssl资源
 */
int protocol_https_close(int sock, SSL *ssl)
{
	int ret;
    ret = SSL_shutdown(ssl);
    if (ret != 1) {
        SSL_shutdown(ssl);
    }
    closesocket(sock);
    SSL_free(ssl);
    return HYWB_OK;
}

/**
 * ssl进行握手连接
 */
SSL *protocol_https_connect(int sock)
{
    SSL *ssl;
    SSL_CTX *ctx;
    int ret;
   
	ctx = protocol_https_get_ssl_ctx();
    ssl = SSL_new(ctx);
    ret = SSL_set_fd(ssl, sock);   //把socket和SSL关联
    if (ret == 0) {
        SSL_free(ssl);
        return NULL;
    }
    
    RAND_poll();
    while (RAND_status() == 0) {
        unsigned short rand_ret = rand() % 65536;
        RAND_seed(&rand_ret, sizeof(rand_ret));
    }
    
    ret = SSL_connect(ssl);
    if (ret != 1) {
        SSL_free(ssl);
        return NULL;    
    }
    return ssl;
}


#endif