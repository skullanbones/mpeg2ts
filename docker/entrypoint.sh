#!/bin/bash

# Add local user
# Either use the LOCAL_USER_ID if passed in at runtime or
# fallback

USER_ID=${LOCAL_USER_ID:-9001}
echo "LOCAL_USER_ID: $LOCAL_USER_ID"
USER=docker
UPWD=Docker!

echo "Starting with USER: $USER and UID : $USER_ID"
useradd --shell /bin/bash --uid $USER_ID -o -c "" -m "$USER"
# Add user to sudoers
echo "docker ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/10-installer
# Add root password
echo "root":$UPWD | chpasswd
# Add user password
echo "$USER:$UPWD" | chpasswd
export HOME=/home/$USER
su $USER
echo "Starting user:"
whoami
