#!/usr/bin/bash
# installSDK.sh
# CloudBackend 2022-2025.
export RELEASE="2.1.4"
echo "Installing CloudBackend SDK version" ${RELEASE}
echo    "Minimum   o/s: Ubuntu 22.04 LTS"
echo -n "Installed o/s: "
cat /etc/issue | cut -d '\' -f1
mkdir -p ~/cbe                              # if not already created
mydir=${PWD}
echo ${mydir}
if [ "${mydir}" = "$HOME/Downloads" ];
then
  echo "Error: May not run from" ${mydir}
  exit 2
fi
echo "moving" ${mydir} "to ~/cbe"
read -p "continue (Y/n) ? " Answer
if [ "${Answer}" = "n" ] || [ "${Answer}" = "N" ];
then
  echo "exit"
  exit 1
fi
if [ -d ~/cbe/${RELEASE} ];
then
  ls -ld ~/cbe/${RELEASE} ;
  echo "renaming" ~/cbe/${RELEASE} "to" ~/cbe/removed_${RELEASE}_`date --iso`;
  mv ~/cbe/${RELEASE} ~/cbe/removed_${RELEASE}_`date +%FT%H%M`;
fi
mv ${mydir} ~/cbe/${RELEASE}
touch ~/cbe/${RELEASE}
ls -ld ~/cbe/2* ~/cbe/removed_*
cd ~/cbe
if [ -L ~/cbe/current ];
then
  echo "renew symbolic link in " `pwd`
  rm ~/cbe/current                       # remove old symbolic link
fi
ln -s ${RELEASE} current                 # create symbolic link to the current version of the SDK
cd
ls -l cbe/current
echo "done."

echo ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
read -p "Next C++. Press ENTER " Answer
cd ~/cbe/current                            # the release directory
cd C++                                      # SDK C++
pwd
bash installC++SDK.sh                       # install script
echo "done."

echo ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
read -p "Install SDK Java module (Y/n) " Answer
if [ "${Answer}" = "n" ] || [ "${Answer}" = "N" ];
then
  echo "skip Java";
else
  cd ~/cbe/current                            # the release directory
  cd Java                                     # SDK Java
  pwd
  bash installJavaSDK.sh                      # install script
  echo "done."
fi

echo ¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤¤
cd
ls -l ~/cbe/current
echo "next: go to ~/cbe/current"
echo
echo "do: cd .."
