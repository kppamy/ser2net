/*
 *  ser2net - A program for allowing telnet connection to serial ports
 *  Copyright (C) 2001  Corey Minyard <minyard@acm.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef DATAXFER
#define DATAXFER

#include <linux/serial.h>

#include "controller.h"

#ifdef USE_UUCP_LOCKING
extern int uucp_locking_enabled;
#endif

#ifdef linux

#define USE_RS485_FEATURE

/* Check, if the toolchain provides serial_rs485 structure and macros */
#ifndef SER_RS485_ENABLED

#define TIOCGRS485      0x542E
#define TIOCSRS485      0x542F

struct serial_rs485 {
    __u32 flags; /* RS485 feature flags */
#define SER_RS485_ENABLED        (1 << 0) /* If enabled */
#define SER_RS485_RTS_ON_SEND    (1 << 1) /* Logical level for
                                             RTS pin when sending */
#define SER_RS485_RTS_AFTER_SEND (1 << 2) /* Logical level for
                                             RTS pin after sent*/
#define SER_RS485_RX_DURING_TX   (1 << 4)
    __u32 delay_rts_before_send; /* Delay before send (milliseconds) */
    __u32 delay_rts_after_send;  /* Delay after send (milliseconds) */
    __u32 padding[5];            /* Memory is cheap, new structs
                                    are a royal PITA .. */
};
#endif /* SER_RS485_ENABLED */
#endif /* linux */

/* Check, if the toolchain provides SER_RS485_RX_DURING_TX macro
 * (introduced in kernel 3.2) */
#ifndef SER_RS485_RX_DURING_TX
#define SER_RS485_RX_DURING_TX          (1 << 4)
#endif

struct absout {
    int (*out)(struct absout *e, const char *str, ...);
    void *data;
};

/* Create a port given the criteria. */
int portconfig(struct absout *eout,
	       char *portnum,
	       char *state,
	       char *timeout,
	       char *devname,
	       char *devcfg,
	       int  config_num);

/* Shut down all the ports, and provide a way to check when done. */
void shutdown_ports(void);
int check_ports_shutdown(void);

/* Clear out any old ports on a reconfigure. */
void clear_old_port_config(int config_num);

/* Initialize the data transfer code. */
void dataxfer_init(void);

/* Show information about a port (or all ports if portspec is NULL).
   The parameters are all strings that the routine will convert to
   integers.  Error output will be generated on invalid data. */
void showports(struct controller_info *cntlr, char *portspec);

/* Show information about a port (as above) but in a one-line format. */
void showshortports(struct controller_info *cntlr, char *portspec);

/* Set the port's timeout.  The parameters are all strings that the
   routine will convert to integers.  Error output will be generated
   on invalid data. */
void setporttimeout(struct controller_info *cntlr,
		    char *portspec,
		    char *timeout);

/* Set the serial port's configuration.  The parameters are all
   strings that the routine will convert to integers.  Error output
   will be generated on invalid data. */
void setportdevcfg(struct controller_info *cntlr,
		   char *portspec,
		   char *devcfg);

/* Modify the DTR and RTS lines for the port. */
void setportcontrol(struct controller_info *cntlr,
		    char *portspec,
		    char *controls);

/* Set the enable state of a port (off, raw, telnet).  The parameters
   are all strings that the routine will convert to integers.  Error
   output will be generated on invalid data. */
void setportenable(struct controller_info *cntlr,
		   char *portspec,
		   char *enable);

/* Start data monitoring on the given port, type may be either "tcp" or
   "term" and only one direction may be monitored.  This return NULL if
   the monitor fails.  The monitor output will go to the controller
   via the controller_write() call. */
void *data_monitor_start(struct controller_info *cntlr,
			 char *type,
			 char *portspec);

/* Stop monitoring the given id. */
void data_monitor_stop(struct controller_info *cntlr,
		       void   *monitor_id);

/* Shut down the port, if it is connected. */
void disconnect_port(struct controller_info *cntlr,
		     char *portspec);

struct devio;

/* Initialization function for device I/O */
int devcfg_init(struct devio *io, struct absout *eout, const char *instr,
		int (*otherconfig)(void *data, struct absout *eout,
				   const char *item),
		void *data);

struct serial_rs485 *get_rs485_conf(void *data);

#endif /* DATAXFER */
