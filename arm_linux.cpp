#include <chrono>
#include <iostream>
#include <string>
#include <iomanip>

void warm_up(){
	auto busy_loop = []() {for (volatile auto i = 0; i < 1000000; ++i); };
	for (auto i = 0; i < 1000; ++i) busy_loop();
}

#define REPEAT_10TIMES(func) func;func;func;func;func;\
func;func;func;func;func;

#define CREATE_ASIMD_2OP_LATENCY_BENCH_8IMPL(op) __asm__ volatile(#op" s0, s1;"\
		#op" s1, s0;"\
		#op" s0, s1;"\
		#op" s1, s0;"\
		#op" s0, s1;"\
		#op" s1, s0;"\
		#op" s0, s1;"\
		#op" s1, s0;"\
		:::"v0","v1");
#define CREATE_ASIMD_2OP_LATENCY_BENCH_80IMPL(op) REPEAT_10TIMES(CREATE_ASIMD_2OP_LATENCY_BENCH_8IMPL(op))
#define CREATE_ASIMD_2OP_LATENCY_BENCH_800IMPL(op) REPEAT_10TIMES(CREATE_ASIMD_2OP_LATENCY_BENCH_80IMPL(op))
#define CREATE_ASIMD_2OP_LATENCY_BENCH(op) double get_ASIMD_##op## _latency(){\
	auto start = std::chrono::system_clock::now();\
	REPEAT_10TIMES(CREATE_ASIMD_2OP_LATENCY_BENCH_800IMPL(op));\
	return 8000.0 / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count();\
}

#define CREATE_ASIMD_2OP_THROUGHPUT_BENCH_8IMPL(op) __asm__ volatile(#op" s0, s0;"\
		#op" s1, s1;"\
		#op" s2, s2;"\
		#op" s3, s3;"\
		#op" s4, s4;"\
		#op" s5, s5;"\
		#op" s6, s6;"\
		#op" s7, s7;"\
		:::"v0","v1","v2","v3","v4","v5","v6","v7");
#define CREATE_ASIMD_2OP_THROUGHPUT_BENCH_80IMPL(op) REPEAT_10TIMES(CREATE_ASIMD_2OP_THROUGHPUT_BENCH_8IMPL(op))
#define CREATE_ASIMD_2OP_THROUGHPUT_BENCH_800IMPL(op) REPEAT_10TIMES(CREATE_ASIMD_2OP_THROUGHPUT_BENCH_80IMPL(op))
#define CREATE_ASIMD_2OP_THROUGHPUT_BENCH(op) double get_ASIMD_##op## _throughput(){\
	auto start = std::chrono::system_clock::now();\
	REPEAT_10TIMES(CREATE_ASIMD_2OP_THROUGHPUT_BENCH_800IMPL(op));\
	return 8000.0 / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count();\
}

#define CREATE_ASIMD_3OP_LATENCY_BENCH_8IMPL(op) __asm__ volatile(#op" s0, s1, s2;"\
		#op" s1, s2, s0;"\
		#op" s2, s0, s1;"\
		#op" s0, s1, s2;"\
		#op" s1, s2, s0;"\
		#op" s2, s0, s1;"\
		#op" s0, s1, s2;"\
		#op" s1, s2, s0;"\
		:::"v0","v1","v2");
#define CREATE_ASIMD_3OP_LATENCY_BENCH_80IMPL(op) REPEAT_10TIMES(CREATE_ASIMD_3OP_LATENCY_BENCH_8IMPL(op))
#define CREATE_ASIMD_3OP_LATENCY_BENCH_800IMPL(op) REPEAT_10TIMES(CREATE_ASIMD_3OP_LATENCY_BENCH_80IMPL(op))
#define CREATE_ASIMD_3OP_LATENCY_BENCH(op) double get_ASIMD_##op## _latency(){\
	auto start = std::chrono::system_clock::now();\
	REPEAT_10TIMES(CREATE_ASIMD_3OP_LATENCY_BENCH_800IMPL(op));\
	return 8000.0 / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count();\
}

#define CREATE_ASIMD_3OP_THROUGHPUT_BENCH_8IMPL(op) __asm__ volatile(#op" s0, s0, s0;"\
		#op" s1, s1, s1;"\
		#op" s2, s2, s2;"\
		#op" s3, s3, s3;"\
		#op" s4, s4, s4;"\
		#op" s5, s5, s5;"\
		#op" s6, s6, s6;"\
		#op" s7, s7, s7;"\
		:::"v0","v1","v2","v3","v4","v5","v6","v7");
#define CREATE_ASIMD_3OP_THROUGHPUT_BENCH_80IMPL(op) REPEAT_10TIMES(CREATE_ASIMD_3OP_THROUGHPUT_BENCH_8IMPL(op))
#define CREATE_ASIMD_3OP_THROUGHPUT_BENCH_800IMPL(op) REPEAT_10TIMES(CREATE_ASIMD_3OP_THROUGHPUT_BENCH_80IMPL(op))
#define CREATE_ASIMD_3OP_THROUGHPUT_BENCH(op) double get_ASIMD_##op## _throughput(){\
	auto start = std::chrono::system_clock::now();\
	REPEAT_10TIMES(CREATE_ASIMD_3OP_THROUGHPUT_BENCH_800IMPL(op));\
	return 8000.0 / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count();\
}

template<typename Func>
double max_in_10times(Func func){
	double max = 0;
	for (auto i = 0; i < 100; ++i) {
		auto tmp = func();
		if (tmp > max) max = tmp;
	}
	return max;
}

double get_current_clock() {
	auto start = std::chrono::system_clock::now();\
	REPEAT_10TIMES(REPEAT_10TIMES(REPEAT_10TIMES(__asm__ volatile("eor x0, x1, x2;"
		"eor x1, x2, x0;"
		"eor x2, x0, x1;"
		"eor x0, x1, x2;"
		"eor x1, x2, x0;"
		"eor x2, x0, x1;"
		"eor x0, x1, x2;"
		"eor x1, x2, x0;"
		"eor x2, x0, x1;"
		"eor x0, x1, x2;":::"x0","x1","x2"))));
	return 8000.0 / std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - start).count();\
}

#define CREATE_ASIMD_2OP_BENCHS(op) CREATE_ASIMD_2OP_LATENCY_BENCH( op );\
CREATE_ASIMD_2OP_THROUGHPUT_BENCH( op );\
void print_##op (){std::cout << #op << std::string(13-sizeof( #op ), ' ')\
<< max_in_10times(get_ASIMD_##op## _throughput) / max_in_10times(get_current_clock)\
<< std::string(7, ' ') << max_in_10times(get_current_clock) / max_in_10times(get_ASIMD_##op## _latency) << std::endl;}

#define CREATE_ASIMD_3OP_BENCHS(op) CREATE_ASIMD_3OP_LATENCY_BENCH( op );\
CREATE_ASIMD_3OP_THROUGHPUT_BENCH( op );\
void print_##op (){std::cout << #op << std::string(13-sizeof( #op ), ' ')\
<< max_in_10times(get_ASIMD_##op## _throughput) / max_in_10times(get_current_clock)\
<< std::string(7, ' ') << max_in_10times(get_current_clock) / max_in_10times(get_ASIMD_##op## _latency) << std::endl;}

// ps
CREATE_ASIMD_3OP_BENCHS(fadd);

int main() {
	warm_up();
	auto frequency = max_in_10times(get_current_clock);

	std::cout << "max frequency: " << frequency << " GHz" << std::endl;

	std::cout << std::fixed << std::setprecision(2) << std::endl;
	std::cout << "ASIMD instructions" << std::endl;
	std::cout << "instruction throughput latency" << std::endl;

	print_fadd();

	return 0;
}
