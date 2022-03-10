# scrtp

Screen teleport - low latency remote screen control.

⚠ This repo is still under heavy development. A release will be available in the
Releases section on GitHub when both the client and the server will be fully
functioning.

![fake parsec logo](img/fake_parsec_logo.png)

⚠ The image above was entirely made for fun. Please Parsec, don't sue me.

## Compiling

To compile, you need:

 - **Git** to clone the repo, otherwise you can download it through
[this link](https://github.com/EdoardoLaGreca/scrtp/archive/refs/heads/main.zip)
 - **Go** ([here](https://go.dev/doc/install))
 - the **GLFW dependencies**
([here](https://github.com/go-gl/glfw#installation))

The client and the server do not have a strict dependency on each other, you can
build one or the other independently. However, since they use a client-server
architecture, you need a client to connect to the server and you need a server
to share one or more windows to clients.

### Client

```sh
git clone https://github.com/EdoardoLaGreca/scrtp
cd scrtp/client
go get -d
go install github.com/shamaton/msgpackgen # maybe not required after `go get -d`
go generate
go build .
```

If you see this error (or a similar one) after `go generate`

```
net.go:3: running "msgpackgen": exec: "msgpackgen": executable file not found in $PATH
```

make sure that your `PATH` environment variable also contains
`/home/<user>/go/bin`. In case it doesn't, add it and run the command again.

### Server

```sh
git clone https://github.com/EdoardoLaGreca/scrtp
cd scrtp/server
go get -d
go build .
```

## Development

I chose to use Go to write both client and server because of the ease with which
the problem of concurrency can be solved.

Using [Conventional Commits](https://www.conventionalcommits.org) for commit
messages is preferable but not strictly required.

### Implemented features

- [x] = implemented, tested and working
- [ ] = not implemented, not tested or not working

#### Client

 - [x] Keyboard handling
 - [x] Mouse position handling
 - [ ] Mouse click handling
 - [ ] Networking
 - [ ] Video decompression
 - [ ] Window resize

#### Server

 - [ ] Input replication on real window
 - [ ] Window buffer capturing (Windows and Linux)
 - [ ] Networking
 - [ ] Video compression

## License

Scrtp is licensed under the zlib License
([Wikipedia](https://en.wikipedia.org/wiki/Zlib_License), [LICENSE](LICENSE)).
