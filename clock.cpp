#include <chrono>
#include <iostream>
#include <thread>

void warm_up(){
	auto busy_loop = []() {for (volatile auto i = 0; i < 1000000; ++i); };
	for (auto i = 0; i < 1000; ++i) busy_loop();
}
#if defined(__x86_64__) || defined(_WIN32)
#ifdef _MSC_VER
#include <windows.h>
#endif
using cpu_clock = union {
	struct {
		uint32_t l;
		uint32_t h;
	} u32;
	uint64_t u64;
};

double get_base_clock(uint32_t millisec = 10) {
	cpu_clock start_clk, end_clk;
	start_clk.u64 = end_clk.u64 = 0;
	auto start = std::chrono::system_clock::now();

#ifdef _MSC_VER
	__asm {
		cpuid
		rdtsc
		mov start_clk.u32.l, eax
		mov start_clk.u32.h, edx
	}

	while (std::chrono::system_clock::now() < start + std::chrono::milliseconds(millisec));

	__asm {
		cpuid
		rdtsc
		mov end_clk.u32.l, eax
		mov end_clk.u32.h, edx
	}
#else
	__asm__(
		"cpuid;"
		"rdtsc;"
		"mov %%eax, %0;"
		"mov %%edx, %1;"
		: "=r"(start_clk.u32.l), "=r"(start_clk.u32.h)
		:
		: "eax", "ebx", "ecx", "edx", "memory"
	);

	while (std::chrono::system_clock::now() < start + std::chrono::milliseconds(millisec));

	__asm__(
		"cpuid;"
		"rdtsc;"
		"mov %%eax, %0;"
		"mov %%edx, %1;"
		: "=r"(end_clk.u32.l), "=r"(end_clk.u32.h)
		:
		: "eax", "ebx", "ecx", "edx", "memory"
	);
#endif

	return static_cast<double>(end_clk.u64 - start_clk.u64) / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count();
}
#ifdef _MSC_VER
#define CREATE_LATENCY_BENCH_10IMPL(op) __asm op xmm0, xmm1\
		__asm op xmm1, xmm0\
		__asm op xmm0, xmm1\
		__asm op xmm1, xmm0\
		__asm op xmm0, xmm1\
		__asm op xmm1, xmm0\
		__asm op xmm0, xmm1\
		__asm op xmm1, xmm0\
		__asm op xmm0, xmm1\
		__asm op xmm1, xmm0
#else
#define CREATE_LATENCY_BENCH_10IMPL(op) __asm__ volatile(#op" %xmm0, %xmm1;"\
		#op" %xmm1, %xmm0;"\
		#op" %xmm0, %xmm1;"\
		#op" %xmm1, %xmm0;"\
		#op" %xmm0, %xmm1;"\
		#op" %xmm1, %xmm0;"\
		#op" %xmm0, %xmm1;"\
		#op" %xmm1, %xmm0;"\
		#op" %xmm0, %xmm1;"\
		#op" %xmm1, %xmm0;":::"xmm0","xmm1");
#endif
#define CREATE_LATENCY_BENCH_100IMPL(op) CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)

#define CREATE_LATENCY_BENCH(op) CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)


double get_current_clock() {
	auto start = std::chrono::system_clock::now();
	CREATE_LATENCY_BENCH(pxor);
	CREATE_LATENCY_BENCH(pxor);
	CREATE_LATENCY_BENCH(pxor);
	CREATE_LATENCY_BENCH(pxor);
	CREATE_LATENCY_BENCH(pxor);
	CREATE_LATENCY_BENCH(pxor);
	CREATE_LATENCY_BENCH(pxor);
	CREATE_LATENCY_BENCH(pxor);
	CREATE_LATENCY_BENCH(pxor);
	CREATE_LATENCY_BENCH(pxor);
	return 10000.0 / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count();
}
#elif defined(__ARM_ARCH)

#define CREATE_LATENCY_BENCH_10IMPL(op) __asm__ volatile(#op" x0, x1, x2;"\
		#op" x1, x2, x0;"\
		#op" x2, x0, x1;"\
		#op" x0, x1, x2;"\
		#op" x1, x2, x0;"\
		#op" x2, x0, x1;"\
		#op" x0, x1, x2;"\
		#op" x1, x2, x0;"\
		#op" x2, x0, x1;"\
		#op" x0, x1, x2;":::"x0","x1","x2");
#define CREATE_LATENCY_BENCH_100IMPL(op) CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)\
	CREATE_LATENCY_BENCH_10IMPL(op)

#define CREATE_LATENCY_BENCH(op) CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)\
	CREATE_LATENCY_BENCH_100IMPL(op)


double get_current_clock() {
	auto start = std::chrono::system_clock::now();
	CREATE_LATENCY_BENCH(eor);
	CREATE_LATENCY_BENCH(eor);
	CREATE_LATENCY_BENCH(eor);
	CREATE_LATENCY_BENCH(eor);
	CREATE_LATENCY_BENCH(eor);
	CREATE_LATENCY_BENCH(eor);
	CREATE_LATENCY_BENCH(eor);
	CREATE_LATENCY_BENCH(eor);
	CREATE_LATENCY_BENCH(eor);
	CREATE_LATENCY_BENCH(eor);
	return 10000.0 / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count();
}
double get_base_clock(){ return get_current_clock(); }
#endif

int main() {
#ifdef _MSC_VER
	SetThreadAffinityMask(GetCurrentThread(), 1);
#endif
	warm_up();

	std::cout << get_base_clock() << "GHz" << std::endl;
	
	for(auto j = 0; j < 100; ++j){
	std::cout << []() {
		double max = 0;
		for (auto i = 0; i < 10; ++i) {
			auto tmp = get_current_clock();
			if (tmp > max) max = tmp;
		}
		return max;}()
	<< "GHz" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
	return 0;
}
