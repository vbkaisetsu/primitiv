#include <config.h>

#include <chrono>
#include <thread>
#include <vector>
#include <gtest/gtest.h>
#include <primitiv/cuda_device.h>
#include <primitiv/error.h>
#include <primitiv/shape.h>
#include <primitiv/tensor.h>
#include <test_utils.h>

using std::vector;
using test_utils::vector_match;

namespace primitiv {

class CUDADeviceTest : public testing::Test {};

TEST_F(CUDADeviceTest, CheckDeviceType) {
  devices::CUDA dev(0);
  EXPECT_EQ(Device::DEVICE_TYPE_CUDA, dev.type());
}

TEST_F(CUDADeviceTest, CheckInvalidInit) {
  // We probably do not have millions of GPUs in one host.
  EXPECT_THROW(devices::CUDA dev(12345678), Error);
}

TEST_F(CUDADeviceTest, CheckNewDelete) {
  {
    devices::CUDA dev(0);
    {
      Tensor x1 = dev.new_tensor(Shape());  // 1 value
      Tensor x2 = dev.new_tensor(Shape({16, 16}));  // 256 values
      Tensor x3 = dev.new_tensor(Shape({16, 16, 16}, 16));  // 65536 values
    }
    // All tensors are already deleted before arriving here.
  }
  SUCCEED();
}

TEST_F(CUDADeviceTest, CheckDanglingTensor) {
  {
    Tensor x1;
    {
      devices::CUDA dev(0);
      x1 = dev.new_tensor(Shape());
    }
    // x1 still has valid object,
    // but there is no guarantee that the memory is alive.
    // Our implementation only guarantees the safety to delete Tensors anytime.
  }
  SUCCEED();
}

#ifdef PRIMITIV_BUILD_TESTS_PROBABILISTIC
TEST_F(CUDADeviceTest, CheckRandomBernoulli) {
  vector<vector<float>> history;
  for (unsigned i = 0; i < 10; ++i) {
    devices::CUDA dev(0);
    const Tensor x = dev.random_bernoulli(Shape({3, 3}, 3), 0.3);
    const vector<float> x_val = x.to_vector();

    std::cout << "Epoch " << i << ':';
    for (float x_i : x_val) {
      std::cout << ' ' << x_i;
    }
    std::cout << std::endl;

    for (const vector<float> &h_val : history) {
      EXPECT_FALSE(vector_match(x_val, h_val));
    }
    history.emplace_back(x_val);

    // Wait for updating the device randomizer.
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}
#endif  // PRIMITIV_BUILD_TESTS_PROBABILISTIC

TEST_F(CUDADeviceTest, CheckRandomBernoulliWithSeed) {
  const vector<float> expected {
    1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1,
  };
  devices::CUDA dev(0, 12345);
  const Tensor x = dev.random_bernoulli(Shape({4, 4}, 4), 0.3);
  EXPECT_TRUE(vector_match(expected, x.to_vector()));
}

#ifdef PRIMITIV_BUILD_TESTS_PROBABILISTIC
TEST_F(CUDADeviceTest, CheckRandomUniform) {
  vector<vector<float>> history;
  for (unsigned i = 0; i < 10; ++i) {
    devices::CUDA dev(0);
    const Tensor x = dev.random_uniform(Shape({2, 2}, 2), -9, 9);
    const vector<float> x_val = x.to_vector();

    std::cout << "Epoch " << i << ':';
    for (float x_i : x_val) {
      std::cout << ' ' << x_i;
    }
    std::cout << std::endl;

    for (const vector<float> &h_val : history) {
      EXPECT_FALSE(vector_match(x_val, h_val));
    }
    history.emplace_back(x_val);

    // Wait for updating the device randomizer.
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}
#endif  // PRIMITIV_BUILD_TESTS_PROBABILISTIC

TEST_F(CUDADeviceTest, CheckRandomUniformWithSeed) {
  const vector<float> expected {
    -3.6198268e+00, 4.1064610e+00, -6.9007745e+00, 8.5519943e+00,
    -7.7016129e+00, -4.6067810e+00, 8.7706423e+00, -4.9437490e+00,
  };
  devices::CUDA dev(0, 12345);
  const Tensor x = dev.random_uniform(Shape({2, 2}, 2), -9, 9);
  EXPECT_TRUE(vector_match(expected, x.to_vector()));
}

#ifdef PRIMITIV_BUILD_TESTS_PROBABILISTIC
TEST_F(CUDADeviceTest, CheckRandomNormal) {
  vector<vector<float>> history;
  for (unsigned i = 0; i < 10; ++i) {
    devices::CUDA dev(0);
    const Tensor x = dev.random_normal(Shape({2, 2}, 2), 1, 3);
    const vector<float> x_val = x.to_vector();

    std::cout << "Epoch " << i << ':';
    for (float x_i : x_val) {
      std::cout << ' ' << x_i;
    }
    std::cout << std::endl;

    for (const vector<float> &h_val : history) {
      EXPECT_FALSE(vector_match(x_val, h_val));
    }
    history.emplace_back(x_val);

    // Wait for updating the device randomizer.
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}
#endif  // PRIMITIV_BUILD_TESTS_PROBABILISTIC

TEST_F(CUDADeviceTest, CheckRandomNormalWithSeed) {
  const vector<float> expected {
    4.1702256e+00, -2.4186814e+00, 1.5060894e+00, -1.3355234e+00,
    -5.0218196e+00, -5.5439359e-01, 5.8913720e-01, 1.5337296e+00,
  };
  devices::CUDA dev(0, 12345);
  const Tensor x = dev.random_normal(Shape({2, 2}, 2), 1, 3);
  EXPECT_TRUE(vector_match(expected, x.to_vector()));
}

#ifdef PRIMITIV_BUILD_TESTS_PROBABILISTIC
TEST_F(CUDADeviceTest, CheckRandomLogNormal) {
  vector<vector<float>> history;
  for (unsigned i = 0; i < 10; ++i) {
    devices::CUDA dev(0);
    const Tensor x = dev.random_log_normal(Shape({2, 2}, 2), 1, 3);
    const vector<float> x_val = x.to_vector();

    std::cout << "Epoch " << i << ':';
    for (float x_i : x_val) {
      std::cout << ' ' << x_i;
    }
    std::cout << std::endl;

    for (const vector<float> &h_val : history) {
      EXPECT_FALSE(vector_match(x_val, h_val));
    }
    history.emplace_back(x_val);

    // Wait for updating the device randomizer.
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}
#endif  // PRIMITIV_BUILD_TESTS_PROBABILISTIC

TEST_F(CUDADeviceTest, CheckRandomLogNormalWithSeed) {
  const vector<float> expected {
    6.4730049e+01, 8.9038946e-02, 4.5090632e+00, 2.6302049e-01,
    6.5925200e-03, 5.7442045e-01, 1.8024327e+00, 4.6354327e+00,
  };
  devices::CUDA dev(0, 12345);
  const Tensor x = dev.random_log_normal(Shape({2, 2}, 2), 1, 3);
  EXPECT_TRUE(vector_match(expected, x.to_vector()));
}

}  // namespace primitiv
