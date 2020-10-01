#include <chrono>
#include <iostream>
#include <thread>
#include <iomanip>
#include <windows.h>

void warm_up() {
	auto busy_loop = []() {for (volatile auto i = 0; i < 1000000; ++i); };
	for (auto i = 0; i < 1000; ++i) busy_loop();
}
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
	return static_cast<double>(end_clk.u64 - start_clk.u64) / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count();
}
#define REPEAT_10TIMES(func) func;func;func;func;func;\
func;func;func;func;func;

#define CREATE_SSE_2OP_LATENCY_BENCH_8IMPL(op) {__asm op xmm0, xmm1\
		__asm op xmm1, xmm0\
		__asm op xmm0, xmm1\
		__asm op xmm1, xmm0\
		__asm op xmm0, xmm1\
		__asm op xmm1, xmm0\
		__asm op xmm0, xmm1\
		__asm op xmm1, xmm0}
#define CREATE_SSE_2OP_LATENCY_BENCH_80IMPL(op) REPEAT_10TIMES(CREATE_SSE_2OP_LATENCY_BENCH_8IMPL(op))
#define CREATE_SSE_2OP_LATENCY_BENCH_800IMPL(op) REPEAT_10TIMES(CREATE_SSE_2OP_LATENCY_BENCH_80IMPL(op))
#define CREATE_SSE_2OP_LATENCY_BENCH(op) double get_SSE_##op## _latency(){\
	auto start = std::chrono::system_clock::now();\
	REPEAT_10TIMES(CREATE_SSE_2OP_LATENCY_BENCH_800IMPL(op));\
	return 8000.0 / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count();\
}
#define CREATE_SSE_2OP_THROUGHPUT_BENCH_8IMPL(op) {__asm op xmm0, xmm0\
		__asm op xmm1, xmm1\
		__asm op xmm2, xmm2\
		__asm op xmm3, xmm3\
		__asm op xmm4, xmm4\
		__asm op xmm5, xmm5\
		__asm op xmm6, xmm6\
		__asm op xmm7, xmm7}
#define CREATE_SSE_2OP_THROUGHPUT_BENCH_80IMPL(op) REPEAT_10TIMES(CREATE_SSE_2OP_THROUGHPUT_BENCH_8IMPL(op))
#define CREATE_SSE_2OP_THROUGHPUT_BENCH_800IMPL(op) REPEAT_10TIMES(CREATE_SSE_2OP_THROUGHPUT_BENCH_80IMPL(op))
#define CREATE_SSE_2OP_THROUGHPUT_BENCH(op) double get_SSE_##op## _throughput(){\
	auto start = std::chrono::system_clock::now();\
	REPEAT_10TIMES(CREATE_SSE_2OP_THROUGHPUT_BENCH_800IMPL(op));\
	return 8000.0 / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count();\
}
#define CREATE_SSE_3OP_LATENCY_BENCH_8IMPL(op) {__asm op xmm0, xmm1, 0x00\
		__asm op xmm1, xmm0, 0x00\
		__asm op xmm0, xmm1, 0x00\
		__asm op xmm1, xmm0, 0x00\
		__asm op xmm0, xmm1, 0x00\
		__asm op xmm1, xmm0, 0x00\
		__asm op xmm0, xmm1, 0x00\
		__asm op xmm1, xmm0, 0x00}
#define CREATE_SSE_3OP_LATENCY_BENCH_80IMPL(op) REPEAT_10TIMES(CREATE_SSE_3OP_LATENCY_BENCH_8IMPL(op))
#define CREATE_SSE_3OP_LATENCY_BENCH_800IMPL(op) REPEAT_10TIMES(CREATE_SSE_3OP_LATENCY_BENCH_80IMPL(op))
#define CREATE_SSE_3OP_LATENCY_BENCH(op) double get_SSE_##op## _latency(){\
	auto start = std::chrono::system_clock::now();\
	REPEAT_10TIMES(CREATE_SSE_3OP_LATENCY_BENCH_800IMPL(op));\
	return 8000.0 / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count();\
}
#define CREATE_SSE_3OP_THROUGHPUT_BENCH_8IMPL(op) {__asm op xmm0, xmm0, 0x00\
		__asm op xmm1, xmm1, 0x00\
		__asm op xmm2, xmm2, 0x00\
		__asm op xmm3, xmm3, 0x00\
		__asm op xmm4, xmm4, 0x00\
		__asm op xmm5, xmm5, 0x00\
		__asm op xmm6, xmm6, 0x00\
		__asm op xmm7, xmm7, 0x00}
#define CREATE_SSE_3OP_THROUGHPUT_BENCH_80IMPL(op) REPEAT_10TIMES(CREATE_SSE_3OP_THROUGHPUT_BENCH_8IMPL(op))
#define CREATE_SSE_3OP_THROUGHPUT_BENCH_800IMPL(op) REPEAT_10TIMES(CREATE_SSE_3OP_THROUGHPUT_BENCH_80IMPL(op))
#define CREATE_SSE_3OP_THROUGHPUT_BENCH(op) double get_SSE_##op## _throughput(){\
	auto start = std::chrono::system_clock::now();\
	REPEAT_10TIMES(CREATE_SSE_3OP_THROUGHPUT_BENCH_800IMPL(op));\
	return 8000.0 / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count();\
}
template<typename Func>
double max_in_10times(Func func) {
	double max = 0;
	for (auto i = 0; i < 100; ++i) {
		auto tmp = func();
		if (tmp > max) max = tmp;
	}
	return max;
}

double get_current_clock() {
	auto start = std::chrono::system_clock::now(); \
		REPEAT_10TIMES(CREATE_SSE_2OP_LATENCY_BENCH_800IMPL(por));
	return 8000.0 / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count(); \
}

#define CREATE_SSE_2OP_BENCHS(op) CREATE_SSE_2OP_LATENCY_BENCH( op );\
CREATE_SSE_2OP_THROUGHPUT_BENCH( op );\
void print_##op (){std::cout << #op << std::string(13-sizeof( #op ), ' ')\
<< max_in_10times(get_SSE_##op## _throughput) / max_in_10times(get_current_clock)\
<< std::string(7, ' ') << max_in_10times(get_current_clock) / max_in_10times(get_SSE_##op## _latency) << std::endl;}

#define CREATE_SSE_3OP_BENCHS(op) CREATE_SSE_3OP_LATENCY_BENCH( op );\
CREATE_SSE_3OP_THROUGHPUT_BENCH( op );\
void print_##op (){std::cout << #op << std::string(13-sizeof( #op ), ' ')\
<< max_in_10times(get_SSE_##op## _throughput) / max_in_10times(get_current_clock)\
<< std::string(7, ' ') << max_in_10times(get_current_clock) / max_in_10times(get_SSE_##op## _latency) << std::endl;}

// ps
CREATE_SSE_2OP_BENCHS(movaps);

CREATE_SSE_2OP_BENCHS(orps);
CREATE_SSE_2OP_BENCHS(andps);
CREATE_SSE_2OP_BENCHS(andnps);
CREATE_SSE_2OP_BENCHS(xorps);

CREATE_SSE_2OP_BENCHS(addps);
CREATE_SSE_2OP_BENCHS(mulps);
CREATE_SSE_2OP_BENCHS(divps);

CREATE_SSE_2OP_BENCHS(rcpps);
CREATE_SSE_2OP_BENCHS(sqrtps);
CREATE_SSE_2OP_BENCHS(rsqrtps);
CREATE_SSE_3OP_BENCHS(roundps);

CREATE_SSE_3OP_BENCHS(blendps);
CREATE_SSE_3OP_BENCHS(cmpps);

CREATE_SSE_2OP_BENCHS(maxps);
CREATE_SSE_2OP_BENCHS(minps);

CREATE_SSE_3OP_BENCHS(shufps);

int main() {
	SetThreadAffinityMask(GetCurrentThread(), 1);
	warm_up();
	auto frequency = max_in_10times(get_current_clock);

	std::cout << "max frequency: " << frequency << " GHz" << std::endl;

	std::cout << std::fixed << std::setprecision(2) << std::endl;
	std::cout << "SSE instructions" << std::endl;
	std::cout << "instruction throughput latency" << std::endl;

	print_movaps();

	print_orps();
	print_andps();
	print_andnps();
	print_xorps();

	print_addps();
	print_mulps();
	print_divps();

	print_rcpps();
	print_sqrtps();
	print_rsqrtps();
	print_roundps();

	print_blendps();
	print_cmpps();
	print_maxps();
	print_minps();

	print_shufps();

	return 0;
}