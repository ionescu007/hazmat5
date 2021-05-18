/*++

Copyright (c) Alex Ionescu.  All rights reserved.

Module Name:

    localhstring.cpp

Abstract:

    This module implements the required custom marshallers for LOCAL_HSTRING

Author:

    Alex Ionescu (@aionescu) 18-May-2021 - Initial version

Environment:

    User mode only.

--*/

#include <winstring.h>
#include "lclor.h"

_Use_decl_annotations_
void __RPC_FAR* __RPC_USER midl_user_allocate (
    size_t cBytes
    )
{
    return ((void __RPC_FAR*)HeapAlloc(GetProcessHeap(), 0, cBytes));
}

_Use_decl_annotations_
void __RPC_USER midl_user_free (
    void __RPC_FAR* p
    )
{
    HeapFree(GetProcessHeap(), 0, p);
}

unsigned long __RPC_USER LOCAL_HSTRING_UserSize (
    unsigned long __RPC_FAR* pFlags,
    unsigned long StartingSize,
    LOCAL_HSTRING  __RPC_FAR* pMyObj
    )
{
    return HSTRING_UserSize(pFlags, StartingSize, pMyObj);
}

unsigned char __RPC_FAR* __RPC_USER LOCAL_HSTRING_UserMarshal (
    unsigned long __RPC_FAR* pFlags,
    unsigned char __RPC_FAR* pBuffer,
    LOCAL_HSTRING __RPC_FAR* pMyObj
    )
{
    *pFlags = MSHCTX_LOCAL;
    return HSTRING_UserMarshal(pFlags, pBuffer, pMyObj);
}

unsigned char __RPC_FAR* __RPC_USER LOCAL_HSTRING_UserUnmarshal (
    unsigned long __RPC_FAR* pFlags,
    unsigned char __RPC_FAR* pBuffer,
    LOCAL_HSTRING __RPC_FAR* pMyObj
    )
{
    return HSTRING_UserUnmarshal(pFlags, pBuffer, pMyObj);
}

void __RPC_USER LOCAL_HSTRING_UserFree (
    unsigned long __RPC_FAR* pFlags,
    LOCAL_HSTRING  __RPC_FAR* pMyObj)
{
    HSTRING_UserFree(pFlags, pMyObj);
}
