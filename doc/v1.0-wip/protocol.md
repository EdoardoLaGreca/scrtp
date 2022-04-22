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

 1. The server sends its public key to the client as an unencrypted packet.
 2. The client sends its public key to the server as an unencrypted packet.
 3. From now on, the packets will be encrypted using the public keys.
 4. The client sends its protocol version. If it does not match with the one
    used by the server, the server sends back an error.
 5. The server sends a list of windows in the following form
    ```
    window name 1\n
    window name 2\n
    window name 3\n
    ```
    where `\n` represents a newline character.
 6. The client chooses a window by its line number (the first has line number 1
    while the entire desktop has number 0). If that line number does not exist
    (it is negative or too high), the server sends back an error.
 7. The server sends the window size.
 8. The client sends the desired video quality. If it is below 1 or above 5, the
    server sends back an error.
 9. Now the client and the server are ready to go: the server can begin to send
    the window frames while the client can begin to send the input signals.

## Concurrency

A sequential approach would be ideal if the window did not update (unless on
user input) and had no animations. In a modern world, it is not conceivable to
use a sequential approach hoping that the final user will not need window
updates in-between input signals.

To deal with this issue, Scrtp takes advantage of concurrency. The frames sent
by the server are concurrently independent of the input signals sent by the
client, as if they were sent in two different channels.

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

All packets use UDP for both ease of use and speed.

The packets have the following structure:

```
0       1   3    n+3 n+5    n+m+5 (bytes)
+-------+---+-----+---+-------+
| flags | n | key | m | value |
+-------+---+-----+---+-------+
```

where:

 - `flags` is a byte in which every bit is a flag
 - `key` is a string representing the meaning of the value
 - `n` is the length of the key in bytes
 - `value` is the value associated with the key
 - `m` is the length of the value in bytes

The data type of the `value` field is implicit, which means that it must be
known to both the client and the server so that is not misinterpreted.

## Flags

As a reference, the representation below describes the bit positions in the
`flags` field.

```
+---+---+---+---+---+---+---+---+
| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
+---+---+---+---+---+---+---+---+
```

<table>
   <tr>
      <th> position </th>
      <th> meaning </th>
   </tr>
   <tr>
      <td> 1 </td>
      <td> (unused) </td>
   </tr>
   <tr>
      <td> 2 </td>
      <td> (unused) </td>
   </tr>
   <tr>
      <td> 3 </td>
      <td> (unused) </td>
   </tr>
   <tr>
      <td> 4 </td>
      <td> (unused) </td>
   </tr>
   <tr>
      <td> 5 </td>
      <td> (unused) </td>
   </tr>
   <tr>
      <td> 6 </td>
      <td> (unused) </td>
   </tr>
   <tr>
      <td> 7 </td>
      <td> (unused) </td>
   </tr>
   <tr>
      <td> 8 </td>
      <td>
         acknowledgement required (1 = yes, 0 = no)
      </td>
   </tr>
</table>

### Acknowledgement

If the acknowledgement is required, the receiver must send a packet in which:

 - the acknowledgement flag is set to 0
 - the key is `ack`
 - the value is the key of the packet to acknowledge

If two packets with the same key must be acknowledged, it is recommended for the
second packet to wait the acknowledgement of the first one to avoid an
acknowledgement mess.

## Keys

All the strings use the UTF-8 encoding.

<table>
   <tr>
      <th> name </th>
      <th> data type </th>
      <th> requires confirmation </th>
      <th> description </th>
   </tr>
   <tr>
      <td> version </td>
      <td> string </td>
      <td> yes </td>
      <td> the protocol version used by the client </td>
   </tr>
   <tr>
      <td> ack </td>
      <td> string </td>
      <td> no </td>
      <td>
         the acknowledgement of the previous packet (see
         <a href="#acknowledgement">Acknowledgement</a>)
      </td>
   </tr>
   <tr>
      <td> pubkey </td>
      <td> string </td>
      <td> yes </td>
      <td> the client's public key </td>
   </tr>
   <tr>
      <td> error </td>
      <td> string </td>
      <td> yes </td>
      <td> an error message explaining what went wrong </td>
   </tr>
   <tr>
      <td> wins </td>
      <td> string </td>
      <td> yes </td>
      <td>
         the server's available windows, the window names are separated by
         newlines
      </td>
   </tr>
   <tr>
      <td> winid </td>
      <td> integer </td>
      <td> yes </td>
      <td> the window ID chosen by the client, 0 for the whole desktop </td>
   </tr>
   <tr>
      <td> winsize </td>
      <td> integer </td>
      <td> yes </td>
      <td> the window size (width and height) </td>
   </tr>
   <tr>
      <td> quality </td>
      <td> integer </td>
      <td> yes </td>
      <td>
         the quality of the server's frames, from 1 (lowest) to 5 (highest)
      </td>
   </tr>
   <tr>
      <td> compfr </td>
      <td> array of bytes (variable size) </td>
      <td> no </td>
      <td> the compressed window frame </td>
   </tr>
   <tr>
      <td> kbdev </td>
      <td> array of 2 integers </td>
      <td> no </td>
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
      <td> no </td>
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
      <td> no </td>
      <td>
         a mouse move event represented as 2 integers of 16 bits:
         <ul>
            <li> the first for the new mouse position on the X-axis </li>
            <li> the second for the new mouse position on the Y-axis </li>
         </ul>
      </td>
   </tr>
   <tr>
      <td> msscr </td>
      <td> array of 2 doubles </td>
      <td> no </td>
      <td>
         a mouse scroll event represented as 2 doubles:
         <ul>
            <li> the first for the scroll offset on the X-axis </li>
            <li> the second for the scroll offset on the Y-axis </li>
         </ul>
      </td>
   </tr>
   <tr>
      <td> end </td>
      <td> boolean </td>
      <td> yes </td>
      <td> a request to end connection </td>
   </tr>
</table>
