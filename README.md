# libscript

A plugin-based library for language-independent application extensibility.

It allows decoupling an application from the virtual machines provided by the various scripting languages.

The main library, `libscript`, is a thin layer that provides a language-independent scripting API,
allowing the application to register its functions and invoke code to be performed.
Libscript then invokes the appropriate plugin (`libscript-python`, `libscript-ruby`, `libscript-lua`,
etc.) to run the code. This way, the application can support all those scripting languages without
adding each of them as a dependency.

libscript was created by [Hisham Muhammad](http://hisham.hm) as a case study for a
[MSc dissertation](http://www.lua.inf.puc-rio.br/publications/hisham06study.pdf)
at [http://www.lua.inf.puc-rio.br](PUC-Rio).
