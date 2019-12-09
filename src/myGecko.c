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
#include <string.h>

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
	LOG_INFO("Client Addr on/off: %d, bedroom addr: %d", client_addr, bedroom_client_addr_flash);
	if(client_addr == bedroom_client_addr_flash) //handle motion sensor alert
	{
		LOG_INFO("Motion: %d", request->on_off);
		LOG_INFO("Client Addr: %d", client_addr);

		if(request->on_off) //Motion detected
		{
			displayPrintf(DISPLAY_ROW_MOTIONVALUE, "Room Occupied");
			displayPrintf(DISPLAY_ROW_LIGHTVALUE, "Light Intensity: %d%%", last_lightness);
			set_duty_cycle(last_lightness); //Set intensity to last value before room was empty
			store_flash(LAST_MOTION_FLASH_ADDR, 0, 1);
		}
		else //Motion not detected
		{
			displayPrintf(DISPLAY_ROW_MOTIONVALUE, "Room Empty");
			displayPrintf(DISPLAY_ROW_LIGHTVALUE, "Light Intensity: %d%%", 0);
			set_duty_cycle(0);
			store_flash(LAST_MOTION_FLASH_ADDR, 0, 0);
		}
	}
	else //handle flame alert
	{
		LOG_INFO("Flame alert received from kitchen node");
		displayPrintf(DISPLAY_ROW_FLAMEVALUE, "FIRE!!!");
		gpioBuzzer(1); //Turn on buzzer to indicate fire
		gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_STOP_BUZZER, 1); //start timer to turn off buzzer after 2 seconds
		buzzer_count++;

		store_flash(BUZZER_COUNT_FLASH_ADDR, buzzer_count, 0);

	}
}

/***************************************************************************//**
 * This function is a handler for generic on/off change event. Nothing to be done
 * in this function.
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
}

/***************************************************************************//**
 * This function process the requests for the generic level model
 * on primary element.
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
	LOG_INFO("Client Addr level request: %d, bedroom addr: %d", client_addr, bedroom_client_addr_flash);
	if(client_addr == bedroom_client_addr_flash) //Handle light intensity values
	{
		uint8_t duty_cycle;
		uint16_t lumen;

		LOG_INFO("Change in light intensity: %d", request->level);
		LOG_INFO("Addr of sender level %d", client_addr);

		//Values above 190 indicates an intensity of 5%, for demonstration purposes light should not be turned off
		if(request->level >= 190)
			lumen = 190;
		else
			lumen = request->level;

		/* Lumen values inversely proportional to duty cycle */
		duty_cycle = CALCULATE_DUTY_CYCLE(lumen);

		set_duty_cycle(duty_cycle);

		last_lightness = duty_cycle;

		displayPrintf(DISPLAY_ROW_LIGHTVALUE, "Light Intensity: %d%%", last_lightness);

		store_flash(LAST_LIGHT_FLASH_ADDR, 0, last_lightness);
	}
	else //Handling kitchen node gas leak alert
	{
		LOG_INFO("Gas Level alert received from kitchen low power node: %d", request->level);
		LOG_INFO("Addr of sender %d", client_addr);

		displayPrintf(DISPLAY_ROW_FLASH2, "Gas Leak: %d", request->level);
		gpioBuzzer(1); //switch on buzzer
		gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_ID_STOP_BUZZER, 1); //start timer to switch off buzzer after 2 seconds
		buzzer_count++;

		store_flash(BUZZER_COUNT_FLASH_ADDR, buzzer_count, 0); //store buzzer count in flash

	}
}

/***************************************************************************//**
 * This function is a handler for generic level change event on primary element.
 * Nothing to be done in this function.
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
}

/*
 * @func - store_flash
 * @brief - Store data in flash
 * @parameters - addr: Address in flash where data is to be stored
 * 				 buzzer_val: Number of times buzzer triggered
 * 				 last_light_val: last light value
 * @return - none
 */
void store_flash(int addr, uint32_t buzzer_val, uint8_t last_light_val)
{
	struct gecko_msg_flash_ps_save_rsp_t* pSave;

	switch(addr)
	{
		case BUZZER_COUNT_FLASH_ADDR: //Store buzzer count
			pSave = gecko_cmd_flash_ps_save(addr, sizeof(uint32_t), (const uint8*)&buzzer_val);

			if (pSave->result)
			{
				LOG_INFO("PS save of buzzer count failed, code %x\r\n", pSave->result);
			}
			break;

		case LAST_LIGHT_FLASH_ADDR: //Store last light value
			pSave = gecko_cmd_flash_ps_save(addr, sizeof(uint8_t), (const uint8*)&last_light_val);

			if (pSave->result)
			{
				LOG_INFO("PS save of last light value failed, code %x\r\n", pSave->result);
			}
			break;

		case LAST_MOTION_FLASH_ADDR: //Store last motion value
			pSave = gecko_cmd_flash_ps_save(addr, sizeof(uint8_t), (const uint8*)&last_light_val);

			if (pSave->result)
			{
				LOG_INFO("PS save of last light value failed, code %x\r\n", pSave->result);
			}
			break;

		case BEDROOM_ADDR: //Store bedroom address
			pSave = gecko_cmd_flash_ps_save(addr, sizeof(uint8_t), (const uint8*)&buzzer_val);

			if (pSave->result)
			{
				LOG_INFO("PS save of bedroom addr failed, code %x\r\n", pSave->result);
			}
			break;

		case KITCHEN_ADDR: //store kitchen address
			pSave = gecko_cmd_flash_ps_save(addr, sizeof(uint8_t), (const uint8*)&buzzer_val);

			if (pSave->result)
			{
				LOG_INFO("PS save of kitchen addr failed, code %x\r\n", pSave->result);
			}
			break;

	}
}

/*
 * @func - init_models
 * @brief - Initialize models
 * @parameters - none
 * @return - none
 */
void init_models()
{
	mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID, 0, onoff_request, onoff_change);
	mesh_lib_generic_server_register_handler(MESH_GENERIC_LEVEL_SERVER_MODEL_ID, 0, level_request, level_change);
}

/*
 * @func - init_mesh
 * @brief - Initialize Server parameters.
 * 			Restore and display flash memory contents.
 * @parameters - void.
 * @return - void.
 */
void init_mesh(void)
{
	struct gecko_msg_flash_ps_load_rsp_t* pLoad;
	struct gecko_msg_flash_ps_load_rsp_t* pLoad_motion;
	uint32_t buzzer_cnt;

	mesh_lib_init(malloc,free,8);
	BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_generic_server_init());
	BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_friend_init());

	/* If not newly provisioned, load client addr from flash */
	if(!isNewProvision)
		load_addr();

	/* Load buzzer count data from flash */
	pLoad = gecko_cmd_flash_ps_load(BUZZER_COUNT_FLASH_ADDR);

	if (pLoad->result)
	{
		LOG_INFO("Failed to load buzzer value from flash");
	}
	else
	{
		displayPrintf(DISPLAY_ROW_FLASH1, "Buzzer: %d", pLoad->value.data[0]);
	}

	/* Load last light value */
	pLoad = gecko_cmd_flash_ps_load(LAST_LIGHT_FLASH_ADDR);

	if (pLoad->result)
	{
		LOG_INFO("Failed to load last light value from flash");
		last_lightness = INITIAL_LIGHT_VALUE; //When unit is first provisioned, keep default light value as 50
	}
	else
	{
		LOG_INFO("Last light: %d", pLoad->value.data[0]);
		last_lightness = pLoad->value.data[0];

		/* Load last motion value from memory */
		pLoad_motion = gecko_cmd_flash_ps_load(LAST_MOTION_FLASH_ADDR);

		if(pLoad_motion->result)
		{
			LOG_INFO("Failed to load last motion value from flash");
		}
		else
		{
			LOG_INFO("Last Motion: %d", pLoad_motion->value.data[0]);

			if(pLoad_motion->value.data[0]) //If motion is present, light LED with last intensity value stored in memory
			{
				LOG_INFO("Last Lightness: %d", last_lightness);
				set_duty_cycle(last_lightness);
				displayPrintf(DISPLAY_ROW_LIGHTVALUE, "Light Intensity: %d%%", last_lightness);
				displayPrintf(DISPLAY_ROW_MOTIONVALUE, "Room Occupied");
			}
			else //If motion not present, keep LED off
			{
				set_duty_cycle(0);
				displayPrintf(DISPLAY_ROW_LIGHTVALUE, "Light Intensity: %d%%", 0);
				displayPrintf(DISPLAY_ROW_MOTIONVALUE, "Room Empty");
			}
		}
	}

	buzzer_count = 0;

	/* Reset flash to 0 after displaying the stats */
	store_flash(BUZZER_COUNT_FLASH_ADDR, buzzer_count, 0);

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
 * @func - load_addr
 * @brief - Load client addr from flash
 * @parameters - void
 * @return - none
 */
void load_addr(void)
{
	struct gecko_msg_flash_ps_load_rsp_t* pLoad;

	LOG_INFO("Loading data from flasg");

	/* Load kitchen addr */
	pLoad = gecko_cmd_flash_ps_load(KITCHEN_ADDR);
	if (pLoad->result)
	{
		LOG_INFO("Failed to load kitchen addr from flash");
	}
	else
	{
		kitchen_client_addr_flash = (pLoad->value.data[0]);
	}

	/* Load bedroom addr */
	pLoad = gecko_cmd_flash_ps_load(BEDROOM_ADDR);
	if (pLoad->result)
	{
		LOG_INFO("Failed to load bedroom addr from flash");
	}
	else
	{
		bedroom_client_addr_flash = (pLoad->value.data[0]);
		LOG_INFO("Bedroom addr from flash: %d", bedroom_client_addr_flash);
	}
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

			isNewProvision = 0;
			uint8_t bedroom_lpn_addr[] = BEDROOM_LPN_BT_ADDRESS;
			uint8_t kitchen_lpn_addr[] = KITCHEN_LPN_BT_ADDRESS;

			struct gecko_msg_system_get_bt_address_rsp_t *bt_public_addr = gecko_cmd_system_get_bt_address();
			displayPrintf(DISPLAY_ROW_BTADDR, "%x.%x.%x.%x.%x.%x", bt_public_addr->address.addr[5],bt_public_addr->address.addr[4],bt_public_addr->address.addr[3],bt_public_addr->address.addr[2],bt_public_addr->address.addr[1],bt_public_addr->address.addr[0]);
			displayPrintf(DISPLAY_ROW_BTADDR3, "LPN Connected: %d", number_lpn_connected);
			displayPrintf(DISPLAY_ROW_NAME, "Friend Node");
			displayPrintf(DISPLAY_ROW_BTADDR2, "");
			displayPrintf(DISPLAY_ROW_STATUS, "");
			displayPrintf(DISPLAY_ROW_FLASH2, "");
			displayPrintf(DISPLAY_ROW_GASVALUE, "");

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

		 isNewProvision = 1;
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
				gpioBuzzer(0);
				displayPrintf(DISPLAY_ROW_FLASH2, "");
				displayPrintf(DISPLAY_ROW_FLAMEVALUE, "");
		 }
		 break;

	 case gecko_evt_mesh_friend_friendship_established_id:

		 number_lpn_connected++;
		 displayPrintf(DISPLAY_ROW_BTADDR3, "LPN Connected: %d", number_lpn_connected);

		 /* if newly provisioned, store client addr in flash the first time */
		 if(isNewProvision == 1)
		 {
				 LOG_INFO("Storing addr in memory, isNewProvision: %d", isNewProvision);
				 bedroom_client_addr_flash = evt->data.evt_mesh_friend_friendship_established.lpn_address;
				 store_flash(BEDROOM_ADDR, bedroom_client_addr_flash, 0);
				 kitchen_client_addr_flash = KITCHEN_NODE_ADDR;
				 store_flash(KITCHEN_ADDR, kitchen_client_addr_flash, 0);
				 isNewProvision = 0;
		 }
		 LOG_INFO("evt gecko_evt_mesh_friend_friendship_established, lpn_address=%d\r\n", evt->data.evt_mesh_friend_friendship_established.lpn_address);
		 break;

	 case gecko_evt_mesh_friend_friendship_terminated_id:

		 number_lpn_connected--;
		 displayPrintf(DISPLAY_ROW_BTADDR3, "LPN Connected: %d", number_lpn_connected);
		 LOG_INFO("evt gecko_evt_mesh_friend_friendship_terminated, reason=%x\r\n", evt->data.evt_mesh_friend_friendship_terminated.reason);
		 break;
	}
}
