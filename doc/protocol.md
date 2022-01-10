# protocol

This file defines the protocol that is being used to exchange messages between
the client and the server.

## Overview

As you may have noticed from the repo structure, Scrtp uses a client-server
model. The computer which sends its own screen represents the server, while
external computers which receive the screen and send peripheral signals (like a
mouse click or a key press on the keyboard) act as clients.

The following outline describes an overview of the messages sent. The client is
represented using the letter `C` while the server is represented using the
letter `S`. Symbols like `>>` and `<<` are used to specify the message
direction.

```
 C      S
 |      |
 +-->>--+ (1) The client sends a connection request to the server.
 |      |
 +--<<--+ (2) The server replies with "OK" (and a list of windows) or "NO".
 |      |
 +-->>--+ (3) The client chooses a window.
 |      |
 +--<<--+ (4) The server sends the current window frame.
 |      |
 +-->>--+ (5) The client sends an input signal.
 |      |
 +--<<--+ (6) The server sends another frame.
 |      |
 +-->>--+ (7) The client sends another input signal.
 |      |
 .      .
 .      .
 .      .
 |      |
 +-->>--+ (n) Eventually, the client closes the connection.
```

Note that, if the server replies with "NO" in step `(2)`, the server will close
the connection.

## Steps

The following steps are meant to examine in depth the outline shown in the
overview. The number of each step corresponds with the number in the outline.

 1. When the client sends a connection request to the server, a TCP connection
    is established between the two. Then, the client begins the process of
    authentication by sending something that can confirm its authenticity (see
    [auth](auth.md)).

 2. The server replies with a 2 characters string which can be either `OK` or
    `NO`. If it is `OK`, the authentication has successfully terminated and the
    server also sends a list of windows currently open in the computer,
    separated by a newline character. Otherwise, in case the server replied with
    `NO`, the connection will be closed by the server itself.

 3. The client now has to choose a window (or the whole desktop) and send the
    chosen window's ID to the server.

 4. The server sends the current frame of the window.

 5. The client sends an input signal such as a mouse click or a key press on the
    keyboard.

 6. The server emulates the input signal on the window and sends the updated
    window frame.

Steps `(5)` and `(6)` repeat for each input signal.

### Concurrency

The sequential steps described above would be ideal if the window did not update
(unless on user input) or had no animations.

(TODO)

## Structure of packets

(TODO)
