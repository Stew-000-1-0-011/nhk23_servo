///**
// * @file wrapper.cpp
// * @author your name (you@domain.com)
// * @brief
// * @version 0.1
// * @date 2023-04-23
// *
// * @copyright Copyright (c) 2023
// * @attention fmiに起因するwarningは無視してください。それ以外のwarningは修正してください。
// */
//
//
//#include <array>
//
//#include "can.h"
//#include "tim.h"
//
//#include <CRSLibtmp/std_type.hpp>
//#include <CRSLibtmp/utility.hpp>
//#include <CRSLibtmp/Can/Stm32/RM0008/can_bus.hpp>
//#include <CRSLibtmp/Can/Stm32/RM0008/filter_manager.hpp>
//
////PA9 TIM1_CH2
////__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,???)
////???に390で右、700で正面、1100で左
//
//using namespace CRSLib::IntegerTypes;
//using namespace CRSLib::Can::Stm32::RM0008;
//
//const char * error_msg{nullptr};
//
//namespace Nhk23Servo
//{
//	void init_can_other() noexcept;
//
//	void fifo0_callback(const ReceivedMessage& message) noexcept;
//	void servo_callback(const ReceivedMessage& message) noexcept;
//	void inject_callback(const ReceivedMessage& message) noexcept;
//
//	enum Index : u8
//	{
//		TuskL,
//		TuskR,
//		Trunk
//	};
//
//	enum Command : u8
//	{
//		SettingUp,
//		Injecting
//	};
//}
//
//i16 current = 0;
//u32 time = 0;
//u32 duration = 0;
//Nhk23Servo::Index selected = Nhk23Servo::TuskL;
//
//// Live Expressionでいじって(稀に誤作動するからEMSは手放さないこと。多分HAL_GetTickの戻り値が0に戻ったあたりのなんかなんじゃないかなあ...)
//volatile u32 duration_injecting = 1000;
//volatile u32 duration_setting = 300;
//volatile i32 current_injecting = 13000;
//volatile i32 current_setting = 2750;
//
//extern "C" void main_cpp()
//{
//
//	// PWMなど初期化
//	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
//	// *先に*フィルタの初期化を行う。先にCanBusを初期化すると先にNormalModeに以降してしまい、これはRM0008に違反する。
//	Nhk23Servo::init_can_other();
//	// 通信開始
//	CanBus can_bus{can1};
//
//	while(true)
//	{
//		// FIFO0の受信
//		{
//			const auto message = can_bus.receive(Fifo::Fifo0);
//			if(message) Nhk23Servo::fifo0_callback(*message);
//		}
//
//		// 止める
//		if(const auto now = HAL_GetTick(); now - time > duration)
//		{
//			current = 0;
//			duration = 0;
//			CRSLib::Can::DataField data{.buffer={}, .dlc=8};
//			(void)can_bus.post(0x200, data);
//		}
//
//		// 動かす
//		CRSLib::Can::DataField data{.buffer={}, .dlc=8};
//		data.buffer[2 * selected] = (byte)((current & 0xFF'00) >> 8);
//		data.buffer[2 * selected + 1] = (byte)(current & 0x00'FF);
//		(void)can_bus.post(0x200, data);
//	}
//}
//
//
//namespace Nhk23Servo
//{
//	// 各種CANのID
//	constexpr u32 servo_id = 0x110;
//	constexpr u32 inject_current_id_base = 0x120;  // 0x120-0x122
//	constexpr u32 inject_current_id_mask = 0x7FC;
//	constexpr u32 inject_feedback_id_base = 0x130;  // 0x130-0x132
//	constexpr u32 inject_feedback_id_mask = 0x7FC;
//	constexpr u32 motor_state_id_base = 0x201;  // 0x201-0x203
//	constexpr u32 motor_state_id_mask = 0x7FC;
//
//	void init_can_other() noexcept
//	{
//		// ここでCANのMSP(ピンやクロックなど。ここまで書ききるのはキツかった...)の初期化を行う
//		HAL_CAN_DeInit(&hcan);
//		HAL_CAN_MspInit(&hcan);
//
//		enum FilterName : u8
//		{
//			Servo,
//			Inject,
//
//			N
//		};
//
//		FilterConfig filter_configs[N];
//		filter_configs[Servo] = FilterConfig::make_default(Fifo::Fifo0);
//		filter_configs[Inject] = FilterConfig::make_default(Fifo::Fifo0, false);
//
//		// ここでフィルタの初期化を行う
//		FilterManager::initialize(filter_bank_size, filter_configs);
//
//		if(!FilterManager::set_filter(Servo, Filter{.FR1=FilterManager::make_list32(servo_id), .FR2=0}))
//		{
//			error_msg = "Fail to set filter for Servo";
//			Error_Handler();
//		}
//		FilterManager::activate(Servo);
//
//		if(!FilterManager::set_filter(Inject, FilterManager::make_mask32(inject_current_id_base, inject_current_id_mask)))
//		{
//			error_msg = "Fail to set filter for Inject";
//			Error_Handler();
//		}
//		FilterManager::activate(Inject);
//	}
//
//	//////// ここから下はコールバック関数 ////////
//	/// @attention 十分に短い処理しか書かないこと。
//
//	/// @brief fifo0のコールバック
//	/// @param message
//	void fifo0_callback(const ReceivedMessage& message) noexcept
//	{
//		if(message.id == servo_id)
//		{
//			servo_callback(message);
//		}
//		else if(inject_current_id_base <= message.id && message.id <= inject_current_id_base + 2)
//		{
//			inject_callback(message);
//		}
//	}
//
//	/// @brief サーボのコールバック
//	/// @param message
//	void servo_callback(const ReceivedMessage& message) noexcept
//	{
//		switch(static_cast<Index>(message.data.buffer[0]))
//		{
//			case TuskL:
//			{
//				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,1100);
//			}
//			break;
//
//			case TuskR:
//			{
//				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,390);
//			}
//			break;
//
//			case Trunk:
//			{
//				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,700);
//			}
//
//			default:;
//		}
//	}
//
//	/// @brief インジェクターのコールバック
//	/// @param message
//	void inject_callback(const ReceivedMessage& message) noexcept
//	{
//		if(current != 0) return;
//		selected = static_cast<Index>(message.id - inject_current_id_base);
//		switch((Command)message.data.buffer[0])
//		{
//		case Command::SettingUp:
//			current = current_setting;
//			duration = duration_setting;
//		break;
//		case Command::Injecting:
//			current = current_injecting;
//			duration = duration_injecting;
//		break;
//		default:;
//		}
//		time = HAL_GetTick();
//	}
//}
