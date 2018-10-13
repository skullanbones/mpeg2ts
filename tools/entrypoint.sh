#!/bin/bash

# Add local user
# Either use the LOCAL_USER_ID if passed in at runtime or
# fallback

USER_ID=${LOCAL_USER_ID:-9001}
USER=docker

echo "Starting with USER: $USER and UID : $USER_ID"
useradd --shell /bin/bash -u $USER_ID -o -c "" -m "$USER"
export HOME=/home/$USER

exec /usr/local/bin/gosu "$USER" "$@"
