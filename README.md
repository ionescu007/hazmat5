# hazmat5
Local OXID Resolver (LCLOR) : Research and Tooling

Welcome to a repository on my research into DCOM's Local OXID Resolution mechanisms, and RPCSS internals.

## Local OXID Resolver Internals

A lot of research has been done on DCOM internals by numerous folks, as well as into OXID resolution, but mostly from a network/remote perspective, with
the assumption that local resolution works in a similar way. While the remote interfaces *can* be used locally, true local DCOM operates using a completely
undocumented interface called ILocalObjectResolver, which is how clients and servers communicate in order to register their OIDs and OXIDs, as well as lookup
OXIDs and set various parameters.

Digging deep into these internals can lead to some interesting discoveries, and potential tools for system exploration, especially as these interfaces are now
used as part of Container DCOM, and can have some exotic properties for folks doing research into that space.

### `lclor.idl`

The interface behind the Local OR has never been published, other than the usual GitHub dumps of now-public-domain Windows Server 2003 source code. As one can
imagine, the interfaces have received a number of modifications in the last 18 years. Indeed, even between Windows 10 19H1 and 20H1, significant changes were made
in order to support Container DCOM, and to extend the capabilities offered to WinRT COM servers, in light of projects such as Windows Desktop Bridge (Centennial),
through concepts such as "primary OXID" and "deploying" vs "execution" package names.

The first deliverable of this repository is an up-to-date version of an IDL file necessary to communicate with the Local OR on 20H1 and later systems. Note that
flag values, parameter ordering, and procedure numbers have changed since past versions, and this file will _not_ work with earlier versions of Windows 10.

Research was performed using the great RpcView and OleViewDotNet tooling, Hex-Rays, and private symbols for combase.dll, starting with the basic `lclor.idl` file
present on GitHub.

### `rundown.idl`

The full, up-to-date, specification for the IRemUnknown-derived family of interfaces has also never been officially published, other than the limited information
published in the official protocol specification, which covers the over-the-wire messages that are normally used in network DCOM. Additional methods, used locally,
have either been seen in GitHub dumps or some blog posts and presentations from James Forshaw -- but even those have changed in 20H1 and later.

These interfaces can be useful for a variety of local system exploration and analysis, and are easily obtainable by using the private symbols for combase.dll as
well as the same tooling as mentionned above, and the `odeth.idl` file seen on GitHub. The second deliverable of this repository is an up-to-date version we call
`rundown.idl`.

### `lclor.exe`

The upcoming command-line tool will leverage the IDL file in order to implement some functionality for looking up and allocating OXIDs with the Local OR, displaying
relevant and useful information for research and analysis purposes.

See below for additional information, such as input arguments and flags, as well as some sample output.

#### Usage

```
lclor v1.0.0 -- Local OXID Resolver Tool
Copyright (C) 2021 Alex Ionescu (@aionescu)
www.alex-ionescu.com


Usage: lclor.exe [-i | -l <OXID> [-B] | -b <OXID> <IPID>]
    -b        Bind to the given IRemUnknown IPID for the given OXID
    -i        Display information on the Local OR
    -l        Lookup information on the given OXID
    -B        Attempt binding to the IPID after the lookup
````

#### Examples

```
lclor v1.0.0 -- Local OXID Resolver Tool
Copyright (C) 2021 Alex Ionescu (@aionescu)
www.alex-ionescu.com

Looking up OXID 0x36EF4713E3988C5A...

COM Server Version         5.7
Supports Container Version 3
    Capability Flags:      0x0
Linked Primary OXID:       0x36EF4713E3988C5A
Apartment Type:            NTA
Authentication Hint:       RPC_C_AUTHN_LEVEL_PKT
Hosted by process ID:      2212
Process GUID:              {5A6D128D-D460-485D-A88B-56F4F024C5EB}
IRemUnknown IPID:          {0000AC01-08A4-FFFF-744E-5D33BA2A1435}
Primary IRemUnknown IPID:  {0000AC01-08A4-FFFF-744E-5D33BA2A1435}
Binding String:            ncalrpc:[OLE059D40EF2D5CBCA37D9896754A6C]
```

```
lclor v1.0.0 -- Local OXID Resolver Tool
Copyright (C) 2021 Alex Ionescu (@aionescu)
www.alex-ionescu.com

Looking up OXID 0xD6FF45175D39276E...

COM Server Version         5.7
Supports Container Version 3
    Capability Flags:      0x0
Linked Primary OXID:       0xD6FF45175D39276E
Apartment Type:            NTA
Flags:                     StrongNamed AppContainer Suspendable
Authentication Hint:       RPC_C_AUTHN_LEVEL_PKT_INTEGRITY
Hosted by process ID:      13160 (ShellExperienceHost.exe)
Process GUID:              {39368377-6BBF-436B-B5C8-E7ADC99B84F9}
Package name:              Microsoft.Windows.ShellExperienceHost_10.0.21382.1_neutral_neutral_cw5n1h2txyewy
User SID:                  S-1-5-21-1928273713-1136577611-1766458866-1004
AppContainer SID:          S-1-15-2-155514346-2573954481-755741238-1654018636-1233331829-3075935687-2861478708
IRemUnknown IPID:          {00007C01-3368-FFFF-2CCD-509DE410C457}
Primary IRemUnknown IPID:  {00007C01-3368-FFFF-2CCD-509DE410C457}
Binding String:            ncalrpc:[\\Sessions\\1\\AppContainerNamedObjects\\S-1-15-2-155514346-2573954481-755741238-1654018636-1233331829-3075935687-2861478708\\RPC Control\\OLE584D7099BF0C3175917BE7B61119]
```

```
lclor v1.0.0 -- Local OXID Resolver Tool
Copyright (C) 2021 Alex Ionescu (@aionescu)
www.alex-ionescu.com

Binding to OXID 0x36EF4713E3988C5A with IPID {0000AC01-08A4-FFFF-744E-5D33BA2A1435}...

Binding successful (0x000001A9A07D9A58), press any key to exit...
```

## RPCSS Database Internals

The second area of research into DCOM internals is the set of mechanisms inside of RPCSS which manage communication with COM servers and clients, keeping track of
their behavior, caching looked up OXIDs, generating OBJREFs, and allowing for the registration and activation (through the "activation kernel" in `DcomLaunch`) of
COM classes.

### rpcssinfo.js

The upocoming WinDbg Extension will provide, either when doing kernel debugging (with user-mode symbols) or when doing user-mode debugging of the `svchost.exe`
instance hosting `RPCSS` (for example, a memory dump), additional members to the `@$cursession` object, including NatVis containers for the process list, classic
COM server list, WinRT COM server list, and OXID list.

### `rpcssdmp.exe`

The upcoming command-line tool will implement similar capabilities as the WinDbg extension named above, but through a command-line interface that does not require
a debugger.

## References

If you would like to know more about my research or work, I invite you to check out my blog at [http://www.alex-ionescu.com](http://www.alex-ionescu.com) as well as my
training & consulting company, Winsider Seminars & Solutions Inc., at [http://www.windows-internals.com](http://www.windows-internals.com).

James Forshaw is probably the foremost authority on DCOM these days, and his [Troopers 17 talk](https://www.troopers.de/downloads/troopers17/TR17_Demystifying_%20COM.pdf)
is a great initial resource.

You should also definitely read the incredibly informative [Airbus Cybersecurity blog post](https://airbus-cyber-security.com/the-oxid-resolver-part-2-accessing-a-remote-object-inside-dcom/).

The [Inside COM+ book site](https://thrysoee.dk/InsideCOM+/ch19f.htm) also covers the remote OXID resolver.

Of course, the [official protocol specification](https://winprotocoldoc.blob.core.windows.net/productionwindowsarchives/MS-DCOM/%5bMS-DCOM%5d-171201.pdf) is also a key
learning tool.

## Credits

A special thank you to James Forshaw for advice on how to handle the LOCAL_HSTRING unmarshalling correctly.

## License

```
Copyright 2021 Alex Ionescu. All rights reserved. 

Redistribution and use in source and binary forms, with or without modification, are permitted provided
that the following conditions are met: 
1. Redistributions of source code must retain the above copyright notice, this list of conditions and
   the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
   and the following disclaimer in the documentation and/or other materials provided with the 
   distribution. 

THIS SOFTWARE IS PROVIDED BY ALEX IONESCU ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ALEX IONESCU
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the authors and
should not be interpreted as representing official policies, either expressed or implied, of Alex Ionescu.
```
