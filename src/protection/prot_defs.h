/*
	сисколлы крашит на рейдже под модмапом
	крипт на рейдже не тестил

	todo: дописать сюда сдк темиды
*/

#pragma once 

#define DISABLE_ALL_PROTECTION

#include "strings/oxorany.h"
#include "strings/skCrypter.h"

#include "ThemidaSDK.h"
#include "SecureEngineCustomVMs.h"

#pragma comment(lib, "SecureEngineSDK64.lib")

#if defined(DISABLE_XOR) || defined(DISABLE_ALL_PROTECTION)
#define u_crypt
#define m_crypt

#else
// oxorany (hard algorithm)
#define u_crypt(any) _lxy_oxor_any_::oxor_any<decltype(_lxy_oxor_any_::typeofs(any)), _lxy_oxor_any_::array_size(any), __COUNTER__>(any, _lxy_::make_index_sequence<sizeof(decltype(any))>()).get()
// skCrypt (def xor)
#define m_crypt(str) skCrypt_key(str, __TIME__[1], __TIME__[8])

#endif

#if defined(DISABLE_SYSCALLS) || defined(DISABLE_ALL_PROTECTION)
#define _create_thread(threadh, func) CreateThread(0, 0, func, 0, 0, 0);
#else
#define _create_thread(threadh, func) shadowsyscall<NTSTATUS>("NtCreateThreadEx", &threadh, 0x40 | 0x00000004, nullptr, cur_process, (LPTHREAD_START_ROUTINE)func, nullptr, FALSE, 0, 0, nullptr, nullptr);
#endif
