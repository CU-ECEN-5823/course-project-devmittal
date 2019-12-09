/*****************************************************************************
​ ​* ​ ​ @file​ ​  		letimer.h
​ * ​ ​ @brief​ ​ 		header file for myGecko.c
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		October 4th, 2019
​ * ​ ​ @version​ ​ 		2.0
*****************************************************************************/

#ifndef SRC_MYGECKO_H_
#define SRC_MYGECKO_H_

#define CONNECTION_INTERVAL_MIN 60
#define CONNECTION_INTERVAL_MAX 60
#define SLAVE_LATENCY 			3
#define TIMEOUT					600
#define ADVERTISING_MIN			400
#define ADVERTISING_MAX			400
#define ALLOW_BONDING			0x01
#define SECURITY_CONFIGURE_FLAGS 0x0F
#define ACCEPT_BONDING_REQUEST  0x01
#define ACCEPT_PASSKEY			1

/* Software timer macros */
#define TIMER_ID_FACTORY_RESET    77
#define TIMER_ID_STOP_BUZZER 	  88

/* Client addr */
#define KITCHEN_NODE_ADDR		46
#define BEDROOM_NODE_ADDR		44

/* Flash memory addr */
#define BUZZER_COUNT_FLASH_ADDR	0x4004
#define LAST_LIGHT_FLASH_ADDR	0x4008
#define LAST_MOTION_FLASH_ADDR 	0x4010
#define KITCHEN_ADDR			0x4011
#define BEDROOM_ADDR			0x4013

#define INITIAL_LIGHT_VALUE		50

/* Formula to calculate duty cycle */
#define CALCULATE_DUTY_CYCLE(lumen) \
	(100-((lumen*100)/200))

uint8_t connection_handle;
uint8_t server_connection_handle;
uint8_t sm_connection_handle;
uint32_t service_handle;
uint32_t push_button_service_handle;
uint16_t characteristic_handle;
uint16_t push_button_characteristic_handle;
uint8_t health_thermometer_uuid[2];
uint8_t push_button_uuid[16];
uint8_t temperature_measurement_char[2];
/// number of active Bluetooth connections
static uint8 num_connections = 0;
uint32_t buzzer_count;
uint8_t last_lightness;
uint8_t number_lpn_connected;
int kitchen_client_addr;
int bedroom_client_addr;

uint16_t kitchen_client_addr_flash;
uint16_t bedroom_client_addr_flash;
uint8_t isNewProvision;


void gecko_ecen5823_update(uint32_t evt_id, struct gecko_cmd_packet *evt);
void initiate_factory_reset(void);
void init_mesh(void);
void store_flash(int addr, uint32_t buzzer_val, uint8_t last_light_val);
void load_addr(void);

#endif /* SRC_MYGECKO_H_ */
