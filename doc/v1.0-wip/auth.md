# auth

This file defines the authentication method used to ensure the authenticity of
the client computer.

## Overview

The authentication is done by checking a password sent through an asymmetrically
encrypted connection. The cryptosystem used for such task is
[RSA](https://en.wikipedia.org/wiki/RSA_(cryptosystem)).

## In-depth

Before the client sends the password, the client and the server exchange their
RSA public keys. At this point, the client can send the password encrypted so
that the server can verify its authenticity.

Once the server has verified the client's authenticity, it can stop using the
RSA cryptosystem.
