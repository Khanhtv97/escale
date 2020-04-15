/******************************************************************************* */
/*                                                                               */
/*                                                                               */
/* TITLE:       bootrom.s                                                        */
/*                                                                               */
/* PURPOSE:     First code run upon reset:                                       */
/*              - initialize CPU (clocks, memory, etc.)                          */
/*              - copy user program in RAM                                       */
/*              - branch to user start address.                                  */
/*                                                                               */
/*              Clock and memory configuration is performed according to a       */
/*              table (__pokelist_Start, at the end of this file), which is      */
/*              interpreted in real-time by a configuration loop, and has to be  */
/*              updated for each hardware platform.                              */
/*              However since targetpack v1.3.1, the poke table is generated     */
/*              automatically with the romgen tool.                              */
/*                                                                               */
/*              This table consists in a suite of 'instructions', each of which  */
/*              being made of an opcode and a variable number of arguments. The  */
/*              end of the table is signalled using a special opcode (0). Macros */
/*              are defined for each instruction to help filling the table;      */
/*              currently, the following instructions are recognized:            */
/*                                                                               */
/*   Setup table macro                        Operation                          */
/*   --------------------------------------------------------------------------- */
/*   POKE_UPDATE32(A1, A2, AND, SHIFT, OR) *A1 = ((*A2 & AND) << SHIFT) | OR;    */
/*   WHILE_NE32(A, AND, VAL)               while ((*A & AND) != VAL) ;           */
/*   UPDATE32(A, AND, OR)                  *A = (*A & AND) | OR;                 */
/*   OR32(A, V)                            *A = *A | V;                          */
/*   POKE32(A, V)                          *(long*)A = (long)V;                  */
/*   POKE16(A, V)                          *(short*)A = (short)V;                */
/*   POKE8(A, V)                           *(char*)A = (char)V;                  */
/*   END_MARKER                            <<< End of table >>>                  */
/*                                                                               */
/*********************************************************************************/

/* Include */
/* ======= */
#define _SH4REG_ASM_
#if defined(ST_5197)
#include "sti5197reg.h"
#endif
#if defined(ST_5206)
#include "stx5206reg.h"
#endif
#if defined(ST_7105)
#include "sti7105reg.h"
#endif
#if defined(ST_7106)
#include "stx7106reg.h"
#endif
#if defined(ST_7108)
/*#include "stx7108reg.h"*/
#endif
#if defined(ST_7109)
#if (DVD_ADDRESSMODE==29)
#define STB7109_MOVE_LMI_REGS 0
#else
#define STB7109_MOVE_LMI_REGS 1
#endif
#include "stb7109reg.h"
#endif
#if defined(ST_7111)
#include "sti7111reg.h"
#endif
#if defined(ST_7141)
#include "sti7141reg.h"
#endif
#if defined(ST_H205)
/*#include "stih205reg.h"*/
#endif

###########################################################################
################################## MACROS #################################
###########################################################################

/*
 * Write out a series of bytes, monotonically increasing
 * in value from "first" to "last" (inclusive).
 *
 * Usage:   BYTES <first> <last>
 * where <first> is the first byte to generate
 * where <last>  is the last byte to generate
 *
 * Note: this macro uses recursion (one level per byte)
 */
.macro BYTES first=0, last=63
    .byte \first
    .if \last-\first
    BYTES "(\first+1)",\last
    .endif
.endm

###########################################################################
############################ START OF BOOTSTRAP ###########################
###########################################################################

/* ========================================================================
   Name:        main
   Description: main is MAIN. Entry point of the application.
   ======================================================================== */

    .import     UNCOMPRESS_SYMBOL                                           ! Uncompress function import
    .weak       UNCOMPRESS_SYMBOL                                           ! Weak the function
    .section    .text                                                       ! Section name
    .org        0x0                                                         ! Entry point

    /* We switch to running from P0 in the process */
    /* ------------------------------------------ */
start:
    mova    boots1,r0
    mov.l   p0_mask,r1
    and     r1,r0                                                           ! Mask address to P0
    jmp     @r0                                                             ! Start running from P0
    nop

    /* There is 64 bytes of 0x00 for NAND but is applied to other flash as well */
    /* ------------------------------------------------------------------------ */
    .section    .text                                                       ! Section name
    .org        0x80                                                        ! Entry point
magic_pattern:
    bytes       0x00 0x3f                                                   ! 64-bytes: 0x00, 0x01, ..., 0x3f
    .balign 4
    .section    .text                                                       ! Section name
    .org        0xc0                                                        ! Entry point

    /* Start of code here */
    /* ------------------ */
boots1:
    mova    stop_watchdog,r0                                                ! Get address to jump to
    mov.l   p0_mask,r1
    and     r1,r0                                                           ! Mask address to P0
    jmp     @r0                                                             ! Start running from P0
    nop
    .balign 4

    /* Stop the watchdog timer */
    /* ----------------------- */
stop_watchdog:
    mov.l   wtcsr_address, r0
    mov.l   wtcsr_off, r1
    mov.w   r1, @r0

    /* I-cache & D-cache on; D-cache in copy-back mode */
    /* ----------------------------------------------- */
enable_cache:
    mov.l   ccr_address,r0
    mov.l   ccr_mode,r1
    mov.l   r1,@r0
    /* Ensure cache coherency using an rte */
    stc     sr,r0                                                           ! Ensure SR register remains the same after rte
    ldc     r0,ssr
    mova    pull_table_in_cache,r0                                          ! Use rte to skip the data
    ldc     r0,spc
    rte
    nop
    nop
    .balign 4

    /* Insert the loop table in the cache */
    /* ---------------------------------- */
    /*
     * Ensure that the poke loop and the associated data table are in the on-chip
     * caches.
     *
     * The poke loop is structured so that all of it is pulled into the  I-cache on
     * its first iteration. To ensure that the data table is pulled into the D-cache,
     * we simply read all the data.
     *
     * Setting up the on-chip peripherals may cause the EMI (where this FLASH image
     * resides) to briefly hang, but the CPU will be safely executing from cache
     * should this happen.
     *
     */
pull_table_in_cache:
    mov.l   p0_mask,r3                                                      ! R3 = P0 translation mask
1:  mov.l   poke_list_start_address,r4                                      ! R4 = Poke list table start
    mov.l   poke_list_stop_address,r2                                       ! R2 = Poke list table stop
    mov.l   poke_loop_address,r6                                            ! R6 = Poke loop function
2:  and     r3,r4                                                           ! R4 = Converted to P0 addresses
    and     r3,r2                                                           ! R2 = Converted to P0 addresses
    and     r3,r6                                                           ! R6 = Converted to P0 addresses
    mov     r4,r5                                                           ! R5 = poke table start address (scratch)
3:  mov.l   @r5+,r1
    cmp/eq  r5,r2
    bf      3b
    nop

    /*
     * Its now safe to call the poke loop, since the data table will now
     * be in the D-cache.
     */
    jsr @r6
    nop

    /* Ensure cache coherency using an rte (required if poke table put us
     * in SE mode, but harmless if it did not).
     */
    stc     sr,r0                                                           ! Ensure SR register remains the same after rte
    ldc     r0,ssr
    mova    call_copy,r0                                                    ! Use rte to skip the data
    ldc     r0,spc
    rte
    nop
    .balign 4

    /* Copy the code in DDR */
    /* -------------------- */
call_copy:
    mov.l   uncompress_function,r0                                          ! R0 = uncompress()
    tst     r0,r0                                                           ! uncompress() available ?
    bf      1f                                                              ! Yes, so try to unpack the code
    bsr     copy_code                                                       ! Just, copy the code
    nop
    bra     jump_to_user                                                    ! Start the application right now
    nop
1:  bsr     decompress_and_copy_code                                        ! Copy user program and depack it in RAM
    nop
    .balign 4

    /* Jump to user start address */
    /* -------------------------- */
jump_to_user:
    mov.l   dbg_connected_address,r0                                        ! Zero out dbg connected flag
    mov.l   @r0,r0
    mov     #0,r1                                                           ! To prevent comms with MicroConnect
    mov.l   r1,@r0
    mov.l   start_address,r0                                                ! Jump to user start address in RAM
    mov.l   @r0,r0
    jmp     @r0
    nop

###########################################################################
############################# END OF BOOTSTRAP ############################
###########################################################################

/* ========================================================================
   Name:        copy_code
   Description: Copy the code in DDR
   ======================================================================== */

    .balign 4
copy_code:
    mov.l   stack_address,r0                                                ! Setup stack address
    mov.l   @r0,r15                                                         ! r15=stack base
    sts.l   pr,@-r15                                                        ! Preserve PR
    bsr     find_main_bin                                                   !
    nop                                                                     !
    mov     r0,r2                                                           ! r2 = Source address in FLASH
    lds.l   @r15+,pr                                                        ! Restore PR
    mov.l   load_address,r0                                                 ! Get load address in RAM
    mov.l   @r0,r1                                                          ! r1 = load_addr;
    mov.l   image_size_address,r0                                           ! Image size
    mov.l   @r0,r3                                                          ! r3 = image_size;
    add     #3,r3                                                           ! Round to higher multiple of 4
    shlr2   r3                                                              ! Divide by 4 to get size in long words
1:  mov.l   @r2+,r4                                                         ! r4 = *src_addr++;
    mov.l   r4,@r1                                                          ! *load_addr = r4;
    dt      r3                                                              ! image_size--;
    bf/s    1b                                                              ! Continue loop if image_size != 0
    add     #4,r1                                                           ! load_addr++; /* long ptr incr = +4 */
    rts                                                                     ! return;
    nop

/* ========================================================================
   Name:        decompress_and_copy_code
   Description: Copy the code in DDR and unzip the code
   ======================================================================== */

    .balign 4
    /* Method to inflate a zlib compressed section.                      */
    /*                                                                   */
    /*   Move parameters into registers used for a C function call, and  */
    /*   call the C function to do the decompress.                       */
    /*                                                                   */
    /*   R4 = src, R5 = dst, R6 = srclen (bytes)                         */
    /*                                                                   */
    /*   uncompress() function requires:                                 */
    /*                                                                   */
    /*   R4 = dst, R5 = &dstlen, R6 = src, R7 = srclen                   */
decompress_and_copy_code:
    mov.l   stack_address,r0                                                ! Setup stack address
    mov.l   @r0,r15                                                         ! r15=stack base
    sts.l   pr, @-r15                                                       ! Preserve PR
    /*=== ST find the src address  ===*/
    /*bsr     find_main_bin           */                                    !
    /*nop                             */                                    !
    /*mov     r0,r6                   */                                    ! r6=address start of the zipped buffer
    /*=== ST find the src address  ===*/
    /*=== GBS find the src address ===*/
    nop
    mov.l   image_compressed_address,r0                                     ! gbs: r0 is a pointer on to the image_compressed_address
    mov.l   @r0,r6                                                          ! r6=address start of the zipped buffer
    nop
    /*=== GBS find the src address ===*/
    mov.l   load_address,r0                                                 !
    mov.l   @r0,r4                                                          ! r4=depack destination address
    mov.l   image_size_address,r0                                           !
    mov.l   @r0,r5                                                          ! r5=image size after unzip
    mov.l   r5,@-r15                                                        !
    mov     r15,r5                                                          ! r5 is a pointer on to the destlen
    mov.l   image_compressed_size_address,r0                                !
    mov.l   @r0,r7                                                          ! r7=size of the zip buffer
    mov.l   uncompress_function,r0                                          !
    jsr     @r0                                                             ! Call uncompress()
    mov     r15,r5                                                          ! r5=size depacked
    add     #4,r15                                                          !
    lds.l   @r15+, pr                                                       ! Restore PR
    rts
    nop

/* ========================================================================
   Name:        find_main_bin
   Description: Return the address of main.bin in flash (after the unique marker)
   ======================================================================== */

    .balign 4
    /* Method to find the main.bin image in flash.         */
    /* Returns the address following the rom unique marker */
find_main_bin:
    mov.l   page_kb,r6                                                      ! page size in kb is now in R6 - start search
    mov     r6,r7                                                           ! r7 = r6 = contains address to check
check_main_marker:                                                          !
    mova    rom_main_marker_stop,r0                                         !
    mov     r0,r4                                                           ! r4 = @rom_main_marker_stop
    mova    rom_main_marker_start,r0                                        ! r0 = @rom_main_marker_start
    mov     r7,r1                                                           ! r1 = pointer to start to look pattern in flash
1:  mov.l   @r0+,r2                                                         ! While r0 is != rom_main_marker_stop, check words match
    mov.l   @r1+,r3                                                         ! r2 = marker reference, r3 = contain value from flash
    cmp/eq  r2,r3                                                           !
    bf      goto_next_block                                                 ! if not equal, jump to next block
    cmp/eq  r0,r4                                                           ! otherwise, check following marker word, if found
    bf      1b                                                              ! Matched word, but not finished check
    mov.l   rom_main_marker_size,r0                                         ! When we get here we have successfully matched the marker.
    add     r7,r0                                                           ! address to start copying or decompressing from.
    rts                                                                     ! Add the size of the rom marker to R7 and we have the source
    nop                                                                     !
goto_next_block:                                                            !
    add     r6,r7                                                           ! Add 16Kbytes to r7 and restart the marker find tool
    bra     check_main_marker                                               !
    nop                                                                     !
    .balign 4                                                               !
page_kb:                                                                    !
#if (ZIP==1)
    .long   (128*1024)                                                      ! When zip=1, bootstrap is longer and we can support only FLASH with block >= 128kbytes
#else
    .long   (16*1024)                                                       ! When zip=0, bootstrap is short and we can support all flash size as smallest is 16Kbytes
#endif

/* ========================================================================
   Name:        memcpy
   Description: Implementation of memcpy() to avoid dependency on C library
   ======================================================================== */

    .balign 4
    .global _memcpy
    /* R4 = dst, R5 = src, R6 = srclen (bytes) */
_memcpy:
     mov    r4,r0                                                           ! Check if source and destination are word aligned
     or     r5,r0                                                           !
     tst    r6,r6                                                           !
     bt/s   3f                                                              ! Skip zero length copy
     tst    #3,r0                                                           !
     bf     2f                                                              !
     mov    r6,r2                                                           !
     shlr2  r6                                                              !
1:   mov.l  @r5+,r0                                                         ! Copy words...
     dt     r6                                                              !
     mov.l  r0,@r4                                                          !
     bf/s   1b                                                              !
     add    #4,r4                                                           !
     mov    r2,r0                                                           !
     and    #3,r0                                                           ! Fall through to byte copy for any odd bytes
     mov    r0,r6                                                           !
     tst    r6,r6                                                           !
     bt     3f                                                              !
2:   mov.b  @r5+,r0                                                         ! Copy bytes...
     dt     r6                                                              !
     mov.b  r0,@r4                                                          !
     bf/s   2b                                                              !
     add    #1,r4                                                           !
3:   rts                                                                    !
     nop

###########################################################################
################################## POKELOOP ###############################
###########################################################################

#include "poketable.h"
#include "pokeloop.S"

###########################################################################
################################### DATAS #################################
###########################################################################

/* Watchdog register address */
/* ------------------------- */
                                    .balign 4
wtcsr_address:                      .long 0xFFC0000C
wtcsr_off:                          .long 0x0000A500

/* Cache configuration registers */
/* ----------------------------- */
                                    .balign 4
ccr_address:                        .long 0xFF00001C
ccr_mode:                           .long 0x8000090D

/* ROM marker */
/* ---------- */
rom_main_marker_start:              .ascii "______STAPI_SDK_ROMIMAGE______\0\0" ! Ensure this a multiple of 4 bytes in length
rom_main_marker_stop:
rom_main_marker_size:               .long (rom_main_marker_stop-rom_main_marker_start)

/* Table addresses */
/* --------------- */
p0_mask:                            .long   0x1FFFFFFF
poke_list_start_address:            .long __pokelist_Start
poke_list_stop_address:             .long __pokelist_Stop
poke_loop_address:                  .long __pokeLoop

/* User program */
/* ------------ */
#if (ZIP==1)
load_address:                       .long 0x0001FFE4
image_size_address:                 .long 0x0001FFE8
image_compressed_address:           .long 0x0001FFEC
image_compressed_size_address:      .long 0x0001FFF0
start_address:                      .long 0x0001FFF4
stack_address:                      .long 0x0001FFF8
dbg_connected_address:              .long 0x0001FFFC
#else
load_address:                       .long 0x00007FE4
image_size_address:                 .long 0x00007FE8
image_compressed_address:           .long 0x00007FEC
image_compressed_size_address:      .long 0x00007FF0
start_address:                      .long 0x00007FF4
stack_address:                      .long 0x00007FF8
dbg_connected_address:              .long 0x00007FFC
#endif
uncompress_function:                .long UNCOMPRESS_SYMBOL

###########################################################################
################################# POKE LIST ###############################
###########################################################################

    .section    .text                                                       ! Section name
    .org        0x1000                                                      ! Entry point
__pokelist_Start:
    #include "pokelist.h"
    END_MARKER
__pokelist_Stop:
    .balign 4

.end




