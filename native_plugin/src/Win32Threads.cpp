#if !defined(_WIN64)
#  error This code must be compiled in x64 (Win64) mode
#endif

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <functional>
#include <atomic>
#include <mutex>
#include <vector>


namespace Debug {

static std::once_flag initFlag;
static std::atomic<HANDLE> hLog = nullptr;
static const char* const logFilename = "Win32Threads-log.txt";

void init() {
	std::call_once(initFlag, []() {
		hLog = CreateFileA(
			  logFilename
			, GENERIC_WRITE
			, FILE_SHARE_READ
			, nullptr
			, CREATE_ALWAYS
			, FILE_ATTRIBUTE_NORMAL
			, nullptr
		);
	});
}

void outputString(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char buf[1024];
	const auto len = vsprintf_s(buf, sizeof(buf)/sizeof(buf[0]), fmt, args);
	DWORD bytesWritten;
	WriteFile(hLog, buf, len, &bytesWritten, nullptr);
	FlushFileBuffers(hLog);
	va_end(args);
}

} // namespace Debug


using ThreadFunc = std::function<void()>;

std::vector<HANDLE> threadHandles;


static HANDLE startThread(const ThreadFunc& threadFunc) {
	struct Param {
		ThreadFunc threadFunc;
	};

	auto* param = new Param {};
	param->threadFunc = threadFunc;

	const auto threadHandle = CreateThread(
		  nullptr
		, 0
		, [](void* paramPtr) -> DWORD {
			auto* p = reinterpret_cast<Param*>(paramPtr);
			p->threadFunc();
			return 0;
		}
		, reinterpret_cast<void*>(param)
		, 0
		, nullptr
	);

	threadHandles.push_back(threadHandle);

	return threadHandle;
}


static void closeAllThreads() {
	for(const auto threadHandle : threadHandles) {
		CloseHandle(threadHandle);
	}
}


extern "C" __declspec(dllexport) void TestFunc() {
	Debug::outputString("%s(%d) : TestFunc()\n", __FILE__, __LINE__);
	startThread([]() {
		int counter = 0;
		for(;;) {
			Debug::outputString("%s(%d) : counter = %d\n", __FILE__, __LINE__, counter);
			++counter;
			::Sleep(1000);
		}
	});
}


BOOL WINAPI DllMain(HINSTANCE /*hInstDll*/, DWORD fdwReason, LPVOID) {
	switch(fdwReason) {
	case DLL_PROCESS_ATTACH:
		Debug::init();
		Debug::outputString("%s(%d) : DLL_PROCESS_ATTACH - begin\n", __FILE__, __LINE__);
		startThread([]() {
			int counter = 0;
			for(;;) {
				Debug::outputString("%s(%d) : counter = %d\n", __FILE__, __LINE__, counter);
				++counter;
				::Sleep(1000);
			}
		});
		Debug::outputString("%s(%d) : DLL_PROCESS_ATTACH - end\n", __FILE__, __LINE__);
		break;

	case DLL_PROCESS_DETACH:
		Debug::outputString("%s(%d) : DLL_PROCESS_DETACH - begin\n", __FILE__, __LINE__);
		closeAllThreads();
		Debug::outputString("%s(%d) : DLL_PROCESS_DETACH - end\n", __FILE__, __LINE__);
		break;

	default:
		break;
	}
	return TRUE;
}
