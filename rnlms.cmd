/*
 *  Copyright 2001 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 */
/*
 *  ======== hello.cmd ========
 *
 */


MEMORY 
{
   IRAM       : origin = 0x0,         len = 0x40000
}

SECTIONS
{
        .vectors > IRAM
        .text    > IRAM

        .bss     > IRAM
        .cinit   > IRAM
        .const   > IRAM
        .far     > IRAM
        .stack   > IRAM
        .cio     > IRAM
        .sysmem  > IRAM
		.switch  > IRAM
}
