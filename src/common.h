/**
 * @file common.h
 * @brief Routines used by the Flowgrind Daemon and Controller
 */

/*
 * Copyright (C) 2010-2013 Christian Samsel <christian.samsel@rwth-aachen.de>
 * Copyright (C) 2009 Tim Kosse <tim.kosse@gmx.de>
 * Copyright (C) 2007-2008 Daniel Schaffrath <daniel.schaffrath@mac.com>
 *
 * This file is part of Flowgrind. Flowgrind is free software; you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2 as published by the Free Software Foundation.
 *
 * Flowgrind distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <limits.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/tcp.h>

#include "gitversion.h"

#define UNUSED_ARGUMENT(x) (void)x

#ifdef GITVERSION
#define FLOWGRIND_VERSION GITVERSION
#elif defined PACKAGE_VERSION
#define FLOWGRIND_VERSION PACKAGE_VERSION
#else
#define FLOWGRIND_VERSION "(n/a)"
#endif /* GITVERSION */

/* Flowgrind's xmlrpc API version in integer representation */
#define FLOWGRIND_API_VERSION 3

#define DEFAULT_LISTEN_PORT 5999

#define ASSIGN_MIN(s, c) if ((s)>(c)) (s) = (c)
#define ASSIGN_MAX(s, c) if ((s)<(c)) (s) = (c)

#define WRITE 0
#define READ 1

#define INTERVAL 0
#define TOTAL 1

#define MAX_EXTRA_SOCKET_OPTIONS 10
#define MAX_EXTRA_SOCKET_OPTION_VALUE_LENGTH 100

#define MAX_FLOWS 2048

#ifndef TCP_CA_NAME_MAX
#define TCP_CA_NAME_MAX 16
#endif /* TCP_CA_NAME_MAX */

/** Error types */
enum error_type {
	ERR_FATAL,
	ERR_WARNING,
	ERR_USAGE
};

/** Flow endpoint */
enum flow_endpoint {
	SOURCE = 0,
	DESTINATION,
};

enum _extra_socket_option_level
{
	level_sol_socket,
	level_sol_tcp,
	level_ipproto_ip,
	level_ipproto_sctp,
	level_ipproto_tcp,
	level_ipproto_udp
};

/*
 * our data block has the following layout:
 *
 * this_block_size (int32_t), request_block_size (int32_t), data (timeval), trail
 *
 * this_block_size:     the size of our request or response block (we generate
 *                      a request block here)
 *
 * request_block_size:  the size of the response block we request
 *                      0 if we dont request a response block
 *                     -1 indicates this is a response block (needed for parsing data)
 *
 * data                 RTT data if this is a response block
 *
 * trail:               trailing garbage to fill up the blocksize (not used)
 */

#define MIN_BLOCK_SIZE (signed) sizeof (struct _block)
struct _block
{
	int32_t this_block_size;
	int32_t request_block_size;
	struct timespec data;
	struct timespec data2; /* used to access 64bit timeval on 32bit arch */
};

enum _stochastic_distributions
{
	CONSTANT='0',
	NORMAL,
	WEIBULL,
	UNIFORM,
	EXPONENTIAL,
	PARETO,
	LOGNORMAL,
	ONCE
};

struct _trafgen_options
{
	enum _stochastic_distributions distribution;
	double param_one;
	double param_two;

};

/* Common to both endpoints */
struct _flow_settings
{
	char bind_address[1000];

	double delay[2];
	double duration[2];

	double reporting_interval;

	int requested_send_buffer_size;
	int requested_read_buffer_size;

	int maximum_block_size;

	int traffic_dump;
	int so_debug;
	int route_record;
	int pushy;
	int shutdown;

	/** Send at specified rate per second (option -R) */
	char *write_rate_str;
	/** The actual rate we should send */
	int write_rate;

	unsigned int random_seed;

	int flow_control;

	int byte_counting;

	int cork;
	int nonagle;
	char cc_alg[TCP_CA_NAME_MAX];
	int elcn;
	int lcd;
	int mtcp;
	int dscp;
	int ipmtudiscover;

	struct _trafgen_options request_trafgen_options;
	struct _trafgen_options response_trafgen_options;
	struct _trafgen_options interpacket_gap_trafgen_options;

	struct _extra_socket_options {
		int level;
		int optname;
		int optlen;
		char optval[MAX_EXTRA_SOCKET_OPTION_VALUE_LENGTH];
	} extra_socket_options[MAX_EXTRA_SOCKET_OPTIONS];
	int num_extra_socket_options;
};

/* Flowgrinds view on the tcp_info struct for
 * serialization / deserialization */
struct _fg_tcp_info
{
	int tcpi_snd_cwnd;
	int tcpi_snd_ssthresh;
	int tcpi_unacked;
	int tcpi_sacked;
	int tcpi_lost;
	int tcpi_retrans;
	int tcpi_retransmits;
	int tcpi_fackets;
	int tcpi_reordering;
	int tcpi_rtt;
	int tcpi_rttvar;
	int tcpi_rto;
	int tcpi_backoff;
	int tcpi_snd_mss;
	int tcpi_ca_state;
};

/* Report (measurement sample) of a flow */
struct _report
{
	int id;
	/* Is this an INTERVAL or TOTAL (final) report? */
	int type;
	struct timespec begin;
	struct timespec end;
#ifdef HAVE_UNSIGNED_LONG_LONG_INT
	unsigned long long bytes_read;
	unsigned long long bytes_written;
#else
	long bytes_read;
	long bytes_written;
#endif /* HAVE_UNSIGNED_LONG_LONG_INT */
	unsigned int request_blocks_read;
	unsigned int request_blocks_written;
	unsigned int response_blocks_read;
	unsigned int response_blocks_written;

	/* TODO Create an array for IAT / RTT and delay */

	/** Minimum interarrival time */
	double iat_min;
	/** Maximum interarrival time */
	double iat_max;
	/** Accumulated interarrival time */
	double iat_sum;
	/** Minimum one-way delay */
	double delay_min;
	/** Maximum one-way delay */
	double delay_max;
	/** Accumulated one-way delay */
	double delay_sum;
	/** Minimum round-trip time */
	double rtt_min;
	/** Maximum round-trip time */
	double rtt_max;
	/** Accumulated round-trip time */
	double rtt_sum;

	/* on the Daemon this is filled from the os specific
	 * tcp_info struct */
	struct _fg_tcp_info tcp_info;

	int pmtu;
	int imtu;

	int status;

	struct _report* next;
};

void error(enum error_type errcode, const char *fmt, ...);

#endif /* _COMMON_H_*/
