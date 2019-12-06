/*****************************************************************************
​ ​* ​ ​ @file​ ​  		myGecko.c
​ * ​ ​ @brief​ ​ 		Contains BT stack event handler
​ * ​ ​ @Author(s)​  	​​Devansh Mittal
​ * ​ ​ @Date​ ​​ 		November 20th, 2019
​ * ​ ​ @version​ ​ 		3.0
 *   @References 	soc-btmesh-light example project from SI BT mesh SDK
 *   				soc-btmesh-switch example project from SI BT mesh SDK
*****************************************************************************/
#include "gatt_db.h"
#include "native_gecko.h"
#include "main.h"
#include "log.h"
#include "em_letimer.h"
#include "myGecko.h"
#include "display.h"
#include "gecko_ble_errors.h"
#include "ble_device_type.h"
#include "ble_mesh_device_type.h"
#include <math.h>
#include <string.h>
#include "gpio.h"
#include "event.h"
#include "mesh_generic_model_capi_types.h"
#include "mesh_lib.h"
#include "mesh_lighting_model_capi_types.h"
#include "led.h"

/***************************************************************************//**
 * This function process the requests for the generic on/off model.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] client_addr    Address of the client model which sent the message.
 * @param[in] server_addr    Address the message was sent to.
 * @param[in] appkey_index   The application key index used in encrypting the request.
 * @param[in] request        Pointer to the request structure.
 * @param[in] transition_ms  Requested transition time (in milliseconds).
 * @param[in] delay_ms       Delay time (in milliseconds).
 * @param[in] request_flags  Message flags. Bitmask of the following:
 *                           - Bit 0: Nonrelayed. If nonzero indicates
 *                                    a response to a nonrelayed request.
 *                           - Bit 1: Response required. If nonzero client
 *                                    expects a response from the server.
 ******************************************************************************/
static void onoff_request(uint16_t model_id,
                          uint16_t element_index,
                          uint16_t client_addr,
                          uint16_t server_addr,
                          uint16_t appkey_index,
                          const struct mesh_generic_request *request,
                          uint32_t transition_ms,
                          uint16_t delay_ms,
                          uint8_t request_flags)
{
	if(client_addr == KITCHEN_NODE_ADDR)
	{
		LOG_INFO("Flame alert received from kitchen node");
		displayPrintf(DISPLAY_ROW_GASVALUE, "HIGH GAS LEVEL - %d", request->level);
		gpioBuzzer(1);
		gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_STOP_BUZZER, 1);
		buzzer_count++;

		store_flash(BUZZER_COUNT_FLASH_ADDR, buzzer_count);
	}
	else
	{
		LOG_INFO("Change in motion sensor");

		LOG_INFO("Motion: %d", request->on_off);
		LOG_INFO("Client Addr: %d", client_addr);

		if(request->level)
		{

		}
		else
		{
			set_duty_cycle(0);
			last_lightness = 0;
		}
	}
}

/***************************************************************************//**
 * This function is a handler for generic on/off change event.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void onoff_change(uint16_t model_id,
                         uint16_t element_index,
                         const struct mesh_generic_state *current,
                         const struct mesh_generic_state *target,
                         uint32_t remaining_ms)
{
	/*if (current->on_off.on != button_state.onoff_current)
	{
		LOG_INFO("on-off state changed %u to %u\r\n", button_state.onoff_current, current->on_off.on);

		button_state.onoff_current = current->on_off.on;
	}
	else
	{
		LOG_INFO("dummy onoff change - same state as before\r\n");
	}*/
}

static void level_request(uint16_t model_id,
                              uint16_t element_index,
                              uint16_t client_addr,
                              uint16_t server_addr,
                              uint16_t appkey_index,
                              const struct mesh_generic_request *request,
                              uint32_t transition_ms,
                              uint16_t delay_ms,
                              uint8_t request_flags)
{
	if(client_addr == KITCHEN_NODE_ADDR)
	{
		LOG_INFO("Gas Level alert received from kitchen low power node: %d", request->level);
		LOG_INFO("Addr of sender %d", client_addr);

		displayPrintf(DISPLAY_ROW_GASVALUE, "HIGH GAS LEVEL - %d", request->level);
		gpioBuzzer(1);
		gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_STOP_BUZZER, 1);
		buzzer_count++;

		store_flash(BUZZER_COUNT_FLASH_ADDR, buzzer_count);
	}
	else
	{
		LOG_INFO("Change in light intensity: %d", request->level);
		LOG_INFO("Addr of sender %d", client_addr);

		displayPrintf(DISPLAY_ROW_LIGHTVALUE, "Lumen - %d", request->level);

		//Call duty cycle func. Decide on formula
	}
}

/***************************************************************************//**
 * This function is a handler for generic level change event
 * on primary element.
 *
 * @param[in] model_id       Server model ID.
 * @param[in] element_index  Server model element index.
 * @param[in] current        Pointer to current state structure.
 * @param[in] target         Pointer to target state structure.
 * @param[in] remaining_ms   Time (in milliseconds) remaining before transition
 *                           from current state to target state is complete.
 ******************************************************************************/
static void level_change(uint16_t model_id,
                             uint16_t element_index,
                             const struct mesh_generic_state *current,
                             const struct mesh_generic_state *target,
                             uint32_t remaining_ms)
{
	/*if(button_state.pri_level_current != current->level.level)
  	{
    	LOG_INFO("pri_level_change: from %d to %d\r\n", button_state.pri_level_current, current->level.level);
    	button_state.pri_level_current = current->level.level;
    }
	else
	{
    	LOG_INFO("pri_level update -same value (%d)\r\n", home_automation.level_current);
	}*/
}

void store_flash(int addr, uint32_t val)
{
	struct gecko_msg_flash_ps_save_rsp_t* pSave;

	pSave = gecko_cmd_flash_ps_save(addr, sizeof(uint32_t), (const uint8*)&val);

	if (pSave->result)
	{
		LOG_INFO("PS save failed, code %x\r\n", pSave->result);
	}
}

void init_models()
{
	mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID, 0, onoff_request, onoff_change);
	mesh_lib_generic_server_register_handler(MESH_GENERIC_LEVEL_SERVER_MODEL_ID, 0, level_request, level_change);
	//mesh_lib_generic_server_register_handler(MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID, 0, lightness_request, lightness_change);
}

/*
 * @func - init_mesh
 * @brief - Initialize Server/Client parameters.
 * @parameters - void.
 * @return - void.
 *
 * Gas sensor - greater than 650, trigger buzzer. stop after 2 seconds
 * flame sensor - on off model, 1 - buzzer on. stop after 2 seconds.
 */
void init_mesh(void)
{
	struct gecko_msg_flash_ps_load_rsp_t* pLoad;

	mesh_lib_init(malloc,free,8);
	BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_generic_server_init());
	BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_friend_init());

	/* Load data from flash */
	pLoad = gecko_cmd_flash_ps_load(BUZZER_COUNT_FLASH_ADDR);

	if (pLoad->result)
	{
		LOG_INFO("Failed to load buzzer value from flash");
	}
	else
	{
		displayPrintf(DISPLAY_ROW_STATUS, "Flash Stats:");
		displayPrintf(DISPLAY_ROW_FLASH1, "Buzzer: %d", pLoad->value.data[0]);
	}

	pLoad = gecko_cmd_flash_ps_load(LAST_LIGHT_FLASH_ADDR);
	if (pLoad->result)
	{
		LOG_INFO("Failed to load last light value from flash from flash");
		last_lightness = 50;
	}
	else
	{
		//Call function to set duty cycle based on value in flash
	}

	buzzer_count = 0;

	/* Reset flash to 0 after displaying the stats */
	store_flash(BUZZER_COUNT_FLASH_ADDR, buzzer_count);

	init_models();
}

/***************************************************************************//**
 * This function is called to initiate factory reset. Factory reset may be
 * initiated by keeping one of the WSTK pushbuttons pressed during reboot.
 * Factory reset is also performed if it is requested by the provisioner
 * (event gecko_evt_mesh_node_reset_id).
 ******************************************************************************/
void initiate_factory_reset(void)
{
	LOG_INFO("factory reset\r\n");
	displayPrintf(DISPLAY_ROW_ACTION, "Factory Reset");

	/* perform a factory reset by erasing PS storage. This removes all the keys and other settings
	 that have been configured for this node */
	gecko_cmd_flash_ps_erase_all();
	// reboot after a small delay
	gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_FACTORY_RESET, 1);
}

/*
 * @func - gecko_ecen5823_update
 * @brief - Handle BTMesh events
 * @parameters - evt - event to be handled
 * @return - none
 */
void gecko_ecen5823_update(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
	handle_gecko_event(evt_id, evt);

	switch (evt_id) {
	 case gecko_evt_system_boot_id: ;

	 	 /* Factory reset device */
	 	if((!GPIO_PinInGet(PB0_port, PB0_pin)) || (!GPIO_PinInGet(PB1_port, PB1_pin)))
	 	{
	 		initiate_factory_reset();
	 	}
	 	else
	 	{
	 		/* Set attributes to init device */
			char name[20];

			uint8_t bedroom_lpn_addr[] = BEDROOM_LPN_BT_ADDRESS;
			uint8_t kitchen_lpn_addr[] = KITCHEN_LPN_BT_ADDRESS;

			struct gecko_msg_system_get_bt_address_rsp_t *bt_public_addr = gecko_cmd_system_get_bt_address();
			displayPrintf(DISPLAY_ROW_BTADDR, "%x.%x.%x.%x.%x.%x", bt_public_addr->address.addr[5],bt_public_addr->address.addr[4],bt_public_addr->address.addr[3],bt_public_addr->address.addr[2],bt_public_addr->address.addr[1],bt_public_addr->address.addr[0]);
			displayPrintf(DISPLAY_ROW_BTADDR2, "%x.%x.%x.%x.%x.%x", kitchen_lpn_addr[5],kitchen_lpn_addr[4],kitchen_lpn_addr[3],kitchen_lpn_addr[2],kitchen_lpn_addr[1],kitchen_lpn_addr[0]);
			displayPrintf(DISPLAY_ROW_BTADDR3, "%x.%x.%x.%x.%x.%x", bedroom_lpn_addr[5],bedroom_lpn_addr[4],bedroom_lpn_addr[3],bedroom_lpn_addr[2],bedroom_lpn_addr[1],bedroom_lpn_addr[0]);

			displayPrintf(DISPLAY_ROW_NAME, "Friend Node");

			sprintf(name, "5823Pub %02x:%02x", bt_public_addr->address.addr[1],bt_public_addr->address.addr[0]);

			LOG_INFO("Device name: '%s'\r\n", name);
			BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(name), (uint8_t *)name));
	 	}
		break;

	 case gecko_evt_mesh_node_initialized_id:

		 /* Beaconing if not provisioned done in gecko_main.c */
		 if (evt->data.evt_mesh_node_initialized.provisioned)
		 {
			 LOG_INFO("Already provisioned\n");
			 init_mesh();
		 }
		 break;

	 case gecko_evt_mesh_node_provisioning_started_id:

		 displayPrintf(DISPLAY_ROW_ACTION, "Provisioning");
		 break;

	 case gecko_evt_mesh_node_provisioned_id:

		 init_mesh();
		 displayPrintf(DISPLAY_ROW_ACTION, "Provisioned");
		 break;

	 case gecko_evt_mesh_node_provisioning_failed_id:

		 displayPrintf(DISPLAY_ROW_ACTION, "Provision Fail");
		 break;

	 case gecko_evt_mesh_generic_server_client_request_id:

		 mesh_lib_generic_server_event_handler(evt);
		 break;

	 case gecko_evt_mesh_generic_server_state_changed_id:

		 mesh_lib_generic_server_event_handler(evt);
		 break;

	 case gecko_evt_le_connection_opened_id:

		 num_connections++;
		 displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
		 break;

	 case gecko_evt_le_connection_closed_id:

		 if (num_connections > 0)
		 {
			 if (--num_connections == 0)
			 {
				 displayPrintf(DISPLAY_ROW_CONNECTION, "");
			 }
		 }
		 break;

	 case gecko_evt_mesh_node_reset_id:

		 initiate_factory_reset();
		 break;

	 case gecko_evt_system_external_signal_id: ;

		 /* Handle display update timer interrupt */
		 if(((evt->data.evt_system_external_signal.extsignals & DISPLAY_UPDATE) == DISPLAY_UPDATE))
		 {
			 displayUpdate();
		 }
		 break;

	 case gecko_evt_hardware_soft_timer_id:

		 switch (evt->data.evt_hardware_soft_timer.handle)
		 {
			case TIMER_ID_FACTORY_RESET:
				// reset the device to finish factory reset
				gecko_cmd_system_reset(0);
				break;

			case TIMER_ID_STOP_BUZZER:
				//Stop buzzer
				gpioBuzzer(1);
				displayPrintf(DISPLAY_ROW_GASVALUE, "");
		 }
		 break;

	 case gecko_evt_mesh_friend_friendship_established_id:

		 LOG_INFO("evt gecko_evt_mesh_friend_friendship_established, lpn_address=%x\r\n", evt->data.evt_mesh_friend_friendship_established.lpn_address);
		 break;

	 case gecko_evt_mesh_friend_friendship_terminated_id:

		 LOG_INFO("evt gecko_evt_mesh_friend_friendship_terminated, reason=%x\r\n", evt->data.evt_mesh_friend_friendship_terminated.reason);
		 break;
	}
}
