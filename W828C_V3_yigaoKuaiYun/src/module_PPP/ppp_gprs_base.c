#include "hyTypes.h"
#include "hyErrors.h"

#include "ppp_gprs.h"
#include "socket_api.h"
#include "net_api.h"



int netdev_remove_gsm_dev(void)
{
    int rv = 0;
    tNET_STATE  tstate;    
    
    Net_GetDevState(NET_IF_GSM,&tstate);
    
    if (tstate.DevState == IF_DEV_STATE_ACTIVE)
    {
        Net_DeActiveDev(NET_IF_GSM);
        Net_RemoveDev(NET_IF_GSM);
        
        rv = 1;
    }
    
    return rv;
}

int netdev_add_gsm_ip(U32 ipAdd, U32 mask, U32 gw,U32 dns1, U32 dns2)
{
    int rv = 0;
    tNET_DEV    tNet_Dev;//Õ¯¬Á≈‰÷√≤Œ ˝

    if (Net_GetState() != NET_STATE_ACTIVE)
    {
        if (NET_SUCCESS != Net_Active())
        {
            return -1;    
        }       
    }
    
    netdev_remove_gsm_dev();
    
    memset(&tNet_Dev,0,sizeof(tNET_DEV));
    
    tNet_Dev.pHostName = "hyco";	
	tNet_Dev.DHCP = 0;
	tNet_Dev.IpAddr = htonl(ipAdd); 
	tNet_Dev.Mask = htonl(mask);
	tNet_Dev.Gateway = htonl(gw);
	tNet_Dev.DNS0 = htonl(dns1);
	tNet_Dev.DNS1 = htonl(dns2);
	rv = Net_AddDev(NET_IF_GSM, &tNet_Dev);
	
	if (NET_SUCCESS == Net_ActiveDev(NET_IF_GSM))
    {
    	Net_SetDefaultRoute(NET_IF_GSM);
        rv = 1;    
    }
    else
    {
    	Net_RemoveDev(NET_IF_GSM);
         rv = 0;
    }
    
    return rv; 
}


