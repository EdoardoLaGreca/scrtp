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
 +--->>-+ (1) The client sends a connection request to the server.
 |      |
 +-<<---+ (2) The server sends its public key.
 |      |
 +--->>-+ (3) The client sends the password.
 |      |
 +-<<---+ (4) The server replies with "OK" (and a list of windows) or "NO".
 |      |
 +--->>-+ (5) The client chooses a window.
 |      |
 +-<<---+ (6) The server sends the current window frame.
 |      |
 +--->>-+ (7) The client sends an input signal.
 |      |
 +-<<---+ (8) The server sends another frame.
 |      |
 +--->>-+ (9) The client sends another input signal.
 |      |
 .      .
 .      .
 .      .
 |      |
 +--->>-+ (n) Eventually, the client closes the connection.
```

Note that, if the server replies with "NO" in step `(2)`, the server will close
the connection.

## Steps

The following steps are meant to examine in depth the outline shown in the
overview. The number of each step corresponds with the number in the outline.

 1. When the client sends a connection request to the server, a TCP connection
    is established between the two. Then, the client sends its public key (see
    [auth](auth.md)), its protocol version and the desired image quality.

 2. The server sends its public key (see [auth](auth.md)).

 3. The client authenticates by sending its encrypted password (see
    [auth](auth.md)).

 4. The server replies with a 2 characters string which can be either `OK` or
    `NO`. If it is `OK`, the authentication has successfully terminated and the
    server also sends a list of windows currently open in the computer,
    separated by a newline character. Otherwise, in case the server replied with
    `NO`, the connection will be closed by the server itself.

 5. The client now has to choose a window (or the whole desktop) and send the
    chosen window's ID to the server.

 6. The server sends the current frame of the window.

 7. The client sends an input signal such as a mouse click or a key press on the
    keyboard.

 8. The server emulates the input signal on the window and sends the updated
    window frame.

Steps `(7)` and `(8)` repeat for each input signal and each window frame.

After step `(3)`, the client's and server's public key is no longer necessary
and the connection will be encrypted through the password (see
[Encryption](#Encryption)).

Eventually, the client closes the connection.

## Concurrency

The sequential steps described above would be ideal if the window did not update
(unless on user input) and had no animations. In a modern world, it is not
conceivable to use a sequential approach hoping that the final user will not
need window updates in-between input signals.

To deal with this issue, Scrtp take advantage of concurrency. The frames sent by
the server are concurrently independent of the input signals sent by the client,
as if they were sent in two different channels.

```
frames:         S->->->->->->-C
input signals:  S-<-<-<-<-<-<-C
```

## Encryption

Most of packets are encrypted during their exchange. Their encryption is divided
into 2 separate parts: asymmetric encryption (for password checking) and
symmetric encryption (for window frames and input signals).

The reason behind these choices are simple:

 - Asymmetric encryption is ideal for password checking.
 - Symmetric encryption is a good compromise between privacy and performance.

## Structure of packets

Below there are visual representations of the packets sent by the client and by
the server.

Note that constant strings, which never change for a certain kind of packet, are
double-quoted. Every non-quoted space-separated word works as a field whose
content is not constant.

Also note that, since some kinds of packets will be sent continuously, it is
essential to optimize space. Therefore, the representation of these kinds of
packets also has information about the size of single fields.

### Authentication packet

(TODO)

### Server reply to authentication

```
+------+
| "NO" |
+------+
```

or

```
+-----------------------+
| "OK"                  |
| WindowName1 window1ID |
| WindowName2 window2ID |
| WindowNameN windowNID |
| ...                   |
+-----------------------+
```

In the second case (`"OK"`), the content's end is marked using a string
terminator `0x00`.

Window IDs are numbers (represented as strings) while window names are strings.

### Client reply with a window ID

```
+----------+
| windowID |
+----------+
```

Note that, in case the client chooses to see the whole desktop instead of a
single window, `windowID` is 0.

### Server's window frame

Lossless frame compression is one of the key features of this protocol, since it
provides a fast way to send big amounts of data. To make it even faster, it does
not compress the whole image but instead it implements an algorithm to find the
parts of that image that has changed.

```
size (bytes)  0   4   8      12       16               20                 n
              +---+---+-------+--------+----------------+-----------------+
              | x | y | width | height | compWindowSize | compWindowFrame |
              +---+---+-------+--------+----------------+-----------------+
```

The fields `x`, `y`, `width`, `height` and `compWindowSize` are stored as
big-endian unsigned integers. The `compWindowSize` field represents the size in
bytes of the `compWindowFrame` field. The `compWindowFrame` field represents
the content of the compressed window frame.

### Client's input signal

```
size (bytes)  0        2
              +--------+
              | source |
              +--------+
```

The `source` field represents the source of input (like a key pressure on
keyboard, a mouse click, etc...) as an enumeration. Since this field is 2 byte
long, it is able to store up to 65535 possible sources.
