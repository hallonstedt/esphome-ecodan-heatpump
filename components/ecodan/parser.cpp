#include "parser.h"

using std::string;

namespace ecodan {
namespace parser {

// Number functions
static float parseTemperature(uint8_t *packet, uint8_t index) {
  return ((float) packet[index] * 256 + (float) packet[index + 1]) / 100;
}

static float parseOneByteTemperature(uint8_t *packet, uint8_t index) {
  return (float) packet[index] / 2 - 39;
}

static float parseOneByteTemperature40(uint8_t *packet, uint8_t index) {
  return (float) packet[index] / 2 - 40;
}

static float parseHexValue(uint8_t *packet, uint8_t index) {
  return (float) packet[index];
}

static float parseDecValue(uint8_t *packet, uint8_t index) {
  return (float) packet[index];
}

static float parseRuntime(uint8_t *packet, uint8_t index) {
  return (float) (packet[index + 1] * 256 + packet[index + 2]) * 100 + packet[index];
}

static float parseOneByteTemperature20(uint8_t *packet, uint8_t index) {
  return (float) packet[index] / 2 - 20;
}

static float parse3ByteValue(uint8_t *packet, uint8_t index) {
  return (float) packet[index] * 256 + (float) packet[index + 1] + (float) packet[index + 2] / 100;
}

static float parse2ByteHexValue(uint8_t *packet, uint8_t index) {
  return (float) (packet[index] * 256 + packet[index + 1]);
}

static float parse2ByteValueDiv10(uint8_t *packet, uint8_t index) {
  return (float) (packet[index] * 256 + packet[index + 1]) / 10.0f;
}

float parsePacketNumberItem(uint8_t *packet, varTypeEnum varType, uint8_t index) {
  switch (varType) {
  case VarType_TEMPERATURE:
    return parseTemperature(packet, index);
    break;
  case VarType_ONE_BYTE_TEMPERATURE:
    return parseOneByteTemperature(packet, index);
    break;
  case VarType_ONE_BYTE_TEMPERATURE_40:
    return parseOneByteTemperature40(packet, index);
    break;
  case VarType_HEXVALUE:
    return parseHexValue(packet, index);
    break;
  case VarType_DECVALUE:
    return parseDecValue(packet, index);
    break;
  case VarType_RUNTIME:
    return parseRuntime(packet, index);
    break;
  case VarType_ONE_BYTE_TEMPERATURE_20:
    return parseOneByteTemperature20(packet, index);
    break;
  case VarType_3BYTEVALUE:
    return parse3ByteValue(packet, index);
    break;
  case VarType_TOTAL_ENERGY:
    return parse3ByteValue(packet, index) / 10;
    break;
  case VarType_2BYTEHEXVALUE:
    return parse2ByteHexValue(packet, index);
    break;
  case VarType_2BYTEVALUE_DIV10:
    return parse2ByteValueDiv10(packet, index);
    break;
  default:
    return -1;
    break;
  }
}

// Text functions
static string unknownValue(uint8_t value) {
  char textStr[50];
  sprintf(textStr, "Unknown value: %02X", value);
  return textStr;
}

static bool isBcdByte(uint8_t value) {
  return ((value >> 4) & 0x0F) <= 9 && (value & 0x0F) <= 9;
}

static uint8_t bcdToInt(uint8_t value) {
  return ((value >> 4) & 0x0F) * 10 + (value & 0x0F);
}

static string parseTimeDate(uint8_t *packet, uint8_t index) {
  char textStr[50];
  sprintf(textStr, "20%d/%02d/%02d %02d:%02d:%02d", packet[index],
        packet[index + 1], packet[index + 2], packet[index + 3], packet[index + 4], packet[index + 5]);
  return textStr;
}

static string parseOperatingMode(uint8_t *packet, uint8_t index) {
  switch (packet[index]) {
  case 0:
    return "Off";
  case 1:
    return "Hot Water";
  case 2:
    return "Heating";
  case 3:
    return "Cooling";
  case 5:
    return "Frost Protect";
  case 6:
    return "Legionella";
  default:
    return unknownValue(packet[index]);
  }
}

static string parseHotWaterMode(uint8_t *packet, uint8_t index) {
  switch (packet[index]) {
  case 0:
    return "Normal";
  case 1:
    return "Economy";
  default:
    return unknownValue(packet[index]);
  }
}

static string parseModeSetting(uint8_t *packet, uint8_t index) {
  switch (packet[index]) {
  case 0:
    return "Heating Room Temp";
  case 1:
    return "Heating Flow Temp";
  case 2:
    return "Heating Heat Curve";
  case 3:
    return "Cooling Room Temp";
  case 4:
    return "Cooling Flow Temp";
  default:
    return unknownValue(packet[index]);
  }
}

static string parseDeFrost(uint8_t *packet, uint8_t index) {
  switch (packet[index]) {
  case 0:
    return "Off";
  case 1:
    return "Standby";
  case 2:
    return "Defrost";
  case 3:
    return "Waiting Restart";
  default:
    return unknownValue(packet[index]);
  }
}

static string parseHeatCool(uint8_t *packet, uint8_t index) {
  switch (packet[index]) {
  case 0:
    return "Off";
  case 1:
  case 2:
    return "Heating Mode";
  case 4:
    return "Cooling Mode";
  default:
    return unknownValue(packet[index]);
  }
}

static string parseHeatSource(uint8_t *packet, uint8_t index) {
  switch (packet[index]) {
  case 0:
    return "Heat pump";
  case 1:
    return "Immersion heater";
  case 2:
    return "Booster heater";
  case 3:
    return "Immersion + booster";
  case 4:
    return "Boiler";
  default:
    return unknownValue(packet[index]);
  }
}

static string parseHotWaterPhase(uint8_t *packet, uint8_t index) {
  switch (packet[index]) {
  case 0:
    return "Off";
  case 1:
    return "Heat pump phase";
  case 2:
    return "Heater phase";
  default:
    return unknownValue(packet[index]);
  }
}

static string parseDate(uint8_t *packet, uint8_t index) {
  char textStr[50];
  sprintf(textStr, "20%d/%02d/%02d", packet[index],
          packet[index + 1], packet[index + 2]);
  return textStr;
}

static string parseFirmwareVersion(uint8_t *packet, uint8_t index) {
  char textStr[16];
  uint8_t major_raw = packet[index];
  uint8_t minor_raw = packet[index + 1];

  // Mitsubishi documents software versions as packed digits (for example,
  // 01.23 -> 0123). Prefer BCD decoding when the nibbles look valid.
  if (isBcdByte(major_raw) && isBcdByte(minor_raw)) {
    sprintf(textStr, "%02u.%02u", bcdToInt(major_raw), bcdToInt(minor_raw));
    return textStr;
  }

  // Fall back to a big-endian packed integer if the bytes are not BCD.
  uint16_t packed_version = ((uint16_t) major_raw << 8) | minor_raw;
  if (packed_version <= 9999) {
    sprintf(textStr, "%02u.%02u", packed_version / 100, packed_version % 100);
    return textStr;
  }

  sprintf(textStr, "0x%02X%02X", major_raw, minor_raw);
  return textStr;
}

static string parseFirmwareVersionRaw(uint8_t *packet, uint8_t index) {
  char textStr[16];
  sprintf(textStr, "0x%02X 0x%02X", packet[index], packet[index + 1]);
  return textStr;
}

static string parseFtcSoftwareVersion(uint8_t *packet, uint8_t index) {
  // 0xC9 response layout (payload offsets, with gap bytes between fields):
  // index+0: U1 - Protocol Version (BCD)
  // index+2: U2 - Model Version (BCD)
  // index+4: U3 - Capacity of Supply
  // index+5: V  - FTC Version type

  uint8_t proto = packet[index + 0];
  uint8_t model = packet[index + 2];
  uint8_t capacity = packet[index + 4];
  uint8_t ftc_type = packet[index + 5];

  const char *ftc_name;
  switch (ftc_type) {
    case 0:   ftc_name = "FTC2B"; break;
    case 1:   ftc_name = "FTC4"; break;
    case 2:   ftc_name = "FTC5"; break;
    case 3:   ftc_name = "FTC6"; break;
    case 5:   ftc_name = "FTC7"; break;
    case 128: ftc_name = "CAHV1A"; break;
    case 129: ftc_name = "CAHV1B"; break;
    case 130: ftc_name = "CRHV1A"; break;
    case 131: ftc_name = "CRHV1B"; break;
    case 132: ftc_name = "EAHV1A"; break;
    case 133: ftc_name = "EAHV1B"; break;
    case 134: ftc_name = "QAHV1A"; break;
    case 135: ftc_name = "QAHV1B"; break;
    case 144: ftc_name = "PWFY1"; break;
    default:  ftc_name = "Unknown"; break;
  }

  char textStr[96];
  sprintf(textStr, "Protocol %d%d, Model %d%d, Capacity 0x%02X (%s)",
    (proto >> 4) & 0x0F, proto & 0x0F,
    (model >> 4) & 0x0F, model & 0x0F,
    capacity,
    ftc_name);

  return textStr;
}

static string parseOnOffText(uint8_t *packet, uint8_t index) {
  switch (packet[index]) {
  case 0:
    return "Off";
  case 1:
    return "On";
  default:
    return unknownValue(packet[index]);
  }
}

static string parseHeatStage(uint8_t *packet, uint8_t index) {
  switch (packet[index]) {
  case 0:
    return "0-1 kW";
  case 1:
    return "1-2 kW";
  case 2:
    return "2-3 kW";
  case 3:
    return "3-4 kW";
  case 4:
    return "4-5 kW";
  case 5:
    return "5-6 kW";
  case 6:
    return "6-7 kW";
  case 7:
    return "7-8 kW";
  case 8:
    return "8-9 kW";
  case 9:
    return "9-10 kW";
  default:
    return unknownValue(packet[index]);
  }
}

string parsePacketTextItem(uint8_t *packet, varTypeEnum varType, uint8_t index) {
  switch (varType) {
  case VarType_TIME_DATE:
    return parseTimeDate(packet, index);
  case VarType_OPERATING_MODE:
    return parseOperatingMode(packet, index);
  case VarType_HW_MODE:
    return parseHotWaterMode(packet, index);
  case VarType_MODE_SETTING:
    return parseModeSetting(packet, index);
  case VarType_DEFROST:
    return parseDeFrost(packet, index);
  case VarType_HEAT_COOL:
    return parseHeatCool(packet, index);
  case VarType_HEAT_SOURCE:
    return parseHeatSource(packet, index);
  case VarType_HOT_WATER_PHASE:
    return parseHotWaterPhase(packet, index);
  case VarType_DATE:
    return parseDate(packet, index);
  case VarType_ON_OFF:
    return parseOnOffText(packet, index);
  case VarType_HEAT_STAGE:
    return parseHeatStage(packet, index);
  case VarType_FTC_SOFTWARE_VERSION:
    return parseFtcSoftwareVersion(packet, index);
  case VarType_FIRMWARE_VERSION:
    return parseFirmwareVersion(packet, index);
  case VarType_FIRMWARE_VERSION_RAW:
    return parseFirmwareVersionRaw(packet, index);
  default:
    return "";
  }
}

// Boolean functions
static bool parseOnOff(uint8_t *packet, uint8_t index) {
  return packet[index] == 1;
}

bool parsePacketBoolItem(uint8_t *packet, varTypeEnum varType, uint8_t index) {
  switch (varType) {
  case VarType_ON_OFF:
    return parseOnOff(packet, index);
  case VarType_DEFROST:
    return packet[index] != 0;
  default:
    return false;
  }
}

uint8_t parseModeStringToInt(commandVarTypeEnum commandVarType, const std::string &value) {
  switch (commandVarType) {
  case CommandVarType_AC_MODE_SETTING:
    if (value == "Heating Room Temp") {
      return 0;
    }
    if (value == "Heating Flow Temp") {
      return 1;
    }
    if (value == "Heating Heat Curve") {
      return 2;
    }
    if (value == "Cooling Room Temp") {
      return 3;
    }
    if (value == "Cooling Flow Temp") {
      return 4;
    }
    return 0;
  case CommandVarType_HOT_WATER_MODE:
    if (value == "Normal") {
      return 0;
    }
    if (value == "Economy") {
      return 1;
    }
    return 0;
  default:
    return 0;
  }
}

} // namespace parser
} // namespace ecodan
