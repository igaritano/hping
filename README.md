## Description

This is a fork of the original [hping3](https://github.com/antirez/hping) network tool developed by [antirez](https://github.com/antirez).

hping3 is a network tool able to send custom TCP/IP packets and to display target replies like ping do with ICMP replies. hping3 can handle fragmentation, and almost arbitrary packet size and content, using the command line interface.

For the original README, refer to [README_original](README_original).

This fork adds several options such as 
* Option to specify number of seconds to wait for receiving packets
* Option to print received/lost tcp packets and arriving order
* Option to filter received tcp packets based on tcp flags

In addition, it includes several pull request of the original project repository.

For further information refer to [releases section](https://github.com/igaritano/hping/releases/).

## Requirements

* A supported unix-like OS, gcc, root access.
* Libpcap (libpcap-dev).
* Tcl/Tk (tcl-dev) is optional but strongly suggested.

## Build

* ./configure
* make
* make install

## License

hping3 comes under GPL version 2.
