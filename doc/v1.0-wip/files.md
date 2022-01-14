# files

This file defines the files that are being used by Scrtp and their purpose.

As a rule of thumb, server's files are stored in `/etc/scrtp/` while client's
files are stored in `$HOME/.config/scrtp`. As a consequence, for the server to
be able to read and write in its directory, it may need root privileges.

## AES key

The client AES key is stored in `$HOME/.config/scrtp/aes_key`.

That key is personal and must be unique for each combination of user and
computer.

## `client_config` and `server_config`

These two files represent the client and server configuration files and they are
stored in:

 - `$HOME/.config/scrtp/client_config`
 - `/etc/scrtp/server_config`

## Server's known users

The server keeps the keys of its clients in `/etc/scrtp/known_users/<MAC_ADDR>`
where `<MAC_ADDR>` is the MAC address of the remote client computer. In this
file, keys are stored in separate lines, one for each key.

Whenever a new AES key is generated, for the server to be able to recognize it
as a valid client key, it needs to be put in the corresponding file, based on
which MAC address the remote client computer will have.

Every time a client tries to connect to the server, the latter will check the
client's key by searching it in the file named as the remote computer's MAC
adddress.
