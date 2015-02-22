#!/bin/bash

source $(dirname $0)/../credentials.cfg

curl -s "https://api.spark.io/v1/devices/$DEVICE/ipAddress?access_token=$TOKEN" \
  | grep -o '"result"\s*:\s*".*"'
