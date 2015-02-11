# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|
  config.vm.box = "sunside/playerstage"

  config.vm.network "forwarded_port", guest: 6080, host: 6080
  config.vm.network "forwarded_port", guest: 5900, host: 5900

  config.vm.provision "shell", inline: <<-SHELL
  	apt-get -y install --no-install-recommends \
  		libopencv-dev libopencv-highgui-dev \
  		xserver-xorg-video-dummy x11vnc \
  		fluxbox xterm \
  		supervisor \
  		git-core

	# stop supervisord
  	service supervisor stop

  	# patch the hostname
  	echo "ams-playerstage" > /etc/hostname
  	hostname "ams-playerstage"

  	# the default display
  	echo "export DISPLAY=:1" >> /etc/profile

  	# noVNC
  	rm -rf /srv/noVNC 2> /dev/null
  	git clone --single-branch --branch v0.5.1 --depth 1 https://github.com/kanaka/noVNC.git /srv/noVNC

  	# fetch config
  	rm -rf /opt/vagrant-config 2> /dev/null
  	git clone --single-branch --branch vagrant/config --depth 1 https://github.com/sunsided/bht-ams-playerstage.git /opt/vagrant-config
  	chmod -R +rX /opt/vagrant-config
  	
	# wire up xorg.conf
	rm -f /etc/X11/xorg.conf 2> /dev/null
	ln -s /opt/vagrant-config/X11/xorg.conf /etc/X11/xorg.conf

	# wire up supervisord
	rm -rf /etc/supervisor/conf.d
	ln -s /opt/vagrant-config/supervisor /etc/supervisor/conf.d

	# start supervisord
	service supervisor start
  SHELL
end
