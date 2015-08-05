# gcom
Generic Component Object Model, a clean-room reimplementation of Component Object Model (in-process only).

This software dates back to 1999, back when I was working as a contractor for Amiga, Inc. for Fleecy Moss and Bill McEwen.  This software formed the foundation for the AmigaDE's implementation of COM, code-named Andromeda, intended to supercede AmigaOS libraries in a virtual machine environment that, frankly, wasn't flexible enough to support Amiga-style libraries.  Getting Andromeda working on AmigaDE was a feat of engineering, frankly.

I remember telling Fleecy that this software was not for use as a general replacement for system libraries in AmigaOS; however, it's clearly evident by anyone working with AmigaOS 4.x that he didn't listen.  :)  `exec.library/OpenLibrary` now requires you to query an interface in order to gain access to a library's functions.  To everyone who thinks this was a bad design decision, I agree.  However, it's awesome to know that my small contribution to a backwater software product made a bit of a splash in the future evolution of AmigaOS.  Even if it was entirely accidental and unintended.
