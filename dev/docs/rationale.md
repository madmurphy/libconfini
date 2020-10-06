Why libconfini? {#rationale}
============================

The motivation behind this library came from the observation that many system
applications in GNU/Linux rely on _INI-like_ files (systemd, pacman,
network-manager, etc.), and for many users who approach GNU/Linux for the first
time it could be frustrating not to have a GUI for configuring their system.
So, I thought, a general .conf/.ini editor for GNOME _in dconf-style_ had to be
done.

One obstacle to a GUI editor for configuration files is that these often
contain commented suggestions on how to write key/values pairs, what the
default values are, etc.; and these suggestions can make a clear sense while
editing a plain text, but normally disappear when the text is parsed by some
application. So, eventually, an editor would need to be able not to loose the
comments, and should also recognize among the comments a disabled (commented
out) key or an entire disabled section, and be flexible enough to support the
numerous INI dialects that exist out there in the wild (multiline INI files,
support for single/double quotes, different delimiter symbols, nesting
sections, etc.).

With this in mind I wrote down the first algorithms for doing the job. As I
said, it was the project of an INI editor for GNOME (now abandoned)... But I
love the free software paradise, and as soon as I had a set of functions able
to do one single job (i.e., parsing INI files), I thought that it would be nice
to release it as an independent library.

Here is where **libconfini** comes.

It can be synthetized as a smart parser able to allow a complete lossless
analysis and parsing of many different types of INI files, but _K.I.S.S._
enough to not store any parsed data, but rather dispatch it (well formatted) to
a custom listener. It was born for an editor (which per se needs to read
_every_ information out of a jungle of different types of INI file), but is
usable by any application that needs to read INI files but does not need to
read commented INI lines. And when a program needs to parse INI files that
belong to other programs **libconfini** is definitely the perfect tool for the
job.

Despite being highly configurable and flexible, the library is still a small
piece of work. I honestly don't know if it will be useful to the community. But
I definitely do know one thing: **libconfini** is unique in its field.

