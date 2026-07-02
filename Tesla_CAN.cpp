#include "Tesla_CAN.h"

signal_t SCCM_steeringAngle               = {SCCM_steeringAngleSensor, 16, 14, -1, 0};  // 0x129, b0, 16b
signal_t SCCM_rightStalkCounter           = {SCCM_rightStalk,           8,  4, -1, 0};  // 0x229, b8,  4b
signal_t SCCM_rightStalkStatus            = {SCCM_rightStalk,          12,  3, -1, 0};  // 0x229, b12, 3b
signal_t APP_tcStateMachine               = {APP_trafficControl,        2,  3, -1, 0};  // 0x25D, b2,  3b
signal_t APP_tcControlType                = {APP_trafficControl,       11,  5, -1, 0};  // 0x25D, b11, 5b
signal_t APP_tcControlLightState          = {APP_trafficControl,       24,  3, -1, 0};  // 0x25D, b24, 3b
signal_t EPAS3S_handsOnLevel              = {EPAS3S_sysStatus,         39,  2, -1, 0};  // 0x370, b39, 2b
signal_t DAS_autopilotState               = {DAS_status,                0,  4, -1, 0};  // 0x399, b0,  4b
signal_t VCLEFT_frontOccupancySwitch      = {VCLEFT_switchStatus,      50,  2,  0, 1};  // 0x3C2, b50, 2b, m0
signal_t VCFRONT_indicatorLeftInternal    = {VCFRONT_lighting,         50,  2, -1, 0};  // 0x3F5, b50, 2b
signal_t VCFRONT_indicatorRightInternal   = {VCFRONT_lighting,         52,  2, -1, 0};  // 0x3F5, b52, 2b
signal_t UI_frontRightSeatTrackForward    = {UI_frontSeatRequests,      0,  1, -1, 0};  // 0x4F3, b0,  1b
signal_t UI_frontRightSeatTrackBack       = {UI_frontSeatRequests,      1,  1, -1, 0};  // 0x4F3, b1,  1b
signal_t UI_frontRightSeatBackrestBack    = {UI_frontSeatRequests,      4,  1, -1, 0};  // 0x4F3, b4,  1b
signal_t UI_frontRightSeatBackrestForward = {UI_frontSeatRequests,      5,  1, -1, 0};  // 0x4F3, b5,  1b
signal_t UI_fsdStopsControlEnabled        = {UI_autopilotControl,      38,  1,  0, 0};  // 0x3FD, b38, 1b
signal_t UI_fsdContinueOnGreenWithCIPV    = {UI_autopilotControl,      39,  1,  0, 0};  // 0x3FD, b39, 1b
signal_t UI_applyEceR79                   = {UI_autopilotControl,      19,  1,  1, 0};  // 0x3FD, b19, 1b
signal_t UI_hardCoreSummon                = {UI_autopilotControl,      47,  1,  1, 0};  // 0x3FD, b20, 1b
signal_t UI_alcOffHighwayEnable           = {UI_driverAssistControl,   56,  1,  1, 0};  // 0x3F8, b56, 1b


twai_handle_t can0_handle = NULL; 
twai_handle_t can1_handle = NULL;

// ---------------------------------------------------------------------------
// 라벨 테이블
// ---------------------------------------------------------------------------
const char* APP_tcStateMachine_state[] = {
  "DISABLED",
  "STANDBY",
  "AWARE",
  "WARNING",
  "STOPPING",
  "STOPPED",
  "CONTINUING"
};

const char* APP_tcControlType_state[] = {
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

const char* APP_tcControlLightState_state[] = {
  "LIGHTSTATE_NONE",
  "LIGHTSTATE_RED",
  "LIGHTSTATE_GREEN",
  "LIGHTSTATE_YELLOW",
  "LIGHTSTATE_OFF",
  "LIGHTSTATE_WHITE",
  "LIGHTSTATE_OTHER"
};

const char* VCLEFT_frontOccupancySwitch_state[] = {
  "SNA",
  "OFF",
  "ON",
  "FAULT"
};

const char* DAS_autopilotState_state[] = {
  "DISABLED",
  "UNAVAILABLE",
  "AVAILABLE",
  "ACTIVE_NOMINAL",
  "ACTIVE_RESTRICTED",
  "ACTIVE_NAV",
  "ACTIVE_FSD",
  "NULL",
  "ABORTING",
  "ABORTED",
  "NULL",
  "NULL",
  "NULL",
  "NULL",
  "FAULT",
  "SNA"
};

const char* VCFRONT_indicatorInternal_state[] = {
  "OFF",
  "ACTIVE_LOW",
  "ACTIVE_HIGH",
};

const char* SCCM_rightStalkStatus_state[] = {
  "IDLE",
  "UP_1",
  "UP_2",
  "DOWN_1",
  "DOWN_2",
  "INIT",
  "SNA"
};

void beginCAN(gpio_num_t CAN_TX_PIN, gpio_num_t CAN_RX_PIN, int controller_id) {
  // Initialize configuration structures using macro initializers
  twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT_V2(controller_id, CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_NORMAL);
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  twai_handle_t handle = NULL;

  // Install TWAI driver
  ESP_ERROR_CHECK(twai_driver_install_v2(&g_config, &t_config, &f_config, &handle));

  // Start TWAI driver
  ESP_ERROR_CHECK(twai_start_v2(handle));

  if (controller_id == 0) {
    can0_handle = handle;
  } else if (controller_id == 1) {
    can1_handle = handle;
  }
}

String CAN2String(twai_message_t msg) {
  String result = "";
  result += "CAN ID: 0x" + String(msg.identifier, HEX) + ", ";
  result += "DLC: " + String(msg.data_length_code) + ", ";
  result += "Data: ";
  for (int i = 0; i < msg.data_length_code; i++) {
      result += String(msg.data[i], HEX) + " ";
  }
  result += "\n";
  // Serial.print(result);
  return result;
}

void sendCAN(twai_handle_t handle, twai_message_t msg) {
  if (twai_transmit_v2(handle, &msg, pdMS_TO_TICKS(100)) != ESP_OK) {
    Serial.println("[CAN] 송신 실패");
  }
}

void setBit(twai_message_t msg, uint8_t bit_index, bool value) {
  uint8_t byte_idx = bit_index / 8;   // 어느 바이트인지
  uint8_t bit_pos  = bit_index % 8;   // 그 바이트 안에서 몇 번째 비트인지 (LSB 기준)
  if (byte_idx >= 8) return;          // CAN 클래식은 최대 8바이트 범위 보호
  if (value) {
    msg.data[byte_idx] |=  (1u << bit_pos);   // 해당 비트 1로 set
  } else {
    msg.data[byte_idx] &= ~(1u << bit_pos);   // 해당 비트 0으로 clear
  }
}

uint16_t getByte(twai_message_t msg, uint8_t start_index, uint8_t bit_count) {
  uint16_t result = 0;
  for (uint8_t i = 0; i < bit_count; i++) {
    uint8_t bit_index = start_index + i;
    uint8_t byte_idx  = bit_index / 8;
    if (byte_idx >= 8) break;          // 범위 보호 추가
    uint8_t bit_pos   = bit_index % 8;
    uint8_t bit = (msg.data[byte_idx] >> bit_pos) & 0x01;
    result |= ((uint16_t)bit << i);
  }
  return result;
}

void setByte(twai_message_t &msg, uint8_t start_index, uint8_t bit_count, uint16_t value) {
  for (uint8_t i = 0; i < bit_count; i++) {
    uint8_t bit_index = start_index + i;
    uint8_t byte_idx  = bit_index / 8;
    if (byte_idx >= 8) break;          // 범위 보호 추가
    uint8_t bit_pos   = bit_index % 8;
    uint8_t bit_val = (uint8_t)(value >> i) & 0x01;
    msg.data[byte_idx] &= ~(1U << bit_pos);
    msg.data[byte_idx] |= (bit_val << bit_pos);
  }
}

uint16_t getMux(twai_message_t msg, uint8_t bits) {
  return getByte(msg, 0, bits);
}

bool getSignal(twai_message_t msg, signal_t *signal) {
    uint16_t temp = getByte(msg, signal->bit, signal->len);
    if (signal->value != temp) {
        signal->value = temp;
        return true;
    }
    return false;
}

void setSignal(twai_message_t &msg, signal_t *signal) {
    setByte(msg, signal->bit, signal->len, signal->value); 
}

void oneRightStalk(twai_message_t msg) {
  getSignal(msg, &SCCM_rightStalkCounter);
  SCCM_rightStalkCounter.value += 1;
  msg.data[0] = magicBytes229[SCCM_rightStalkCounter.value];
  msg.data[1] = 0x30 | SCCM_rightStalkCounter.value;
  sendCAN(can0_handle, msg);
}