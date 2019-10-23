#!/bin/bash
#script for installing the neccassary modules for the wifi-hotspot
sudo apt install python3-pip
sudo pip3 install --upgrade virtualenv
pip3 install future
sudo apt-get install python3-future
sudo apt-get install python3-setuptools
sudo apt-get install python3-packaging
pip3 install pywhich
pip3 install pytest-shutil
sudo apt-get update
sudo apt-get upgrade
sudo apt update && sudo apt --yes --force-yes install dnsmasq hostapd python3-dev python3-pip && sudo pip3 install pyaccesspoint
sudo touch /etc/pyaccesspoint/accesspoint.json
sudo echo '{"ssid": "OpenDrone", "inet": null, "wlan": "wlan0", "password": "1234567890", "netmask": "255.255.255.0", "ip": "10.0.0.138"}' >> /etc/accesspoint/accesspoint.json
