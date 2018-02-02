#include <primitiv/config.h>

#include <primitiv/eigen_device.h>
#include <primitiv/device_ops/eigen_utils.h>

namespace primitiv {
namespace devices {

EIGEN_DEV_FW_X(sqrt, x.sqrt());
EIGEN_DEV_BW_X(sqrt, .5 * gy / y);

}  // namespace devices
}  // namespace primitiv
