# protocol

This file defines the protocol that is being used to exchange messages between
the client and the server.

## Overview

As you may have noticed from the repo structure, Scrtp uses a client-server
model. The computer which sends its own screen frames represents the server,
while external computers which receive the screen frames and send peripheral
signals (like a mouse click or a key press on the keyboard) act as clients.

The following outline describes an overview of the messages sent. The client is
represented using the letter `C` while the server is represented using the
letter `S`. Symbols like `>>` and `<<` are used to specify the message
direction.

The following steps require the client and the server to share a common AES key,
which will be used for encryption during the whole connection, from the first
packet to the last one. More about it on [Encryption](#Encryption).

```
C      S
|      |
+--->>-+ (1) The client sends a connection request to the server with some info.
|      |
+-<<---+ (2) The server replies with "OK" and a list of available windows or
|      |     "NO" and an error message.
|      |
+--->>-+ (3) The client chooses a window.
|      |
+-<<---+ (4) The server sends the current window frame.
|      |
+--->>-+ (5) The client sends an input signal.
|      |
+-<<---+ (6) The server sends another frame.
|      |
+--->>-+ (7) The client sends another input signal.
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
    is established between the two. Then, the client sends its key (see
    [Authentication](#Authentication)), its MAC address, its protocol version
    and the desired image quality.

 2. The server replies with a 2 characters string which can be either `OK` or
    `NO`. If it is `OK`, the authentication has successfully terminated and the
    server also sends a list of windows currently open in the computer,
    separated by a newline character. Otherwise, in case the server replied with
    `NO`, an error message is sent and the connection will be closed by the
    server itself.

 3. The client now has to choose a window (or the whole desktop) and send the
    chosen window's ID to the server. In case it's the whole desktop, the window
    ID is equal to 0.

 4. The server sends the current frame of the window.

 5. The client sends an input signal such as a mouse click or a key press on the
    keyboard.

 6. The server emulates the input signal on the window and sends the updated
    window frame.

Steps `(5)` and `(6)` repeat for each input signal and each window frame.

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

## Authentication

The authentication in Scrtp is a really simple process: it just requires an AES
key. That key is personal and must be unique for each combination of user and
machine (see [files#AES-key](files.md#AES-key)).

The length of the AES key is 128 bit.

## Encryption

All the packets are encrypted using an AES key, the same used for
authentication.

## Structure of packets

Below there are visual representations of the packets sent by the client and by
the server.

Whenever the packet's fields are placed in different lines or there is no size
specified for them, it means that they are separated by newlines and their end
is marked using the string terminator `\0`.

Note that constant strings, which never change for a certain kind of packet, are
double-quoted. Every non-quoted space-separated word works as a field whose
content is not constant.

Also notice that, since some kinds of packets will be sent continuously, it is
essential to optimize space. Therefore, the representation of these kinds of
packets also has information about the size of single fields.

### Authentication packet

```
+------------+
| AESkey     |
| MACaddr    |
| protoVer   |
| imgQuality |
+------------+
```

### Server reply to authentication

```
+-------+
| "NO"  |
| issue |
+-------+
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

In the first case (`"NO"`), `issue` contains the error message.

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
size (bytes)  0   4   8      12       16                 n
              +---+---+-------+--------+-----------------+
              | x | y | width | height | compWindowFrame |
              +---+---+-------+--------+-----------------+
```

The fields `x`, `y`, `width` and `height` are stored as big-endian unsigned
integers. The `compWindowFrame` field represents the content of the compressed
window frame. The compression algorithm used to compress the window frame is
[Zstandard](https://en.wikipedia.org/wiki/Zstandard).

### Client's input signal

```
size (bytes)  0      1        5        9
              +------+--------+--------+
              | type | value1 | value2 |
              +------+--------+--------+
```

The `type` field represents the type (or "source") of input (e.g. keyboard,
mouse click, mouse movement, etc...) as an enumeration stored in a 8-bit-long
integer.

The fields `value1` and `value2` represent the values of the input source (e.g.
which keyboard key has been pressed, which position the mouse was in when one of
its buttons has been clicked, etc...) as enumerations stored in 32-bit-long
integers.

The meaning of the fields `value1` and `value2` differ basing on the value of
`type`.

#### Input signal enumeration

The following table maps the possible enumeration values in the `type` field.
The table also contains an explaination of the meaning of the fields `value1`
and `value2` based on the value of `type`.

<table>
   <tr>
      <th> type </th>
      <th> meaning </th>
      <th> meaning of value1 and value2 </th>
   </tr>
   <tr>
      <td> 0 </td>
      <td> keyboard key press </td>
      <td rowspan="2">
         The field value1 is used to represent a UTF-8 character. In case there
         is a modifier key (any key that does not provide any UTF-8 character by
         itself, except for SHIFT) among the pressed keys, it is going to be
         identified as a combination of keys and each byte of value1 and value2
         will store a keyboard key, identified by an enumeration (see below).
      </td>
   </tr>
   <tr>
      <td> 1 </td>
      <td> keyboard key release </td>
   </tr>
   <tr>
      <td> 2 </td>
      <td> mouse left click </td>
      <td rowspan="3">
         The fields value1 and value2 represent respectively the x-axis and
         y-axis mouse coordinates.
      </td>
   </tr>
   <tr>
      <td> 3 </td>
      <td> mouse middle click </td>
   </tr>
   <tr>
      <td> 4 </td>
      <td> mouse right click </td>
   </tr>
</table>

The following table maps the possible enumeration values with keyboard keys and
it is used in case the pressed keys contain a modifier key. The missing keys are
mapped in the same way as they are in the ASCII standard (e.g. alphanumeric
characters).

<table>
   <tr>
      <th> value </th>
      <th> key </th>
   </tr>
   <tr>
      <td> 0 </td>
      <td> SHIFT </td>
   </tr>
   <tr>
      <td> 1 </td>
      <td> CTRL (Control) </td>
   </tr>
   <tr>
      <td> 2 </td>
      <td> ALT </td>
   </tr>
   <tr>
      <td> 3 </td>
      <td> SUPER </td>
   </tr>
   [TODO]
</table>