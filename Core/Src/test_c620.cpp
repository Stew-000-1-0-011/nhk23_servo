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
//struct MotorState
//{
//	i16 angle{0};
//	i16 speed{0};
//	i16 current{0};
//	i8 temperature{0};
//};
//
//constinit MotorState motor{};
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
//
//	while(true)
//	{
//		if(const auto message = can_bus.receive(Fifo::Fifo1); message)
//		{
//			motor_state
//		}
//	}
//}
//
//
//namespace Nhk23Servo
//{
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
//			MotorState,
//
//			N
//		};
//
//		FilterConfig filter_configs[N];
//		filter_configs[MotorState] = FilterConfig::make_default(Fifo::Fifo1, false);
//
//		// ここでフィルタの初期化を行う
//		FilterManager::initialize(filter_bank_size, filter_configs);
//
//		if(!FilterManager::set_filter(MotorState, FilterManager::make_mask32(motor_state_id_base, motor_state_id_mask)))
//		{
//			error_msg = "Fail to set filter for MotorState";
//			Error_Handler();
//		}
//		FilterManager::activate(MotorState);
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
//		else if(inject_speed_id_base <= message.id && message.id <= inject_speed_id_base + 2)
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
//		if(speed != 0) return;
//		selected = static_cast<Index>(message.id - inject_speed_id_base);
//		speed = speed_max;
//		time = HAL_GetTick();
//	}
//}
