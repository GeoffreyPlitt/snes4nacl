Vagrant.configure("2") do |config|
  config.vm.box = "nacl-vagrant-pepper37-0.1.1"
  config.vm.box_url = 'https://github.com/GeoffreyPlitt/nacl-vagrant/releases/download/pepper37-0.1.1/nacl-vagrant-pepper37-0.1.1.box'
  config.vm.provision :shell, :inline => $BOOTSTRAP_SCRIPT # see below
  config.vm.network :forwarded_port, host: 8080, guest: 8080
  config.ssh.forward_agent = true
end

$BOOTSTRAP_SCRIPT = <<EOF
  set -e # Stop on any error

  export DEBIAN_FRONTEND=noninteractive

  # Make vagrant automatically go to /vagrant when we ssh in.
  echo "cd /vagrant" | sudo tee -a ~vagrant/.profile

  echo VAGRANT IS READY.
EOF
