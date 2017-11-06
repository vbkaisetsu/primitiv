#/bin/bash
set -xe

# before_install
sudo apt install -y qemu qemu-kvm sshpass xz-utils
wget https://github.com/vbkaisetsu/qemu-freebsd-ssh-enabled/raw/master/FreeBSD-11.1-RELEASE-amd64.qcow2.xz
xz -d ./FreeBSD-11.1-RELEASE-amd64.qcow2.xz
sudo qemu-system-x86_64 -enable-kvm ./FreeBSD-11.1-RELEASE-amd64.qcow2 -m 1600 -net user,hostfwd=tcp::10022-:22 -net nic -nographic &

sleep 60

export SSHCMD="sshpass -p password ssh -o ServerAliveInterval=30 -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no root@localhost -p10022"
export SCPCMD="sshpass -p password scp -o ServerAliveInterval=30 -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no -P10022 -r"

$SCPCMD $TRAVIS_BUILD_DIR root@localhost:primitiv

# install
$SSHCMD "env ASSUME_ALWAYS_YES=YES pkg update"
$SSHCMD "env ASSUME_ALWAYS_YES=YES pkg install protobuf cmake git googletest python3"
$SSHCMD "python3 -m ensurepip"
$SSHCMD "pip3 install numpy cython"

# script
$SSHCMD "cd primitiv && cmake . -DPRIMITIV_BUILD_TESTS=ON -DCMAKE_INSTALL_PREFIX=/usr"
$SSHCMD "cd primitiv && make VERBOSE=1"
$SSHCMD "cd primitiv && make test"
$SSHCMD "cd primitiv && make install"
$SSHCMD "cd primitiv/python-primitiv && ./setup.py build"
$SSHCMD "cd primitiv/python-primitiv && ./setup.py test"

# after_script
sudo killall qemu-system-x86_64
rm ./FreeBSD-11.1-RELEASE-amd64.qcow2
