#ifndef AnyHolic_Tesla_CAN_H
#define AnyHolic_Tesla_CAN_H

#include <Arduino.h>
#include "driver/twai.h"

typedef struct {
  uint16_t  id;  
  uint8_t   bit;    // 시작 비트
  uint8_t   len;    // 비트 길이
  int8_t    mux;
  uint8_t   value;
} signal_t;

// CAN ID 정의
enum {
  SCCM_rightStalk      = 0x229,  //  553 VEH
  SCCM_leftStalk       = 0x249,  //  585 VEH, CH
  APP_trafficControl   = 0x25D,  //  605      CH, PARTY
  DAS_status           = 0x399,  //  921      CH
  VCLEFT_switchStatus  = 0x3C2,  //  962 VEH, CH
  VCFRONT_lighting     = 0x3F5,  // 1013 VEH, CH
  UI_autopilotControl  = 0x3FD,  // 1021      CH
  UI_frontSeatRequests = 0x4F3   // 1267 VEH 
};

const uint8_t magicBytes229[16] = { 0x49, 0x4B, 0x5D, 0x62, 0x4C, 0x4E, 0xD2, 0xF6, 0x43, 0xAA, 0xF9, 0x83, 0x46, 0x20, 0x3E, 0x34 };
// const uint8_t magicBytes249[16] = { 0x9B, 0xE8, 0x2A, 0xD3, 0xD3, 0x83, 0x4C, 0x5E, 0x3F, 0x5E, 0xE2, 0x28, 0x3A, 0x13, 0xAF, 0xCE };

extern signal_t APP_tcStateMachine;
extern signal_t APP_tcControlType;
extern signal_t APP_tcControlLightState;
extern signal_t VCLEFT_frontOccupancySwitch;
extern signal_t VCFRONT_indicatorLeftInternal;
extern signal_t VCFRONT_indicatorRightInternal;
extern signal_t UI_frontRightSeatTrackForward;
extern signal_t UI_frontRightSeatTrackBack;
extern signal_t UI_frontRightSeatBackrestBack;
extern signal_t UI_frontRightSeatBackrestForward;
extern signal_t UI_fsdStopsControlEnabled;
extern signal_t UI_fsdContinueOnGreenWithCIPV;
extern signal_t UI_applyEceR79;
extern signal_t UI_hardCoreSummon;
extern signal_t SCCM_rightStalkStatus;
extern signal_t DAS_autopilotState;

extern const char* APP_tcStateMachine_state[];
extern const char* APP_tcControlType_state[];
extern const char* APP_tcControlLightState_state[];
extern const char* VCLEFT_frontOccupancySwitch_state[];
extern const char* VCFRONT_indicatorInternal_state[];
extern const char* DAS_autopilotState_state[];

extern twai_handle_t can0_handle;
extern twai_handle_t can1_handle;

void beginCAN(gpio_num_t CAN_TX_PIN, gpio_num_t CAN_RX_PIN, int controller_id = 0);
String CAN2String(twai_message_t msg);
void sendCAN(twai_handle_t handle, twai_message_t msg);
void setBit(uint8_t* data, uint8_t bit_index, bool value);
uint8_t getByte(const uint8_t* data, uint8_t start_index, uint8_t bit_count);
uint8_t getMux(const uint8_t* data, uint8_t bits);
bool updateValue(const uint8_t *data, signal_t *state_var);
void oneRightStalk(twai_message_t msg);

#endif