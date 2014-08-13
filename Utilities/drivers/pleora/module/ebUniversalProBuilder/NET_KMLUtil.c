
#include "NET_KMLUtil.h"

#include <linux/skbuff.h>

OS_UINT32 NET_GetPacketSize( OS_VOID* aData )
{
    struct sk_buff* lSKBuffer = ( struct sk_buff * ) aData;

    return ( OS_UINT32 ) lSKBuffer->len;
}

OS_VOID NET_Copy( OS_VOID* aData, OS_UINT32 aOffset, OS_UINT8* aDest, OS_UINT32 aSize )
{
    struct sk_buff* lSKBuffer = ( struct sk_buff * ) aData;

	if( ( aOffset + aSize ) > lSKBuffer->len )
	{
		aSize = lSKBuffer->len - aOffset;
	}

    skb_copy_bits( lSKBuffer, aOffset, aDest, aSize );  
}
