/*
 * my_accessory.c
 * Define the accessory in C language using the Macro in characteristics.h
 *
 *  Created on: 2020-05-15
 *      Author: Mixiaoxiao (Wang Bin)
 */

#include <homekit/homekit.h>
#include <homekit/characteristics.h>

void my_accessory_identify(homekit_value_t _value) {
  printf("accessory identify\n");
}

// Switch (HAP section 8.38)
// required: ON
// optional: NAME

// format: bool; HAP section 9.70; write the .setter function to get the switch-event sent from iOS Home APP.
homekit_characteristic_t cha_switch_on_01 = HOMEKIT_CHARACTERISTIC_(ON, false);
// format: string; HAP section 9.62; max length 64
homekit_characteristic_t cha_name_01 = HOMEKIT_CHARACTERISTIC_(NAME, "Switch off");

// format: bool; HAP section 9.70; write the .setter function to get the switch-event sent from iOS Home APP.
homekit_characteristic_t cha_switch_on_02 = HOMEKIT_CHARACTERISTIC_(ON, false);
homekit_characteristic_t cha_name_02 = HOMEKIT_CHARACTERISTIC_(NAME, "Switch Emoji");

// format: bool; HAP section 9.70; write the .setter function to get the switch-event sent from iOS Home APP.
homekit_characteristic_t cha_switch_on_03 = HOMEKIT_CHARACTERISTIC_(ON, false);
homekit_characteristic_t cha_name_03 = HOMEKIT_CHARACTERISTIC_(NAME, "Switch lightning");

homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]) {
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]) {
            HOMEKIT_CHARACTERISTIC(NAME, "ESP8266 Switch"),
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Arduino HomeKit"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "0123456"),
            HOMEKIT_CHARACTERISTIC(MODEL, "ESP8266"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "1.0"),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, my_accessory_identify),
            NULL
        }),
    HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]){
      &cha_switch_on_01,
      &cha_name_01,
      NULL
    }),
    HOMEKIT_SERVICE(SWITCH, .characteristics=(homekit_characteristic_t*[]){
      &cha_switch_on_02,
      &cha_name_02,
      NULL
    }),
    HOMEKIT_SERVICE(SWITCH, .characteristics=(homekit_characteristic_t*[]){
      &cha_switch_on_03,
      &cha_name_03,
      NULL
    }),
        NULL
    }),
    NULL
};

homekit_server_config_t config = {
    .accessories = accessories,
    .password = "555-11-123"
};
