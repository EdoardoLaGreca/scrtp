# protocol

This file defines the protocol that is being used to exchange messages between
the client and the server.

## Overview

Scrtp uses a client-server model. The computer which sends its own screen
frames represents the server, while external computers which receive the screen
frames and send peripheral signals (like a mouse click or a key press on the
keyboard) act as clients.

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
    chosen window ID to the server. In case it's the whole desktop, the window
    ID is 0.

 4. The server sends the current window frame.

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

Below you can find visual/structural representations of the packets sent between
the client and the server.

The representations are made using the JSON format. However, the protocol does
not use the JSON format. Instead, it uses [MessagePack](https://msgpack.org/),
which is more lightweight and therefore more suitable.

Whenever the field's value or the number of items in an array may vary, it is
written as `...`.

### Authentication packet

```
{
   "hpw": "...",
   "ver": "..."
}
```

<table>
   <tr>
      <th> Field name </th>
      <th> Description </th>
      <th> Possible values </th>
   </tr>
      <td> hpw </td>
      <td> The server's hashed password (using ?) </td>
      <td> Any string that fits the hash function's output size </td>
   <tr>
   </tr>
   <tr>
      <td> ver </td>
      <td> The client's protocol version </td>
      <td> Any valid protocol version </td>
   </tr>
</table>

### Server reply to authentication

```
{
   "ok": ...,
   "issue": "...",
   "windows": [
      {
         "name": "...",
         "id": ...,
      },
      {
         "name": "...",
         "id": ...,
      },
      ...
   ]
}
```

<table>
   <tr>
      <th> Field name </th>
      <th> Description </th>
      <th> Possible values </th>
   </tr>
   <tr>
      <td> ok </td>
      <td> Can the client connect to the server? true = yes, false = no </td>
      <td> true or false (boolean) </td>
   </tr>
   <tr>
      <td> issue </td>
      <td> Information about the issue, used if ok = false </td>
      <td> Can either be a string (ok = false) or null (ok = true) </td>
   </tr>
   <tr>
      <td> windows </td>
      <td> Contains an array of currently open windows with their name and ID
         (each ID is unique), used if ok = true </td>
      <td> Strings for names, integers for IDs; if ok = false, the array is
         empty </td>
   </tr>
</table>

The entire desktop has ID = 0. Every other window has ID > 0 and IDs are unique
for every window.

### Client reply with a window ID

```
{
   "id": ...
}
```

<table>
   <tr>
      <th> Field name </th>
      <th> Description </th>
      <th> Possible values </th>
   </tr>
   <tr>
      <td> id </td>
      <td> The chosen window's ID </td>
      <td> Integer with value greater or equal to 0 </td>
   </tr>
</table>

### Server's window frame

```
{
   "width": ...,
   "height": ...,
   "compfr": [
      ...
   ]
}
```

<table>
   <tr>
      <th> Field name </th>
      <th> Description </th>
      <th> Possible values </th>
   </tr>
   <tr>
      <td> width </td>
      <td> The frame width </td>
      <td> Integer with value greater than 0 </td>
   </tr>
   <tr>
      <td> height </td>
      <td> The frame height </td>
      <td> Integer with value greater than 0 </td>
   </tr>
    <tr>
      <td> compfr </td>
      <td> The compressed and encoded frame content </td>
      <td> An array of bytes (as integers) </td>
   </tr>
</table>

The server's window frame is compressed through [H.264](https://en.wikipedia.org/wiki/Advanced_Video_Coding). 

### Client's input signal

[TODO]

```
{
   "source": ...,
   "mposx": ...,
   "mposy": ...,
   "iscomb": ...,
   "character": "...",
   "key1": ...,
   "key2": ...
}
```

<table>
   <tr>
      <th> Field name </th>
      <th> Description </th>
      <th> Possible values </th>
   </tr>
   <tr>
      <td> source </td>
      <td> The enumerated input source </td>
      <td> Integer with value greater or equal to 0 </td>
   </tr>
   <tr>
      <td> mposx </td>
      <td> The mouse position on the X-axis </td>
      <td> Integer with value greater than 0 </td>
   </tr>
   <tr>
      <td> mposy </td>
      <td> The mouse position on the Y-axis </td>
      <td> Integer with value greater than 0 </td>
   </tr>
   <tr>
      <td> iscomb </td>
      <td> Is it a key combination? true = yes, false = no </td>
      <td> true or false (boolean) </td>
   </tr>
   <tr>
      <td> character </td>
      <td>  </td>
      <td>  </td>
   </tr>
   <tr>
      <td> key1 </td>
      <td>  </td>
      <td>  </td>
   </tr>
   <tr>
      <td> key2 </td>
      <td>  </td>
      <td>  </td>
   </tr>
</table>

<!--

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
characters). The value 0 is reserved to mark the end of the combination.

<table>
   <tr>
      <th> value </th>
      <th> key </th>
   </tr>
   <tr>
      <td> 1 </td>
      <td> SHIFT </td>
   </tr>
   <tr>
      <td> 2 </td>
      <td> CTRL (Control) </td>
   </tr>
   <tr>
      <td> 3 </td>
      <td> ALT </td>
   </tr>
   <tr>
      <td> 4 </td>
      <td> SUPER </td>
   </tr>
   [TODO]
</table>

-->
