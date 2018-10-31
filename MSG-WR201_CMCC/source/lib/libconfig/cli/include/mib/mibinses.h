    /*
     * Notice: file generated by MIBMAKE from mibinses.mdf
     * DO NOT EDIT THIS FILE!!!  Edit mibinses.mdf
     */
/*
 * $Id: mibinses.h,v 1.1.1.1 2011/08/17 11:11:56 brwang Exp $
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


#if ! defined( MIBINSES_H )
# define MIBINSES_H

# include "mibgen.h"
# include "mibglob.h"
# include "mibfiltr.h"
# include "mibtsidle.h"

# define SESOPTIONS_IDLELIMIT 120

# if ( MDF_MAX_INFORMATION_RATE == FYES )
#  define MAX_INFO_RATE_MAX 	64000
#  define MAX_INFO_RATE_MIN 	0
# endif /* MDF_MAX_INFORMATION_RATE */

# if ( USERIF & CLI )
extern const MibFieldNumberParams	filterNumberParams;
# endif /* ( USERIF & CLI ) */
    /*
     * How to build the factory instance of this profile.
     */
extern ConstPtr
mibFactorySesOptions( MibProfileType	mibType,
		      Uint32		info );

    /*
     * Our parameter table is made external as the profile is used
     * within other profiles.
     */
extern const MibFieldComplexParams	sesOptionsParams;
extern const MibFieldEnumParams		adslCapUpStrmRateEnumParams;
extern const MibFieldEnumParams		adslCapDwnStrmRateEnumParams;

extern const MibFieldEnumParams		adslDmtUpStrmRateEnumParams;
extern const MibFieldEnumParams		adslDmtDwnStrmRateEnumParams;

extern const MibFieldEnumParams		sdslLineRateEnumParams;



    /*
     * RateTypeEnum:
     *
     * Rate Type for connection profile selection.
     *
     *  rateTypeDisabled
     *
     *  rateTypeSdsl
     *
     *  rateTypeAdslCap
     *
     *  rateTypeAdslDmtCell
     *
     *  rateTypeAdslDmt
     */
typedef enum RateTypeEnum {
    rateTypeDisabled = 0,
    rateTypeSdsl = 1,
    rateTypeAdslCap = 2,
    rateTypeAdslDmtCell = 3,
    rateTypeAdslDmt = 4
} RateTypeEnum;


static __inline__ Bool
validRateTypeEnum (int x)
{
    switch (x) {
    case rateTypeDisabled:
    case rateTypeSdsl:
    case rateTypeAdslCap:
    case rateTypeAdslDmtCell:
    case rateTypeAdslDmt:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * RateModeEnum:
     *
     * Rate Mode for connection/line profile selection.
     *
     *  rateModeAutoBaud
     *
     *  rateModeSingleBaud
     */
typedef enum RateModeEnum {
    rateModeAutoBaud = 1,
    rateModeSingleBaud = 2
} RateModeEnum;


static __inline__ Bool
validRateModeEnum (int x)
{
    switch (x) {
    case rateModeAutoBaud:
    case rateModeSingleBaud:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * AdslCapUpStrmRateEnum:
     *
     * Rates for ADSL-CAP up stream rate selection.
     *
     *  UP_meg1088000
     *
     *  UP_kilo952000
     *
     *  UP_kilo816000
     *
     *  UP_kilo680000
     *
     *  UP_kilo544000
     *
     *  UP_kilo408000
     *
     *  UP_kilo272000
     *
     *  UP_MAX
     */
typedef enum AdslCapUpStrmRateEnum {
    UP_meg1088000 = 50,
    UP_kilo952000 = 51,
    UP_kilo816000 = 52,
    UP_kilo680000 = 53,
    UP_kilo544000 = 54,
    UP_kilo408000 = 55,
    UP_kilo272000 = 56,
    UP_MAX
} AdslCapUpStrmRateEnum;


static __inline__ Bool
validAdslCapUpStrmRateEnum (int x)
{
    switch (x) {
    case UP_meg1088000:
    case UP_kilo952000:
    case UP_kilo816000:
    case UP_kilo680000:
    case UP_kilo544000:
    case UP_kilo408000:
    case UP_kilo272000:
    case UP_MAX:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * AdslCapDwnStrmRateEnum:
     *
     * Rates for ADSL-CAP down stream rate selection.
     *
     *  DN_meg7168000
     *
     *  DN_meg6272000
     *
     *  DN_meg5120000
     *
     *  DN_meg4480000
     *
     *  DN_meg3200000
     *
     *  DN_meg2688000
     *
     *  DN_meg2560000
     *
     *  DN_meg2240000
     *
     *  DN_meg1920000
     *
     *  DN_meg1600000
     *
     *  DN_meg1280000
     *
     *  DN_kilo960000
     *
     *  DN_kilo640000
     *
     *  DN_MAX
     */
typedef enum AdslCapDwnStrmRateEnum {
    DN_meg7168000 = 0,
    DN_meg6272000 = 1,
    DN_meg5120000 = 2,
    DN_meg4480000 = 3,
    DN_meg3200000 = 4,
    DN_meg2688000 = 5,
    DN_meg2560000 = 6,
    DN_meg2240000 = 7,
    DN_meg1920000 = 8,
    DN_meg1600000 = 9,
    DN_meg1280000 = 10,
    DN_kilo960000 = 11,
    DN_kilo640000 = 12,
    DN_MAX
} AdslCapDwnStrmRateEnum;


static __inline__ Bool
validAdslCapDwnStrmRateEnum (int x)
{
    switch (x) {
    case DN_meg7168000:
    case DN_meg6272000:
    case DN_meg5120000:
    case DN_meg4480000:
    case DN_meg3200000:
    case DN_meg2688000:
    case DN_meg2560000:
    case DN_meg2240000:
    case DN_meg1920000:
    case DN_meg1600000:
    case DN_meg1280000:
    case DN_kilo960000:
    case DN_kilo640000:
    case DN_MAX:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * AdslDmtDwnStrmRateEnum:
     *
     * Rates for ADSL-DMT down stream rate selection.
     *
     *  DDN_megDwnAuto
     *
     *  DDN_meg9504000
     *
     *  DDN_meg8960000
     *
     *  DDN_meg8000000
     *
     *  DDN_meg7168000
     *
     *  DDN_meg6272000
     *
     *  DDN_meg5120000
     *
     *  DDN_meg4480000
     *
     *  DDN_meg3200000
     *
     *  DDN_meg2688000
     *
     *  DDN_meg2560000
     *
     *  DDN_meg2240000
     *
     *  DDN_meg1920000
     *
     *  DDN_meg1600000
     *
     *  DDN_meg1280000
     *
     *  DDN_kilo960000
     *
     *  DDN_kilo640000
     *
     *  DDN_kilo512000
     *
     *  DDN_kilo384000
     *
     *  DDN_kilo256000
     *
     *  DDN_MAX
     */
typedef enum AdslDmtDwnStrmRateEnum {
    DDN_megDwnAuto = 100,
    DDN_meg9504000 = 101,
    DDN_meg8960000 = 102,
    DDN_meg8000000 = 103,
    DDN_meg7168000 = 104,
    DDN_meg6272000 = 105,
    DDN_meg5120000 = 106,
    DDN_meg4480000 = 107,
    DDN_meg3200000 = 108,
    DDN_meg2688000 = 109,
    DDN_meg2560000 = 110,
    DDN_meg2240000 = 111,
    DDN_meg1920000 = 112,
    DDN_meg1600000 = 113,
    DDN_meg1280000 = 114,
    DDN_kilo960000 = 115,
    DDN_kilo640000 = 116,
    DDN_kilo512000 = 117,
    DDN_kilo384000 = 118,
    DDN_kilo256000 = 119,
    DDN_MAX
} AdslDmtDwnStrmRateEnum;


static __inline__ Bool
validAdslDmtDwnStrmRateEnum (int x)
{
    switch (x) {
    case DDN_megDwnAuto:
    case DDN_meg9504000:
    case DDN_meg8960000:
    case DDN_meg8000000:
    case DDN_meg7168000:
    case DDN_meg6272000:
    case DDN_meg5120000:
    case DDN_meg4480000:
    case DDN_meg3200000:
    case DDN_meg2688000:
    case DDN_meg2560000:
    case DDN_meg2240000:
    case DDN_meg1920000:
    case DDN_meg1600000:
    case DDN_meg1280000:
    case DDN_kilo960000:
    case DDN_kilo640000:
    case DDN_kilo512000:
    case DDN_kilo384000:
    case DDN_kilo256000:
    case DDN_MAX:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * AdslDmtUpStrmRateEnum:
     *
     * Rates for ADSL-DMT up stream rate selection.
     *
     *  DUP_megUpAuto
     *
     *  DUP_meg1088000
     *
     *  DUP_kilo928000
     *
     *  DUP_kilo800000
     *
     *  DUP_kilo672000
     *
     *  DUP_kilo544000
     *
     *  DUP_kilo384000
     *
     *  DUP_kilo256000
     *
     *  DUP_kilo128000
     *
     *  DUP_MAX
     */
typedef enum AdslDmtUpStrmRateEnum {
    DUP_megUpAuto = 150,
    DUP_meg1088000 = 151,
    DUP_kilo928000 = 152,
    DUP_kilo800000 = 153,
    DUP_kilo672000 = 154,
    DUP_kilo544000 = 155,
    DUP_kilo384000 = 156,
    DUP_kilo256000 = 157,
    DUP_kilo128000 = 158,
    DUP_MAX
} AdslDmtUpStrmRateEnum;


static __inline__ Bool
validAdslDmtUpStrmRateEnum (int x)
{
    switch (x) {
    case DUP_megUpAuto:
    case DUP_meg1088000:
    case DUP_kilo928000:
    case DUP_kilo800000:
    case DUP_kilo672000:
    case DUP_kilo544000:
    case DUP_kilo384000:
    case DUP_kilo256000:
    case DUP_kilo128000:
    case DUP_MAX:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * SdslUnitTypeEnum:
     *
     * SDSL Unit Types.
     *
     *  SDSL_API_COE
     *
     *  SDSL_API_CPE
     */
typedef enum SdslUnitTypeEnum {
    SDSL_API_COE,
    SDSL_API_CPE
} SdslUnitTypeEnum;


static __inline__ Bool
validSdslUnitTypeEnum (int x)
{
    switch (x) {
    case SDSL_API_COE:
    case SDSL_API_CPE:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * SdslLineRateEnum:
     *
     * Rates for SDSL line rate selection.
     *
     *  kilo144000
     *
     *  kilo272000
     *
     *  kilo400000
     *
     *  kilo528000
     *
     *  kilo784000
     *
     *  meg1168000
     *
     *  meg1552000
     *
     *  SDSL_MAX
     */
typedef enum SdslLineRateEnum {
    kilo144000 = 0,
    kilo272000 = 1,
    kilo400000 = 2,
    kilo528000 = 3,
    kilo784000 = 4,
    meg1168000 = 5,
    meg1552000 = 6,
    SDSL_MAX
} SdslLineRateEnum;


static __inline__ Bool
validSdslLineRateEnum (int x)
{
    switch (x) {
    case kilo144000:
    case kilo272000:
    case kilo400000:
    case kilo528000:
    case kilo784000:
    case meg1168000:
    case meg1552000:
    case SDSL_MAX:
        return TRUE;
    default:
        return FALSE;
    }
}

    /*
     * SesOptions:
     *
     * Session configuration options.
     *
     *  callFilter The name of the filter used to determine if a packet should
     *                                  cause the idle timer to be reset or,
     *                                  when the answer profile is used for
     *                                  connection profile defaults, if a call
     *                                  should be placed.
     *
     *  dataFilter The name of the filter used to determine if packets should
     *                                  be forwarded or dropped.
     *
     *  idleFilter The number of the filter used to determine if a packet
     *                                  should cause the idle timer to be reset
     *                                  or, when the answer profile is used for
     *                                  connection profile defaults, if a call
     *                                  should be placed.
     *
     *  routingFilter The number of the filter used to determine if packets
     *                                  should be forwarded or dropped.
     *
     *  filtPersist Determines if filters should persist between calls.
     *
     *  fill1 filler to get to 16 bit boundary for next field.
     *
     *  idleLimit The number of seconds of no activity before a call will be
     *                                  dropped. The value 0 disables the idle
     *                                  timer.
     *
     *  tsIdleMode The method used to determine when the terminal server idle
     *                                  session timer is reset.
     *
     *  tsIdleLimit The number of seconds of no activity as defined by the idle
     *                                  mode before a session will be
     *                                  terminated.
     *
     *  preemptLimit The number of seconds of idle time that a session may have
     *                                  before being prempted. If zero a
     *                                  session will not be preempted.
     *                                  preemptLimit *must* be less than
     *                                  idleLimit.
     *
     *  backup Name of the backup connection profile.
     *
     *  secondary A secondary internet profile used for dial back-up support.
     *
     *  backupEnabled internal field used with backup profiles.
     *
     *  atmpGateway This profile is an ATMP gateway connection to an mobile
     *                                  client's home network.
     *
     *  idlePercent Percent threshold for tearing down the last channel on a
     *                                  MPP connection. (not used on TNT).
     *
     *  maxCallDuration The number of seconds of connect time before a call
     *                                  will be dropped. The value 0 disables
     *                                  the connection timer.
     *
     *  vtpGateway This profile is an VTP gateway connection to an mobile
     *                                  client's home network.
     *
     *  blockCountLimit Maximum number of failed attempts allowed for
     *                                  connection before blocking the call.
     *
     *  blockDuration Number of seconds that connection attempts to non
     *                                  responding network will be blocked
     *                                  before allowing retries.
     *
     *  maxATMPTunnels If this profile is an ATMP gateway, then this parameter
     *                                  specifies the maximum number of ATMP
     *                                  tunnels allowed to the ATMP home
     *                                  network specified by the name of this
     *                                  profile.
     *
     *  maxVTPTunnels If this profile is a VTP gateway, then this parameter
     *                                  specifies the maximum number of VTP
     *                                  tunnels allowed to the VTP home network
     *                                  specified by the name of this profile.
     *
     *  redialDelayLimit Number of seconds delay before allowing a redial
     *
     *  rateType The per ses rate type.
     *
     *  rateMode The per ses rate mode.
     *
     *  adslCapUpRate The per session adsl-cap up stream data rate
     *
     *  adslCapDwnRate The per session adsl-cap down stream data rate
     *
     *  sdslRate The per session symetrical data rate. This parameter only
     *                                  pertains to the 24sdsl_data_card and
     *                                  24sdsl_voice_card.
     *
     *  adslDmtUpRate The per session adsl-dmt up stream data rate
     *
     *  adslDmtDwnRate The per session adsl-dmt down stream data rate
     *
     *  maxRxDataRate The maximum information rate to be received from the
     *                                  named station (kbps).
     *
     *  maxTxDataRate The maximum information rate to be transmitted to the
     *                                  named station (kbps).
     */
typedef struct st_sesOptions {
#if ( PLATFORM & TNT )
    char                              callFilter[ MAX_PROFILE_NAME_LENGTH + 1 ];
#endif /* ( PLATFORM & TNT ) */
#if ( PLATFORM & TNT )
    char                              dataFilter[MAX_PROFILE_NAME_LENGTH + 1 ];
#endif /* ( PLATFORM & TNT ) */
    char                              tmrgFilter[MAX_PROFILE_NAME_LENGTH+1];
    char                              downTmrgFilter[MAX_PROFILE_NAME_LENGTH+1];

    Uint16                            routingFilter;
    char                              inDataFilter[MAX_PROFILE_NAME_LENGTH+1];
    char                              outDataFilter[MAX_PROFILE_NAME_LENGTH+1];

    Uint16                            idleFilter;
    char                              inIdleFilter[MAX_PROFILE_NAME_LENGTH+1];
    char                              outIdleFilter[ MAX_PROFILE_NAME_LENGTH+1];

    Boolean                           filtPersist;
    Uint8                             fill1;
    Uint16                            idleLimit;
    dataIdleMode		      idleMode;
    TSIdleMode                        tsIdleMode;
    Uint16                            tsIdleLimit;
    Uint16                            preemptLimit;
#if ( PLATFORM & ( MPORT | P50 | PDSL | M200 | TNT ))
    char                                backup[ MAX_HOST_NAME_LENGTH + 1 ];
#endif /* ( PLATFORM & ( MPORT | P50 | PDSL | M200 | TNT )) */
#if ( PLATFORM & ( P25 | P50 | PDSL | M200 ))
    char                              secondary[ MAX_HOST_NAME_LENGTH + 1 ];
#endif /* ( PLATFORM & ( P25 | P50 | PDSL | M200 )) */
    Boolean                           backupEnabled;
#if ( MDF_ATMP == FYES ) && ( ! Q_TNT )
    Boolean                             atmpGateway;
#endif /* ( MDF_ATMP == FYES ) && ( ! Q_TNT ) */
    Percentage                          idlePercent;
    Uint32                              maxCallDuration;
#if ( MDF_VTP == FYES ) && ( ! Q_TNT )
    Boolean                             vtpGateway;
#endif /* ( MDF_VTP == FYES ) && ( ! Q_TNT ) */
#if ( PLATFORM & ( MPORT | P50 | PDSL | M200 ))
    Uint16                              blockCountLimit;
#endif /* ( PLATFORM & ( MPORT | P50 | PDSL | M200 )) */
#if ( PLATFORM & ( MPORT | P50 | PDSL | M200 ))
    Uint16                              blockDuration;
#endif /* ( PLATFORM & ( MPORT | P50 | PDSL | M200 )) */
#if ( MDF_ATMP == FYES ) && ( ! Q_TNT )
    Uint16                              maxATMPTunnels;
#endif /* ( MDF_ATMP == FYES ) && ( ! Q_TNT ) */
#if ( MDF_VTP == FYES ) && ( ! Q_TNT )
    Uint16                              maxVTPTunnels;
#endif /* ( MDF_VTP == FYES ) && ( ! Q_TNT ) */
#if ( MDF_PIPE_REDIAL_DELAY == FYES )
    Uint16                              redialDelayLimit;
#endif /* ( MDF_PIPE_REDIAL_DELAY == FYES ) */
#if ( MDF_XDSL_RATE_CTRL == FYES )
    RateTypeEnum                        rateType;
#endif /* ( MDF_XDSL_RATE_CTRL == FYES ) */
#if ( MDF_XDSL_RATE_CTRL == FYES )
    RateModeEnum                        rateMode;
#endif /* ( MDF_XDSL_RATE_CTRL == FYES ) */
#if ( MDF_XDSL_RATE_CTRL == FYES )
    AdslCapUpStrmRateEnum               adslCapUpRate;
#endif /* ( MDF_XDSL_RATE_CTRL == FYES ) */
#if ( MDF_XDSL_RATE_CTRL == FYES )
    AdslCapDwnStrmRateEnum              adslCapDwnRate;
#endif /* ( MDF_XDSL_RATE_CTRL == FYES ) */
#if ( MDF_XDSL_RATE_CTRL == FYES )
    SdslLineRateEnum                    sdslRate;
#endif /* ( MDF_XDSL_RATE_CTRL == FYES ) */
#if ( MDF_XDSL_RATE_CTRL == FYES )
    AdslDmtUpStrmRateEnum               adslDmtUpRate;
#endif /* ( MDF_XDSL_RATE_CTRL == FYES ) */
#if ( MDF_XDSL_RATE_CTRL == FYES )
    AdslDmtDwnStrmRateEnum              adslDmtDwnRate;
#endif /* ( MDF_XDSL_RATE_CTRL == FYES ) */
#if ( MDF_MAX_INFORMATION_RATE == FYES )
    Uint32                              maxRxDataRate;
    Uint32                              maxRxDataPPS;
#endif /* ( MDF_MAX_INFORMATION_RATE == FYES ) */
#if ( MDF_MAX_INFORMATION_RATE == FYES )
    Uint32                              maxTxDataRate;
    Uint32                              maxTxDataPPS;
#endif /* ( MDF_MAX_INFORMATION_RATE == FYES ) */
}
#if ( USERIF & CLI )
__PACK__
#endif
SesOptions;

#endif /* MIBINSES_H */
