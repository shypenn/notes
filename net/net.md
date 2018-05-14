## Terms

* IETF (Internet Engineering Task Force): who  develops and promotes voluntary Internet Standards, e.g.  Internet protocol suite (TCP/IP);
* RFC (Request for Comments): internet standard;
* octet: binary octet, 8 bits, one byte.

## Acronyms:

* NCB		--	network control block
* MSL		--	maximum segment lifetime
* WSAStartup	--	Windows Socket Application
* MAC: media access control;

## Commands:

* URG:  Urgent Pointer field significant
* ACK:  Acknowledgment field significant
* PSH:  Push Function
* RST:  Reset the connection
* SYN:  Synchronize sequence numbers
* FIN:  No more data from sender

## TCP Status:

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


## TCP Connection State Diagram
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

## Three way handshake

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


## IP address

dword, 4-bytes, can be any form of:

* any 4 numbers of combination of decimal/hex/octet together separated by dot

	123.125.114.144

	123.0x7d.114.0220

* any leading bytes till all four can be coalesced together:

	123.0x7d.29328

	2071818896


## socks5
* It resides between transport and application layers.
* So it adopts **TCP** to negotiate and **TCP/UDP** to transport.
* It's invoked directly by application.

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
