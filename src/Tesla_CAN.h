#ifndef Tesla_CAN_H
#define Tesla_CAN_H

typedef struct {
  uint16_t  id;  
  uint8_t   bit;    // 시작 비트
  uint8_t   len;    // 비트 길이
  uint8_t   mux;
  uint8_t   value;
} signal_t;

// CAN ID 정의
enum {
  SCCM_leftStalk       = 0x249,  //  585 VEH, CH
  APP_trafficControl   = 0x25D,  //  605      CH, PARTY
  VCLEFT_switchStatus  = 0x3C2,  //  962 VEH, CH
  VCFRONT_lighting     = 0x3F5,  // 1013 VEH, CH
  UI_autopilotControl  = 0x3FD,  // 1021      CH
  UI_frontSeatRequests = 0x4F3   // 1267 VEH 
};

signal_t APP_tcStateMachine             = {APP_trafficControl,   2, 3, NULL, 0};  // 0x25D, b2,  3b
signal_t APP_tcControlType              = {APP_trafficControl,  11, 5, NULL, 0};  // 0x25D, b11, 5b
signal_t APP_tcControlLightState        = {APP_trafficControl,  24, 3, NULL, 0};  // 0x25D, b24, 3b
signal_t VCLEFT_frontOccupancySwitch    = {VCLEFT_switchStatus, 50, 2,    0, 0};  // 0x3C2, b50, 2b, m0
signal_t VCFRONT_indicatorLeftInternal  = {VCFRONT_lighting,    50, 2, NULL, 0};  // 0x3F5, b50, 2b
signal_t VCFRONT_indicatorRightInternal = {VCFRONT_lighting,    52, 2, NULL, 0};  // 0x3F5, b52, 2b

String APP_tcStateMachine_Label[] = {
  "DISABLED",
  "STANDBY",
  "AWARE",
  "WARNING",
  "STOPPING",
  "STOPPED",
  "CONTINUING",
};

String APP_tcControlType_Label[] {
  "INVALID",
  "UNKNOWN",
  "STOP_SIGN",
  "TRAFFIC_LIGHT",
  "YIELD",
  "CROSSWALK",
  "KEEP_CLEAR_ENTER",
  "KEEP_CLEAR_EXIT",
  "SUICIDE_LEFT",
  "PEDESTRIAN_CROSSING",
  "RAMP_METER",
  "SPEED_BUMP",
  "SPEED_HUMP",
  "TRAFFIC_RULE",
  "ALL_WAY_STOP_SIGN",
  "BIKE_MERGE_FROM_LEFT",
  "BIKE_MERGE_FROM_RIGHT",
  "NO_STOP",
  "T_IMPLICIT",
  "T_IMPLICIT_BY_NAME",
  "T_IMPLICIT_BY_GEOM",
  "BEV_JUNCTION",
  "T_ARM"
};

String APP_tcControlLightState_Label[] {
  "LIGHTSTATE_NONE",
  "LIGHTSTATE_RED",
  "LIGHTSTATE_GREEN",
  "LIGHTSTATE_YELLOW",
  "LIGHTSTATE_OFF",
  "LIGHTSTAET_WHITE",
  "LIGHTSTATE_OTHER"
};

String VCLEFT_frontOccupancySwitch_Label[] {
  "SNA",
  "OFF",
  "ON",
  "FAULT"
};

String VCFRONT_indicatorInternal_Label[] {
  "OFF",
  "ACTIVE_LOW",
  "ACTIVE_HIGH"
};

#endif