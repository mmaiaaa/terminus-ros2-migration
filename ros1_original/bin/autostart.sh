#!/bin/bash

cd /home/anbot/system/bin

gnome-terminal -x bash -c 'export RUN_AFTER_BASHRC="ls --help"; exec bash'

#add    sh start.sh "$RUN_AFTER_BASHRC"    in ~/.bashrc
