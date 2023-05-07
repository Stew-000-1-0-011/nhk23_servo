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
//
//	enum Index : u8
//	{
//		TuskL,
//		TuskR,
//		Trunk
//	};
//}
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
//	}
//}
//
//
//namespace Nhk23Servo
//{
//	// 各種CANのID
//	constexpr u32 servo_id = 0x110;
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
//
//			N
//		};
//
//		FilterConfig filter_configs[N];
//		filter_configs[Servo] = FilterConfig::make_default(Fifo::Fifo0);
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
//				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,450);
//			}
//			break;
//
//			case Trunk:
//			{
//				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,800);
//			}
//
//			default:;
//		}
//	}
//}
