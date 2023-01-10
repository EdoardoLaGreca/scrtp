# scrtp

Screen teleport - low latency remote desktop control.

⚠ This repo is still under heavy development. A 1.0 release will be available in the Releases section on GitHub when both the client and the server will be fully functioning.

[click here for a very funny image, SFW](https://raw.githubusercontent.com/EdoardoLaGreca/scrtp/main/img/fake_parsec_logo.png)

⚠ The image above was entirely made for fun. Please Parsec, don't sue me.

## Usage

### Default values for options

The default values can be set through environment variables.

 - `SCRTP_IPVER` can be set to either `4` or `6`, it represents the default IP version.

Option values specified as command arguments override default values, but their effect will last for the entire program execution only.

### Client

Usage: `scrtp [options ...] host port`

where:

 - `host` and `port` refer to the server's host and port, respectively
 - `options` can be one or more of:
   - `--ipv N` Prefer one IP version over the other. `N` can either be 4 or 6. Setting this option to 4 or 6 means that the `host` field should follow that IP version syntax.

### Server

Usage: `scrtpd [options ...] host port`

where:

 - `host` and `port` refer to the host and port to listen to for new connections
 - `options` can be one or more of:
   - `--ipv N` Prefer one IP version over the other. `N` can either be 4 or 6. Setting this option to 4 or 6 means that the `host` field should follow that IP version syntax.

## Compiling source files

All commands and relative paths below presume that your current working directory is the repo root directory.

Before proceeding, install `make` (any POSIX-compliant `make`, in case you have multiple choices), which is the build system in use in this repo.

### Requirements

Please keep in mind that I'm a human being and I may forget to update written things such as compilation requirements.

Before compiling, make sure you have all the required dependencies (both compile-time and runtime) and `make` installed in your system. Otherwise, it will probably fail.

#### Client

**Compile-time dependencies**: (none)

**Runtime dependencies**: (see shared/reqbins and client/reqbins)

To compile, run:

```
make client
```

#### Server

**Compile-time dependencies**: (none)

**Runtime dependencies**: (see shared/reqbins and server/reqbins)

To compile, run:

```
make server
```

#### Both

To compile both the client *and* the server, run:

```
make all
```

## Development

If you'd like to contribute, please make sure to carefully read the [doc/style.md](doc/style.md) file before.

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
 - [ ] Audio handling (encoding and decoding, see [Opus](https://opus-codec.org/))

## License

Scrtp is licensed under the zlib License ([Wikipedia](https://en.wikipedia.org/wiki/Zlib_License), [LICENSE](LICENSE)).
