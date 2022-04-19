# scrtp

Screen teleport - low latency remote screen control.

⚠ This repo is still under heavy development. A release will be available in the
Releases section on GitHub when both the client and the server will be fully
functioning.

![fake parsec logo](img/fake_parsec_logo.png)

⚠ The image above was entirely made for fun. Please Parsec, don't sue me.

## Usage

The default values, if any, used by the client and the server can be found in
the repo's [etc](etc/) directory as config files.

### Client

```
scrtp [OPTIONS]... HOST PORT

OPTIONS:
    -v  Print verbose output (useful during debugging).
```

### Server

```
scrtpd [OPTIONS]...

OPTIONS:
    -v  Print verbose output (useful during debugging).
    -c FILE
        Use FILE as configuration file instead of the default file located in
        /etc/scrtp/config_server. If the default file cannot be found, use
        default values.
```

## Requirements

The client and the server do not have a strict dependency on each other, you can
build one or the other independently. However, since they use a client-server
architecture, you need a client to connect to the server and you need a server
to let other clients connect.

Please keep in mind that I'm a human being and I may forget to update written
things such as compilation requirements.

### Client

Compile-time dependencies:
 - `glfw-devel`

Runtime dependencies:
 - `glfw`

[TODO]

### Server

[TODO]

## Development

I chose to use Go to write both client and server because of the ease with which
the problem of concurrency can be solved.

Using [Conventional Commits](https://www.conventionalcommits.org) for commit
messages is preferable but not strictly required.

### Roadmap

During the development, this roadmap may be subject to changes.

- [x] = implemented, tested and working
- [ ] = not implemented, not tested or not working

#### Client

 - [x] Window creation
 - [ ] Keyboard handling
 - [ ] Mouse click handling
 - [ ] Mouse position handling
 - [ ] Mouse scroll handling
 - [ ] Networking
 - [ ] Video decompression
 - [ ] Window resize

#### Server

 - [ ] Window buffer capturing (Windows and Linux)
 - [ ] Input replication on real window
 - [ ] Networking
 - [ ] Video compression
 - [ ] Window resize


#### Optional

 - [ ] Support for more input methods (gamepad, etc...)
 - [ ] Performance improvements by optimizing the protocol
 - [ ] Remove runtime dependencies (see
       [statifier](http://statifier.sourceforge.net/) and
       [Ermine](http://www.magicermine.com/))

## License

Scrtp is licensed under the zlib License
([Wikipedia](https://en.wikipedia.org/wiki/Zlib_License), [LICENSE](LICENSE)).
