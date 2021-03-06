# scrtp

Screen teleport - low latency remote screen control.

⚠ The development is at a standstill due to missing documentation on the SVT-AV1 library, more info [here](https://gitlab.com/AOMediaCodec/SVT-AV1/-/issues/1941).

⚠ This repo is still under heavy development. A release will be available in the
Releases section on GitHub when both the client and the server will be fully
functioning.

[click here for a very funny image, SFW](https://raw.githubusercontent.com/EdoardoLaGreca/scrtp/main/img/fake_parsec_logo.png)

⚠ The image above was entirely made for fun. Please Parsec, don't sue me.

## Usage

The default values, if any, used by the client and the server can be found in
the repo's [etc](etc/) directory as config files.

### Client

```
scrtp [OPTIONS]... HOST PORT

OPTIONS:
    -v       Print verbose output (useful during debugging).
    -6       Use IPv6 if possible.
```

### Server

```
scrtpd [OPTIONS]...

OPTIONS:
    -v       Print verbose output (useful during debugging).
    --4and6  Use IPv4 and IPv6 (default is only IPv4).
    --only6  Use only IPv6 (default is only IPv4).
    -c FILE  Use FILE as configuration file instead of the default file located
             in /etc/scrtp/config_server. If the default file cannot be found,
             use default values.
```

## Requirements

The client and the server do not have a strict dependency on each other, you can
build one or the other independently. However, since they use a client-server
architecture, you need a client to connect to the server and you need a server
to let other clients connect.

Please keep in mind that I'm a human being and I may forget to update written
things such as compilation requirements.

### Client

![](https://img.shields.io/github/workflow/status/EdoardoLaGreca/scrtp/client-scrtp)

Compile-time dependencies:
 - `libglfw3-dev` (a.k.a. `glfw-devel`)

Runtime dependencies:
 - `libglfw3` (a.k.a. `glfw`)

[TODO]

### Server

[TODO]

## Development

If you'd like to contribute, please make sure to carefully read the
[doc/style.md](doc/style.md) file before. Also, start commit messages with a
lowercase letter.

Using [Conventional Commits](https://www.conventionalcommits.org) for commit
messages is preferable but not strictly required.

### Roadmap

During the development, this roadmap may be subject to changes.

- [x] = implemented, tested and working
- [ ] = not implemented, not tested or not working

#### Client

 - [x] Window creation
 - [x] Keyboard handling
 - [x] Mouse click handling
 - [x] Mouse position handling
 - [x] Mouse scroll handling
 - [ ] Networking
 - [ ] Video decoding
 - [ ] Cryptography
 - [ ] Window resize
 - [ ] IPv6

#### Server

 - [ ] Window buffer capturing (Windows and Linux)
 - [ ] Input replication on real window
 - [ ] Networking
 - [ ] Video encoding
 - [ ] Cryptography
 - [ ] Window resize
 - [ ] IPv6

#### Optional

 - [ ] Support for more input methods (gamepad, etc...)
 - [ ] Performance improvements by optimizing the protocol
 - [ ] Remove runtime dependencies (see [Ermine](http://www.magicermine.com/))
 - [ ] Audio handling (encoding and decoding, see
       [Opus](https://opus-codec.org/))
 - [ ] Add a GitHub Action workflow for memory checks using
       [Valgrind](https://valgrind.org/)

## License

Scrtp is licensed under the zlib License
([Wikipedia](https://en.wikipedia.org/wiki/Zlib_License), [LICENSE](LICENSE)).
