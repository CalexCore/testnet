#include <benchmark/benchmark.h>

#include "HashBasedBenchmark.h"

static void HeightArguments(benchmark::internal::Benchmark* b) {
  for (int i = 0; i <= CN_SOFT_SHELL_WINDOW * 2; i += 128)
      b->Args({i});
}

BENCHMARK_DEFINE_F(HashBasedBenchmark, BM_CN_SoftShell)(benchmark::State& state) {
  unsigned char const * data = HashBasedBenchmark::data();
  uint32_t height = static_cast<uint32_t>(state.range(0));
  for (auto _ : state)
  {
    for(std::size_t i = 0; i < BlockCount; ++i)
      cn_soft_shell_slow_hash_v1(data + i * BlockSize, BlockSize, HashPlaceholder, height);
  }
}

BENCHMARK_REGISTER_F(HashBasedBenchmark, BM_CN_SoftShell)->Apply(HeightArguments)->Unit(benchmark::kMillisecond);
