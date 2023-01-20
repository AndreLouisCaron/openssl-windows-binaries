# OpenSSL binaries for Windows

This is a simple pipeline to build them myself, in the cloud, on demand.

## Why?

The OpenSSL project does not supply binaries.  On most Linux distributions, you can easily install them usind the platform's package manager.  On Windows, it's a bit tricker.

Several third-party websites provide them for download.  While many of
them may be well intentioned, it only takes one malicious (or hacked)
website to cause you a world of pain, so I wouldn't trust any of them.

## How?

It's quite easy to build OpenSSL on Windows.  The `README.md` and
`NOTES-WINDOWS.md` files in the source code contain clear
instructions.  All you need is:

- Visual Studio (can be used free of charge for open source development);
- Strawberry Perl (free); and
- NASM (also free).

However, I don't like installing rarely used development tools on my
computers, so I prefer doing it via a VM of some kind.  Fortunately,
GitHub actions make this really easy!

## Where can I download them?

You should really build them yourself.  Maybe this recipe can help you
as much as it helped me :-)
