/* 
 * $smu-mark$ 
 * $name: sendudp.c$ 
 * $author: Salvatore Sanfilippo <antirez@invece.org>$ 
 * $copyright: Copyright (C) 1999 by Salvatore Sanfilippo$ 
 * $license: This software is under GPL version 2 of license$ 
 * $date: Fri Nov  5 11:55:49 MET 1999$ 
 * $rev: 8$ 
 */ 

/* $Id: send.c,v 1.1.1.1 2003/08/31 17:23:53 antirez Exp $ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include "hping2.h"
#include "globals.h"

static void select_next_random_source(void)
{
	unsigned char ra[4];

	ra[0] = hp_rand() & 0xFF;
	ra[1] = hp_rand() & 0xFF;
	ra[2] = hp_rand() & 0xFF;
	ra[3] = hp_rand() & 0xFF;
	memcpy(&local.sin_addr.s_addr, ra, 4);

	if (opt_debug)
		printf("DEBUG: the source address is %u.%u.%u.%u\n",
		    ra[0], ra[1], ra[2], ra[3]);
}

static void select_next_random_dest(void)
{
	unsigned char ra[4];
	char a[4], b[4], c[4], d[4];

	if (sscanf(targetname, "%4[^.].%4[^.].%4[^.].%4[^.]", a, b, c, d) != 4)
	{
		fprintf(stderr,
			"wrong --rand-dest target host, correct examples:\n"
			"  x.x.x.x, 192,168.x.x, 128.x.x.255\n"
			"you typed: %s\n", targetname);
		exit(1);
	}
	a[3] = b[3] = c[3] = d[3] = '\0';

	ra[0] = a[0] == 'x' ? (hp_rand() & 0xFF) : strtoul(a, NULL, 0);
	ra[1] = b[0] == 'x' ? (hp_rand() & 0xFF) : strtoul(b, NULL, 0);
	ra[2] = c[0] == 'x' ? (hp_rand() & 0xFF) : strtoul(c, NULL, 0);
	ra[3] = d[0] == 'x' ? (hp_rand() & 0xFF) : strtoul(d, NULL, 0);
	memcpy(&remote.sin_addr.s_addr, ra, 4);

	if (opt_debug) {
		printf("DEBUG: the dest address is %u.%u.%u.%u\n",
				ra[0], ra[1], ra[2], ra[3]);
	}
}

/* The signal handler for SIGALRM will send the packets */
void send_packet (int signal_id)
{
	int errno_save = errno;

        if (count == -1 || count >= sent_pkt) {
	  if (opt_rand_dest)
	    select_next_random_dest();
       	  if (opt_rand_source)
	    select_next_random_source();

	  if (opt_rawipmode) send_rawip();
	  else if (opt_icmpmode) send_icmp();
	  else if (opt_udpmode) send_udp();
	  else send_tcp();
        }
	
	sent_pkt++;
	Signal(SIGALRM, send_packet);
	
	if (count != -1 && count < sent_pkt) { /* count reached? */
          sent_pkt = count;
	  Signal(SIGALRM, print_statistics);
	  if (!opt_waitpkts) {
	    opt_waitpkts = COUNTREACHED_TIMEOUT;
	  }	  
	  alarm(opt_waitpkts);
	} else if (!opt_listenmode) {
	  if (opt_waitinusec == FALSE) {
	    if (sending_wait) {
	      alarm(sending_wait);
	    } else {
	      send_packet(SIGALRM); // alarm(0)
	    }
	  } else {
	    if (opt_waitinuseczero) { // timer zero
 	      send_packet(SIGALRM);
	    } else {
	      // stop timer on last pkt in order to alarm(opt_waitpkts) take effect
	      // otherwise timer raises SIGALRM without waiting for any packet
	      if (count == (sent_pkt+1)) { 
		usec_delay.it_interval.tv_usec = 0;
		setitimer(ITIMER_REAL, &usec_delay, NULL); // stop timer
	      }
	      if (sent_pkt == 1) { // set timer once
		setitimer(ITIMER_REAL, &usec_delay, NULL); // timer restarts automatically
	      }
	    }
	  }
	}
	errno = errno_save;
}
