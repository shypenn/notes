## Terms

* IETF (Internet Engineering Task Force): who  develops and promotes voluntary Internet Standards, e.g.  Internet protocol suite (TCP/IP);
* RFC (Request for Comments): internet standard;
* octet: binary octet, 8 bits, one byte.

## Acronyms:

* NCB		--	network control block
* MSL		--	maximum segment lifetime
* WSAStartup	--	Windows Socket Application
* MAC: media access control;

## IP 

### header

    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |Version|  IHL  |Type of Service|          Total Length         |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         Identification        |Flags|      Fragment Offset    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Time to Live |    Protocol   |         Header Checksum       |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                       Source Address                          |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Destination Address                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Options                    |    Padding    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

## IPv4 address

dword, 4-bytes, can be any form of:

* any 4 numbers of combination of decimal/hex/octet together separated by dot

  > in network order (big-endian)
  > addr: low -> high
  > magn: high -> low

	123.125.114.144

	0x7b.0x7d.0x72.0x90

	123.0x7d.114.0220

* any leading bytes till all four can be coalesced together:

	0x7b.0x7d.0x7290

	123.0x7d.29328

	2071818896


## TCP
### header

    0                   1                   2                   3   
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |          Source Port          |       Destination Port        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                        Sequence Number                        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Acknowledgment Number                      |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |  Data |           |U|A|P|R|S|F|                               |
   | Offset| Reserved  |R|C|S|S|Y|I|            Window             |
   |       |           |G|K|H|T|N|N|                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |           Checksum            |         Urgent Pointer        |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                    Options                    |    Padding    |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                             data                              |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

### flags:

* URG:  Urgent Pointer field significant
* ACK:  Acknowledgment field significant
* PSH:  Push Function
* RST:  Reset the connection
* SYN:  Synchronize sequence numbers
* FIN:  No more data from sender

### Status:

* LISTEN - represents waiting for a connection request from any remote TCP and port.

* SYN-SENT - represents waiting for a matching connection request after having sent a connection request.

* SYN-RECEIVED - represents waiting for a confirming connection request acknowledgment after having both received and sent a connection request.

* ESTABLISHED - represents an open connection, data received can be delivered to the user.  The normal state for the data transfer phase of the connection.

* FIN-WAIT-1 - represents waiting for a connection termination request from the remote TCP, or an acknowledgment of the connection termination request previously sent.

* FIN-WAIT-2 - represents waiting for a connection termination request from the remote TCP.

* CLOSE-WAIT - represents waiting for a connection termination request from the local user.

* CLOSING - represents waiting for a connection termination request acknowledgment from the remote TCP.

* LAST-ACK - represents waiting for an acknowledgment of the connection termination request previously sent to the remote TCP (which includes an acknowledgment of its connection termination request).

* TIME-WAIT - represents waiting for enough time to pass to be sure the remote TCP received the acknowledgment of its connection termination request.

* CLOSED - represents no connection state at all.


### Connection State Diagram
```                                   
                              +---------+ ---------\      active OPEN  
                              |  CLOSED |            \    -----------  
                              +---------+<---------\   \   create TCB  
                                |     ^              \   \  snd SYN    
                   passive OPEN |     |   CLOSE        \   \           
                   ------------ |     | ----------       \   \         
                    create TCB  |     | delete TCB         \   \       
                                V     |                      \   \     
                              +---------+            CLOSE    |    \   
                              |  LISTEN |          ---------- |     |  
                              +---------+          delete TCB |     |  
                   rcv SYN      |     |     SEND              |     |  
                  -----------   |     |    -------            |     V  
 +---------+      snd SYN,ACK  /       \   snd SYN          +---------+
 |         |<-----------------           ------------------>|         |
 |   SYN   |                    rcv SYN                     |   SYN   |
 |   RCVD  |<-----------------------------------------------|   SENT  |
 |         |                    snd ACK                     |         |
 |         |------------------           -------------------|         |
 +---------+   rcv ACK of SYN  \       /  rcv SYN,ACK       +---------+
   |           --------------   |     |   -----------                  
   |                  x         |     |     snd ACK                    
   |                            V     V                                
   |  CLOSE                   +---------+                              
   | -------                  |  ESTAB  |                              
   | snd FIN                  +---------+                              
   |                   CLOSE    |     |    rcv FIN                     
   V                  -------   |     |    -------                     
 +---------+          snd FIN  /       \   snd ACK          +---------+
 |  FIN    |<-----------------           ------------------>|  CLOSE  |
 | WAIT-1  |------------------                              |   WAIT  |
 +---------+          rcv FIN  \                            +---------+
   | rcv ACK of FIN   -------   |                            CLOSE  |  
   | --------------   snd ACK   |                           ------- |  
   V        x                   V                           snd FIN V  
 +---------+                  +---------+                   +---------+
 |FINWAIT-2|                  | CLOSING |                   | LAST-ACK|
 +---------+                  +---------+                   +---------+
   |                rcv ACK of FIN |                 rcv ACK of FIN |  
   |  rcv FIN       -------------- |    Timeout=2MSL -------------- |  
   |  -------              x       V    ------------        x       V  
    \ snd ACK                 +---------+delete TCB         +---------+
     ------------------------>|TIME WAIT|------------------>| CLOSED  |
                              +---------+                   +---------+
```                                   

### Three way handshake

1. A --> B  SYN my sequence number is X
2. A <-- B  ACK your sequence number is X;
			SYN my sequence number is Y
3. A --> B  ACK your sequence number is Y   


## http2/SPDY

* push data that client didn't request;
* most client only support in https;
* header compression
* stream priority;
* multiplex streams in a single connection;

 
## stateless
requires no context data is stored, instead passing as parameters;

## http pipleline 

**asyn requests, sync responses**

doesn't wait the previous request got response(responses still are ordered by requests got);

	> don't apply to non-idempotent requests such as 'post';


## URI (URL+URN)
```
         foo://example.com:8042/over/there?name=ferret#nose
         \_/ \________________/\_________/ \_________/ \__/
          |           |            |            |        |
       scheme     authority       path        query   fragment
          |   _____________________|__
         / \ /                        \
         urn:example:animal:ferret:nose
```
* scheme ` ALPHA  DIGIT  "+"  "-"  "." `

* authority ` // [ userinfo "@" ] host [ ":" port ] [ / | ? | # ]`

* userinfo ` unreserved pct-encoded sub-delims ":" `

* host ` IP-literal IPv4address reg-name `

* scheme & path are required. **path can be empty**

* reserved chars (delimiters);

	* gen-delims  = `:  /  ?  #  [  ]  @`

	* sub-delims  = `!  $  &  '  (  ) *  +  ,  ;  =`

	>encodeURI() only percent-encode `[]` in above delims;
	>
	>encodeURIComponent() percent-encode all but `!'()*`;

* unreserved chars (no special meaning, no need to percent-encoding);

	`ALPHA  DIGIT  -  .  _  ~`

* unicode in host name is converted to `Punycode` in most browsers.

	whatever unicode chars are extracted and appended with their encoding number: 
	
	`xn--<ascii>-<unicode>`

	`www.ab人cde.org`	=>	`www.xn--abcde-dd2h.org`


## Data netwowk

built on top of existing physical networks, such as phone netwok, television network and even power network.

## Dial-up
- through a voice connection to Internet Service Provider (ISP), by calling its phone number.
- use the same frequency that phone uses, which expands in the low area of spectrum called Baseband.

## Digital subscriber line (DSL)
- use higher frequency than voice, called Broadband
- connection is permanent without calling.
- data and phone can transmit simultaneously.
- ADSL (asymmetrical DSL), has high downstream speed and lower upstream.

## devices

### Repeater

* physical layer
* two ports
* replicate data from input to output

### Hub

* Repeater with multiple output ports
* replicate data to all outputs
* broadcast

### Switcher

* link layer
* pass data to its destination according to MAC address.

### gateway/hop 

converts protocol to another.


## Physical Layer

- unit: Bit

- DTE (Data Terminal Equipment): computer, router;


- DCE (Data Circuit-terminating Equipment): modem, multiplexer;

## Data Link Layer (LAN)

communicate between neighboring DTEs.

- Frame: header + payload + trailer(checksum)
- link address -  6 bytes, unique in LAN
- Maximum Transfer Unit (MTU): the data length limit in a single frame;

## Network layer (WAN)

communicate between terminals belonging to different LAN. (IP, ICMP...)

- Packet: header + payload, acts as payload of data link frame
- Router is required, it acts as a translater between Link Layer protocols (unpack and pack datagram into different types of frames). Routers communicating with each other become a LAN.
- network address - IP, ...

## Transport layer 

communicate between two Applications residing on different DTEs

- Segment (TCP), Datagram (UDP)
- Transport address - port

## TCP/IP family

Application protocols (user + service)

  ↑

TCP/UDP [transport layer]

  ↑

Internet Protocol [network layer]

  ↑

[link layer + physical layer]

## Internet (best-effort network) 
- is not synchronous as phone; 
- stable bandwidth is achieved by over-dimensioning/over-provisioning or Quality of Service (QoS), which adopts Packet Prioritizing.

## IP is connectionless

- CLNS - Connectionless network service.
- CONS - connection oriented network service

## IPTV

adopts UDP transportation.

## Packet driver

gets only frames that are valid, damaged ones are dropped by hardware, so to catch them a specific device is needed.

## Network interface card

drops frames which are destined to other target, to receive all, turn on its Promiscuous mode. (** switch connected LAN doesnot work**)

## LAN

- stations linked via shared medium, the medium has specific Link Protocol.
- divided into MAC (media access control) and LLC (logical link control); 
- MAC half belongs to L1 and half to L2.

## Extended LAN 

multiple LANs linked via a Switch, which translates frames between different link protocols.

## serial/parallel transmision 

refers to bits of word/s.

## asyncronous arrhythmic data transport 

refers to chars, bits within a char are transported syncly.

## parity bit 

acts as checksum.

## window

a number indicating how many bytes the sender can receive (e.g. how large its buffer leaves).


## 10base5, 10base2, 100baseT... 
- prefix number indicating frequency 
- suffix indicating media (thick/thin coaxial cable, twisted-pair...)

## IP packet
- Header length
  * count of dword
  * basic header length is 5
  * optional header should also be multiple of dword;

- ICMP,IGMP are encapsulated in payload of IP.

- IP payload can contain whatever data besides higher level protocol data.

- Protocol id/number: 

http://www.iana.org./numbers.html

- fragments can be reassembled only by receipient (shouldn't by router), since routes may change.

- A complete IP datagram has its "fragment offset" set to 0 and "last fragment" flaged.


## ICMP
- `redirect` usually is used for dynamic config to assign routers.

- router solicitation is active router querying.

## Ping

adopts ICMP echo.

## Trace route
- tracert.exe: adopts ICMP echo;
- traceroute in unix: unused UDP port, with increasing TTL from 1, trying three times each.
- time sync calcs Round Trip Time (RTT), then deduced the time in counterpart
> Network Time Protocol assumes a Symmetric network, that is the times are equal between tow sides of the round trip.

RTT = reply-got - request-sent - ( reply-sent - request-got )  
ServerNow = reply-sent + RTT/2

## source routing
if the router in a route has its IP matching the one in the IP list pointed by "ptr", it sends this datagram to the next IP. 

the last one in the list would be the final destination.

_in this way, data may be routed through evil server or to unreachable intranet destination._

## ARP,RARP 
- in the same level of IP.

- ARP (address resolution protocol) is for querying MAC address for a specific IP in LAN.

- RARP functions as DHCP, but is superseded by it.

## IP group address [224 - 240)
(http://www.iana.org/assignments/multicast-addresses)

- LAN (dedicated): 224.0.0.0 - 224.0.0.255;
```
224.0.0.1 - All stations (computer + router) within LAN 
224.0.0.2 - All routers within LAN
...
```
- WAN: 224.0.1.0 - 239.255.255.255;

## IP address 
`network addr + computer addr`

* special forms

  - 0s + 0s: this computer in this network

  - 0s + computer: the computer in this network

  - network + 0s: the network

  - network + 1s: broadcast to _the_ network (direct broadcast)

  - 1s + 1s: broadcast to _this_ network (limited broadcast)

* standard network classes

network mask for them are called `standard network mask` (no subnetwork).

always minimum one zero is present to prevent colliding with limited broadcast address.

  - A: 1-byte (the first bit is 0), 0-127

  - B: 2-byte (the first two bits are 10), 128-191

  - C: 3-byte (the first three bits are 110), 192-223

  - D (multicast): 4-byte (the four three bits are 1110), 224-239

  - D (reserved): 240-

* Intranet IP addresses:
  - 10.0.0.0/8  
    site local

  - 172.16.0.0/12 (- 172.31)

  - 192.168.0.0/16

  - 169.254.0.0/16  
    link local address, assigned automatically by OS when no IP is assigned by DHCP server;

* RFC  3330  (Special-Use  IPv4 Addresses)

## IPv6
  `001 + RIR(20) + LIR(9) + site(16) + network(16) + interface ID`

* RIR
  - 2001:0000::/23 IANA
  - 2001:0200::/23 APNIC (Asia and Pacific)
  - 2001:0400::/23 ARIN (America)
  - 2001:0600::/23 RIPE NCC (Europe)

* interface ID: MAC address with `FFFE` inserted between third and four byte.

* multicast
   `FF + 000T + scope(4bits) + group address(6bytes)`
  - T
    0: permanent/well-known
    1: temporary/transient
  - scope
    + 1: Interface-local scope
    + 2: Link-local scope
    + 5: Site-local scope
    + 8: Organization-local scope
    + E: Global scope
  - special (T is 0)
    + ::1 - all stations
    + ::2 - all routers
    + ::9 - all routers using RIP

* loopback: ::1

* 6to4: 2002:V4ADDR:SLA ID:Interface ID

* link local: 1111 1110 10.../10  FE8-FEB

* site local: 1111 1110 11.../10  FEC-FEF

## Dynamic Address Assignment:
DHCP - LAN
PPP - point to point

## Unnumbered interface:
- A router has two interfaces, onr for LAN is ethernet, another for other router is serial.
- An network interface should have an IP address.
- Any connected interfaces forms a network, so in the case of the two connected routers.
- To save up this tiny network IPs, let their Serial interfaces borrow its ethernet address without its own.
- The two connected routers are combined virtually with only two interfaces connecting two LANs.

## Route table
- lists networks, masks and its gateway address, along with metrics.
- it is processed descendingly, from most specific (mask has more ones) to less.
- the last item is 0.0.0.0, its next hop is another router. unmatched ip is sent to it.
- metrics is hops to the destination network.

## TCP
- is duplex, two directions are independent, data can be sent when ACK is received no matter what state is of the other.

- Connection state regards two way connections as a whole.

- 3 steps are required to establish both ways TCP connections

- whereas 4 are needed for closing. since ACK can not be combined with the other FIN when closing. data sending may be continuing the other way.

- in `TIME_WAIT` state, e.g. after the last ACK is sent, the sender will wait for a pre-configed period of time before closing, in case the ACK gets lost.

- Response delay is feasible only for tiny data and non-real-time service.

- ISN (initial sequence number) is random, instead of 0 or 1.

- ACK number indicates the next seq expected (data before it is received)

- data length is deduced from `IP-data-length - TCP-header-length`

- ACK is sent immediately when received.

## DNS 
- records are called RR (resource record)

- Reverse translation applys Pointer Record, which stores IP in form of plain domain with the top level of in-addr.arpa/addr.arpa (ip4/6).

## FTP 
* active/passive mode indicates the role (client/server) of server in data channel.

## telnet 
* can mimic raw tcp client (alike netcat socat), when 8-bit clearness problem is prevented.

> gopher is predecessor of www.

## Proxy 

- client -> [proxy server -> proxy client] -> server

- invoke directly by application. (may leak DNS request, then turn on 'remote DNS lookups' if supported)

- adopts proxify/socksify tool to intercept request in system level

- ssh provide 'dynamic port forwarding', which setup a socks server in local machine, and then relay them over SSH to ssh service on target server, which then send the request. (if target server is locahost, it acts as a pure socks server)

- linux has no uniform way to set system proxy

- windows has system proxy (IE proxy), in 'Internet Options', however application can decide whether to use it
  `InternetQueryOption`

### http

* when send to http proxy, "path" contains full URL in http header.

* http proxy adopts CONNECT method.

### socks5
* It resides between transport and application layers.
* So it adopts **TCP** to negotiate and **TCP/UDP** to transport.
* `Socks` in chinese is 套接字

### shadowsocks

#### shadowsocks-libev
* ss-local: explicit
* ss-redir: transparent (iptable forwarding)
* ss-tunnel: specific target address and port (-L)

## file:///c|/windows
- domain/host name is omited after the second slash
- `|` is replacement of colon(:)

## certificate
* contains public key, bound to a domain, verified by CA with its own root certificate.

* root certificates of public CAs are preinstalled.

* CA verify domain owner by email when issue certificate, or even more strict checking offline.

## forward secrecy
prevent from decrypting by later diaclousure of key.

## VPN 
* divides to two types:
  - Point-to-point
  - Router-to-router

* modify route table to achieve intercepting requests.

## public key 
can be calced from private key, but not vice versa.

## TLS

### format

```
  struct {
      uint8 major;
      uint8 minor;
  } ProtocolVersion;

  enum {
      change_cipher_spec(20), alert(21), handshake(22),
      application_data(23), (255)
  } ContentType;

1. 
  struct {
      ContentType type;
      ProtocolVersion version;
      uint16 length;
      opaque fragment[TLSPlaintext.length];
  } TLSPlaintext;

2. 
  struct {
      ContentType type;       /* same as TLSPlaintext.type */
      ProtocolVersion version;/* same as TLSPlaintext.version */
      uint16 length;
      opaque fragment[TLSCompressed.length];
  } TLSCompressed;
3.
  struct {
      ContentType type;
      ProtocolVersion version;
      uint16 length;
      select (SecurityParameters.cipher_type) {
          case stream: GenericStreamCipher;
          case block:  GenericBlockCipher;
          case aead:   GenericAEADCipher;
      } fragment;
  } TLSCiphertext;

```
