# protocol

This file defines the protocol that is being used to exchange messages between
the client and the server.

## Overview

Scrtp uses a client-server model. The computer which sends its own screen
frames represents the server, while external computers which receive the screen
frames and send peripheral signals (like a mouse click or a key press on the
keyboard) act as clients.

## Steps

Keep in mind that, whenever an error occur, either on the client side or the
server side, and that error is so bad that the communication cannot continue,
the side where the error occurred should send an `error` packet with a brief
description of what happened.

The first packet that the client must send is the public key for authenticating
into the server.

[TODO]

## Concurrency

A sequential approach would be ideal if the window did not update (unless on
user input) and had no animations. In a modern world, it is not conceivable to
use a sequential approach hoping that the final user will not need window
updates in-between input signals.

To deal with this issue, Scrtp take advantage of concurrency. The frames sent by
the server are concurrently independent of the input signals sent by the client,
as if they were sent in two different channels.

```
frames:         S->->->->->->-C
input signals:  S-<-<-<-<-<-<-C
```

## Authentication and encryption

The protocol makes use of public key for both authentication and cryptography.

## Compression

Scrtp uses [H.264](https://en.wikipedia.org/wiki/Advanced_Video_Coding) for
video compression with `zerolatency` tune. See
[here](https://trac.ffmpeg.org/wiki/Encode/H.264#Preset)
for a complete list of available H.264 presets.

<table>
   <tr>
      <th> value of quality </th>
      <th> quality </th>
      <th> H.264 preset </th>
   </tr>
   <tr>
      <td> 1 </td>
      <td> lowest </td>
      <td> ultrafast </td>
   </tr>
   <tr>
      <td> 2 </td>
      <td> low </td>
      <td> veryfast </td>
   </tr>
   <tr>
      <td> 3 </td>
      <td> medium </td>
      <td> medium </td>
   </tr>
   <tr>
      <td> 4 </td>
      <td> high </td>
      <td> slower </td>
   </tr>
   <tr>
      <td> 5 </td>
      <td> highest </td>
      <td> veryslow </td>
   </tr>
</table>

## Structure of packets

Below you can find visual/structural representations of the packets sent between
the client and the server.

The packets have the following structure:

```
0   1   3    n+3 n+5    n+m+5 (bytes)
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
      <td> an error message explaining what went wrong </td>
   </tr>
   <tr>
      <td> wins </td>
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
         the quality of the server's frames, from 1 (lowest) to 5 (highest)
      </td>
   </tr>
   <tr>
      <td> compfr </td>
      <td> array of bytes (variable size) </td>
      <td> the compressed window frame </td>
   </tr>
   <tr>
      <td> kbdev </td>
      <td> array of 2 integers </td>
      <td>
         a keyboard input event represented as 2 integers of 8 bits:
         <ul>
            <li>
               the first for the state of the key (1 for "pressed" and 0 for
               "released")
            </li>
            <li> the second for the key code </li>
         </ul>
      </td>
   </tr>
   <tr>
      <td> msclk </td>
      <td> array of 2 integers </td>
      <td>
         a mouse click event represented as 2 integers of 8 bits:
         <ul>
            <li>
               the first for the state of the button (1 for "pressed" and 0 for
               "released")
            </li>
            <li>
               the second for the button code (0 for "left", 1 for "right" and 2
               for "middle")
            </li>
         </ul>
      </td>
   </tr>
   <tr>
      <td> msmv </td>
      <td> array of 2 integers </td>
      <td>
         a mouse move event represented as 2 integers of 8 bits:
         <ul>
            <li> the first for the new mouse position on the X-axis </li>
            <li> the second for the new mouse position on the Y-axis </li>
         </ul>
      </td>
   </tr>
   <tr>
      <td> msscr </td>
      <td> array of 2 doubles </td>
      <td>
         a mouse scroll event represented as 2 doubles:
         <ul>
            <li> the first for the scroll offset on the X-axis </li>
            <li> the second for the scroll offset on the Y-axis </li>
         </ul>
      </td>
   </tr>
</table>
