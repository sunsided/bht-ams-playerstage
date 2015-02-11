# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|
  config.vm.box = "sunside/playerstage"

  config.vm.provision "shell", inline: <<-SHELL
  	apt-get -y install libopencv-dev libopencv-highgui-dev
  SHELL
end
