#!/bin/bash

# An example configuration for test2hosts.sh.
# Copy this to config-remote.sh and edit it.
# Then test2hosts.sh can run.

# A host that you can ssh to with credentials, without a password prompt.
# To make such credentials:
#   On localhost, ssh-keygen -t rsa; then append ~/.ssh/id_rsa.pub to $host's ~/.ssh/authorized_keys.
#   On $host, chmod go-rw ~/.ssh/authorized_keys
#   On $host in /etc/ssh/sshd_config, set PubkeyAuthentication yes; service ssh --full-restart
host=myhost.mydomain.com

# On $host, the port that sshd listens to.
# To change it from the default of 22, to /etc/ssh/sshd_config add a line like `Port 1234`.
# Then `sudo service ssh --full-restart`.
# You may need to poke holes in firewalls for this TCP port, e.g.,
# ufw allow 123/tcp comment 'ssh on nonstandard port'
sshport=123

# On $host, the dir containing ckserver.
dirRemote=/foo/bar/clockkit/ClockKit

# On $host, the address on which ckserver will listen
# (0.0.0.0 for all networks;  more safely, $host's public IP address;
# but 127.0.0.1 will fail make test-remote, because that's *not* remote).
address=127.0.0.1

# On $host, the port on which ckserver will listen.
# You may need to poke holes in firewalls for this UDP port, e.g.,
# ufw allow 4567/udp comment ClockKit
port=4567

# The timeout for localhost's ./ckphaselock connecting to $host.
timeout_msec=500
