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

You do not need the client to run the server and vice versa.

### Client

```sh
git clone https://github.com/EdoardoLaGreca/scrtp
cd scrtp/client
go get -d
go build .
```

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

#### Client

#### Server

## License

Scrtp is licensed under the zlib License
([Wikipedia](https://en.wikipedia.org/wiki/Zlib_License), [LICENSE](LICENSE)).
