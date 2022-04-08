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

[TODO]

```
C      S
|      |
+--->>-+ (1) The client sends a connection request to the server.
|      |
+-<<---+ (2) The server replies with a list of available windows or an error
|      |     message and changes the connection from TCP to UDP.
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

Note that, if the server replies with an error message in step `(2)`, the server
will close the connection.

## Steps

[TODO]

<!--
The following steps are meant to examine in depth the outline shown in the
overview. The number of each step corresponds with the number in the outline.

The exact structure of the packets can be found in
[Structure of packets](#structure-of-packets).

 1. When the client sends a connection request to the server, an encrypted
    TCP connection is established between the two (see
    [Authentication and encryption](#authentication-and-encryption)) and the
    client sends the authentication packet.

 2. The server replies with a boolean value. If it is true, the authentication
    has successfully terminated and the server also sends a list of windows
    currently open in the computer by specifying their name and ID. Otherwise,
    in case the server replies with false, an error message is sent and the
    connection will be closed by the server itself. An additional token is sent
    to keep the client logged in while the connection changes from TCP to UDP.

 3. The client now has to choose a window (or the whole desktop) by its ID and
    specify the video quality. If the window ID does not exist, the server
    closes the connection. The client also sends the token received from the
    server.

 4. The server sends the current window frame.

 5. The client sends an input signal such as a mouse click or a key press on the
    keyboard.

 6. The server emulates the input signal on the window and sends the updated
    window frame.

Steps `(5)` and `(6)` repeat for each input signal and each window frame.

Eventually, the client closes the connection.
-->

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

## Authentication and encryption

The protocol makes use of public key for both authentication and cryptography.

## Structure of packets

Below you can find visual/structural representations of the packets sent between
the client and the server.

The packets have the following structure:

```
0   1   3    n+3 n+5    n+m+5
+---+---+-----+---+-------+
| c | n | key | m | value |
+---+---+-----+---+-------+
```

where:

 - `c` is a boolean (integer of 8 bits) value that is `0` if the packet does not
   need confirmation of being received, any other value means the opposite
 - `key` is a string representing the meaning of the value
 - `n` is the length of the key in bytes
 - `value` is the value associated with the key
 - `m` is the length of the value in bytes

The data type of the `value` field must be known by both the client and the
server so that is not misinterpreted.

Both the client and the server can request a packet by sending that same packet
but with `m` = 0, so that there is no value.

## Keys

All the strings use the UTF-8 encoding.

<table>
   <tr>
      <th> name </th>
      <th> data type </th>
      <th> description </th>
   </tr>
   <tr>
      <td> version </td>
      <td> string </td>
      <td> the protocol version used by the client </td>
   </tr>
   <tr>
      <td> pubkey </td>
      <td> string </td>
      <td> the client's public key </td>
   </tr>
   <tr>
      <td> error </td>
      <td> string </td>
      <td> an error message explaining what went wrong  </td>
   </tr>
   <tr>
      <td> windows </td>
      <td> string </td>
      <td>
         the server's available windows together with their IDs, the pairs of
         window name and ID are separated by commas
      </td>
   </tr>
   <tr>
      <td> winid </td>
      <td> integer </td>
      <td> the window ID chosen by the client, 0 for the whole desktop </td>
   </tr>
   <tr>
      <td> winsize </td>
      <td> integer </td>
      <td> the window size (width and height) </td>
   </tr>
   <tr>
      <td> quality </td>
      <td> integer </td>
      <td>
         the quality of the server's frames, from 0 (lowest) to 5 (highest)
      </td>
   </tr>
</table>

<!--
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
      <td> The server's hashed password (using SHA3-256) </td>
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
   "token": (string),
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
      <td> token </td>
      <td>
         A randomized token, used to keep the client logged in while the
         connection changes from TCP to UDP
      </td>
      <td> Can be any string of any length </td>
   </tr>
   <tr>
      <td> windows </td>
      <td>
         Contains an array of currently open windows with their name and ID
         (each ID is unique), used if ok = true
      </td>
      <td> Strings for names, integers for IDs; if ok = false, the array is
         empty </td>
   </tr>
</table>

The entire desktop has ID = 0. Every other window has ID > 0 and IDs are unique
for every window.

### Client reply with a window ID

```
{
   "id": (integer),
   "quality": (string),
   "token": (string)
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
   <tr>
      <td> quality </td>
      <td> The video quality </td>
      <td> Can be any 
         <a href="https://trac.ffmpeg.org/wiki/Encode/H.264#Preset">
            H.264 preset
         </a>
      </td>
   </tr>
   <tr>
      <td> token </td>
      <td> The server's token </td>
      <td> Can be any string, but it must match the server's given token </td>
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

The server's window frame is compressed through
[H.264](https://en.wikipedia.org/wiki/Advanced_Video_Coding). 

### Client's input signal

```
{
   "source": (integer),
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
      <td> mposx </td>
      <td> The mouse position on the X-axis </td>
      <td> Integer with value greater or equal to -1 </td>
   </tr>
   <tr>
      <td> mposy </td>
      <td> The mouse position on the Y-axis </td>
      <td> Integer with value greater or equal to -1 </td>
   </tr>
   <tr>
      <td> keys </td>
      <td> An array of currently pressed keys represented as strings </td>
      <td> An array of strings </td>
   </tr>
</table>

If `mposx` and `mposy` have value -1, it means that the mouse position has not
changed. This special case is mostly used when input is from keyboard.

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
   (left click), `mc` (middle click), `rc` (right click).

The scroll wheel is sent as a key and represented as
`scroll:<x-offset>:<y-offset>` where `x-offset` and `y-offset` are respectively
the scroll offsets on the X-axis and Y-axis. If the wheel is scrolled following
the X-axis, the representation is `scroll:<value>:` while, if the wheel is
scrolled following the Y-axis, is `scroll::<value>`.

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
-->