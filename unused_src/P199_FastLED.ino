//#######################################################################################################
//#################################### Plugin 199: FastLED
//#######################################
//#######################################################################################################

// Command: FastLED <led nr>,<red>,<green>,<blue>

#include <FastLED.h>

#define PLUGIN_199
#define PLUGIN_ID_199 199
#define PLUGIN_NAME_199 "FastLED Basic"
#define PLUGIN_VALUENAME1_199 ""

#define COLOR_ORDER GRB
#define CHIPSET_DEFAULT WS2811
#define NUM_LEDS_DEFAULT 60
#define LED_PIN_DEFAULT 5

CRGB leds[NUM_LEDS_DEFAULT];

boolean initialized = false;

boolean Plugin_199(byte function, struct EventStruct *event, String &string) {
  boolean success = false;

  switch (function) {

  case PLUGIN_DEVICE_ADD: {
    Device[++deviceCount].Number = PLUGIN_ID_199;
    Device[deviceCount].Type = DEVICE_TYPE_SINGLE;
    Device[deviceCount].Custom = true;
    Device[deviceCount].TimerOption = false;
    break;
  }

  case PLUGIN_GET_DEVICENAME: {
    string = F(PLUGIN_NAME_199);
    break;
  }

  case PLUGIN_GET_DEVICEVALUENAMES: {
    strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0],
             PSTR(PLUGIN_VALUENAME1_199));
    break;
  }

  case PLUGIN_WEBFORM_LOAD: {
    char tmpString[128];
    sprintf_P(tmpString, PSTR("<TR><TD>Led Count:<TD><input type='text' "
                              "name='plugin_199_leds' size='3' value='%u'>"),
              Settings.TaskDevicePluginConfig[event->TaskIndex][0]);
    string += tmpString;

    string += F("<TR><TD>GPIO:<TD>");
    addPinSelect(false, string, "taskdevicepin1",
                 Settings.TaskDevicePin1[event->TaskIndex]);

    success = true;
    break;
  }

  case PLUGIN_WEBFORM_SAVE: {
    String plugin1 = WebServer.arg(F("plugin_199_leds"));
    Settings.TaskDevicePluginConfig[event->TaskIndex][0] = plugin1.toInt();
    String plugin2 = WebServer.arg(F("plugin_199_leds"));
    Settings.TaskDevicePluginConfig[event->TaskIndex][1] = plugin2.toInt();
    success = true;
    break;
  }

  case PLUGIN_INIT: {
    if (!initialized) {
      FastLED
          .addLeds<CHIPSET_DEFAULT, LED_PIN_DEFAULT, COLOR_ORDER>(
              leds, NUM_LEDS_DEFAULT)
          .setCorrection(TypicalLEDStrip);
      initialized = true;
    }
    success = true;
    break;
  }

  case PLUGIN_WRITE: {
    if (initialized) {
      String tmpString = string;
      int argIndex = tmpString.indexOf(',');
      if (argIndex)
        tmpString = tmpString.substring(0, argIndex);

      if (tmpString.equalsIgnoreCase(F("FastLED"))) {
        char Line[80];
        char TmpStr1[80];
        TmpStr1[0] = 0;
        string.toCharArray(Line, 80);
        int Par4 = 0;
        if (GetArgv(Line, TmpStr1, 5))
          Par4 = str2int(TmpStr1);
        /*
      Plugin_199_pixels->setPixelColor(
          event->Par1 - 1,
          Plugin_199_pixels->Color(event->Par2, event->Par3, Par4));
      Plugin_199_pixels
          ->show(); // This sends the updated pixel color to the hardware.
          */
        success = true;
      }
    }
    break;
  }
  }
  return success;
}

void FIRE(uint8_t mode, uint16_t par1, uint16_t par2, uint16_t par3) {}

void RAINBOW(uint8_t mode, uint16_t par1, uint16_t par2, uint16_t par3) {}

void GRADIENT(uint8_t mode, uint16_t par1, uint16_t par2, uint16_t par3) {}
