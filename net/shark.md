* adopt `npcap` (upgraded from winpcap) in windows 

another way to switch adpater mode is WlanHelper.exe, rather than wireshark

* Monitor mode
  - RFMON (Radio Frequency MONitor) mode
  - wireless only

* protocol parser is applied based on port

* shows seq number as relative, different from the raw data.

* field occurrence: index of array starting from 1 (0 indicates whole array)

* hidden(unchecked) columns are also processed in background, better remove for perfomance.

* Bytes in flight: unacknowledged sequences

* profile setting files are contained in a folder named after it.

* folders -> help/about

* capture options
  - ctrl + click: multiple interfaces

* wifi
  - by default
    + radio packet headers are translated to __fake__ Ethernet headers.
    + packets for management, control and destined to other receipts are filtered.
    + no radio-layer information

* capture filter
 - data: `proto [ expr : size ]`

  ip[2:4] == 0x1234

* display filter
 - reference fields in `Expression` dialog;
 - substring: `http.method contains "GET"`
 - ip subnet: `ip.addr==10.3.0.0/16`
 - ip range: `ip.addr > 10.3.0.1 && ip.addr < 10.3.0.5`
 - `ip.addr!=10.3.0.0/16` <=> `ip.src!=10.3.0.0/16 || ip.dst!=10.3.0.0/16`
  take care of `!=` in field matching more than one fields.
 - `!(ip.addr==10.3.0.0/16)` <=> `ip.src!=10.3.0.0/16 && ip.dst!=10.3.0.0/16`
 - `!tcp.flags.syn==1` <=> `!tcp.flags.syn || tcp.flags.syn!=1`
 - `matches` apply [Perl-Compatible Regular Expressions](http://pcre.org/current/doc/html/pcre2syntax.html)
  + case insensitive: "(?i)string"

* time delta in a conversion
 - export a conversation to a file, to take advantage of `frame.time_delta`;
 - `tcp.time_delta` (`Calculate conversation timestamps` is enabled in your TCP preferences)

* DHCP protocol is named `bootp` in filter

* DHCP v6 protocol is not based on 'bootp'

* tcp.stream: index of conversation.

* ip.addr includes ip.src and ip.dst

* `out-of-order` is retransmission.

* `duplicate ACK` notifies lossed segments

* re-transmission
  - `Re-transmissions`: ACK is not received before timeout (RTO)
  - `fast Re-transmissions`: duplicate ACKs are received (indicating packets loss)
  - `spurious Re-transmissions`: already ACKed
    + ACK doesn't reach the sender when capturing at the receiver side
    + attacker injects packet with the same SEQ
