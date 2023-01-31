# OpenSSL binaries for Windows

This is a simple pipeline to build them myself, in the cloud, on demand.

## Why?

The OpenSSL project does not supply binaries.  On most Linux
distributions, you can easily install them using the platform's
package manager and you can trust those.  On Windows, it's a bit
tricker.

Several third-party websites provide OpenSSL binaries for Windows and
you can download them for free.  While many of them may be well
intentioned, it only takes one malicious (or hacked) website to cause
you a world of pain, so I wouldn't trust any of them.

## How does it work?

It's quite easy to build OpenSSL on Windows.  The `README.md` and
`NOTES-WINDOWS.md` files in the source code contain clear
instructions.  All you need is:

- Visual Studio (can be used free of charge for open source development);
- Strawberry Perl (free); and
- NASM (also free).

However, I don't like installing rarely used development tools on my
computers, so I prefer doing it via a VM of some kind.

Fortunately, GitHub actions make this really easy!  GitHub provides
runners that already include Visual Studio, Strawberry perl and so on.
The only missing components are:

- [NASM](https://www.nasm.us/); and
- a build script that follows the steps in the OpenSSL documentation.

This repository contains a GitHub actions workflow that downloads NASM
and follows the recommended build process, then uploads the artifacts
as GitHub releases.

## Where can I download the binaries?

You can download them from this project's GitHub releases, but you
really shouldn't trust OpenSSL binaries built by some random person on
the internet.  You can't really review the binaries for tampering.

It's much easier to review the GitHub workflow and trust that instead.
If you do, just fork this repository and then push a Git tag for the
OpenSSL version you wish to use (e.g. `git tag 3.0.7 && git push
origin 3.0.7`).  That way, you'll have a set of binaries you can trust.

## How to I install the binaries?

The easiest way to install them is to use
[7-zip](https://www.7-zip.org/) on the command-line in an admin
command prompt (run as administrator).

For 64-bit binaries, run the following commands:

```
7z x openssl-binaries-x64.zip -o"%ProgramW6432%\" OpenSSL\ -y
7z x openssl-binaries-x64.zip -o"%CommonProgramW6432%\" SSL\ -y
```

For 32-bit binaries, run the following commands:

```
7z x openssl-binaries-x86.zip -o"%ProgramFiles(x86)%\" OpenSSL\ -y
7z x openssl-binaries-x86.zip -o"%CommonProgramFiles(x86)%\" SSL\ -y
```

After these steps, you will have the same files in the same place as
if you just built OpenSSL yourself and run `nmake install`.

## How do I use the installed binaries?

This all depends on what build system you use.  The `test/` folder has
an example that shows how to consume this with [CMake](https://cmake.org/).

CMake's built-in `FindOpenSSL` module is very convenient, but it has a
few shortcomings in my opinion regardless of the DLL or static variant
that you use.  See the `test/` folder for a fully working example.

### Using the DLL variant

This is the default build for OpenSSL and suits Linux systems very
well because it allows your Linux distribution to patch OpenSSL bug
fixes without you having to recompile all your OpenSSL-based
applications.  However, I find it cumbersome to work with on Windows,
were we cannot install OpenSSL DLL in a central location for multiple
applications.

On some machines, you can simply extract the binaries in their default
location (e.g. where `nmake install` would put them) and then add the
OpenSSL binaries onto `PATH`, but this is unreliable because:

1. Multiple applications that use the same `.dll` file name can
   conflict with each other.  Your application could be broken by
   another application that appears before yours in `PATH`, or your
   application could break another that appears after yours in `PATH`.
2. If any application installs the OpenSSL binaries in
   `C:\Windows\System32` (as is the case on GitHub Windows runners),
   it will be impossible for your application to resolve the OpenSSL
   binaries correctly regardless of how you setup `PATH`.

To avoid these issues, I highly recommend copying the `.dll` files and
installing them alongside your application.  The CMake example in the
`test/` folder does this using a post-build step.

### Using the static library variant

Because of the problems descibed above with the DLL variant, it might
be simpler to just link your application with a self-contained OpenSSL
static library.

When linking your application, you'll also need to link against a few
Windows libraries such as `crypt32` and `ws2_32`.  The complete list
is in the OpenSSL documentation, but you may not need all of them
depending on which OpenSSL functions you call from your application.
