# An example configuration for test2hosts.sh.
# Copy this to config-remote.sh and edit it.
# Then test2hosts.sh can run.

# A host that you can ssh to without a password prompt.
host=myhost.mydomain.com

# On $host, the port that sshd listens to.  Usually 22.
sshport=123

# On $host, the dir containing ckserver.
dirRemote=/foo/bar/clockkit/ClockKit

# On $host, the port on which ckserver will listen.
port=4567

# The timeout for localhost's ./ckphaselock connecting to $host.
timeout_msec=500
