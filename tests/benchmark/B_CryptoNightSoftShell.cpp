#include <benchmark/benchmark.h>

#include <random>
#include <crypto/hash.h>

using random_bytes_engine = std::independent_bits_engine<
    std::default_random_engine, CHAR_BIT, uint16_t>;

static void HeightArguments(benchmark::internal::Benchmark* b) {
  for (int i = 0; i <= CN_SOFT_SHELL_WINDOW * 2; ++i)
      b->Arg(i);
}

static void BM_CN_SoftShell(benchmark::State& state) {
  random_bytes_engine rbe;
  std::vector<uint16_t> data;
  data.resize(76 / 2);
  std::generate(begin(data), end(data), std::ref(rbe));

  uint32_t height = static_cast<uint32_t>(state.range(0));

  Crypto::Hash hash;
  for (auto _ : state)
    cn_soft_shell_slow_hash_v1(data.data(), data.size() * 2, hash, height);
}
// Register the function as a benchmark
BENCHMARK(BM_CN_SoftShell)->Apply(HeightArguments);

BENCHMARK_MAIN();
