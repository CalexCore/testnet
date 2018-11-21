#include <benchmark/benchmark.h>

#include <random>
#include <climits>

#include <crypto/hash.h>

namespace  {
  using random_bytes_engine = std::independent_bits_engine<
      std::default_random_engine, CHAR_BIT, uint16_t>;

  const std::size_t NumRndBlocks = 10;
  const std::size_t BlockSize = 76;
}

static void HeightArguments(benchmark::internal::Benchmark* b) {
  for (int i = 0; i <= CN_SOFT_SHELL_WINDOW; i += 16)
      b->Args({i});
}

static void AmityFlavoredHeights(benchmark::internal::Benchmark* b) {
  for (uint32_t i = 0; i <= Crypto::amity_flavored_window_size(); i += 1)
      b->Args({i});
}

static void BM_CN_SoftShell(benchmark::State& state) {
  random_bytes_engine rbe;
  std::vector<uint16_t> data;
  data.resize(NumRndBlocks * BlockSize / 2);
  std::generate(begin(data), end(data), std::ref(rbe));

  uint32_t height = static_cast<uint32_t>(state.range(0));

  Crypto::Hash hash;
  for (auto _ : state)
  {
    for(std::size_t i = 0; i < NumRndBlocks; ++i)
      cn_soft_shell_slow_hash_v1(data.data() + i * (BlockSize / 2), BlockSize, hash, height);
  }
  state.counters["offset"] = height / 16;
  state.counters["type"] = 0;
}

BENCHMARK(BM_CN_SoftShell)->Apply(HeightArguments);

static void BM_CN_AF_SoftShell(benchmark::State& state) {
  random_bytes_engine rbe;
  std::vector<uint16_t> data;
  data.resize(NumRndBlocks * BlockSize / 2);
  std::generate(begin(data), end(data), std::ref(rbe));

  uint32_t height = static_cast<uint32_t>(state.range(0));

  Crypto::Hash hash;
  for (auto _ : state)
  {
    for(std::size_t i = 0; i < NumRndBlocks; ++i)
      amity_flavored_slow_hash_v0(data.data() + i * (BlockSize / 2), BlockSize, hash, height);
  }
  state.counters["offset"] = height;
  state.counters["type"] = 1;
}

BENCHMARK(BM_CN_AF_SoftShell)->Apply(AmityFlavoredHeights);

BENCHMARK_MAIN();
