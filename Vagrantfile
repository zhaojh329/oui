Vagrant.configure("2") do |config|
  config.vm.box = "vladimir-babichev/openwrt-19.07.3"
  config.vm.hostname = "openwrt"
  config.vm.post_up_message = "Please login using username 'root' and password 'vagrant' at http://openwrt.lan"
  config.vm.network "private_network", ip: "10.10.10.10"

  config.vm.provision "shell", inline: <<-SHELL
    opkg update 
    opkg install uhttpd-mod-ubus
    /etc/init.d/rpcd restart 
    /etc/init.d/uhttpd restart
  SHELL
end
