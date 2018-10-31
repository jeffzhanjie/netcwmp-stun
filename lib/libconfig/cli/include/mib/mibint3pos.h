    /*
     * Notice: file generated by MIBMAKE from mibint3pos.mdf
     * DO NOT EDIT THIS FILE!!!  Edit mibint3pos.mdf
     */
#line 3 "mibint3pos.mdf"
/*
 * $Id: mibint3pos.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
 *
 *      Copyright (c) 1996 UTT Technologies, Inc.
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *
 *
 *                      CONFIDENTIAL INFORMATION
 *                      ------------------------
 *      This Document contains Confidential Information or Trade Secrets,
 *      or both, which are the property of UTT Technologies, Inc.
 *      This document may not be copied, reproduced, reduced to any
 *      electronic medium or machine readable form or otherwise duplicated
 *      and the information herein may not be used, disseminated or
 *      otherwise disclosed, except with the prior written consent of
 *      UTT Technologies, Inc.
 */

#line 32 "mibint3pos.mdf"

#if (( ! defined( MIBINT3POS_H ) ) && \
     ( ( MDF_X25_T3POS == FYES ) ))
#define MIBINT3POS_H

#include "mibgen.h"
#include "mibglob.h"


    /*
     * How to build the factory instance of this profile.
     */
extern ConstPtr
mibFactoryT3PosOptions( MibProfileType	mibType,
			Uint32		info );

    /*
     * Our parameter table is made external as the profile is used
     * within other profiles.
     */
extern const MibFieldComplexParams	t3PosOptionsParams;


    /*
     * T3PosMode:
     *
     * Operation mode for T3POS/PAD as described in GR-2803
     *
     *  T3POS_MODE_LOCAL T3POS local mode.
     *
     *  T3POS_MODE_BIN_LOCAL T3POS Binary-local mode.
     *
     *  T3POS_MODE_TRANSPARENT T3POS transparent mode.
     *
     *  T3POS_MODE_BLIND T3POS blind mode.
     *
     *  T3POS_MODE_OPEN T3POS open mode (waiting for opening frame to select
     *                                  mode).
     *
     *  T3POS_MODE_LAST Number of T3POS mode
     */
typedef enum T3PosMode {
#line 73 "mibint3pos.mdf"
    T3POS_MODE_LOCAL,
#line 76 "mibint3pos.mdf"
    T3POS_MODE_BIN_LOCAL,
#line 79 "mibint3pos.mdf"
    T3POS_MODE_TRANSPARENT,
#line 82 "mibint3pos.mdf"
    T3POS_MODE_BLIND,
#line 85 "mibint3pos.mdf"
    T3POS_MODE_OPEN,
#line 88 "mibint3pos.mdf"
    T3POS_MODE_LAST
} T3PosMode;


static __inline__ Bool
validT3PosMode (int x)
{
    switch (x) {
#line 73 "mibint3pos.mdf"
    case T3POS_MODE_LOCAL:
#line 76 "mibint3pos.mdf"
    case T3POS_MODE_BIN_LOCAL:
#line 79 "mibint3pos.mdf"
    case T3POS_MODE_TRANSPARENT:
#line 82 "mibint3pos.mdf"
    case T3POS_MODE_BLIND:
#line 85 "mibint3pos.mdf"
    case T3POS_MODE_OPEN:
#line 88 "mibint3pos.mdf"
    case T3POS_MODE_LAST:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * T3PosMaxBlockSize:
     *
     * T3POS Max Block Size.
     *
     *  T3POS_MAX_BLKSZ_512
     *
     *  T3POS_MAX_BLKSZ_1024
     *
     *  T3POS_MAX_BLKSZ_LAST Number of T3POS Max block size enum.
     */
typedef enum T3PosMaxBlockSize {
#line 98 "mibint3pos.mdf"
    T3POS_MAX_BLKSZ_512,
#line 101 "mibint3pos.mdf"
    T3POS_MAX_BLKSZ_1024,
#line 104 "mibint3pos.mdf"
    T3POS_MAX_BLKSZ_LAST
} T3PosMaxBlockSize;


static __inline__ Bool
validT3PosMaxBlockSize (int x)
{
    switch (x) {
#line 98 "mibint3pos.mdf"
    case T3POS_MAX_BLKSZ_512:
#line 101 "mibint3pos.mdf"
    case T3POS_MAX_BLKSZ_1024:
#line 104 "mibint3pos.mdf"
    case T3POS_MAX_BLKSZ_LAST:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * T3PosMethodOfHostNotif:
     *
     * T3POS Method of Host Notification.
     *
     *  T3POS_METHOD_HNOTIF_NONE T3POS Method of Host Notification: None
     *
     *  T3POS_METHOD_HNOTIF_CRP T3POS Method of Host Notification: Call Request
     *                                  Packet
     *
     *  T3POS_METHOD_HNOTIF_MSF T3POS Method of Host Notification: Mode Switch
     *                                  Frame
     *
     *  T3POS_METHOD_HNOTIF_LAST Number of T3POS Method of Host Notification.
     */
typedef enum T3PosMethodOfHostNotif {
#line 114 "mibint3pos.mdf"
    T3POS_METHOD_HNOTIF_NONE,
#line 117 "mibint3pos.mdf"
    T3POS_METHOD_HNOTIF_CRP,
#line 120 "mibint3pos.mdf"
    T3POS_METHOD_HNOTIF_MSF,
#line 123 "mibint3pos.mdf"
    T3POS_METHOD_HNOTIF_LAST
} T3PosMethodOfHostNotif;


static __inline__ Bool
validT3PosMethodOfHostNotif (int x)
{
    switch (x) {
#line 114 "mibint3pos.mdf"
    case T3POS_METHOD_HNOTIF_NONE:
#line 117 "mibint3pos.mdf"
    case T3POS_METHOD_HNOTIF_CRP:
#line 120 "mibint3pos.mdf"
    case T3POS_METHOD_HNOTIF_MSF:
#line 123 "mibint3pos.mdf"
    case T3POS_METHOD_HNOTIF_LAST:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * T3PosPidSelection:
     *
     * T3POS PID selection.
     *
     *  T3POS_PID_X29 X.29 PID
     *
     *  T3POS_PID_T3POS T3POS PID
     *
     *  T3POS_PID_LAST Number of T3POS PID enum.
     */
typedef enum T3PosPidSelection {
#line 133 "mibint3pos.mdf"
    T3POS_PID_X29,
#line 136 "mibint3pos.mdf"
    T3POS_PID_T3POS,
#line 139 "mibint3pos.mdf"
    T3POS_PID_LAST
} T3PosPidSelection;


static __inline__ Bool
validT3PosPidSelection (int x)
{
    switch (x) {
#line 133 "mibint3pos.mdf"
    case T3POS_PID_X29:
#line 136 "mibint3pos.mdf"
    case T3POS_PID_T3POS:
#line 139 "mibint3pos.mdf"
    case T3POS_PID_LAST:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * T3PosDataFormat:
     *
     * T3POS Opening frame and Local mode Data Format.
     *
     *  T3POS_DFORMAT_7E1 7 bit, even parity, 1 stop bit
     *
     *  T3POS_DFORMAT_7O1 7 bit, odd parity, 1 stop bit
     *
     *  T3POS_DFORMAT_7M1 7 bit, mark parity, 1 stop bit
     *
     *  T3POS_DFORMAT_7S1 7 bit, space parity, 1 stop bit
     *
     *  T3POS_DFORMAT_8N1 8 bit, no parity, 1 stop bit
     *
     *  T3POS_DFORMAT_LAST Number of data format enum.
     */
typedef enum T3PosDataFormat {
#line 149 "mibint3pos.mdf"
    T3POS_DFORMAT_7E1,
#line 152 "mibint3pos.mdf"
    T3POS_DFORMAT_7O1,
#line 155 "mibint3pos.mdf"
    T3POS_DFORMAT_7M1,
#line 158 "mibint3pos.mdf"
    T3POS_DFORMAT_7S1,
#line 161 "mibint3pos.mdf"
    T3POS_DFORMAT_8N1,
#line 164 "mibint3pos.mdf"
    T3POS_DFORMAT_LAST
} T3PosDataFormat;


static __inline__ Bool
validT3PosDataFormat (int x)
{
    switch (x) {
#line 149 "mibint3pos.mdf"
    case T3POS_DFORMAT_7E1:
#line 152 "mibint3pos.mdf"
    case T3POS_DFORMAT_7O1:
#line 155 "mibint3pos.mdf"
    case T3POS_DFORMAT_7M1:
#line 158 "mibint3pos.mdf"
    case T3POS_DFORMAT_7S1:
#line 161 "mibint3pos.mdf"
    case T3POS_DFORMAT_8N1:
#line 164 "mibint3pos.mdf"
    case T3POS_DFORMAT_LAST:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * T3PosAccessType:
     *
     * T3POS Link Access Type.
     *
     *  T3POS_AXTYPE_DEDICATED Dedicated link access type.
     *
     *  T3POS_AXTYPE_DIAL Dial-in link access type.
     *
     *  T3POS_AXTYPE_LAST Number of access type.
     */
typedef enum T3PosAccessType {
#line 174 "mibint3pos.mdf"
    T3POS_AXTYPE_DEDICATED,
#line 177 "mibint3pos.mdf"
    T3POS_AXTYPE_DIAL,
#line 180 "mibint3pos.mdf"
    T3POS_AXTYPE_LAST
} T3PosAccessType;


static __inline__ Bool
validT3PosAccessType (int x)
{
    switch (x) {
#line 174 "mibint3pos.mdf"
    case T3POS_AXTYPE_DEDICATED:
#line 177 "mibint3pos.mdf"
    case T3POS_AXTYPE_DIAL:
#line 180 "mibint3pos.mdf"
    case T3POS_AXTYPE_LAST:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * T3PosOnOffSelection:
     *
     * T3POS ON/OFF selection
     *
     *  T3POS_ONOFF_OFF OFF selection
     *
     *  T3POS_ONOFF_ON ON selection
     *
     *  T3POS_ONOFF_LAST Number of T3POS On/Off enum.
     */
typedef enum T3PosOnOffSelection {
#line 189 "mibint3pos.mdf"
    T3POS_ONOFF_OFF,
#line 192 "mibint3pos.mdf"
    T3POS_ONOFF_ON,
#line 195 "mibint3pos.mdf"
    T3POS_ONOFF_LAST
} T3PosOnOffSelection;


static __inline__ Bool
validT3PosOnOffSelection (int x)
{
    switch (x) {
#line 189 "mibint3pos.mdf"
    case T3POS_ONOFF_OFF:
#line 192 "mibint3pos.mdf"
    case T3POS_ONOFF_ON:
#line 195 "mibint3pos.mdf"
    case T3POS_ONOFF_LAST:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * T3PosOptions:
     *
     * T3Posinet configuration options.
     *
     *  t3PosEnabled Hidden, for answer profile
     *
     *  x25Profile Name of the x25 profile that this profile is associated
     *                                  with.
     *
     *  recvPassword For future use. We currently use the password in the PPP
     *                                  sub-profile.
     *
     *  fill2
     *
     *  maxCalls Max number of unsuccessful calls
     *
     *  x121Addr X.121 address to auto-call upon session startup.
     *
     *  fill1
     *
     *  x25RevCharge Reverse charge request
     *
     *  x121Answer Answer number: mandatory if call mode is incoming or both.
     *
     *  fill3
     *
     *  t3PosHostInitMode T3POS Host Initialization mode:
     *                                  LOCAL/BIN-LOCAL/TRANSPARENT/BLIND
     *
     *  t3PosDteInitMode T3POS DTE Initialization mode:
     *                                  LOCAL/BIN-LOCAL/TRANSPARENT/BLIND
     *
     *  t3PosEnqHandling T3POS ENQ handling param
     *
     *  t3PosMaxBlockSize T3POS Max block size: 512/1024
     *
     *  t3PosT1 T3POS T1 timer.
     *
     *  t3PosT2 T3POS T2 timer.
     *
     *  t3PosT3 T3POS T3 timer.
     *
     *  t3PosT4 T3POS T4 timer.
     *
     *  t3PosT5 T3POS T5 timer.
     *
     *  t3PosT6 T3POS T6 timer.
     *
     *  t3PosMethodOfHostNotif T3POS Method of Host Notification.
     *
     *  t3PosPidSelection T3POS PID selection: X.29, T3POS.
     *
     *  t3PosAckSuppression T3POS ACK suppression.
     *
     *  x25Rpoa X.25 RPOA facility.
     *
     *  x25Cug X.25 CUG index.
     *
     *  x25Nui X.25 NUI facility.
     *
     *  t3PosDataFormat T3POS Open/Local Data Format.
     *
     *  t3PosAccessType T3POS Link access type.
     */
typedef struct t3PosOptions {
#line 205 "mibint3pos.mdf"
    Boolean                             t3PosEnabled;
#line 211 "mibint3pos.mdf"
    char                                x25Profile[ MAX_X25_NAME_LEN + 1 ];
#line 216 "mibint3pos.mdf"
#if ( PLATFORM & TNT )
    char                                recvPassword[ MAX_PASSWORD_LENGTH + 1 ];
#endif /* ( PLATFORM & TNT ) */
#line 224 "mibint3pos.mdf"
#if ( ! ( PLATFORM & TNT ))
    char                                fill2;
#endif /* ( ! ( PLATFORM & TNT )) */
#line 230 "mibint3pos.mdf"
    Uint16                              maxCalls;
#line 235 "mibint3pos.mdf"
    char                                x121Addr[ MAX_X25_AUTO_CALL_LEN + 1 ];
#line 240 "mibint3pos.mdf"
    char                                fill1;
#line 245 "mibint3pos.mdf"
    Boolean                             x25RevCharge;
#line 250 "mibint3pos.mdf"
    char                                x121Answer[ MAX_X25_NAME_LEN + 1 ];
#line 255 "mibint3pos.mdf"
    char                                fill3;
#line 260 "mibint3pos.mdf"
    T3PosMode                           t3PosHostInitMode;
#line 265 "mibint3pos.mdf"
    T3PosMode                           t3PosDteInitMode;
#line 270 "mibint3pos.mdf"
    T3PosOnOffSelection                 t3PosEnqHandling;
#line 275 "mibint3pos.mdf"
    T3PosMaxBlockSize                   t3PosMaxBlockSize;
#line 280 "mibint3pos.mdf"
    Uint16                              t3PosT1;
#line 285 "mibint3pos.mdf"
    Uint16                              t3PosT2;
#line 290 "mibint3pos.mdf"
    Uint16                              t3PosT3;
#line 295 "mibint3pos.mdf"
    Uint16                              t3PosT4;
#line 300 "mibint3pos.mdf"
    Uint16                              t3PosT5;
#line 305 "mibint3pos.mdf"
    Uint16                              t3PosT6;
#line 310 "mibint3pos.mdf"
    T3PosMethodOfHostNotif              t3PosMethodOfHostNotif;
#line 315 "mibint3pos.mdf"
    T3PosPidSelection                   t3PosPidSelection;
#line 320 "mibint3pos.mdf"
    T3PosOnOffSelection                 t3PosAckSuppression;
#line 325 "mibint3pos.mdf"
    char                                x25Rpoa[ MAX_X25_RPOA_LEN + 1 ];
#line 330 "mibint3pos.mdf"
    char                                x25Cug[ MAX_X25_CUG_LEN + 1 ];
#line 335 "mibint3pos.mdf"
    char                                x25Nui[ MAX_X25_NUI_LEN + 1 ];
#line 340 "mibint3pos.mdf"
    T3PosDataFormat                     t3PosDataFormat;
#line 345 "mibint3pos.mdf"
    T3PosAccessType                     t3PosAccessType;
}
#if ( USERIF & CLI )
__PACK__
#endif
T3PosOptions;
#line 411 "mibint3pos.mdf"

#endif /* (( ! defined( MIBINT3POS_H ) ) && ( MDF_X25B == FYES )) */
