Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/trusty64"
  config.vm.provision :shell, :inline => $BOOTSTRAP_SCRIPT # see below
  config.ssh.forward_agent = true
end

$BOOTSTRAP_SCRIPT = <<EOF
  set -e # Stop on any error

  export DEBIAN_FRONTEND=noninteractive

  # Make vagrant automatically go to /vagrant when we ssh in.
  echo "cd /vagrant" | sudo tee -a ~vagrant/.profile

  # APT packages
  sudo apt-get install -y zip libc6-i386 lib32gcc1

  # NACL SDK
  VERSION=pepper_37
  mkdir tmp
  cd tmp
  wget http://storage.googleapis.com/nativeclient-mirror/nacl/nacl_sdk/nacl_sdk.zip
  unzip nacl_sdk.zip
  cd nacl_sdk
  ./naclsdk list
  ./naclsdk update $VERSION
  echo 'export NACL_SDK_ROOT='`pwd`"/$VERSION" | sudo tee -a ~vagrant/.profile
  sudo chown -R vagrant .

  echo VAGRANT IS READY.
EOF
