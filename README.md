# scrtp

Screen teleport - low latency remote screen control.

⚠ This repo is still under heavy development. A release will be available in the
Releases section on GitHub when both the client and the server will be fully
functioning.

![fake parsec logo](img/fake_parsec_logo.png)

⚠ The image above was entirely made for fun. Please Parsec, don't sue me.

## Compiling

The client and the server do not have a strict dependency on each other, you can
build one or the other independently. However, since they use a client-server
architecture, you need a client to connect to the server and you need a server
to share one or more windows to clients.

[TODO]

Please keep in mind that I'm a human being and I may forget to update written
things such as compilation requirements.

### Client

To compile, you need:

 - **Git** to clone the repo, otherwise you can download it through
[this link](https://github.com/EdoardoLaGreca/scrtp/archive/refs/heads/main.zip)

[TODO]

### Server

To compile, you need:

 - **Git** to clone the repo, otherwise you can download it through
[this link](https://github.com/EdoardoLaGreca/scrtp/archive/refs/heads/main.zip)

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

 - [ ] Window creation
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

## License

Scrtp is licensed under the zlib License
([Wikipedia](https://en.wikipedia.org/wiki/Zlib_License), [LICENSE](LICENSE)).
