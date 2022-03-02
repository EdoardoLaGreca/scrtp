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

 2. The server replies with a boolean value. If it is true, the authentication
    has successfully terminated and the server also sends a list of windows
    currently open in the computer, separated by a newline character. Otherwise,
    in case the server replied with false, an error message is sent and the
    connection will be closed by the server itself.

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

Whenever the number of items in an array may vary, it is written as "...". Also,
the values are replaced with their type surrounded by parentheses.

### Authentication packet

```
{
   "hpw": (string),
   "ver": (string)
}
```

<table>
   <tr>
      <th> Field name </th>
      <th> Description </th>
      <th> Possible values </th>
   </tr>
      <td> hpw </td>
      <td> The server's hashed password (using
         <a href="https://en.wikipedia.org/wiki/SHA-3">SHA3-256</a>) </td>
      <td> Any string that fits the hash function's output size (256 bits = 32
         bytes) </td>
   <tr>
   </tr>
   <tr>
      <td> ver </td>
      <td> The client's protocol version </td>
      <td> Any valid protocol version as a string </td>
   </tr>
</table>

### Server reply to authentication

```
{
   "ok": (boolean),
   "issue": (string),
   "windows": [
      {
         "name": (string),
         "id": (integer),
      },
      {
         "name": (string),
         "id": (integer),
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
   "id": (integer)
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
   "width": (integer),
   "height": (integer),
   "compfr": [
      (integer), (integer), ...
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

```
{
   "source": (integer),
   "ispress": (boolean),
   "mposx": (integer),
   "mposy": (integer),
   "keys": [
      (string), (string), (string), ...
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
      <td> source </td>
      <td> The enumerated input source </td>
      <td> Integer with value greater or equal to 0 </td>
   </tr>
   <tr>
      <td> ispress </td>
      <td> Has the key/mouse button been pressed or released? true = pressed,
         false = released </td>
      <td> true or false (boolean) </td>
   </tr>
   <tr>
      <td> mposx </td>
      <td> The mouse position on the X-axis </td>
      <td> Integer with value greater than 0 or null </td>
   </tr>
   <tr>
      <td> mposy </td>
      <td> The mouse position on the Y-axis </td>
      <td> Integer with value greater than 0 or null </td>
   </tr>
   <tr>
      <td> keys </td>
      <td> An array of keys, modifier keys start with an underscore </td>
      <td> An array of strings </td>
   </tr>
</table>

<table>
   <tr>
      <th> Value of source </th>
      <th> Meaning </th>
   </tr>
   <tr>
      <td> 0 </td>
      <td> Unknown or unsupported input source </td>
   </tr>
   <tr>
      <td> 1 </td>
      <td> Mouse </td>
   </tr>
   <tr>
      <td> 2 </td>
      <td> Keyboard </td>
   </tr>
</table>

The length of the `keys` array is equal to the number of keys pressed at the
same time. The `keys` array has length 1 if the user just typed a character
while it has length greater than 1 if the user pressed or released a key
combination.

The various types of keys are written into strings as follows:

 - Character keys (tab key included) are usually written as they are. In case of
   alphabetic characters, they are written in the case indicated by the Caps
   Lock key (e.g. `F`, `k`, ...).
 - Function keys are written as `Fx` where `x` is a number between 1 and 12
   inclusive (e.g. `F4`, `F12`, ...).
 - Modifier keys and system command keys are preceded by an underscore (e.g.
   `_C`, `_A`, ...; see below).
 - Mouse buttons (in case `source` refers to mouse) are represented as: `lc`
   (left click), `mc` (middle click), `rc` (right click), `swf` (scroll wheel
   forward) and `swb` (scroll wheel backward).

Below there is a table that shows representations for modifier and system
command keys.

<table>
   <tr>
      <th> Key </th>
      <th> Representation </th>
   </tr>
   <tr>
      <td> Control (Ctrl) </td>
      <td> "_C" </td>
   </tr>
   <tr>
      <td> Alternate (Alt) </td>
      <td> "_A" </td>
   </tr>
   <tr>
      <td> Alternate Graphics (AltGr) </td>
      <td> "_G" </td>
   </tr>
   <tr>
      <td> Shift </td>
      <td> "_S" </td>
   </tr>
   <tr>
      <td> Super/Windows/Command </td>
      <td> "_X" </td>
   </tr>
   <tr>
      <td> Escape (Esc) </td>
      <td> "_E" </td>
   </tr>
   <tr>
      <td> Enter/Return </td>
      <td> "_N" </td>
   </tr>
   <tr>
      <td> Delete (Del) </td>
      <td> "_D" </td>
   </tr>
   <tr>
      <td> Insert (Ins) </td>
      <td> "_I" </td>
   </tr>
   <tr>
      <td> Print screen (Prt Sc) </td>
      <td> "_P" </td>
   </tr>
   <tr>
      <td> Backspace </td>
      <td> "_B" </td>
   </tr>
   <tr>
      <td> Left arrow </td>
      <td> "_<" </td>
   </tr>
   <tr>
      <td> Right arrow </td>
      <td> "_>" </td>
   </tr>
   <tr>
      <td> Up arrow </td>
      <td> "_^" </td>
   </tr>
   <tr>
      <td> Down arrow </td>
      <td> "_V" </td>
   </tr>
</table>
